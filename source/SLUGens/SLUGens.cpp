/*
	SuperCollider real time audio synthesis system
 Copyright (c) 2002 James McCartney. All rights reserved.
	http://www.audiosynth.com

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

//UGens by Nick Collins
//SLUGens released under the GNU GPL as extensions for SuperCollider 3, by Nick Collins http://composerprogrammer.com/index.html

#include "SC_PlugIn.h"
#include <stdio.h>

//#define SLUGENSRESEARCH 1


static InterfaceTable *ft;

struct SortBuf : public Unit   //Karplus Strong with sorting algorithm on a buffer
{
	uint32 mBufNum,mBufSize;
	int mOutStep;
	int mSorti, mSortj, mSortdone;

	float* mBufCopy;
};

struct GravityGrid : public Unit
{
	float x[9],y[9],velx,vely,posx,posy;
	float * m_weights;
};


//max 25 masses
struct GravityGrid2 : public Unit
{
	//float x[25],y[25],
	float velx,vely,posx,posy;
	//int nummasses;
	float * m_weights; //[25];
};


//bufnum, 0 capturetrigger 1, duration of repeat 2, ampdropout 3
struct Breakcore : public Unit
{
	uint32 mBufNum,mBufSize;
	float * mBuf;
	int captureon,captureneeds,repeatpos,capturepos;
	float m_prevtrig;
	//int duration;
};


//find maximum value within last x blocks
struct Max : public Unit {
	int m_blocks, m_counter,m_last;
	float m_max;
	float* m_blockmaxes;
};

//print in lang a value every x blocks
struct PrintVal : public Unit {
	int m_count,m_block,m_id;
};

//from http://www.musicdsp.org/showone.php?id=97
struct EnvDetect : public Unit {
	float m_env; //m_atk,m_rel,
};

struct FitzHughNagumo : public Unit
{
	float u,w;
};

struct DoubleWell : public Unit
{
	float x,y,t;
};

struct DoubleWell2 : public Unit
{
	float x,y,t;
};


struct DoubleWell3 : public Unit
{
	float x,y,t;
};

struct WeaklyNonlinear : public Unit
{
	float x,y,t;
};

struct WeaklyNonlinear2 : public Unit
{
	float x,y,t;
};

struct TermanWang : public Unit
{
	float x,y;
};

struct LTI : public Unit
{
	int sizea,sizeb;
	float * bufa; //feedback coeff
	float * bufb; //feedforward coeff
	float * mema, *memb;
	int posa, posb;
};


struct NL : public Unit
{
	int sizea,sizeb;
	float * bufa; //feedback coeff
	float * bufb; //feedforward coeff
	float * mema, *memb;

	int numasummands,numbsummands;

	int *aindices, *bindices;

	int posa, posb;
};


struct NL2 : public Unit
{
	int sizea,sizeb;
	//float * buf; //crossterm data instructions
	float * mema, *memb;
	int posa, posb;
};


//initial draft, on single blocks only, would need to expand as multiblock for better spectral approximation (though with greater delay)
//do not use with block size greater than 64!
struct LPCError : public Unit
{
	int p;	//current accuracy - p poles in resynthesis, 1-20 at first allowed
			//float x0;
	float last[64];
	float coeff[64]; //LPC filter coeff
					 //float * calc1; //storage for calculations

	float R[65];//autocorrelation coeffs;
		float preva[65];
		float a[65];

};


struct KmeansToBPSet1 : public Unit
{
	int numdatapoints, maxmeans, nummeans;
	float * data;
	float * means, *newmeans;
	int * newmeancount;
	float * bpx, *bpy;
	int numbps;

	double mPhase;
	float mFreqMul, mSpeed;

	int newmeanflag, newdataflag;

	//for non-random starting/reset data
	float * m_initdata;
	int m_meansindex;
};


struct Instruction : public Unit
{
	uint32 mBufNum,mBufSize;
	float * mBuf;
	int bufpos;
	//float lastlastAmp;
	float lastAmp, newAmp;
	int interpsteps, interpnow;
	float prob;
	//float lastInstruction;

	//int duration;
};


//WaveTerrain(bufnum, x, y, xsize, ysize, wrapbehaviour)
struct WaveTerrain : public Unit {
    //everything local to next function now, no persistent between block data
};



//also VMScan for 1 dimensional version?
//also VMscanND?
//VMScan2D(bufnum)
struct VMScan2D : public Unit
{
	uint32 mBufNum,mBufSize;
	float * mBuf;
	int bufpos;
	//float lastlastAmp;
	float lastx, lasty, newx, newy;
	int interpsteps, interpnow;
	float prob;

	//instruction set leads to x and y position
};

//also VM for nonlinear filter equations; stability not guaranteed, so various strategies for recovery




struct SLOnset : public Unit
{
	float * m_memory1, * m_memory2;
	int m_memorysize1, m_before, m_after, m_memorysize2, m_hysteresiscount, m_memorycounter1, m_memorycounter2;
};


//only sample length delays for now too
//could have pitch control later, but totaldelay, junctionposition also too indirect; d1 and d2 lengths
//pitch and junctionpos at initialisation only for now, g is loss function
//output= TwoTube.ar(input, scatteringcoefficient,lossfactor,d1length,d2length);
//waveguide synthesis experiment
//scattering junction control k will be audio input
struct TwoTube : public Unit
{
	float * delay1right, * delay1left; //tube 1
	float * delay2right, * delay2left; //tube 2
	int d1length, d2length;
	float lossfactor;
	float f1in, f1out;	//averaging filters f1, f2 for frequency dependent losses; need a storage slot for previous values
	float f2in, f2out;
	int d1rightpos, d1leftpos, d2rightpos, d2leftpos;
};




struct NTube : public Unit
{
	int numtubes;
	float ** delayright, ** delayleft; //tubes
	int position; //can be same for all lines!
	int maxlength, modulo;
	float delayconversion;
	float f1in, f1out;	//averaging filters f1, f2 for frequency dependent losses; need a storage slot for previous values
	float f2in, f2out;

	//convenience variables for copying particular input data
	float * losses;
	float * scattering;
	float * delays;

	float * rightouts;
	float * leftouts;

};


struct EnvFollow : public Unit
{
	float eprev_;

};

struct Sieve1 : public Unit
{

	float * buffer_;
	int maxdatasize_;
	int currentsize_;
	int bufferpos_;
	//int interpsize_;

	float phase_;
	//int interp_;
	int alternate_;
	int swap_;


};



//see http://www.scholarpedia.org/article/Oregonator

//see also A Brief History of Oscillators and Hair Styles of European Men AASU Math/CS Colloquium, April 2002.
struct Oregonator: public Unit {

    float x, y, z;

};


struct Brusselator: public Unit {

    float x, y;

};


struct SpruceBudworm: public Unit {

    float x, y;

};






extern "C" {

	void SortBuf_next_k(SortBuf *unit, int inNumSamples);
	void SortBuf_Ctor(SortBuf* unit);
	//void SortBuf_Dtor(SortBuf* unit);

	void GravityGrid_next_k(GravityGrid *unit, int inNumSamples);
	void GravityGrid_Ctor(GravityGrid* unit);

	void GravityGrid2_next_k(GravityGrid2 *unit, int inNumSamples);
	void GravityGrid2_Ctor(GravityGrid2* unit);

	void Breakcore_next_k(Breakcore *unit, int inNumSamples);
	void Breakcore_Ctor(Breakcore* unit);

	void Max_next(Max *unit, int inNumSamples);
	void Max_Ctor(Max* unit);
	void Max_Dtor(Max* unit);

	void PrintVal_next(PrintVal *unit, int inNumSamples);
	void PrintVal_Ctor(PrintVal* unit);

	void EnvDetect_next(EnvDetect *unit, int inNumSamples);
	void EnvDetect_Ctor(EnvDetect* unit);

	void FitzHughNagumo_next_k(FitzHughNagumo *unit, int inNumSamples);
	void FitzHughNagumo_Ctor(FitzHughNagumo* unit);

	void DoubleWell_next_k(DoubleWell *unit, int inNumSamples);
	void DoubleWell_Ctor(DoubleWell* unit);

	void DoubleWell2_next_k(DoubleWell2 *unit, int inNumSamples);
	void DoubleWell2_Ctor(DoubleWell2* unit);

	void DoubleWell3_next_k(DoubleWell3 *unit, int inNumSamples);
	void DoubleWell3_Ctor(DoubleWell3* unit);

	void WeaklyNonlinear_next_k(WeaklyNonlinear *unit, int inNumSamples);
	void WeaklyNonlinear_Ctor(WeaklyNonlinear* unit);

	void WeaklyNonlinear2_next_k(WeaklyNonlinear2 *unit, int inNumSamples);
	void WeaklyNonlinear2_Ctor(WeaklyNonlinear2* unit);

	void TermanWang_next_k(TermanWang *unit, int inNumSamples);
	void TermanWang_Ctor(TermanWang *unit);

	//arbitrary potential well function polynomial leads to buffer coefficients/powers of up to order 10 passed in
	//void PotentialWell_next_a(PotentialWell *unit, int inNumSamples);
	//void PotentialWell_Ctor(PotentialWell* unit);

	void LTI_next_a(LTI *unit, int inNumSamples);
	void LTI_Ctor(LTI* unit);
	void LTI_Dtor(LTI* unit);

	void NL_next_a(NL *unit, int inNumSamples);
	void NL_Ctor(NL* unit);
	void NL_Dtor(NL* unit);

	void NL2_next_a(NL2 *unit, int inNumSamples);
	void NL2_Ctor(NL2* unit);
	void NL2_Dtor(NL2* unit);

	void LPCError_next_a(LPCError *unit, int inNumSamples);
	void LPCError_Ctor(LPCError* unit);
	//void LPCError_Dtor(LPCError* unit);

	void KmeansToBPSet1_next_a(KmeansToBPSet1 *unit, int inNumSamples);
	void KmeansToBPSet1_Ctor(KmeansToBPSet1* unit);
	void KmeansToBPSet1_Dtor(KmeansToBPSet1* unit);
	void MakeBPSet(KmeansToBPSet1 *unit);

	void Instruction_next_a(Instruction *unit, int inNumSamples);
	void Instruction_Ctor(Instruction* unit);
	void readinstruction(Instruction *unit, int command, float param);

	void WaveTerrain_next_a(WaveTerrain *unit, int inNumSamples);
	void WaveTerrain_Ctor(WaveTerrain* unit);

	void VMScan2D_next_a(VMScan2D *unit, int inNumSamples);
	void VMScan2D_Ctor(VMScan2D* unit);
	void readinstructionVMScan2D(VMScan2D *unit, int command, float param);


	void SLOnset_next(SLOnset *unit, int inNumSamples);
	void SLOnset_Ctor(SLOnset* unit);
	void SLOnset_Dtor(SLOnset* unit);

	void TwoTube_next(TwoTube *unit, int inNumSamples);
	void TwoTube_Ctor(TwoTube* unit);
	void TwoTube_Dtor(TwoTube* unit);

	void NTube_next(NTube *unit, int inNumSamples);
	void NTube_Ctor(NTube* unit);
	void NTube_Dtor(NTube* unit);

    void EnvFollow_next(EnvFollow *unit, int inNumSamples);
	void EnvFollow_Ctor(EnvFollow* unit);
	//void BlitB3_Dtor(BlitB3* unit);

	void Sieve1_next(Sieve1 *unit, int inNumSamples);
	void Sieve1_Ctor(Sieve1* unit);

    void Oregonator_next(Oregonator *unit, int inNumSamples);
	void Oregonator_Ctor(Oregonator* unit);

    void Brusselator_next(Brusselator *unit, int inNumSamples);
	void Brusselator_Ctor(Brusselator* unit);

    void SpruceBudworm_next(SpruceBudworm *unit, int inNumSamples);
	void SpruceBudworm_Ctor(SpruceBudworm* unit);
}



//include local buffer test in one place
SndBuf * SLUGensGetBuffer(Unit * unit, uint32 bufnum) {

	SndBuf *buf;
	World *world = unit->mWorld;

	if (bufnum >= world->mNumSndBufs) {
		int localBufNum = bufnum - world->mNumSndBufs;
		Graph *parent = unit->mParent;
		if(localBufNum <= parent->localMaxBufNum) {
			buf = parent->mLocalSndBufs + localBufNum;
		} else {
			if(unit->mWorld->mVerbosity > -1){ Print("SLUGens buffer number error: invalid buffer number: %i.\n", bufnum); }
			SETCALC(*ClearUnitOutputs);
			unit->mDone = true;
			return NULL;
		}
	} else {
		buf = world->mSndBufs + bufnum;
	}

	return buf;
}



void SortBuf_Ctor( SortBuf* unit ) {

	SETCALC(SortBuf_next_k);

	//World *world = unit->mWorld;

	uint32 bufnum = (uint32)ZIN0(0);
	//if (bufnum >= world->mNumSndBufs) bufnum = 0;

	SndBuf * buf= SLUGensGetBuffer(unit,bufnum);

	if (buf) {

		unit->mBufNum=bufnum;

		//printf("%d \n",bufnum);
		///SndBuf *buf = world->mSndBufs + bufnum;

		unit->mBufSize = buf->samples;

		unit->mBufCopy= buf->data; //(float*)RTAlloc(unit->mWorld, unit->mBufSize * sizeof(float));

		//initialise to copy
		//int i=0;
		//	for(i=0; i<unit->mBufSize;++i)
		//		unit->mBufCopy[i]=buf->data[i];
		//
		unit->mSorti=unit->mBufSize - 1;
		unit->mSortj=1;
		unit->mSortdone=0;
		unit->mOutStep=1; //allows one run through before it sorts

	}
}

//void SortBuf_Dtor(SortBuf *unit)
//{
//	//RTFree(unit->mWorld, unit->mBufCopy);
//}

void SortBuf_next_k( SortBuf *unit, int inNumSamples ) {

	float *out = ZOUT(0);

	int sortrate= (int)ZIN0(1);
	float reset= (float)ZIN0(2);
	//float freq= (float)ZIN0(3);

	//printf("%d \n",unit->mWorld->mBufCounter);

	int outstep=unit->mOutStep;

	//output, doing sorts if requested
	int bufsize=unit->mBufSize;

	float * data= unit->mBufCopy;

	int sorti, sortj, sortdone;
	float temp;
	sorti=unit->mSorti;
	sortj=unit->mSortj;
	sortdone=unit->mSortdone;

	//printf("sortparams %d %d %d \n",sorti,sortj,sortdone);

	if(reset>0.5 && (unit->mWorld->mBufCounter>10)) {
		//		//printf("reset!\n");
		reset=0.0;
		//SndBuf *buf = unit->mWorld->mSndBufs + unit->mBufNum;
		//float *source=buf->data;

		//for(int i=0; i<bufsize;++i)
		//	data[i]=source[i];

		sorti=(bufsize - 1);
		sortj=1;
		sortdone=0;
	}


	for (int j=0; j<inNumSamples;++j)
	{

		if (outstep ==0) { //SORT

			int sorttodo=sortrate;

			//printf("sort! %d \n", sorttodo);

			while((sorttodo>0) && (sortdone==0)){

				if (data[sortj-1] > data[sortj]) {
					temp = data[sortj-1];
					data[sortj-1] = data[sortj];
					data[sortj] = temp;
				}

				++sortj;

				if(sortj>sorti) {
					--sorti;
					sortj=1;
					if(sorti<0)
					{
						sortdone=1;
						//printf("sort finished %d \n",sortdone);

					}

				}

				sorttodo--;
			}

		}

		//printf("outstep %d bufsize %d \n",outstep,bufsize);

		//else output
		ZXP(out) = data[outstep];

		outstep= (outstep+1)%bufsize;
	}

	unit->mSorti=sorti;
	unit->mSortj=sortj;
	unit->mSortdone=sortdone;

	unit->mOutStep=outstep;
}


void GravityGrid_Ctor(GravityGrid* unit) {
	int i;

	//World *world = unit->mWorld;

	for(i=0;i<9;++i) {

		unit->x[i]=(i%3)-1;

		unit->y[i]=1-(i/3);

		//printf("i %d x %f y %f \n",i,unit->x[i],unit->y[i]);

	}

	unit->velx=0.0;
	unit->vely=0.0;
	unit->posx=0.0;
	unit->posy=0.0;

	//for (i=0;i<9;++i)
	//	unit->m_weights[i]= g_weights[i];
	unit->m_weights=NULL;

	//must create int because have -1 as indicator of no buffer!
	int bufnum= (int)ZIN0(4);
	//uint32 bufnum = (uint32)ZIN0(4);

	if (bufnum>=0) {

		SndBuf * buf= SLUGensGetBuffer(unit, (uint32)bufnum);

		if (buf) {

			if(buf->samples==9) {
				unit->m_weights= buf->data;

			}

		}
		else
		{
			unit->mDone = true;
		}

		//if (!(bufnum > world->mNumSndBufs || bufnum<0)) {
		//		SndBuf *buf = world->mSndBufs + bufnum;
		//
		//		if(buf->samples==9) {
		//			unit->m_weights= buf->data;
		//
		//		}

		}

	SETCALC(GravityGrid_next_k);

	}


//can't have independent x and y, causes trouble- zero has a pusher at it with infinite size!
//must use euclidean distances
void GravityGrid_next_k(GravityGrid *unit, int inNumSamples) {

	float *out = ZOUT(0);
	float accelx,accely, xdiff,ydiff, rdiff; //hyp;

	int reset= (int)ZIN0(0);
	float rate= (float)ZIN0(1);

	float velx, vely, posx, posy;

	velx= unit->velx;
	vely=unit->vely;
	posx=unit->posx;
	posy=unit->posy;

	if(reset) {
		//RGen& rgen = *unit->mParent->mRGen;

		//printf("reset! \n");

		posx= (float)ZIN0(2); //0.8*(2*rgen.frand() - 1.0); //never right near edge
		posy= (float)ZIN0(3); //0.8*(2*rgen.frand() - 1.0);

		velx=0.0;
		vely=0.0;

		if (posx>0.99) posx=0.99;
		if (posx<-0.99) posx=-0.99;
		if (posy<-0.99) posy=-0.99;
		if (posy>0.99) posy=0.99;


	}

	float *x,*y, *w;

	x=unit->x;
	y=unit->y;
	w=unit->m_weights;

	if (w) {
		for (int j=0; j<inNumSamples;++j) {

			accelx=0.0;
			accely=0.0;

			//could use this to decide on run function but can't be bothered, negligible on loop performance and
			//keeps code in one place for edits

			for(int i=0;i<9;++i) {

				if(i!=4) {

					xdiff= ((posx)-(x[i]));
					ydiff= ((y[i])-(posy));

					//hyp= 1.0/sqrt(xdiff*xdiff+ydiff*ydiff);

					rdiff= w[i]*0.0001*sqrt(xdiff*xdiff+ydiff*ydiff); //(0.0001/(xdiff*xdiff+ydiff*ydiff));

					accelx+= rdiff*xdiff; //rdiff*(xdiff*hyp);

					accely+= rdiff*ydiff; //rdiff*(ydiff*hyp);

				}

			}


			velx=velx+accelx;
			vely=vely+accely;

			posx=posx+(rate*(velx));
			posy=posy+(rate*(vely));

			//assumes fmod works correctly for negative values
			if ((posx>1.0) || (posx<-1.0)) posx=fabs(fmod((posx-1.0),4.0)-2.0)-1.0;
			if ((posy>1.0) || (posy-1.0)) posy=fabs(fmod((posy-1.0),4.0)-2.0)-1.0;

			//correction: (better with the distorted version above!)
			//if ((posx>1.0) || (posx<-1.0)) posx=sc_fold(posx, -1.0f, 1.0f);
			//if ((posy>1.0) || (posy<-1.0)) posy=sc_fold(posy, -1.0f, 1.0f);




			//printf("%f %f %f %f %f %f %f \n",accelx, accely, unit->velx,unit->vely,unit->posx,unit->posy, unit->posy*unit->posy+ unit->posx*unit->posx);

			float sign;
			if (fabs(posx) < 0.0000001) sign= 1;
			else
				sign= (posx/fabs(posx));

			ZXP(out) = sign*0.5*(posy*posy+ posx*posx);

			//printf("%f \n",(unit->posx/fabs(unit->posx))*0.5*(unit->posy*unit->posy+ unit->posx*unit->posx));
		}
	}
	else {

		for (int j=0; j<inNumSamples;++j) {

			accelx=0.0;
			accely=0.0;

			//could use this to decide on run function but can't be bothered, negligible on loop performance and
			//keeps code in one place for edits

			for(int i=0;i<9;++i) {

				if(i!=4) {

					xdiff= ((posx)-(x[i]));
					ydiff= ((y[i])-(posy));

					rdiff= 0.0001*sqrt(xdiff*xdiff+ydiff*ydiff);
					accelx+= rdiff*xdiff;
					accely+= rdiff*ydiff;

				}

			}

			velx=velx+accelx;
			vely=vely+accely;

			posx=posx+(rate*(velx));
			posy=posy+(rate*(vely));

			//assumes fmod works correctly for negative values
			if ((posx>1.0) || (posx<-1.0)) posx=fabs(fmod((posx-1.0),4.0)-2.0)-1.0;
			if ((posy>1.0) || (posy-1.0)) posy=fabs(fmod((posy-1.0),4.0)-2.0)-1.0;


			//if ((posx>1.0) || (posx<-1.0)) posx=sc_fold(posx, -1.0f, 1.0f);
			//if ((posy>1.0) || (posy<-1.0)) posy=sc_fold(posy, -1.0f, 1.0f);


			//printf("%f %f %f %f %f %f %f \n",accelx, accely, unit->velx,unit->vely,unit->posx,unit->posy, unit->posy*unit->posy+ unit->posx*unit->posx);


			//this also gives a discontinuity!
			float sign;
			if (fabs(posx) < 0.0000001) sign= 1;
			else
				sign= (posx/fabs(posx));

			ZXP(out) = sign*0.5*(posy*posy+ posx*posx);

			//printf("%f \n",(unit->posx/fabs(unit->posx))*0.5*(unit->posy*unit->posy+ unit->posx*unit->posx));
		}

	}


	unit->velx=velx;
	unit->vely=vely;
	unit->posx=posx;
	unit->posy=posy;


}



void GravityGrid2_Ctor(GravityGrid2* unit) {
	//int i;

	//World *world = unit->mWorld;

	//get buffer which contains initial number of masses (max of 25), positions and weights

	//uint32 bufnum = (uint32)ZIN0(4);

	//int num;
	int bufnum= (int)ZIN0(4);
	//uint32 bufnum = (uint32)ZIN0(4);

	if (bufnum>=0) {

		SndBuf * snd= SLUGensGetBuffer(unit,(int)bufnum);

		if (snd) {

			//if (!(bufnum > world->mNumSndBufs || bufnum<0)) {
			//SndBuf *snd = world->mSndBufs + bufnum;
			float *buf = snd->data;

			//num= (int)(buf[0]+0.001f);
			//unit->nummasses = num;

			unit->m_weights= buf;

			//for (i=0; i<num; ++i) {
			//
			//		int pos= i*3+1;
			//
			//		unit->x[i]=buf[pos];
			//		unit->y[i]=buf[pos+1];
			//		unit->m_weights[i]=buf[pos+2];
			//		}

			} else
			{unit->mDone=false;}

		}

	unit->velx=0.0;
	unit->vely=0.0;
	unit->posx=0.0;
	unit->posy=0.0;

	SETCALC(GravityGrid2_next_k);

	}

float gg_lookupsin[100];
float gg_lookupcos[100];


//can't have independent x and y, causes trouble- zero has a pusher at it with infinite size!
//must use euclidean distances
void GravityGrid2_next_k(GravityGrid2 *unit, int inNumSamples) {

	float *out = ZOUT(0);
	float accelx,accely, xdiff,ydiff, rdiff, hyp, theta;
	int lookupindex;
	float tmp;

	int reset= (int)ZIN0(0);
	float rate= (float)ZIN0(1);

	float velx, vely, posx, posy;

	velx= unit->velx;
	vely=unit->vely;
	posx=unit->posx;
	posy=unit->posy;

	//reset resets velocity and position
	if(reset) {
		//RGen& rgen = *unit->mParent->mRGen;

		//printf("reset! \n");

		posx= (float)ZIN0(2); //0.8*(2*rgen.frand() - 1.0); //never right near edge
		posy= (float)ZIN0(3); //0.8*(2*rgen.frand() - 1.0);

		velx=0.0;
		vely=0.0;

		if (posx>0.99) posx=0.99;
		if (posx<-0.99) posx=-0.99;
		if (posy<-0.99) posy=-0.99;
		if (posy>0.99) posy=0.99;

	}

	//float *x,*y,
	float *w;

	//x=unit->x;
	//y=unit->y;
	w=unit->m_weights;

	int num= (int)(w[0]+0.001f);

	for (int j=0; j<inNumSamples;++j) {

		accelx=0.0;
		accely=0.0;

		//could use this to decide on run function but can't be bothered, negligible on loop performance and
		//keeps code in one place for edits

		for(int i=0;i<num;++i) {

			int index= i*3;

			//xdiff= ((posx)-(x[i]));
			//ydiff= ((y[i])-(posy));

			xdiff= ((posx)-(w[index]));
			ydiff= ((w[index+1])-(posy));


			if((xdiff < 0.01) && (xdiff>(-0.00001))) xdiff= 0.01;
			if((ydiff < 0.01) && (ydiff>(-0.00001))) ydiff= 0.01;
			if((xdiff > (-0.01)) && (xdiff<(0.0))) xdiff= -0.01;
			if((ydiff > (-0.01)) && (ydiff<(0.0))) ydiff= -0.01;

			theta= atan2(ydiff, xdiff);
			//result is between -pi and pi

			lookupindex= (int)(((theta/twopi)*99.999)+50); //gives index into lookup tables

			//lookup tables for cos and sin

			//hyp= 1.0/sqrt(xdiff*xdiff+ydiff*ydiff);

			hyp = (xdiff*xdiff+ydiff*ydiff);

			//if(hyp < 0.01) hyp =0.01;

			//need lookup table

			//force of gravity
			//w[i]
			rdiff= w[index+2]*0.0001*(1.0/hyp); //*sqrt(xdiff*xdiff+ydiff*ydiff); //(0.0001/(xdiff*xdiff+ydiff*ydiff));

			tmp= rdiff*(gg_lookupcos[lookupindex]);

			accelx+= tmp; //rdiff*(xdiff*hyp);

			tmp= rdiff*(gg_lookupsin[lookupindex]);

			accely+= tmp; //rdiff*ydiff; //rdiff*(ydiff*hyp);

		}


		velx=velx+accelx;
		vely=vely+accely;

		//constraints on vel to avoid runaway speeds
		if ((velx>1.0) || (velx<-1.0)) velx=sc_fold(velx, -1.0f, 1.0f);
		if ((vely>1.0) || (vely<-1.0)) vely=sc_fold(vely, -1.0f, 1.0f);

		posx=posx+(rate*(velx));
		posy=posy+(rate*(vely));

		//WRONG assumes fmod works correctly for negative values
		//if ((posx>1.0) || (posx<-1.0)) posx=fabs(fmod((posx-1.0),4.0)-2.0)-1.0;
		//if ((posy>1.0) || (posy-1.0)) posy=fabs(fmod((posy-1.0),4.0)-2.0)-1.0;

		//correction: (better with the distorted version above!)
		if ((posx>1.0) || (posx<-1.0)) posx=sc_fold(posx, -1.0f, 1.0f);
		if ((posy>1.0) || (posy<-1.0)) posy=sc_fold(posy, -1.0f, 1.0f);

		//could also be wrap

		//printf("%f %f %f %f %f %f %f \n",accelx, accely, unit->velx,unit->vely,unit->posx,unit->posy, unit->posy*unit->posy+ unit->posx*unit->posx);


		//removed because discontinuous
		//float sign;
		//			if (fabs(posx) < 0.0000001) sign= 1;
		//			else
		//				sign= (posx/fabs(posx));
		//
		ZXP(out) = (posy*posy+ posx*posx)*2.0f-1.0f; //posx; //sign*0.5*(posy*posy+ posx*posx);

		//printf("%f \n",(unit->posx/fabs(unit->posx))*0.5*(unit->posy*unit->posy+ unit->posx*unit->posx));
	}


	unit->velx=velx;
	unit->vely=vely;
	unit->posx=posx;
	unit->posy=posy;


}





void Breakcore_Ctor( Breakcore* unit ) {

	SETCALC(Breakcore_next_k);

	World *world = unit->mWorld;

	uint32 bufnum = (uint32)ZIN0(0);
	if (bufnum >= world->mNumSndBufs) bufnum = 0;
	unit->mBufNum=bufnum;

	//printf("Breakcore activated with bufnum %d \n",bufnum);

	SndBuf *buf = world->mSndBufs + bufnum;

	unit->mBufSize = buf->samples;

	unit->mBuf = buf->data;

	unit->captureon= 0;
	unit->captureneeds= 1000;
	unit->capturepos= 0;
	unit->repeatpos=0;
	unit->m_prevtrig=0;

	//unit->duration=1000;
}


//bufnum, 0 capturein 1, capturetrigger 2, duration of repeat 3, ampdropout 4

void  Breakcore_next_k(  Breakcore *unit, int inNumSamples ) {

	float *out = ZOUT(0);

	float curtrig= ZIN0(2);
	//int duration= unit->duration; //(int)ZIN0(3);
	//float ampdropout= (float)ZIN0(3);
	float * in = ZIN(1);

	//printf("duration %d curtrig %f\n",duration, curtrig);


	//int bufsize=unit->mBufSize;

	float * data= unit->mBuf;



	if (unit->m_prevtrig <= 0.f && curtrig > 0.f){

		//printf("capture \n");

		unit->captureon=1;
		unit->captureneeds=(int)ZIN0(3);

		//unit->duration=(int)ZIN0(3);
		//duration= unit->duration;
		//unit->captureneeds=duration; //can't be less than 64
		unit->capturepos=0;
		unit->repeatpos=0;
	}

	int captureon,captureneeds,repeatpos,capturepos;

	captureon= unit->captureon;
	captureneeds= unit->captureneeds;
	capturepos= unit->capturepos;
	repeatpos= unit->repeatpos;

	for (int j=0; j<inNumSamples;++j)
	{

		if(captureon==1) {

			data[capturepos]=in[j];

			capturepos++;

			if(capturepos==captureneeds) {
				unit->capturepos=0;
				unit->captureon=0;
				captureon=0;
			}
		};


		ZXP(out)=data[repeatpos];

		repeatpos= (repeatpos+1)%captureneeds;

	}

	unit->capturepos= capturepos;
	unit->repeatpos= repeatpos;

	unit->m_prevtrig = curtrig;

}



void Max_Ctor( Max* unit ) {

	SETCALC(Max_next);

	int msamp= (int) ZIN0(1);
	//integer division
	unit->m_blocks= sc_max(msamp/(unit->mWorld->mFullRate.mBufLength),1);

	//printf("%d \n",unit->m_blocks);

	unit->m_blockmaxes= (float*)RTAlloc(unit->mWorld, unit->m_blocks * sizeof(float));
	//initialise to zeroes
	for(int i=0; i<unit->m_blocks; ++i)
		unit->m_blockmaxes[i]=0.f;

	unit->m_counter=0;
	unit->m_last=unit->m_blocks-1;
	unit->m_max=0.0;

	ZOUT0(0) = ZIN0(0);

}

void Max_Dtor(Max *unit) {
	RTFree(unit->mWorld, unit->m_blockmaxes);
}

void Max_next( Max *unit, int inNumSamples ) {

	int j;
	float *in = ZIN(0);
	//float *out = ZOUT(0);
	float max=0;

	//only to be used at .kr
	//printf("samp to calc %d", inNumSamples);

	int ksamps = unit->mWorld->mFullRate.mBufLength;

	//find max this block
	for(j=0; j<ksamps; ++j) {
		float next= fabs(ZXP(in));
		//printf("next %f\t",next);
		if(next>max) max=next;
	}

	//printf("max %f  unit->m_max  %f \t",max, unit->m_max);

	//now check max over last m_blocks

	if(max>(unit->m_max)) {

		unit->m_max=max;
		unit->m_last=unit->m_counter;
		unit->m_blockmaxes[unit->m_counter]=max;
	}
	else {

		unit->m_blockmaxes[unit->m_counter]=max;

		//if max being lost
		if(unit->m_counter==unit->m_last) {//calc new max position
			max=0;
			int last=0;

			for(j=0; j<unit->m_blocks; ++j){
				float test= unit->m_blockmaxes[j];

				if(test>max) {max=test; last=j;}
			}

			unit->m_max=max;
			unit->m_last=last;

		}

	}

	max=unit->m_max;

	unit->m_counter=(unit->m_counter+1)%(unit->m_blocks);

	//control rate, just one out value
	//output max
	//for(j=0; j<inNumSamples; ++j) {
	ZOUT0(0)=max;
	//}

}

void PrintVal_Ctor(PrintVal* unit) {
	SETCALC(PrintVal_next);

	unit->m_block= (int) ZIN0(1);

	unit->m_id= (int) ZIN0(2);

	unit->m_count=0;
}


void PrintVal_next(PrintVal *unit, int inNumSamples) {

	if(unit->m_count==0) {printf("%d  %f\n",unit->m_id,ZIN0(0));}

	unit->m_count=(unit->m_count+1)%(unit->m_block);
}


void EnvDetect_Ctor(EnvDetect* unit) {
	SETCALC(EnvDetect_next);

	//	unit->m_atk= (float) exp(-1/(SAMPLERATE*ZIN0(1)));
	//
	//	unit->m_rel= (float) exp(-1/(SAMPLERATE*ZIN0(2)));
	//
	//printf("atk %f rel %f\t",unit->m_atk,unit->m_rel);

	unit->m_env=0.0;
}


void EnvDetect_next(EnvDetect *unit, int inNumSamples) {

	float *in = ZIN(0);
	float *out = ZOUT(0);

	//int ksamps = unit->mWorld->mFullRate.mBufLength;

	float envelope= unit->m_env;
	//float ga= unit->m_atk;
	//	float gr= unit->m_rel;
	//

	float ga= (float) exp(-1/(SAMPLERATE*ZIN0(1)));
	float gr= (float) exp(-1/(SAMPLERATE*ZIN0(2)));

	//find max this block
	for(int j=0; j<inNumSamples; ++j) {
		float next= fabs(ZXP(in));
		//printf("next %f\t",next);

		if(envelope<next){
			envelope*=ga;
			envelope+= (1-ga)*next;
		}
		else {
			envelope*=gr;
			envelope+= (1-gr)*next;
		}

		ZXP(out)= envelope;

	}

	unit->m_env=envelope;

}




void FitzHughNagumo_Ctor(FitzHughNagumo* unit) {

	unit->u=0.0;
	unit->w=0.0;

	SETCALC(FitzHughNagumo_next_k);

}


void FitzHughNagumo_next_k(FitzHughNagumo *unit, int inNumSamples) {

	float *out = ZOUT(0);

	int reset= (int)ZIN0(0);
	float urate= (float)ZIN0(1);
	float wrate= (float)ZIN0(2);
	float b0= (float)ZIN0(3);
	float b1= (float)ZIN0(4);

	float u,w;

	u= unit->u;
	w=unit->w;

	if(reset) {

		u= (float)ZIN0(5);
		w= (float)ZIN0(6);
	}

	for (int j=0; j<inNumSamples;++j) {

		//the naive Euler update, could compare Runge-Kutta later
		float dudt= urate*(u-(0.33333*u*u*u)-w);
		float dwdt= wrate*(b0+b1*u-w);

		u+=dudt;
		w+=dwdt;

		//assumes fmod works correctly for negative values
		if ((u>1.0) || (u<-1.0)) u=fabs(fmod((u-1.0),4.0)-2.0)-1.0;
		//if ((posy>1.0) || (posy-1.0)) posy=fabs(fmod((posy-1.0),4.0)-2.0)-1.0;

		ZXP(out) = u;

		//printf("%f \n",(unit->posx/fabs(unit->posx))*0.5*(unit->posy*unit->posy+ unit->posx*unit->posx));
	}

	unit->u=u;
	unit->w=w;

}







void DoubleWell_Ctor(DoubleWell* unit) {

	unit->x=0.0;
	unit->y=0.0;
	unit->t=0.0;

	SETCALC(DoubleWell_next_k);

}


void DoubleWell_next_k(DoubleWell *unit, int inNumSamples) {

	float *out = ZOUT(0);

	int reset= (int)ZIN0(0);
	float xrate= (float)ZIN0(1);
	float yrate= (float)ZIN0(2);
	float F= (float)ZIN0(3);
	float w= (float)ZIN0(4);
	float delta= (float)ZIN0(5);

	float x,y,t;

	x= unit->x;
	y=unit->y;
	t=unit->t;

	if(reset) {

		x= (float)ZIN0(6);
		y= (float)ZIN0(7);
		t=0;
	}

	//Runge Kutta? would require four cos calls
	for (int j=0; j<inNumSamples;++j) {

		//the naive Euler update, could compare Runge-Kutta later
		float dxdt= xrate*y;

		//could make Fcos term another ar or kr input itself?
		//float dydt= yrate*(-delta*y+ x - x*x*x + F*cos(w*t));

		//Runge-Kutta
		float temp,z,k1,k2,k3,k4;
		temp= x - x*x*x + F*cos(w*t);
		z= y;
		k1= yrate*(-delta*z+ temp);
		z=y+0.5*k1;
		k2= yrate*(-delta*z+ temp);
		z=y+0.5*k2;
		k3= yrate*(-delta*z+ temp);
		z=y+k3;
		k4= yrate*(-delta*z+ temp);

		float dydt= 0.166667*(k1+2*k2+2*k3+k4);

		t=t+1;
		x+=dxdt;
		y+=dydt;


		//assumes fmod works correctly for negative values- which it doesn't- this is erroneous code, but kept for backwards compatability and
		//because it can make some curious sounds!
		if ((x>1.0) || (x<-1.0)) x=fabs(fmod((x-1.0),4.0)-2.0)-1.0;
		//		//if ((posy>1.0) || (posy-1.0)) posy=fabs(fmod((posy-1.0),4.0)-2.0)-1.0;
		//
		ZXP(out) = x;
		//

		//printf("%f \n",(unit->posx/fabs(unit->posx))*0.5*(unit->posy*unit->posy+ unit->posx*unit->posx));
	}

	unit->x=x;
	unit->y=y;
	unit->t=t;
}






void DoubleWell2_Ctor(DoubleWell2* unit) {

	unit->x=0.0;
	unit->y=0.0;
	unit->t=0.0;

	SETCALC(DoubleWell2_next_k);

}


void DoubleWell2_next_k(DoubleWell2 *unit, int inNumSamples) {

	float *out = ZOUT(0);

	int reset= (int)ZIN0(0);
	float xrate= (float)ZIN0(1);
	float yrate= (float)ZIN0(2);
	float F= (float)ZIN0(3);
	float w= (float)ZIN0(4);
	float delta= (float)ZIN0(5);

	float x,y,t;

	x= unit->x;
	y=unit->y;
	t=unit->t;

	if(reset) {

		//printf("reset %d \n",reset);

		x= (float)ZIN0(6);
		y= (float)ZIN0(7);
		t=0;
	}

	for (int j=0; j<inNumSamples;++j) {

		//improved Euler update using trial (information not just from left derivative)

		//could make Fcos term another ar or kr input itself?
		//float dydt= yrate*(-delta*y+ x - x*x*x + F*cos(w*t));

		//Improved Euler Method
		float trialx, dy, dytrial, dydt, dxdt, lasty;

		trialx = x+(y*xrate);

		dy= F*cos(w*(t*yrate)) + (x) - (x*x*x) - delta*y;

		dytrial= F*cos(w*(t*yrate)) + (trialx) - (trialx*trialx*trialx) - delta*y;

		dydt= (dy+dytrial)*0.5*yrate;

		lasty=y;

		y+=dydt;

		dxdt=((y+lasty)*0.5)*xrate;

		t=t+1;
		x+=dxdt;

		//previous equation was wrong
		if ((x>3.0) || (x<-3.0)) {

			//printf("folding, x %f calc %f \n",x, sc_fold(x, -3.0f, 3.0f));
			x=sc_fold(x, -3.0f, 3.0f);

		}

		//printf("checkx %f\n",x);
		ZXP(out) = 0.33*x;

		//scaling to keep it within bounds
		//if ((x>3.0) || (x<-3.0)) x=fabs(fmod((x-3.0),12.0)-6.0)-3.0;

		//ZXP(out) = 0.33*x;

		//printf("%f \n",(unit->posx/fabs(unit->posx))*0.5*(unit->posy*unit->posy+ unit->posx*unit->posx));
	}

	unit->x=x;
	unit->y=y;
	unit->t=t;
}





void DoubleWell3_Ctor(DoubleWell3* unit) {

	unit->x=0.0;
	unit->y=0.0;
	unit->t=0.0;

	SETCALC(DoubleWell3_next_k);

}


void DoubleWell3_next_k(DoubleWell3 *unit, int inNumSamples) {

	float *out = ZOUT(0);

	int reset= (int)ZIN0(0);
	float rate= (float)ZIN0(1);
	float *forcing= ZIN(2); //audio rate forcing input
	float delta= (float)ZIN0(3);


	float x,y,t;

	x= unit->x;
	y=unit->y;
	t=unit->t;

	if(reset) {

		x= (float)ZIN0(4);
		y= (float)ZIN0(5);
		t=0;
	}

	//Runge Kutta? would require four cos calls
	for (int j=0; j<inNumSamples;++j) {

		//the naive Euler update, could compare Runge-Kutta later
		float dxdt= rate*y;

		//could make Fcos term another ar or kr input itself?
		//float dydt= yrate*(-delta*y+ x - x*x*x + F*cos(w*t));

		//Runge-Kutta
		float temp,z,k1,k2,k3,k4;
		temp= x - x*x*x + ZXP(forcing);
		z= y;
		k1= rate*(-delta*z+ temp);
		z=y+0.5*k1;
		k2= rate*(-delta*z+ temp);
		z=y+0.5*k2;
		k3= rate*(-delta*z+ temp);
		z=y+k3;
		k4= rate*(-delta*z+ temp);

		float dydt= 0.166667*(k1+2*k2+2*k3+k4);

		t=t+1;
		x+=dxdt;
		y+=dydt;

		if ((x>3.0) || (x<-3.0)) x=sc_fold(x, -3.0f, 3.0f);
		ZXP(out) = 0.33*x;

	}

	unit->x=x;
	unit->y=y;
	unit->t=t;
}





void WeaklyNonlinear_Ctor(WeaklyNonlinear* unit) {

	unit->x=0.0;
	unit->y=0.0;
	unit->t=0.0;

	SETCALC(WeaklyNonlinear_next_k);

}


void WeaklyNonlinear_next_k(WeaklyNonlinear *unit, int inNumSamples) {

	float *out = ZOUT(0);

	float *in = ZIN(0);

	int reset= (int)ZIN0(1);
	float xrate= (float)ZIN0(2);
	float yrate= (float)ZIN0(3);
	float w0= (float)ZIN0(4);
	w0=w0*twopi/(SAMPLERATE); //convert frequency in Hertz to angular frequency
	w0=w0*w0; //constant needed for equation
			  //float eta= (float)ZIN0(5);

	float x,y,t;

	x= unit->x;
	y=unit->y;
	t=unit->t;

	if(reset) {

		x= (float)ZIN0(5);
		y= (float)ZIN0(6);
		t=0;
	}

	//alpha*(x^xexponent*-beta)*y^yexponent
	float alpha=(float)ZIN0(7);
	float xexponent=(float)ZIN0(8);
	float beta=(float)ZIN0(9);
	float yexponent=(float)ZIN0(10);

	//Runge Kutta? would require four cos calls
	for (int j=0; j<inNumSamples;++j) {

		//the naive Euler update
		float dxdt= xrate*y;

		float nonlinear= 0.0;

		if(alpha>0.000001 || alpha<(-0.000001)) {
			nonlinear= alpha * (pow(x,xexponent)+beta)*(pow(y,yexponent));
		}

		float dydt= yrate*(ZXP(in) - w0*x + nonlinear);

		//t=t+1;
		x+=dxdt;
		y+=dydt;

		//assumes fmod works correctly for negative values
		if ((x>1.0) || (x<-1.0)) x=fabs(fmod((x-1.0),4.0)-2.0)-1.0;
		//if ((posy>1.0) || (posy-1.0)) posy=fabs(fmod((posy-1.0),4.0)-2.0)-1.0;

		ZXP(out) = x;

		//printf("%f \n",(unit->posx/fabs(unit->posx))*0.5*(unit->posy*unit->posy+ unit->posx*unit->posx));
	}

	unit->x=x;
	unit->y=y;
	unit->t=t;

}




void WeaklyNonlinear2_Ctor(WeaklyNonlinear2* unit) {

	unit->x=0.0;
	unit->y=0.0;
	unit->t=0.0;

	SETCALC(WeaklyNonlinear2_next_k);

}

void WeaklyNonlinear2_next_k(WeaklyNonlinear2 *unit, int inNumSamples) {

	float *out = ZOUT(0);

	float *in = ZIN(0);

	int reset= (int)ZIN0(1);
	float xrate= (float)ZIN0(2);
	float yrate= (float)ZIN0(3);
	float w0= (float)ZIN0(4);
	w0=w0*twopi/(SAMPLERATE); //convert frequency in Hertz to angular frequency
	w0=w0*w0; //constant needed for equation
			  //float eta= (float)ZIN0(5);

	float x,y,t;

	x= unit->x;
	y=unit->y;
	t=unit->t;

	if(reset) {

		x= (float)ZIN0(5);
		y= (float)ZIN0(6);
		t=0;
	}

	//alpha*(x^xexponent*-beta)*y^yexponent
	float alpha=(float)ZIN0(7);
	float xexponent=(float)ZIN0(8);
	float beta=(float)ZIN0(9);
	float yexponent=(float)ZIN0(10);

	//Runge Kutta? would require four cos calls
	for (int j=0; j<inNumSamples;++j) {

		//the naive Euler update
		float dxdt= xrate*y;

		float nonlinear= 0.0;

		if(alpha>0.000001 || alpha<(-0.000001)) {
			nonlinear= alpha * (pow(x,xexponent)+beta)*(pow(y,yexponent));
		}

		float dydt= yrate*(ZXP(in) - w0*x + nonlinear);

		//t=t+1;
		x+=dxdt;
		y+=dydt;

		//assumes fmod works correctly for negative values
		//if ((x>1.0) || (x<-1.0)) x=fabs(fmod((x-1.0),4.0)-2.0)-1.0;
		//if ((posy>1.0) || (posy-1.0)) posy=fabs(fmod((posy-1.0),4.0)-2.0)-1.0;

		if ((x>1.0) || (x<-1.0)) x=sc_fold(x, -1.0f, 1.0f);
		ZXP(out) = x;


		//printf("%f \n",(unit->posx/fabs(unit->posx))*0.5*(unit->posy*unit->posy+ unit->posx*unit->posx));
	}

	unit->x=x;
	unit->y=y;
	unit->t=t;

}





void TermanWang_Ctor(TermanWang* unit) {

	unit->x=0.0;
	unit->y=0.0;

	SETCALC(TermanWang_next_k);

}

void TermanWang_next_k(TermanWang *unit, int inNumSamples) {

	float *out = ZOUT(0);

	float *in = ZIN(0);

	int reset= (int)ZIN0(1);
	float xrate= (float)ZIN0(2);
	float yrate= (float)ZIN0(3);
	float alpha= (float)ZIN0(4);
	float beta= (float)ZIN0(5);
	float eta= (float)ZIN0(6);

	float x,y;

	x= unit->x;
	y=unit->y;

	if(reset) {
		x= (float)ZIN0(7);
		y= (float)ZIN0(8);
	}

	for (int j=0; j<inNumSamples;++j) {

		//the naive Euler update
		float dxdt= xrate* ( (3.0*x) + (x*x*x)  -y + (ZXP(in)));  //+ 2 left out since can be returned via input

		float dydt= yrate*(eta * ((alpha*(1.0+(tanh(x*beta)))) -y));

		x+=dxdt;
		y+=dydt;

		if ((x>1.0) || (x<-1.0)) x=sc_fold(x, -1.0f, 1.0f);

		//control on y too?

		ZXP(out) = x;

		//printf("%f \n",(unit->posx/fabs(unit->posx))*0.5*(unit->posy*unit->posy+ unit->posx*unit->posx));
	}

	unit->x=x;
	unit->y=y;

}



void LTI_Ctor( LTI* unit ) {

	//World *world = unit->mWorld;

	uint32 bufnum = (uint32)ZIN0(1);

	//if (bufnum >= world->mNumSndBufs) bufnum = 0;
	//SndBuf *buf = world->mSndBufs + bufnum;

	SndBuf * buf= SLUGensGetBuffer(unit,bufnum);

	if (buf) {

		unit->sizea = buf->samples;

		unit->bufa = buf->data;

		bufnum = (uint32)ZIN0(2);

		buf= SLUGensGetBuffer(unit,bufnum);

		if (buf) {

			//if (bufnum >= world->mNumSndBufs) bufnum = 0;
			//buf = world->mSndBufs + bufnum;

			unit->sizeb = buf->samples;

			unit->bufb = buf->data;

			unit->mema= (float*)RTAlloc(unit->mWorld, unit->sizea * sizeof(float));

			//initialise to zeroes
			for(int i=0; i<unit->sizea; ++i)
				unit->mema[i]=0.f;

			unit->posa=0;

			unit->memb= (float*)RTAlloc(unit->mWorld, unit->sizeb * sizeof(float));

			//initialise to zeroes
			for(int i=0; i<unit->sizeb; ++i)
				unit->memb[i]=0.f;

			unit->posb=0;

			SETCALC(LTI_next_a);

		}
	}

}

void LTI_Dtor(LTI *unit) {
	RTFree(unit->mWorld, unit->mema);
	RTFree(unit->mWorld, unit->memb);
}

//bufnum, 0 capturein 1, capturetrigger 2, duration of repeat 3, ampdropout 4
void  LTI_next_a(LTI *unit, int inNumSamples ) {
	int i,j, pos;

	float total;

	float *out = ZOUT(0);
	float *in = ZIN(0);

	float * bufa= unit->bufa, *bufb= unit->bufb, *mema= unit->mema,*memb= unit->memb;
	int sizea=unit->sizea, posa= unit->posa;
	int sizeb= unit->sizeb, posb= unit->posb;

	for (j=0; j<inNumSamples;++j) {
		total=0.0;

		//sum last x inputs
		memb[posb]= ZXP(in);

		for (i=0; i<sizeb; ++i) {
			pos= (posb+sizeb-i)%sizeb;
			total+= memb[pos]*bufb[i];
		}

		//update x memory
		posb=(posb+1)%sizeb;

		//sum last y outputs

		for (i=0; i<sizea; ++i) {
			pos= (posa+sizea-i)%sizea;

			total+= mema[pos]*bufa[i];
		}

		//update y memory
		posa=(posa+1)%sizea;
		mema[posa]= total;

		//output total
		ZXP(out) = total;

		//printf("%f \n",);
	}

	unit->posa=posa;
	unit->posb=posb;

}









void NL_Ctor( NL* unit ) {

	int i;
	//int maxindex;

	//World *world = unit->mWorld;

	uint32 bufnum = (uint32)ZIN0(1);

	SndBuf * buf= SLUGensGetBuffer(unit,bufnum);

	if (buf) {

		//if (bufnum >= world->mNumSndBufs) bufnum = 0;
		//SndBuf *buf = world->mSndBufs + bufnum;

		if (buf->samples%3!=0) printf("feedback data input format wrong, not multiple of 3 size\n");

		unit->numasummands = buf->samples/3;

		unit->aindices= (int*)RTAlloc(unit->mWorld, unit->numasummands * sizeof(int));

		for(i=0; i<unit->numasummands; ++i)
			unit->aindices[i] = (int)(buf->data[3*i]+0.01); //rounding carefully

		unit->sizea = unit->aindices[unit->numasummands-1] + 1; //last index must be maximal

		unit->bufa = buf->data;

		bufnum = (uint32)ZIN0(2);

		buf= SLUGensGetBuffer(unit,bufnum);

		if (buf) {

			//if (bufnum >= world->mNumSndBufs) bufnum = 0;
			//buf = world->mSndBufs + bufnum;

			if (buf->samples%3!=0) printf("feedforward data input format wrong, not multiple of 3 size\n");

			unit->numbsummands = buf->samples/3;

			unit->bindices= (int*)RTAlloc(unit->mWorld, unit->numbsummands * sizeof(int));

			for(i=0; i<unit->numbsummands; ++i)
				unit->bindices[i] = (int)(buf->data[3*i]+0.01); //rounding carefully

			unit->sizeb = unit->bindices[unit->numbsummands-1] + 1; //last index must be maximal

			unit->bufb = buf->data;

			//printf("numa %d sizea %d numb %d sizeb %d \n", unit->numasummands, unit->sizea, unit->numbsummands, unit->sizeb);

			unit->mema= (float*)RTAlloc(unit->mWorld, unit->sizea * sizeof(float));

			//initialise to zeroes
			for(i=0; i<unit->sizea; ++i)
				unit->mema[i]=0.f;

			unit->posa=0;

			unit->memb= (float*)RTAlloc(unit->mWorld, unit->sizeb * sizeof(float));

			//initialise to zeroes
			for(i=0; i<unit->sizeb; ++i)
				unit->memb[i]=0.f;

			unit->posb=0;

			SETCALC(NL_next_a);

		}

	}

}

void NL_Dtor(NL *unit) {
	RTFree(unit->mWorld, unit->mema);
	RTFree(unit->mWorld, unit->memb);
	RTFree(unit->mWorld, unit->aindices);
	RTFree(unit->mWorld, unit->bindices);
}


void  NL_next_a(NL *unit, int inNumSamples ) {
	int i,j, pos;

	float total;
	int index;
	float exponent, coefficient;
	float val;

	float *out = ZOUT(0);
	float *in = ZIN(0);

	float guard1 = ZIN0(3);
	float guard2 = ZIN0(4);

	float * bufa= unit->bufa, *bufb= unit->bufb, *mema= unit->mema,*memb= unit->memb;
	int * aindices = unit->aindices, *bindices = unit-> bindices;
	int numasummands= unit->numasummands, numbsummands= unit->numbsummands;
	int sizea=unit->sizea, posa= unit->posa;
	int sizeb= unit->sizeb, posb= unit->posb;

	for (j=0; j<inNumSamples;++j) {
		total=0.0;

		//sum last x inputs
		memb[posb]= ZXP(in);

		//CAN'T TAKE POWER OF NEGATIVE NUMBER IF EXPONENT FRACTIONAL!
		for (i=0; i<numbsummands; ++i) {

			index= 3*i;
			exponent= bufb[index+2];
			coefficient= bufb[index+1];
			index= bindices[i];

			pos= (posb+sizeb-index)%sizeb;

			val= memb[pos];

			if ((val < 0.0))
				total+= (pow(fabs(val),exponent))*(-1.0*coefficient);
			else
				total+= (pow(val,exponent))*coefficient;

			//printf("index %d totalb %f change %f %f %f \n", index, total,val,exponent,coefficient);


		}

		//update x memory
		posb=(posb+1)%sizeb;

		//sum last y outputs
		//printf("post b total! g1 %f %f %f g2 %f %f \n", total, fabs(total), guard1, fabs(total- mema[posa]), guard2);


		for (i=0; i<numasummands; ++i) {

			index= 3*i;
			exponent= bufa[index+2];
			coefficient= bufa[index+1];
			index= aindices[i];

			pos= (posa+sizea-index)%sizea;

			val= mema[pos];

			if ((val < 0.0))
				total+= (pow(fabs(val),exponent))*(-1.0*coefficient);
			else
				total+= (pow(val,exponent))*coefficient;


			//printf("index %d totala %f change %f %f %f \n", index, total,val,exponent,coefficient);

		}

		//printf("post a total! g1 %f %f %f g2 %f %f \n", total, fabs(total), guard1, fabs(total- mema[posa]), guard2);

		//check for blow-ups!
		if ((fabs(total)>guard1) || (fabs(total- mema[posa])>guard2)) {

			//printf("blowup! g1 %f %f %f g2 %f %f \n", total, fabs(total), guard1, fabs(total- mema[posa]), guard2);
			for (i=0; i<sizea; ++i) {
				mema[i]=0.0;
			}
			total=0.0;
		}


		//update y memory
		posa=(posa+1)%sizea;
		mema[posa]= total;

		//output total
		ZXP(out) = total;

		//printf("%f \n",);
	}

	unit->posa=posa;
	unit->posb=posb;

}








void NL2_Ctor( NL2* unit ) {

	int i;
	//int maxindex;


	unit->sizea = (int)(ZIN0(2)+0.01); //last index must be maximal

	unit->sizeb = (int)(ZIN0(3)+0.01); //last index must be maximal

	//printf("numa %d sizea %d numb %d sizeb %d \n", unit->numasummands, unit->sizea, unit->numbsummands, unit->sizeb);

	unit->mema= (float*)RTAlloc(unit->mWorld, unit->sizea * sizeof(float));

	//initialise to zeroes
	for(i=0; i<unit->sizea; ++i)
		unit->mema[i]=0.f;

	unit->posa=0;

	unit->memb= (float*)RTAlloc(unit->mWorld, unit->sizeb * sizeof(float));

	//initialise to zeroes
	for(i=0; i<unit->sizeb; ++i)
		unit->memb[i]=0.f;

	unit->posb=0;

	SETCALC(NL2_next_a);

}

void NL2_Dtor(NL2 *unit) {
	RTFree(unit->mWorld, unit->mema);
	RTFree(unit->mWorld, unit->memb);
}


void  NL2_next_a(NL2 *unit, int inNumSamples ) {
	int i,j,k, pos;

	float total;
	int index;
	float exponent; //, coefficient;
	float val;

	float *out = ZOUT(0);
	float *in = ZIN(0);

	float guard1 = ZIN0(4);
	float guard2 = ZIN0(5);

	//Dynamic buffer checks
	//World *world = unit->mWorld;

	uint32 bufnum = (uint32)ZIN0(1);

	SndBuf * sndbuf= SLUGensGetBuffer(unit,bufnum);

	if (sndbuf) {

		//if (bufnum >= world->mNumSndBufs) bufnum = 0;
		//SndBuf *sndbuf = world->mSndBufs + bufnum;

		float *buf = sndbuf->data;
		//int bufsize= sndbuf->samples;
		int bufdone = 0;
		int numcrossterms;

		float *mema= unit->mema,*memb= unit->memb;

		int sizea=unit->sizea, posa= unit->posa;
		int sizeb= unit->sizeb, posb= unit->posb;

		for (j=0; j<inNumSamples;++j) {
			total=0.0;

			//sum last x inputs
			memb[posb]= ZXP(in);

			//bool check= true;

			numcrossterms = (int)buf[0];

			bufdone= 1;

			for (k= 0; k<numcrossterms; ++k) {

				float product = buf[bufdone];

				int numbinproduct = (int)buf[bufdone+1];

				bufdone +=2;

				for (i=0; i<numbinproduct; ++i) {

					index= (int)buf[bufdone];
					//coefficient= buf[bufdone+1];
					exponent= buf[bufdone+1];

					pos= (posb+sizeb-index)%sizeb;

					val= memb[pos];

					if ((val < 0.0))
						product*= (pow(fabs(val),exponent))*(-1.0); //(-1.0*coefficient);
					else
						product*= (pow(val,exponent)); //*coefficient;

					bufdone += 2;

				}

				int numainproduct = (int)buf[bufdone];

				bufdone +=1;

				for (i=0; i<numainproduct; ++i) {

					index= (int)buf[bufdone];
					//coefficient= buf[bufdone+1];
					exponent= buf[bufdone+1];

					pos= (posa+sizea-index)%sizea;

					val= mema[pos];

					if ((val < 0.0))
						product*= (pow(fabs(val),exponent))*(-1.0); //(-1.0*coefficient);
					else
						product*= (pow(val,exponent)); //*coefficient;

					bufdone += 2;

				}

				total += product;

			};



			//update x memory
			posb=(posb+1)%sizeb;

			//sum last y outputs
			//printf("post b total! g1 %f %f %f g2 %f %f \n", total, fabs(total), guard1, fabs(total- mema[posa]), guard2);

			//check for blow-ups!
			if ((fabs(total)>guard1) || (fabs(total- mema[posa])>guard2)) {

				//printf("blowup! g1 %f %f %f g2 %f %f \n", total, fabs(total), guard1, fabs(total- mema[posa]), guard2);
				for (i=0; i<sizea; ++i) {
					mema[i]=0.0;
				}
				total=0.0;
			}

			//update y memory
			posa=(posa+1)%sizea;
			mema[posa]= total;

			//output total
			ZXP(out) = total;

			//printf("%f \n",);
		}

		unit->posa=posa;
		unit->posb=posb;

	}
}




void LPCError_Ctor(LPCError* unit) {

	//unit->x0= 0;
	unit->p=1;

	for (int i=0; i<64;++i) {
		unit->coeff[i]=0.0;
		unit->last[i]=0.0;
	}

	printf("SETUP LPCError \n");

	SETCALC(LPCError_next_a);

}


//efficiency, place as globals the storage arrays for calculations
void LPCError_next_a(LPCError *unit, int inNumSamples) {

	int i,j;
	float sum; //, previous[20];
	int prevcount, pos;

	float *out = OUT(0);

	float *in = IN(0);

	//	for (j=0;j<64;++j)
	//	printf("%f ", in[j]);
	//
	//	printf("\n");
	//
	//	for (j=0;j<64;++j)
	//	printf("%f ", out[j]);
	//
	//	printf("\n");

	prevcount=0;
	//
	//	for(i=0; i<20; ++i) {
	//		previous[i]=0.0;
	//	}
	//
	//	previous[0]= unit->x0;
	//
	float * coeff=unit->coeff;
	float * last=unit->last;

	int p= unit->p;

	//printf("first p? %d",p);

	for (j=0; j<p;++j) {
		out[j]= last[j];
	}

	prevcount=p-1;

	//output first, using coeffs from last time, which is why this is a Non Alias Unit
	for (j=p; j<inNumSamples;++j) {

		sum=0.0;

		for(i=0; i<p; ++i) {
			pos= (prevcount+p-i)%p;

			//where is pos used?
			sum += last[pos]*coeff[i];
		}

		prevcount=(prevcount+1)%p;

		last[prevcount]=(-sum);

		//ZXP(out)=
		out[j]= (-sum);
	}

	//unit->x0= in[1];



	p= (int)ZIN0(1); //user selected p value from 1 to 64

	//safety
	if(p<1) p=1;
	if(p>64) p=64;

	//printf("p? %d",p);

	unit->p=p;

	//ready for next time
	for (j=0; j<p;++j) {
		last[j]= in[j];
	}


	//calculate new LPC filter coefficients following (Makhoul 1975) autocorrelation, deterministic signal, Durbin iterative matrix solver

	//float R[21];//autocorrelation coeffs;
	//float preva[21];
	//float a[21];
	float E, k;

	float * R= unit->R;
	float * preva=unit->preva;
	float * a= unit->a;

	for(i=0; i<=p; ++i) {
		sum=0.0;

		for (j=0; j<= 63-i; ++j)
			sum+= in[j]*in[j+i];

		R[i]=sum;
	}

	E= R[0];
	k=0;

	for(i=0; i<=(p+1); ++i) {
		a[i]=0.0;
		preva[i]=0.0; //CORRECTION preva[j]=0.0;
	}

	if(E<0.0000001) {

		//zero power, so zero all coeff

		for (i=0; i<p;++i)
			unit->coeff[i]=0.0;

		//printf("early return %f\n", E);
		return;

	};

	for(i=1; i<=p; ++i) {

		sum=0.0;

		for(j=1;j<i;++j)
			sum+= a[j]*R[i-j];

		k=(-1.0*(R[i]+sum))/E;

		a[i]=k;

		for(j=1;j<=(i-1);++j)
			a[j]=preva[j]+(k*preva[i-j]);

		for(j=1;j<=i;++j)
			preva[j]=a[j];

		E= (1-k*k)*E;

	}


	//solution is the final set of a
	for(i=0; i<p; ++i) {
		coeff[p-1-i]=a[i+1];

		//printf("a %f R %f",a[i+1], R[i]);
	}

	//	printf("\n");
	//
	//	for (j=0;j<64;++j)
	//	printf("%f ", in[j]);
	//
	//	printf("\n");
	//
	//	for (j=0;j<64;++j)
	//	printf("%f ", out[j]);
	//
	//	printf("\n\n");


}


void KmeansToBPSet1_Ctor(KmeansToBPSet1* unit) {

	unit->mFreqMul = unit->mRate->mSampleDur;
	unit->mPhase = 1.f;	//should immediately decide on new target
	unit->mSpeed = 100*unit->mFreqMul;

	unit->numdatapoints= (int) ZIN0(1);
	unit->maxmeans= (int) ZIN0(2);

	//World *world = unit->mWorld;

	//get buffer containing data
	uint32 bufnum = (uint32)(ZIN0(7)+0.001);

	unit->m_initdata=NULL;

	SndBuf * buf= SLUGensGetBuffer(unit,bufnum);

	if (buf) {

		//if (!(bufnum > world->mNumSndBufs || bufnum<0)) {
		//	SndBuf *buf = world->mSndBufs + bufnum;

		if(buf->samples == ((2*unit->numdatapoints) + (2*unit->maxmeans))) {
			//must be of the form 2*numdatapoints then 2*maxmeans
			unit->m_initdata=buf->data;
			unit->m_meansindex= 2*unit->numdatapoints;
		}
		} else
			//don't cancel performance of UGen, but pointer to buf was NULL
		{unit->mDone = false; }

	unit->data= (float*)RTAlloc(unit->mWorld, 2*unit->numdatapoints * sizeof(float));
	unit->means= (float*)RTAlloc(unit->mWorld, 2*unit->maxmeans * sizeof(float));
	unit->newmeans= (float*)RTAlloc(unit->mWorld, 2*unit->maxmeans * sizeof(float));
	unit->newmeancount= (int*)RTAlloc(unit->mWorld, unit->maxmeans * sizeof(int));

	//two extra for guard elements, (0,0) and (1,0)
	unit->bpx= (float*)RTAlloc(unit->mWorld, (unit->maxmeans+2) * sizeof(float));
	unit->bpy= (float*)RTAlloc(unit->mWorld, (unit->maxmeans+2) * sizeof(float));

	//initialise random data
	RGen& rgen = *unit->mParent->mRGen;
	int i=0;



	if(unit->m_initdata) {

		for(i=0; i<(2*unit->numdatapoints);++i) {
			unit->data[i]=unit->m_initdata[i];
		}

		for(i=0; i<(2*unit->maxmeans);++i) {
			unit->means[i]=unit->m_initdata[unit->m_meansindex+i];
		}

				} else {

					for(i=0; i<(2*unit->numdatapoints);++i) {
						unit->data[i]=rgen.frand();//x
												   //unit->data[2*i+1]=rgen.frand(); //y 2*rgen.frand() - 1.0; easier to keep these calcs in 0.0-1.0 range
					}

					for(i=0; i<(2*unit->maxmeans);++i) {
						unit->means[i]=rgen.frand();
					}

				}


	for(i=0; i<(unit->maxmeans);++i) {
		//unit->means[2*i]=rgen.frand();
		//unit->means[2*i+1]=rgen.frand();
		unit->newmeans[2*i]=0.0;
		unit->newmeans[2*i+1]=0.0;
		unit->newmeancount[i]=0;
	}

	unit->nummeans=1;
	unit->numbps=2;

	unit->bpx[0]=0.0; unit->bpy[0]=0.0;
	unit->bpx[1]=1.0; unit->bpy[1]=0.0;

	//MakeBPSet(unit);

	unit->newmeanflag=0;
	unit->newdataflag=0;

	SETCALC(KmeansToBPSet1_next_a);
	}

void KmeansToBPSet1_Dtor(KmeansToBPSet1* unit) {

	RTFree(unit->mWorld, unit->data);
	RTFree(unit->mWorld, unit->means);
	RTFree(unit->mWorld, unit->newmeans);
	RTFree(unit->mWorld, unit->newmeancount);
	RTFree(unit->mWorld, unit->bpx);
	RTFree(unit->mWorld, unit->bpy);

}


//convert current means into a breakpoint set
void MakeBPSet(KmeansToBPSet1 *unit) {

	int i;

	float * means=unit->means;
	float * bpx=unit->bpx;
	float * bpy=unit->bpy;
	int nummeans=unit->nummeans;

	//make final BPs (sort)

	//sort by x, count as you go

	int numsorted=1;
	float last, minmatch, match, xnow;
	int lastindex;

	//first guard
	bpx[0]=0.0; bpy[0]=0.0;

	last=0.0;

	lastindex=0;

	while (lastindex>=0) {

		lastindex=-1;
		minmatch=1.0;

		for(i=0; i<(nummeans);++i) {

			xnow=means[2*i];

			if(xnow>last) {

				match=xnow-last;

				if(match<minmatch) {minmatch=match; lastindex=i;}

			}

		}

		if(lastindex>=0) {

			bpx[numsorted]=means[2*lastindex];
			bpy[numsorted]=2*means[2*lastindex+1]-1; //2x-1 to convert [0,1] to [-1,1]
			last=bpx[numsorted];
			numsorted+=1;
		}

	}


	bpx[numsorted]=1.0; bpy[numsorted]=0.0;
	numsorted+=1;

	unit->numbps= numsorted;

}

void KmeansToBPSet1_next_a(KmeansToBPSet1 *unit, int inNumSamples) {

	int i,j,k;

	float *out = ZOUT(0);

	int newnummeans= (int) ZIN0(3);
	if (newnummeans> unit->maxmeans) newnummeans= unit->maxmeans;
	if (newnummeans<1) newnummeans= 1;

	int nummeans=unit->nummeans;

	int newmeanflag=unit->newmeanflag;
	int newdataflag=unit->newdataflag;

	if((int)ZIN0(5)) newmeanflag=1;
	if((int)ZIN0(4)) newdataflag=1;

	//phase gives proportion for linear interpolation automatically
	double phase = unit->mPhase;

	float speed= unit->mSpeed;

	RGen& rgen = *unit->mParent->mRGen;
	//linear distribution 0.0 to 1.0 using rgen.frand()

	float * data=unit->data;
	float * means=unit->means;
	float * newmeans=unit->newmeans;
	int * newmeancount=unit->newmeancount;
	float * bpx=unit->bpx;
	float * bpy=unit->bpy;
	int numbps= unit->numbps;

	int numdata= unit->numdatapoints;

	float soft = ZIN0(6);

	//have to be careful to avoid making invalid breakpoint sets.
	if(soft<0.0) soft=0.0;
	if(soft>1.0) soft=1.0;

	float soft2= 1.0-soft;

	float freq = ZIN0(0);

	//phase duration of a sample
	float minphase=unit->mFreqMul;

	speed= freq*minphase;

	for (k=0; k<inNumSamples;++k) {
		float z;

		if (phase >= 1.f) {
			phase -= 1.f;

			if (newdataflag) {

				if(unit->m_initdata) {

					for(i=0; i<(2*numdata);++i) {
						unit->data[i]=unit->m_initdata[i];
					}

				} else

					for(i=0; i<(2*numdata);++i) {
						unit->data[i]=rgen.frand();
					}

						newdataflag=0;
			}

			if (newmeanflag || (newnummeans != nummeans)) {

				nummeans= newnummeans;
				unit->nummeans=nummeans;

				if(unit->m_initdata) {

					for(i=0; i<(2*nummeans);++i) {
						means[i]=unit->m_initdata[unit->m_meansindex+i];
					}

				} else {

					for(i=0; i<(2*nummeans);++i) {
						means[i]=rgen.frand();
						//means[2*i]=rgen.frand();
						//means[2*i+1]=rgen.frand();
					}

				}

				newmeanflag=0;

				//make final BPs
				MakeBPSet(unit);
				numbps= unit->numbps;

			}

			else {  //update means, else stick with new ones first time

				for(i=0; i<(nummeans);++i) {
					newmeans[2*i]=0.0;
					newmeans[2*i+1]=0.0;
					newmeancount[i]=0;
				}

				//do calculations

				int closest=0;
				float lowscore=1000.0;
				float x,y, dist, tmp;

				for(i=0; i<numdata;++i) {

					x=data[2*i];
					y=data[2*i+1];

					closest=0;
					lowscore=1000.0;

					//find closest
					for(j=0; j<nummeans;++j) {

						tmp= x-means[2*j];
						dist= y-means[2*j+1];

						dist=tmp*tmp+dist*dist;

						if(dist<lowscore) {
							closest=j;
							lowscore= dist;
						}

					}

					newmeans[2*closest]+= x;
					newmeans[2*closest+1]+= y;

					newmeancount[closest]+=1;
				}

				for(i=0; i<(nummeans);++i) {

					if(newmeancount[i]) {
						tmp= 1.0/((float)newmeancount[i]);

						means[2*i]=(soft2*means[2*i])+(soft*(newmeans[2*i])*tmp);
						means[2*i+1]=(soft2*means[2*i+1])+(soft*(newmeans[2*i+1])*tmp);
					}

				}

				MakeBPSet(unit);
				numbps= unit->numbps;
			}



		}

		//use BPs, find bounding BPs from phase position, linear interpolation

		int ind= 0;

		float dist;
		float mindist=1.0;

		for(i=1; i<(numbps);++i) {

			if(bpx[i]<=phase) {
				dist= phase-bpx[i];

				if(dist<mindist) {
					mindist=dist; ind=i;
				}

			}

		}

		if(ind==(numbps-1)) ind= numbps-2;

		float prop= (phase- bpx[ind])/(bpx[ind+1]-bpx[ind]);

		//search bpx for enclosing elements

		//printf("ind %d bpy1 %f bpy2 %f prop %f\n",ind,bpy[ind],bpy[ind+1],prop);

		z = ((1.0-prop)*bpy[ind])+(prop*bpy[ind+1]);

		phase +=  speed;
		ZXP(out) = z;
	}

	unit->mPhase = phase;
	unit->mSpeed = speed;

	unit->newmeanflag=newmeanflag;
	unit->newdataflag=newdataflag;
	unit->nummeans=nummeans;




}



void Instruction_Ctor(Instruction* unit) {

	//World *world = unit->mWorld;

	uint32 bufnum = (uint32)ZIN0(0);

	SndBuf * buf= SLUGensGetBuffer(unit,bufnum);

	if (buf) {

		//if (bufnum >= world->mNumSndBufs) bufnum = 0;
		unit->mBufNum=bufnum;

		//SndBuf *buf = world->mSndBufs + bufnum;

		unit->mBufSize = buf->samples;

		if(unit->mBufSize%2==1) printf("Not multiple of 2 size buffer \n");

		unit->mBuf= buf->data;

		unit->bufpos=0;

		//unit->lastlastAmp=0.0;
		unit->lastAmp=0.0;
		unit->newAmp=0.0;
		unit->interpsteps=10;
		unit->interpnow=10;
		//unit->lastInstruction=0;
		unit->prob=1.0;

		SETCALC(Instruction_next_a);
	}
}


void readinstruction(Instruction *unit, int command, float param){

	bool newbreakpoint=false;
	float tmp= unit->newAmp;
	int steps, pos;

	RGen& rgen = *unit->mParent->mRGen;

	//return early if instruction 8 had lowered probability
	if (rgen.frand()>unit->prob) {
		unit->prob=1.0;
		return;
	}

	switch (command){

		case 0: //interpolate, param is how long for

			unit->interpnow=1;

			steps= (int)(param*500+0.5); // test larger than 0;

			//printf("%d \n",steps);

			if (steps<1) steps=1;
				if(steps>5000) steps=5000;
					unit->interpsteps=steps;

			break;

		case 1: //new random breakpoint

			tmp= param*((2*rgen.frand() - 1.0)); //amp degree from param

			newbreakpoint=true;

			break;

		case 2: //perturb

			tmp= (param*(2*rgen.frand() - 1.0))+tmp; //amp degree from param

			if(tmp>1.0) tmp=2.0-tmp;
				if(tmp<(-1.0)) tmp=(-2.0)-tmp;

					newbreakpoint=true;

			break;
		case 3: //invert

			tmp= (1-param)*tmp+param*(-tmp); //interpolate to inverse
			newbreakpoint=true;

			break;

		case 4: //interpolate last two

			tmp= ((1-param)*unit->lastAmp)+(param*tmp); //usually 0.5
			newbreakpoint=true;

			break;

		case 5: //damp

			tmp= (tmp)*param;
			newbreakpoint=true;

			break;

		case 6: //damp

			tmp= param;
			newbreakpoint=true;

			break;



		case 8: //do next command with probability

			unit->prob= param;

			break;

		case 9: //buffer pointer back to start (or to param)
			pos= (int)(param+0.5); // test larger than 0;

			if(pos%2==1) --pos;

				if(pos<0) pos=0;

					unit->bufpos= pos%(unit->mBufSize);

			break;

		default: //if get to here, unknown instruction, step on through buffer is automatic

			break;
	}



	if(newbreakpoint) {
		//unit->lastlastAmp=unit->lastAmp;
		unit->lastAmp=unit->newAmp;
		unit->newAmp=tmp;
	}


}


void Instruction_next_a(Instruction *unit, int inNumSamples) {

	int multicalltest=0;

	float *out = ZOUT(0);

	//if interpolating, keep interpolating, else read next instruction

	float t;
	int command;
	float param;
	int interpnow=unit->interpnow;
	int interpsteps= unit->interpsteps;

	bool cancontinue=true;

	for(int j=0; j<inNumSamples; ++j) {

		//interpolating, output samples
		if (interpnow<=interpsteps) {

			t= (float)interpnow/((float)interpsteps);

			ZXP(out)= (1-t)*unit->lastAmp+ (t*unit->newAmp);

			interpnow=interpnow+1;
		}
		else //run instructions until new interpolation or unsafe
		{

			cancontinue=true;

			while (cancontinue && (multicalltest<inNumSamples)) {

				command= (int) (unit->mBuf[unit->bufpos]+ 0.5);
				param= unit->mBuf[unit->bufpos+1]; //for parameter control, PLUS UPDATE BELOW

				//increment buffer position safely
				unit->bufpos= (unit->bufpos+2)%(unit->mBufSize);

				if(unit->bufpos%2==1)
					unit->bufpos= 0; //reset if troublesome movement

				readinstruction(unit,command, param);

				if (command==0) {

					cancontinue=false;
					interpnow=unit->interpnow;
					interpsteps= unit->interpsteps;
				}

				++multicalltest; //safety in case you get stuck
			}

			//safety, set up interpolation through the remaining samples
			if (multicalltest==inNumSamples) {

				interpnow=1;
				interpsteps= inNumSamples-j;

			}

		}



	}

	unit->interpnow=interpnow;
	unit->interpsteps=interpsteps;

}




void WaveTerrain_Ctor(WaveTerrain* unit) {

    SETCALC(WaveTerrain_next_a);

}

void WaveTerrain_next_a(WaveTerrain *unit, int inNumSamples) {

    int j;

    float *out = ZOUT(0);

    uint32 bufnum = (uint32)ZIN0(0);

	SndBuf * buf= SLUGensGetBuffer(unit,bufnum);

    int works = 1;

	if (buf) {

		int totalsize = buf->samples;

		int xsize = (int)(ZIN0(3)+0.0001); //safety on round down
		int ysize = (int)(ZIN0(4)+0.0001);

		if((xsize * ysize)> totalsize) {

            //could set xsize and ysize to (int)sqrt(totalsize) but may not be desired result?
            //could set ysize to total/xsize too... unless xsize itself >totalsize

			//printf("WaveTerrain: size mismatch between xsize*ysize and actual buffer size. UGen will output silence \n");
			works = 0;

        } else {

            float * terrain = buf->data;

            float *xin = IN(1);
            float *yin = IN(2);


            float x, y, xindex, yindex;
            int xfloor, yfloor, xnext, ynext;
            float xprop,yprop;

            float vll,vlr,vul,vur;

            for (int j=0; j<inNumSamples;++j) {

                x= xin[j]; //0.0 to 1.0
                y= yin[j];

                //safety
                x= sc_wrap(x, 0.0f, 1.f);
                y= sc_wrap(y, 0.0f, 1.f);

                xindex= x*xsize;
                yindex= y*ysize;

                //	if (xindex<0.0f)
                //			xindex=0.0f;
                //		else if (xindex>=xsize)
                //			xindex= xsize-0.00001;
                //
                //		if (yindex<0.0f)
                //			yindex=0.0f;
                //		else if (yindex>=ysize)
                //			yindex= ysize-0.00001;

                //these are guaranteed in range from wrap above give or take floating point error on round down?
                //added modulo because very occasional error with index up to xsize or ysize
                xfloor= ((int)xindex)%xsize;
                yfloor= ((int)yindex)%ysize;

                //these aren't; needs further wrap
                xnext= (xfloor+1)%xsize;
                ynext= (yfloor+1)%ysize;

                xprop= xindex-xfloor;
                yprop= yindex-yfloor;

                //printf("x %f, y %f, xfloor %d, yfloor %d, xnext, %d, ynext %d, xprop %f, yprop %f \n", x, y, xfloor, yfloor, xnext, ynext, xprop, yprop);

                //now have to look up in table and interpolate; linear within the 4 vertices of a square cell for now, cubic over 16 vertices maybe later

                //format for terrain should be rows of xsize, indexed from lower left
                vll= terrain[(xsize*yfloor)+ xfloor];
                vlr= terrain[(xsize*yfloor)+ xnext];
                vul= terrain[(xsize*ynext)+ xfloor];
                vur= terrain[(xsize*ynext)+ xnext];

                ZXP(out) = (1.0-xprop)*(vll+(yprop*(vul-vll))) + (xprop*(vlr+(yprop*(vur-vlr))));

                //printf("%f \n",);
            }


        }

    } else {

        works = 0;
    }


    //output silence for this block
    if(works==0) {

        for (int j=0; j<inNumSamples;++j)
            ZXP(out) = 0.0f;

            }



}


void VMScan2D_Ctor(VMScan2D* unit) {

	//World *world = unit->mWorld;

	uint32 bufnum = (uint32)ZIN0(0);

	SndBuf * buf= SLUGensGetBuffer(unit,bufnum);

	if (buf) {

		//if (bufnum >= world->mNumSndBufs) bufnum = 0;
		unit->mBufNum=bufnum;

		///SndBuf *buf = world->mSndBufs + bufnum;

		unit->mBufSize = buf->samples;

		if(unit->mBufSize%2==1) printf("Not multiple of 2 size buffer \n");

		unit->mBuf= buf->data;

		unit->bufpos=0;

		//unit->lastlastAmp=0.0;
		unit->lastx=0.0;
		unit->newx=0.0;
		unit->lasty=0.0;
		unit->newy=0.0;
		unit->interpsteps=10;
		unit->interpnow=11;
		//unit->lastInstruction=0;
		unit->prob=1.0;

		SETCALC(VMScan2D_next_a);
	}
}



void VMScan2D_next_a(VMScan2D *unit, int inNumSamples) {

	int multicalltest=0;

	//float *out = ZOUT(0);

	float * outx= OUT(0);
	float * outy= OUT(1);

	//if interpolating, keep interpolating, else read next instruction

	float t;
	int command;
	float param;
	int interpnow=unit->interpnow;
	int interpsteps= unit->interpsteps;

	bool cancontinue=true;

	for(int j=0; j<inNumSamples; ++j) {

		//interpolating, output samples
		if (interpnow<=interpsteps) {

			t= (float)interpnow/((float)interpsteps);

			//ZXP(out)= (1-t)*unit->lastAmp+ (t*unit->newAmp);

			outx[j]=(1-t)*unit->lastx+ (t*unit->newx);
			outy[j]=(1-t)*unit->lasty+ (t*unit->newy);

			++interpnow; //=interpnow+1;
		}
		else //run instructions until new interpolation or unsafe
		{

			cancontinue=true;

			while (cancontinue && (multicalltest<inNumSamples)) {

				command= (int) (unit->mBuf[unit->bufpos]+ 0.5);
				param= unit->mBuf[unit->bufpos+1]; //for parameter control, PLUS UPDATE BELOW

				//increment buffer position safely
				unit->bufpos= (unit->bufpos+2)%(unit->mBufSize);

				if(unit->bufpos%2==1)
					unit->bufpos= 0; //reset if troublesome movement

				readinstructionVMScan2D(unit,command, param);

				if (command==0) {

					cancontinue=false;
					interpnow=unit->interpnow;
					interpsteps= unit->interpsteps;
				}

				++multicalltest; //safety in case you get stuck
			}

			//safety, set up interpolation through the remaining samples
			if (multicalltest>=inNumSamples) {

				interpnow=1;
				interpsteps= inNumSamples-j;

			}

		}



	}

	unit->interpnow=interpnow;
	unit->interpsteps=interpsteps;

}

void readinstructionVMScan2D(VMScan2D *unit, int command, float param) {

	bool newbreakpoint=false;
	float tmpx= unit->newx;
	float tmpy= unit->newy;
	int steps, pos;

	RGen& rgen = *unit->mParent->mRGen;

	//return early if instruction 8 had lowered probability
	if (rgen.frand()>unit->prob) {
		unit->prob=1.0;
		return;
	}

	switch (command){

		case 0: //interpolate, param is how long for

			unit->interpnow=1;

			steps= (int)(param*500+0.5); // test larger than 0;

			//printf("%d \n",steps);

			if (steps<1) steps=1;
				if(steps>5000) steps=5000;
					unit->interpsteps=steps;

			break;

		case 1: //new random breakpoint

			tmpx= param*(rgen.frand()); //degree from param
			tmpy= param*(rgen.frand());

			newbreakpoint=true;

			break;

		case 2: //perturb

			tmpx+= (param*(2*rgen.frand() - 1.0)); // degree from param
			tmpy+= (param*(2*rgen.frand() - 1.0));

			if(tmpx>1.0) tmpx=1.0-tmpx;
				if(tmpx<(0.0)) tmpx= (-tmpx);

					if(tmpy>1.0) tmpy=1.0-tmpy;
						if(tmpy<(0.0)) tmpy= (-tmpy);

							newbreakpoint=true;

			break;
		case 3: //invert

			tmpx += param*(1.0-(2*tmpx));
			tmpy += param*(1.0-(2*tmpy));

			newbreakpoint=true;

			break;

		case 4: //interpolate last two

			tmpx = ((1.0-param)*unit->lastx)+(param*tmpx);
			tmpy = ((1.0-param)*unit->lasty)+(param*tmpy);

			newbreakpoint=true;

			break;

		case 5: //damp

			tmpx= (tmpx)*param;
			tmpy= (tmpy)*param;

			newbreakpoint=true;

			break;

		case 6: //set x
			unit->lastx=unit->newx;
			unit->newx=param;
			break;

		case 7: //set y
			unit->lasty=unit->newy;
			unit->newy=param;

			break;

		case 8: //do next command with probability

			unit->prob= param;

			break;

		case 9: //buffer pointer back to start (or to param)
			pos= (int)(param+0.5); // test larger than 0;

			if(pos%2==1) --pos;

				if(pos<0) pos=0;

					unit->bufpos= pos%(unit->mBufSize);

			break;

		default: //if get to here, unknown instruction, step on through buffer is automatic

			break;
	}



	if(newbreakpoint) {
		unit->lastx=unit->newx;
		unit->lasty=unit->newy;
		unit->newx=tmpx;
		unit->newy=tmpy;
	}



}














void SLOnset_Ctor(SLOnset* unit) {

	unit->m_memorysize1= (int) (ZIN0(1)+0.0001);
	unit->m_before= (int) (ZIN0(2)+0.0001);
	unit->m_after= (int) (ZIN0(3)+0.0001);

	unit->m_memorysize2 = unit->m_before + unit->m_after +1; //unit->m_memorysize2 each side plus central

	unit->m_memory1 = (float*)RTAlloc(unit->mWorld, unit->m_memorysize1 * sizeof(float));
	unit->m_memory2 = (float*)RTAlloc(unit->mWorld, unit->m_memorysize2 * sizeof(float));

	int i;

	//initialise to zeroes!
	for (i=0; i<unit->m_memorysize1; ++i)
		unit->m_memory1[i]= 0.0;
	for (i=0; i<unit->m_memorysize2; ++i)
		unit->m_memory2[i]= 0.0;

	unit->m_hysteresiscount= 0;

	unit->m_memorycounter1 = 0;
	unit->m_memorycounter2 = 0;

	SETCALC(SLOnset_next);

}

void SLOnset_Dtor(SLOnset* unit) {

	RTFree(unit->mWorld, unit->m_memory1);
	RTFree(unit->mWorld, unit->m_memory2);

}

//log2(max(abs(tmp))+1);

void SLOnset_next(SLOnset *unit, int inNumSamples) {

	int i;

	//value to store
	float * in= IN(0);
	float * out= OUT(0);

	float maxval= (-1.0);
	float tmp;

	//assumption of 64 block size really underlies all this
	for (i=0; i<inNumSamples; ++i) {

		tmp= fabs(in[i]);

		if (tmp>maxval) maxval= tmp;
	}

	maxval= log2(maxval+1.0);

	//update storage array
	float * memory1= unit->m_memory1;
	float * memory2= unit->m_memory2;

	int counter1= unit->m_memorycounter1;
	int counter2= unit->m_memorycounter2;

	int memorysize1= unit->m_memorysize1;
	int memorysize2= unit->m_memorysize2;


	memory1[counter1] = maxval;

	float evidence=0.0; //=maxval

	for (i=0; i<memorysize1; ++i) {

		if (i!=counter1) {
			tmp = maxval-memory1[i];

            if (tmp>0.0)
                evidence = evidence + tmp;
            else
				evidence = evidence - 1.0; //%10.0;

		}

	}

	//now update memory2
	maxval=sc_max(evidence,0.f);

	memory2[counter2] = maxval;

	int index;

	evidence=0.0;

	float threshold= ZIN0(4);

	int centreindex= counter2- unit->m_after + memorysize2;

	float now =memory2[centreindex%memorysize2]; //-threshold; 	 //will perturb calculations

	for (i=1; i<=unit->m_before; ++i) {

		index= (centreindex - i + memorysize2)%memorysize2;
		tmp = now-memory2[index];

		if (tmp <0.0)
            tmp = 0.0; //(- 10.0);

        evidence = evidence+ tmp;
	}

	for (i=1; i<=unit->m_after; ++i) {

		index= (centreindex + i + memorysize2)%memorysize2;
		tmp = now-memory2[index];

		if (tmp <0.0)
			tmp = 0.0; //(- 10.0);

		evidence = evidence+ tmp;
	}


	out[0]=0.0;
	if(unit->m_hysteresiscount==0) {
		if(evidence>threshold){
			out[0]=1.0;
			unit->m_hysteresiscount=(int) (ZIN0(5)+0.0001); //5;
		}

	}
	else
		--unit->m_hysteresiscount;

	unit->m_memorycounter1 = (counter1+1)%memorysize1;
	unit->m_memorycounter2 = (counter2+1)%memorysize2;

}




////output= TwoTube.ar(input, scatteringcoefficient,lossfactor,d1length,d2length);
void TwoTube_Ctor(TwoTube* unit) {

	unit->d1length= (int)ZIN0(3); //(int) (ZIN0(1)+0.0001);
	unit->d2length= (int)ZIN0(4); //(int) (ZIN0(2)+0.0001);
	unit->lossfactor= ZIN0(2); //s(int) (ZIN0(3)+0.0001);

	unit->delay1right = (float*)RTAlloc(unit->mWorld, unit->d1length * sizeof(float));
	unit->delay1left = (float*)RTAlloc(unit->mWorld, unit->d1length * sizeof(float));
	unit->delay2right = (float*)RTAlloc(unit->mWorld, unit->d2length * sizeof(float));
	unit->delay2left = (float*)RTAlloc(unit->mWorld, unit->d2length * sizeof(float));

	int i;

	//initialise to zeroes!
	for (i=0; i<unit->d1length; ++i) {
		unit->delay1right[i]= 0.0;
		unit->delay1left[i]= 0.0;
	}
	for (i=0; i<unit->d2length; ++i) {
		unit->delay2right[i]= 0.0;
		unit->delay2left[i]= 0.0;
	}

	unit->f1in= 0.0;
	unit->f1out= 0.0;
	unit->f2in=0.0;
	unit->f2out=0.0;

	unit->d1rightpos= 0;
	unit->d1leftpos= 0;
	unit->d2rightpos= 0;
	unit->d2leftpos= 0;

	SETCALC(TwoTube_next);
}

void TwoTube_Dtor(TwoTube* unit) {

	RTFree(unit->mWorld, unit->delay1right);
	RTFree(unit->mWorld, unit->delay1left);
	RTFree(unit->mWorld, unit->delay2right);
	RTFree(unit->mWorld, unit->delay2left);

}

void TwoTube_next(TwoTube *unit, int inNumSamples) {

	int i;

	//value to store
	float * in= IN(0);
	float * out= OUT(0);
	float k= (float)ZIN0(1); //scattering coefficient updated at control rate?
	float loss= unit->lossfactor;

	float * d1right= unit->delay1right;
	int d1rightpos= unit->d1rightpos;
	float * d2right= unit->delay2right;
	int d2rightpos= unit->d2rightpos;
	float * d2left= unit->delay2left;
	int d2leftpos= unit->d2leftpos;
	float * d1left= unit->delay1left;
	int d1leftpos= unit->d1leftpos;

	int d1length=unit->d1length;
	int d2length=unit->d2length;

	//have to store filter state around loop; probably don't need to store output, but oh well
	float f1in=unit->f1in;
	float f2in=unit->f2in;
	float f2out=unit->f2out;
	float f1out=unit->f1out;

	for (i=0; i<inNumSamples; ++i) {

		//update outs of all delays
		float d1rightout= d1right[d1rightpos];
		float d1leftout= d1left[d1leftpos];
		float d2rightout= d2right[d2rightpos];
		float d2leftout= d2left[d2leftpos];
		//
		//			if(i==1) {
		//			printf("delay outputs %f %f %f %f", d1rightout, d2rightout, d2leftout, d1leftout);
		//			}

		//output value
		out[i]=d2rightout;

		//update all filters
		f1out= loss*0.5*(f1in+d1leftout);
		f1in= d1leftout;

		//should change s factor later, and independent time varying gains...
		f2out= loss*(0.5*f2in+0.5*d2rightout);
		f2in= d2rightout;

		//calculate inputs of all delays
		d1right[d1rightpos]= in[i]+f1out;
		d2right[d2rightpos]= d1rightout*(1+k)+ ((-k)*d2leftout);
		d2left[d2leftpos]= f2out;
		d1left[d1leftpos]= d1rightout*k+ ((1-k)*d2leftout);

		//update delay line position pointers

		d1rightpos= (d1rightpos+1)%d1length;
		d2rightpos= (d2rightpos+1)%d2length;
		d1leftpos= (d1leftpos+1)%d1length;
		d2leftpos= (d2leftpos+1)%d2length;
	}


	unit->d1rightpos= d1rightpos;
	unit->d1leftpos= d1leftpos;
	unit->d2rightpos= d2rightpos;
	unit->d2leftpos= d2leftpos;


	unit->f1in= f1in;
	unit->f2in= f2in;
	unit->f2out=f2out;
	unit->f1out= f1out;



}





////output= NTube.ar(input, loss, karray, delaylengtharray);
void NTube_Ctor(NTube* unit) {

	int i,j;

	int numinputs = unit->mNumInputs;
	int numtubes= (numinputs-1)/3;  //NOW 1+ (N+1) + N-1 + N 3N+1//WAS 1+1+N-1+N = 2N+1
	unit->numtubes= numtubes;

	if(numtubes<2) {
		printf("too few tubes! only %d \n", numtubes);
		return;
	}

	unit->maxlength= 1024; //no frequencies below about 50 Hz for an individual section
	unit->modulo= unit->maxlength-1;

	unit->delayconversion= unit->mRate->mSampleRate; //multiplies delay time in seconds to make delay time in samples

	//printf("num tubes only %d and delayconversion %f \n", numtubes, unit->delayconversion);

	unit->delayright= (float**)RTAlloc(unit->mWorld, numtubes * sizeof(float *));
	unit->delayleft= (float**)RTAlloc(unit->mWorld, numtubes * sizeof(float *));

	for (i=0; i<numtubes; ++i) {

		unit->delayright[i]= 	(float*)RTAlloc(unit->mWorld, unit->maxlength * sizeof(float));
		unit->delayleft[i]= 	(float*)RTAlloc(unit->mWorld, unit->maxlength * sizeof(float));


		float * pointer1 = 	unit->delayright[i];
		float * pointer2 = 	unit->delayleft[i];


		for (j=0; j<unit->maxlength; ++j) {
			pointer1[j]= 0.0;
			pointer2[j]= 0.0;
		}

	}

	unit->losses= (float*)RTAlloc(unit->mWorld, (numtubes+1) * sizeof(float));
	unit->scattering= (float*)RTAlloc(unit->mWorld, (numtubes-1) * sizeof(float));
	unit->delays= (float*)RTAlloc(unit->mWorld, numtubes * sizeof(float));

	unit->rightouts= (float*)RTAlloc(unit->mWorld, numtubes * sizeof(float));
	unit->leftouts= (float*)RTAlloc(unit->mWorld, numtubes * sizeof(float));


	unit->position=0;

	unit->f1in= 0.0;
	unit->f1out= 0.0;
	unit->f2in=0.0;
	unit->f2out=0.0;

	SETCALC(NTube_next);
}

void NTube_Dtor(NTube* unit) {

	int i;

	for (i=0; i<unit->numtubes; ++i) {

		RTFree(unit->mWorld, unit->delayright[i]);
		RTFree(unit->mWorld, unit->delayleft[i]);

	}

	RTFree(unit->mWorld, unit->delayright);
	RTFree(unit->mWorld, unit->delayleft);

	RTFree(unit->mWorld, unit->scattering);
	RTFree(unit->mWorld, unit->delays);
	RTFree(unit->mWorld, unit->losses);


	RTFree(unit->mWorld, unit->rightouts);
	RTFree(unit->mWorld, unit->leftouts);

}

void NTube_next(NTube *unit, int inNumSamples) {

	int i,j;

	int numtubes= unit->numtubes;

	//value to store
	float * in= IN(0);
	float * out= OUT(0);

	float ** right= unit->delayright;
	int pos= unit->position;
	float ** left= unit->delayleft;

	//GET FREQUENCIES AND SCATTERING COEFFICIENTS
	float * losses= unit->losses;
	float * scatteringcoefficients= unit->scattering;
	float * delays= unit->delays;

	int arg=1;

	//used to be single argument
	//float loss= (float)ZIN0(1);

	for (i=0; i<(numtubes+1); ++i)	{

		losses[i]= ZIN0(arg);
		++arg;
	}

	//	for (i=0; i<(numtubes+1); ++i)	{
	//
	//		printf("loss %d is %f ",i, losses[i]);
	//	}
	//	printf("\n");

	for (i=0; i<(numtubes-1); ++i) {

		scatteringcoefficients[i]= ZIN0(arg);
		++arg;
	}

	int maxlength= unit->maxlength;
	float maxlengthf= (float) maxlength;
	float maxlengthfminus1= (float) (maxlength-1);
	int modulo= unit->modulo;

	float delayconv= unit->delayconversion;

	for (i=0; i<numtubes; ++i) {

		float delayinsec= ZIN0(arg);
		float delayinsamples= delayconv*delayinsec;

		if(delayinsamples<0.0) delayinsamples=0.0;
		if(delayinsamples>maxlengthfminus1) delayinsamples= maxlengthfminus1;

		delays[i]= delayinsamples; //ZIN0(arg);

		//printf("delay %d is %f \n", i, delays[i]);
		++arg;
	}

	//have to store filter state around loop; probably don't need to store output, but oh well
	float f1in=unit->f1in;
	float f2in=unit->f2in;
	float f2out=unit->f2out;
	float f1out=unit->f1out;

	float * delayline;
	float * delayline2;
	float past;
	int pos1, pos2;
	float interp; //for linear interpolation of position

	float * rightouts= unit->rightouts;
	float * leftouts= unit->leftouts;

	for (i=0; i<inNumSamples; ++i) {

		//update all outs

		for (j=0; j<numtubes; ++j) {

			//calculate together since share position calculation, same delay length in each tube section
			delayline= right[j];
			delayline2= left[j];

			past = fmod(pos+maxlengthf- delays[j], maxlengthf);

			pos1= past; //round down
			interp= past-pos1;
			pos2= (pos1+1)&modulo;

			//printf("check tube %d for sample %d where pos1 %d pos2 %d interp %f \n",j,i,pos1,pos2, interp);

			//printf("%p %p \n",delayline, delayline2);

			//int h;
			//			for (h=0; h<maxlength; ++h) {
			//				printf("%f ",delayline[h]);
			//			}
			//			printf("\n");
			//			for (h=0; h<maxlength; ++h) {
			//				printf("%f ",delayline2[h]);
			//			}
			//			printf("\n");
			//
			//linear interpolation to allow non sample frequencies
			rightouts[j]= ((1.0-interp)*delayline[pos1]) + (interp*delayline[pos2]);
			leftouts[j]= ((1.0-interp)*delayline2[pos1]) + (interp*delayline2[pos2]);

		}


		//printf("got to here! %d \n",i);


		//output value
		out[i]=rightouts[numtubes-1];

		//including filters at the ends:

		//update all filters
		f1out= losses[0]*0.5*(f1in+leftouts[0]);
		f1in= leftouts[0];

		//should change s factor later, and independent time varying gains...
		f2out= losses[numtubes]*(0.5*f2in+0.5*rightouts[numtubes-1]);
		f2in= rightouts[numtubes-1];

		delayline= right[0];
		delayline2= left[numtubes-1];

		delayline[pos]= in[i]+f1out;
		delayline2[pos]= f2out;

		//then update all other ins via numtubes-1 scattering junctions


		//printf("got to here 2! %d \n",i);

		for (j=0; j<(numtubes-1); ++j) {

			float k = scatteringcoefficients[j];

			delayline= right[j+1];
			delayline2= left[j];


			//version one: no internal friction, too long
			//delayline[pos]= rightouts[j]*(1+k)+ ((-k)*leftouts[j+1]);
			//delayline2[pos]= rightouts[j]*k+ ((1-k)*leftouts[j+1]);

			float loss= losses[j+1];
			//always a loss at interface to avoid continual recirculation; separate internal loss parameter?
			delayline[pos]= rightouts[j]*(1+k)+ (loss*(-k)*leftouts[j+1]);
			delayline2[pos]= (rightouts[j]*k*loss)+ ((1-k)*leftouts[j+1]);


			//calculate inputs of all delays
			//d2right[d2rightpos]= d1rightout*(1+k)+ ((-k)*d2leftout);
			//d1left[d1leftpos]= d1rightout*k+ ((1-k)*d2leftout);

		}

		//update common delay line position pointer
		//update position
		pos= (pos+1)&modulo;


		//printf("got to here 3! %d %d \n",i,pos);
	}

	unit->f1in= f1in;
	unit->f2in= f2in;
	unit->f2out=f2out;
	unit->f1out= f1out;


	unit->position= pos;
}







void EnvFollow_Ctor( EnvFollow* unit ) {

	unit->eprev_ = 0.0f;

	SETCALC(EnvFollow_next);

	OUT0(0) = 0.0f;
}



void EnvFollow_next( EnvFollow *unit, int inNumSamples ) {

	//int numSamples = unit->mWorld->mFullRate.mBufLength;

	float *input = IN(0);
	float *output = OUT(0);

	float c = ZIN0(1);
	float oneminusc= 1.0f-c;

	float e = unit->eprev_;

	for (int i=0; i<inNumSamples; ++i) {

		float  x = input[i];

		//full wave rectify
		x = x<0.0f?-x:x;

		if (x>e)
			e = x;
		else
			e = c*e + oneminusc*x;

		output[i]= e;

	}

	//printf("hello phase %f period %f\n",phase, period);

	unit->eprev_ = e;

}




void Sieve1_Ctor( Sieve1* unit ) {

	SndBuf * buf = SLUGensGetBuffer(unit, ZIN0(0));

	if (buf) {

		unit->maxdatasize_ = buf->samples -1; //take 1 off to allow that working size always first entry

		if(unit->maxdatasize_>=1) {

			unit->buffer_ = buf->data;

			unit->bufferpos_=0;
			unit->currentsize_ = 1;

			//unit->interpsize_;
			//unit->interp_=0;

			unit->alternate_= ZIN0(2);

			unit->swap_ = 1;

			unit->phase_ = 0.0f;

			SETCALC(Sieve1_next);

		}
	}

}



void Sieve1_next( Sieve1 *unit, int inNumSamples ) {

	float *output = OUT(0);

	//int gap = ZIN0(1);
	float gap = ZIN0(1);

	//safety
	if(gap<1.0f) gap = 1.0f;

	//int interp = unit->interp_;

	float * buffer = unit->buffer_;

	RGen& rgen = *unit->mParent->mRGen;


	float phase = unit-> phase_;

	for (int i=0; i<inNumSamples; ++i) {

		//if(interp == 0) {

		if(phase >= gap) {

			//interp = gap;

			phase = fmod(phase, gap); //can't reduce by gap since gap can change per cycle, and phase could be much larger than 2*gap

			++unit->bufferpos_;

			if(unit->bufferpos_==unit->currentsize_) {

				unit->currentsize_ = (int)buffer[0];

				if (unit->currentsize_ > unit->maxdatasize_)
					unit->currentsize_ = unit->maxdatasize_;


				unit->bufferpos_ = 0;
			}

			float chance = buffer[unit->bufferpos_+1];

			float sign = 1.0;

			if(unit->alternate_==1) {

				sign = unit->swap_==1?1.0f:-1.0f;

				unit->swap_ = 1 - unit->swap_;
			}

			if(rgen.frand()<chance)
				output[i]= sign;  //could alternate left and right
			else
				output[i]= 0.0f;


		} else

			output[i]= 0.0f;

		//--interp;

		phase += 1.0f;

	}

	//printf("hello phase %f period %f\n",phase, period);

	//unit->interp_ = interp;
	unit-> phase_ = phase;

}



void Oregonator_Ctor( Oregonator* unit ) {

	unit->x = 0.5f;
    unit->y = 0.5f;
    unit->z = 0.5f;

	SETCALC(Oregonator_next);
}



void Oregonator_next( Oregonator *unit, int inNumSamples ) {

	//int numSamples = unit->mWorld->mFullRate.mBufLength;

	float *output1 = OUT(0);
	float *output2 = OUT(1);
    float *output3 = OUT(2);

	float delta = ZIN0(1);
    float epsilon = ZIN0(2);
    float mu = ZIN0(3);
    float q = ZIN0(4);
    float trig = ZIN0(0);

    float x= unit->x;
    float y= unit->y;
    float z= unit->z;

    float dx, dy, dz;

    //reset triggered
    if(trig>0.0f) {

        x = ZIN0(5);
        y = ZIN0(6);
        z = ZIN0(7);

    }

	for (int i=0; i<inNumSamples; ++i) {

        dx = epsilon*((q*y) -(x*y) + (x*(1-x)));
		dy = mu* (-(q*y) -(x*y) + z);
        dz = x-y;

        x += delta*dx;
        y += delta*dy;
        z += delta*dz;

		output1[i]= x;
        output2[i]= y;
        output3[i]= z;

	}

	//printf("Oregonator: x %f y %f z %f\n",x,y,z);

	unit->x = x;
	unit->y = y;
	unit->z = z;
}


void Brusselator_Ctor( Brusselator* unit ) {

	unit->x = 0.5f;
    unit->y = 0.5f;

	SETCALC(Brusselator_next);
}



void Brusselator_next( Brusselator *unit, int inNumSamples ) {

	float *output1 = OUT(0);
	float *output2 = OUT(1);

	float delta = ZIN0(1);
    float mu = ZIN0(2);
    float gamma = ZIN0(3);
    float trig = ZIN0(0);

    float x= unit->x;
    float y= unit->y;

    float dx, dy;

    //reset triggered
    if(trig>0.0f) {

        x = ZIN0(4);
        y = ZIN0(5);

    }

    float muplusone = 1.0f+mu;

	for (int i=0; i<inNumSamples; ++i) {

        float temp = x*x*y;

        dx = temp - (muplusone*x) + gamma;
        dy =  (mu*x)  - temp;

        x += delta*dx;
        y += delta*dy;

		output1[i]= x;
        output2[i]= y;

	}

	//printf("Oregonator: x %f y %f z %f\n",x,y,z);

	unit->x = x;
	unit->y = y;
}





void SpruceBudworm_Ctor( SpruceBudworm* unit ) {

	unit->x = 0.9f;
    unit->y = 0.1f;

	SETCALC(SpruceBudworm_next);
}



void SpruceBudworm_next( SpruceBudworm *unit, int inNumSamples ) {

	float *output1 = OUT(0);
	float *output2 = OUT(1);


    float trig = ZIN0(0);

    float delta = ZIN0(1);

	float k1 = ZIN0(2);
    float k2 = ZIN0(3);
    float alpha = ZIN0(4);
    float beta = ZIN0(5);
    float mu = ZIN0(6);
    float rho = ZIN0(7);

    float x= unit->x;
    float y= unit->y;

    float dx, dy;

    //reset triggered
    if(trig>0.0f) {

        x = ZIN0(8);
        y = ZIN0(9);

    }

	for (int i=0; i<inNumSamples; ++i) {

        float temp = y*y;
        float temp2 = beta*x;

        dx = (k1* x* (1.0-x)) - (mu*y);
        dy = (k2*y*(1.0- (y/(alpha*x))))  - (rho*(temp/(temp2*temp2 +  temp)));


        x += delta*dx;
        y += delta*dy;

		output1[i]= x;
        output2[i]= y;

	}

	//printf("Oregonator: x %f y %f z %f\n",x,y,z);

	unit->x = x;
	unit->y = y;
}



void preparelookuptables() {

	float theta;

	for (int i=0; i<100; ++i) {

		theta= twopi*(i/99.0)-pi;

		gg_lookupsin[i]=sin(theta);
		gg_lookupcos[i]=cos(theta);

	}

}






#ifdef SLUGENSRESEARCH
extern void initSLUGensResearch(InterfaceTable *);
#endif

PluginLoad(SLUGens)
{

	ft = inTable;

	DefineSimpleUnit(SortBuf);
	DefineSimpleUnit(GravityGrid);
	DefineSimpleUnit(GravityGrid2);
	DefineSimpleCantAliasUnit(Breakcore);
	DefineDtorUnit(Max);
	DefineSimpleUnit(PrintVal);
	DefineSimpleUnit(EnvDetect);
	DefineSimpleUnit(FitzHughNagumo);
	DefineSimpleUnit(DoubleWell);
	DefineSimpleUnit(DoubleWell2);
	DefineSimpleUnit(DoubleWell3);
	DefineSimpleCantAliasUnit(WeaklyNonlinear);
	DefineSimpleCantAliasUnit(WeaklyNonlinear2);
	DefineSimpleUnit(TermanWang);
	DefineDtorUnit(LTI);
	DefineDtorUnit(NL);
	DefineDtorUnit(NL2);
	DefineSimpleCantAliasUnit(LPCError);
	DefineDtorUnit(KmeansToBPSet1);
	DefineSimpleUnit(Instruction);
	DefineSimpleUnit(WaveTerrain);
	DefineSimpleUnit(VMScan2D);
	DefineDtorUnit(SLOnset);
	DefineDtorCantAliasUnit(TwoTube);
	DefineDtorCantAliasUnit(NTube);
    DefineSimpleUnit(EnvFollow);
	DefineSimpleUnit(Sieve1);
    DefineSimpleUnit(Oregonator);
    DefineSimpleUnit(Brusselator);
    DefineSimpleUnit(SpruceBudworm);

#ifdef SLUGENSRESEARCH
	initSLUGensResearch(inTable);
#endif

	//printf("preparing SLUGens lookup tables\n");
	preparelookuptables();
	//printf("SLUGens ready\n");

	//	printf("tests!\n");
	//
	//	float tests[12]= {1.0,1.5,2.0,-1.0,-1.5,-2.0, -3, -4, -5, 3, 4, 17};
	//
	//	for (int i=0; i<12; ++i) {
	//	float x= tests[i];
	//
	//
	//	printf("tests! %f %f \n", x, sc_fold(x,-1.0f,1.0f));
	//
	//	}



}
