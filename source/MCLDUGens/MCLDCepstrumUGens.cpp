/*

Cesprum UGens for SuperCollider, by Dan Stowell.
(c) Dan Stowell 2009, 2010, 2011.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

*/

#include "SC_PlugIn.h"
#include "FFT_UGens.h"
#include "SC_fftlib.h" // for Cepstrum and reverse

// Used by Cepstrum
#define SMALLEST_NUM_FOR_LOG 2e-42

//////////////////////////////////////////////////////////////////////////////////////////////////

struct Cepstrum : Unit
{
	SndBuf *buf_ceps, *buf_spec;
	scfft *m_scfft;
};
struct ICepstrum : Cepstrum {
};

//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	void load(InterfaceTable *inTable);

	void Cepstrum_Ctor(Cepstrum* unit);
	void Cepstrum_next(Cepstrum* unit, int inNumSamples);
	void Cepstrum_Dtor(Cepstrum* unit);

	void ICepstrum_Ctor(ICepstrum* unit);
	void ICepstrum_next(ICepstrum* unit, int inNumSamples);
	void ICepstrum_Dtor(ICepstrum* unit);
}

InterfaceTable *ft;

void init_SCComplex(InterfaceTable *inTable);

//////////////////////////////////////////////////////////////////////////////////////////////////

void Cepstrum_Ctor(Cepstrum* unit){
	SETCALC(Cepstrum_next);
	unit->m_scfft = NULL; // ensure scfft will be inialised, later.
	Cepstrum_next(unit, 1);
}

void ICepstrum_Ctor(ICepstrum* unit){
	SETCALC(ICepstrum_next);
	unit->m_scfft = NULL; // ensure scfft will be inialised, later.
	ICepstrum_next(unit, 1);
}

bool Cepstrum_next_common(Cepstrum* unit, float fbufnum_spec, float fbufnum_ceps, SCFFT_Direction dirn) {
	// Grab the buffer references, check they're sane and match the required sizes etc
	uint32 ibufnum_ceps = (int)fbufnum_ceps;
	uint32 ibufnum_spec = (int)fbufnum_spec;
	World *world = unit->mWorld;
	SndBuf *buf_ceps;
	if (ibufnum_ceps >= world->mNumSndBufs) {
		int localBufNum = ibufnum_ceps - world->mNumSndBufs;
		Graph *parent = unit->mParent;
		if(localBufNum <= parent->localBufNum) {
			buf_ceps = parent->mLocalSndBufs + localBufNum;
		} else {
			buf_ceps = world->mSndBufs;
		}
	} else {
		buf_ceps = world->mSndBufs + ibufnum_ceps;
	}
	SndBuf *buf_spec;
	if (ibufnum_spec >= world->mNumSndBufs) {
		int localBufNum = ibufnum_spec - world->mNumSndBufs;
		Graph *parent = unit->mParent;
		if(localBufNum <= parent->localBufNum) {
			buf_spec = parent->mLocalSndBufs + localBufNum;
		} else {
			buf_spec = world->mSndBufs;
		}
	} else {
		buf_spec = world->mSndBufs + ibufnum_spec;
	}
	if (buf_ceps->samples != buf_spec->samples >> 1){
		Print("Cepstrum_next_common error: cepstrum buffer (%i) must be half the size of fft buffer (%i)\n", buf_ceps->samples, buf_spec->samples);
		return false;
	}
	unit->buf_spec = buf_spec;
	unit->buf_ceps = buf_ceps;

	if(unit->m_scfft == NULL){
		//Print("unit->m_scfft == NULL so setting up.\n");
		unsigned int cepsize = unit->buf_ceps->samples;

        SCWorld_Allocator alloc(ft, unit->mWorld);
		unit->m_scfft = scfft_create(cepsize, cepsize, kRectWindow, unit->buf_ceps->data, unit->buf_ceps->data, dirn, alloc);
	}

	// We could check to make sure that the buffer references match what the scfft struct knows...

	return true;
}

void Cepstrum_next(Cepstrum* unit, int inNumSamples){
	// Second buffer is the ordinary chain buf
	float fbufnum_spec = ZIN0(1);
	if (fbufnum_spec < 0.f) { ZOUT0(0) = -1.f; return; }
	// First buffer holds the cepstrum (2x smaller than FFT chain buf)
	float fbufnum_ceps = ZIN0(0);
	if(!Cepstrum_next_common(unit, fbufnum_spec, fbufnum_ceps, kForward))
		return;

	SndBuf *buf_ceps = unit->buf_ceps;
	SndBuf *buf_spec = unit->buf_spec;
	int numbins = buf_ceps->samples; // "numbins" slightly different usage than in many PV ugens

	// OK so the chain has fired, we've got our source and target SndBufs

	// Ensure the source buf is in polar representation
	ToPolarApx(buf_spec);

	// Copy the __logarithms__ of the alternate slots (the DC and the other magnitudes, except nyq) into the ceps buf
	float *specdata = buf_spec->data;
	float *cepsdata = buf_ceps->data;
	for(int i=0; i<numbins; ++i){
		*cepsdata = std::abs(*specdata);
		if(*cepsdata < SMALLEST_NUM_FOR_LOG){
			//Print("tiny number: %g\n", *cepsdata);
			*cepsdata = SMALLEST_NUM_FOR_LOG;
		}
		*cepsdata = std::log(*cepsdata);
		++specdata;
		++specdata;
		++cepsdata;
	}

	// Now apply in-place FFT
	scfft_dofft(unit->m_scfft);

	// flag the buffer as being a PV cartesian buf
	buf_ceps->coord = coord_Complex;

	// tell the world the chain is ready
	ZOUT0(0) = fbufnum_ceps;
}

void ICepstrum_next(ICepstrum* unit, int inNumSamples){
	// First buffer holds the cepstrum (2x smaller than FFT chain buf)
	float fbufnum_ceps = ZIN0(0);
	if (fbufnum_ceps < 0.f) { ZOUT0(0) = -1.f; return; }
	// Second buffer is the ordinary chain buf
	float fbufnum_spec = ZIN0(1);
	if(!Cepstrum_next_common(unit, fbufnum_spec, fbufnum_ceps, kBackward))
		return;

	SndBuf *buf_ceps = unit->buf_ceps;
	SndBuf *buf_spec = unit->buf_spec;
	int numbins = buf_ceps->samples; // "numbins" slightly different usage than in many PV ugens

	// OK so the chain has fired, we've got our source and target SndBufs

	// Ensure the ceps buf is in cartesian representation
	ToComplexApx(buf_ceps);

	// inverse FFT in-place
	scfft_doifft(unit->m_scfft);

	// undo the logarithm at the same time as copying the magnitudes back into the spectral buffer
	float *specdata = buf_spec->data;
	float *cepsdata = buf_ceps->data;
	for(int i=0; i<numbins; ++i){
		*specdata = std::exp(*cepsdata);
		++specdata;
		++specdata;
		++cepsdata;
	}

	// the chain buffer should already be flagged as polar (from the forward cepstrum), but just in case...
	buf_spec->coord = coord_Polar;

	// tell the world the chain is ready
	ZOUT0(0) = fbufnum_spec;
}

void Cepstrum_Dtor(Cepstrum* unit){
	if(unit->m_scfft) RTFree(unit->mWorld, unit->m_scfft);
}

void ICepstrum_Dtor(ICepstrum* unit){
	if(unit->m_scfft) RTFree(unit->mWorld, unit->m_scfft);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

PluginLoad(MCLDCepstrum)
{
	ft= inTable;

	DefineDtorUnit(Cepstrum);
	DefineDtorUnit(ICepstrum);
}
