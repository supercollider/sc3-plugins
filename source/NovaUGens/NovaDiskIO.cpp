/*
 *
 *    Copyright (C) 2013 Tim Blechmann
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "sndfile.h"
#include "SC_PlugIn.hpp"

#include <array>
#include <atomic>
#include <map>
#include <thread>
#include <nova-tt/semaphore.hpp>

#include "NovaUtils.hpp"

#include <boost/lockfree/queue.hpp>
#include <boost/lockfree/stack.hpp>

static InterfaceTable *ft;

using nova::loop;

struct DiskOutThread
{
	typedef enum {
		OpenFile,
		WriteFrames,
		CloseFile
	} CmdType;

	static const int PayloadSize = 1024 * 64 * sizeof(float); // pessimize: 64 channels, 1024 samples

	struct Cmd
	{
		CmdType cmd;
		int ticket;
		int channels;
		int frames;
		int samplerate;
		char payload[PayloadSize];
	};

	const int NumberOfElements = 256;

	DiskOutThread():
		mQueue(NumberOfElements),
		mPool(NumberOfElements + 8)
	{
		using namespace std;

		loop(NumberOfElements, [&](){
			Cmd * cmd = new Cmd();
			memset(cmd, 0, sizeof(Cmd));
			mPool.push( cmd );
		});

		mRunning = true;
		mThread = move(thread(bind(&DiskOutThread::cmdLoop, this)));
	}

	~DiskOutThread()
	{
		mRunning = false;

		mSemaphore.post();
		mThread.join();

		for(;;) {
			Cmd * cmd;
			bool success = mPool.pop(cmd);
			if (!success)
				break;
			delete cmd;
		}

		for (auto elem : mSndfileMap) {
			SNDFILE * handle = elem.second;
			sf_close(handle);
		}
	}

	int openFile(const char * fileName, int channels, int samplingRate)
	{
		int ret = mTicket.fetch_add(1);

		Cmd * cmd = allocateCmd(OpenFile, ret);
		if (!cmd)
			return -1;

		cmd->channels   = channels;
		cmd->samplerate = samplingRate;
		strcpy(cmd->payload, fileName);

		bool success = mQueue.push(cmd);
		assert(success);
		mSemaphore.post();

		return ret;
	}

	template <typename Functor>
	bool pushFrames(int ticket, Functor const & fillCmdStruct)
	{
		Cmd * cmd = allocateCmd(WriteFrames, ticket);
		if (!cmd)
			return false;

		fillCmdStruct(cmd);

		bool success = mQueue.push(cmd);
		assert(success);
		mSemaphore.post();

		return true;
	}

	bool closeFile(int ticket)
	{
		Cmd * cmd = allocateCmd(CloseFile, ticket);
		if (!cmd)
			return false;

		bool success = mQueue.push(cmd);
		assert(success);
		mSemaphore.post();

		return true;
	}

	void cmdLoop()
	{
		while (mRunning) {
			mSemaphore.wait();
			processQueueEvent();
		}

		while (processQueueEvent()) {
			// nop
		}
	}

private:
	Cmd * allocateCmd(CmdType cmdType, int ticket)
	{
		Cmd * cmd;
		bool success = mPool.pop(cmd);
		if (!success)
			return nullptr;

		cmd->cmd    = cmdType;
		cmd->ticket = ticket;
		return cmd;
	}

	bool processQueueEvent()
	{
		Cmd * cmd;
		bool success = mQueue.pop(cmd);

		if (success) {
			switch (cmd->cmd) {
			case OpenFile:
			{
				SF_INFO info;
				info.samplerate = cmd->samplerate;
				info.channels = cmd->channels;
				info.format   = SF_FORMAT_CAF | SF_FORMAT_FLOAT;

				SNDFILE * handle = sf_open(cmd->payload, SFM_WRITE, &info);
				if (!handle) {
					printf("could not open file: %s %s\n", cmd->payload, sf_error_number(sf_error(handle)));
					break;
				}

				mSndfileMap.insert(std::make_pair(cmd->ticket, handle));
				break;
			}

			case CloseFile:
			{
				SNDFILE * handle = mSndfileMap[cmd->ticket];
				sf_close(handle);
				mSndfileMap.erase(cmd->ticket);
				break;
			}

			case WriteFrames:
			{
				interleave(cmd->channels, cmd->frames, (float*)cmd->payload, mScratchpad.data());

				SNDFILE * handle = mSndfileMap[cmd->ticket];
				int written = sf_writef_float(handle, mScratchpad.data(), cmd->frames);

				break;
			}

			default:
				assert(false);
			}

			bool pushSuccessful = mPool.push(cmd);
			assert(pushSuccessful);
			return true;
		} else
			return false;
	}

	static void interleave(int channels, int frames, const float * source, float * destination)
	{
		nova::loop(frames, [&](int frame) {
			nova::loop(channels, [&](int channel) {
				destination[frame * channels + channel] = source[channel * frames + frame];
			});
		});
	}

	std::thread mThread;
	std::atomic_bool mRunning;

	boost::lockfree::queue<Cmd*> mQueue;
	boost::lockfree::stack<Cmd*> mPool;

	nova::semaphore mSemaphore;

	std::atomic_int mTicket;

	std::map<int, SNDFILE*> mSndfileMap;
	std::array<float, PayloadSize> mScratchpad;
};

std::unique_ptr<DiskOutThread> gThread;

struct NovaDiskOut:
	public SCUnit
{
public:
	NovaDiskOut()
	{
		mChannelCount = in0(0);

		const int indexOfPathSize  = 1 + mChannelCount;
		const int indexOfPathStart = 2 + mChannelCount;
		const int pathSize  = (int)in0(indexOfPathSize);
		char path[16384];

		nova::loop(pathSize, [&](int index) {
			path[index] = (char)in0(indexOfPathStart + index);
		});

		mTicket = gThread->openFile(path, mChannelCount, (int)sampleRate());
		if (mTicket == -1) {
			Print("Cannot open file %s\n", path);
			mCalcFunc = ft->fClearUnitOutputs;
			return;
		}

		mCalcFunc = make_calc_function<NovaDiskOut, &NovaDiskOut::next>();
	}

	~NovaDiskOut()
	{
		gThread->closeFile(mTicket);
	}

private:
	void next(int inNumSamples)
	{
		gThread->pushFrames(mTicket, [&](DiskOutThread::Cmd * cmd) {
			cmd->frames   = inNumSamples;
			cmd->channels = mChannelCount;

			nova::loop(mChannelCount, [&](int channel) {
				const float * data = in(1 + channel);
				float * cmdData    = (float*)cmd->payload + channel * inNumSamples;

				memcpy(cmdData, data, inNumSamples * sizeof(float));
			});
		});
	}

	int mChannelCount;
	int mTicket;
};


DEFINE_XTORS(NovaDiskOut)

PluginLoad(NovaDiskIO)
{
	gThread.reset(new DiskOutThread);

	ft = inTable;
	DefineDtorUnit(NovaDiskOut);
}
