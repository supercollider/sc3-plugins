#ifndef BB_UGENS
#define BB_UGENS
 
#define DETA_CHECK_BUF \
	if (!bufData) { \
                unit->mDone = true; \
		ClearUnitOutputs(unit, 1); \
		return; \
	}

#define DETA_GET_BUF \
	float fbufnum  = DEMANDINPUT_A(0, inNumSamples);; \
	if (fbufnum != unit->m_fbufnum) { \
		uint32 bufnum = (int)fbufnum; \
		World *world = unit->mWorld; \
		if (bufnum < 0) { bufnum = 0; } \
		if (bufnum >= world->mNumSndBufs) { \
			int localBufNum = bufnum - world->mNumSndBufs; \
			Graph *parent = unit->mParent; \
			if(localBufNum <= parent->localBufNum) { \
				unit->m_buf = parent->mLocalSndBufs + localBufNum; \
			} else { \
				bufnum = 0; \
				unit->m_buf = world->mSndBufs + bufnum; \
			} \
		} else { \
			unit->m_buf = world->mSndBufs + bufnum; \
		} \
		unit->m_fbufnum = fbufnum; \
	} \
	SndBuf *buf = unit->m_buf; \
	LOCK_SNDBUF(buf); \
	float *bufData __attribute__((__unused__)) = buf->data; \
	uint32 bufChannels __attribute__((__unused__)) = buf->channels; \
	uint32 bufSamples __attribute__((__unused__)) = buf->samples; \
	uint32 bufFrames = buf->frames; \
	int mask __attribute__((__unused__)) = buf->mask; \
	int guardFrame __attribute__((__unused__)) = bufFrames - 2;

#define DETA_GET_BUF_SHARED \
	float fbufnum  = DEMANDINPUT_A(0, inNumSamples);; \
	if (fbufnum != unit->m_fbufnum) { \
		uint32 bufnum = (int)fbufnum; \
		World *world = unit->mWorld; \
		if (bufnum < 0) { bufnum = 0; } \
		if (bufnum >= world->mNumSndBufs) { \
			int localBufNum = bufnum - world->mNumSndBufs; \
			Graph *parent = unit->mParent; \
			if(localBufNum <= parent->localBufNum) { \
				unit->m_buf = parent->mLocalSndBufs + localBufNum; \
			} else { \
				bufnum = 0; \
				unit->m_buf = world->mSndBufs + bufnum; \
			} \
		} else { \
			unit->m_buf = world->mSndBufs + bufnum; \
		} \
		unit->m_fbufnum = fbufnum; \
	} \
	const SndBuf *buf = unit->m_buf; \
	LOCK_SNDBUF_SHARED(buf); \
	const float *bufData __attribute__((__unused__)) = buf->data; \
	uint32 bufChannels __attribute__((__unused__)) = buf->channels; \
	uint32 bufSamples __attribute__((__unused__)) = buf->samples; \
	uint32 bufFrames = buf->frames; \
	int mask __attribute__((__unused__)) = buf->mask; \
	int guardFrame __attribute__((__unused__)) = bufFrames - 2;

#endif
