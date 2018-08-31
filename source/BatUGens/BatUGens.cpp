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

//UGens by Batuhan Bozkurt http://www.batuhanbozkurt.com
//Initial release

#include "SC_PlugIn.h"

static InterfaceTable *ft;

//Coyote is an onset detector
struct Coyote : public Unit
{
	float trackFallTime, slowLagTime, fastLagTime, fastLagMul, threshArg, minDurArg; //arguments from SC.

	float riseCoef, fallCoef; //attack and decay coefficients for the amplitude tracker.
	float prevAmp; //previous output value from the amplitude tracker
	float slowLagCoef, fastLagCoef; //coefficients for the parallel smoothers
	float slowLagPrev, fastLagPrev; //previous values of parallel smoother outputs, used in calculations

	float64 currentAvg;
	float64 avgLagPrev;
	int64 currentIndex;
	float avgTrig;

	int eTime;
	int gate;
};

//TrigAvg averages the absolute value of all its inputs since the last trigger it receives
struct TrigAvg : public Unit
{
	float64 currentAvg;
	int64 currentIndex;
	float trig;
};

//WAmp is a windowed averager. It outputs the average of the absolute value of its inputs received in last x seconds
struct WAmp : public Unit
{
	float* winBuffer;
	int bufIndex;
	int bufSize;
	float currentSum;
};

//First order markov chain workings on audio signals.
//Transition probabilities for its inputs are stored and synthesized back
struct MarkovSynth : public Unit
{
	int **tBuffer;      //holder for the 2d array of base sample/transition probabilities table
	int *bIndex;        //holds current index of transition probabilities. wraps to zero when it reaches trTableSize.
	int *wrIndex;       //holds the biggest index in transition probability table. goes up to trTableSize and stays there.
	int64 waitTime;     //holds the argument for waitTime to fill inputs before synthesis starts.
	int64 currentTick;  //incremented at everysample and compared to waitTime.
	bool timePassed;    //should we start synthesizing?
	int isRecording;    //holds argument for record enable. if 0, it stops populating the tables
	int prevSample;     //holds the previous sample received to map it to a vaild transition
	int nextBaseSamp;   //holds the last synthesized sample to make a valid transition from it later.
	int trTableSize;    //transition table size
};

//////////////////////////////////////////////////////////
struct NeedleRect : public Unit
{
	int imgWidth, imgHeight;
	float oldX, oldY;
};

/////////////////////////////////////////////////////////

struct SkipNeedle : public Unit
{
	float curStart, curTarget;
	float lastOut;
	float sampOffset;
};


extern "C"
{
	void Coyote_next(Coyote *unit, int inNumSamples);
	void Coyote_Ctor(Coyote* unit);

	void TrigAvg_next(TrigAvg *unit, int inNumSamples);
	void TrigAvg_next_k(TrigAvg *unit, int inNumSamples);
	void TrigAvg_Ctor(TrigAvg *unit);

	void WAmp_next(WAmp *unit, int inNumSamples);
	void WAmp_next_k(WAmp *unit, int inNumSamples);
	void WAmp_Ctor(WAmp *unit);
	void WAmp_Dtor(WAmp *unit);

	void MarkovSynth_Ctor(MarkovSynth *unit);
	void MarkovSynth_Dtor(MarkovSynth *unit);
	void MarkovSynth_next(MarkovSynth *unit, int inNumSamples);

	void NeedleRect_Ctor(NeedleRect *unit);
	void NeedleRect_next(NeedleRect *unit, int inNumSamples);

	void SkipNeedle_Ctor(SkipNeedle *unit);
	void SkipNeedle_next(SkipNeedle *unit, int inNumSamples);
};

void MarkovSynth_Ctor(MarkovSynth *unit)
{
	SETCALC(MarkovSynth_next);

	unit->trTableSize = (int)ceil(ZIN0(3));

	unit->tBuffer = (int**)RTAlloc(unit->mWorld, sizeof(int*) * unit->trTableSize);
	for(int i = 0; i < unit->trTableSize; i++)
	{
		unit->tBuffer[i] = (int*)RTAlloc(unit->mWorld, sizeof(int) * 65537);
	}

	unit->bIndex = (int*)RTAlloc(unit->mWorld, sizeof(int) * 65537);
	for(int i = 0; i < 65537; i++)
	{
		unit->bIndex[i] = 0;
	}
	unit->wrIndex = (int*)RTAlloc(unit->mWorld, sizeof(int) * 65537);
	for(int i = 0; i < 65537; i++)
	{
		unit->wrIndex[i] = 0;
	}

	unit->isRecording = ZIN0(1);
	unit->waitTime = ZIN0(2) * SAMPLERATE;
	unit->currentTick = 0;
	unit->prevSample = 32768;
	unit->nextBaseSamp = 32768;
	unit->timePassed = false;
	ZOUT0(0) = 0;
}

void MarkovSynth_Dtor(MarkovSynth *unit)
{
	for(int i = 0; i < unit->trTableSize; i++)
	{
		RTFree(unit->mWorld, unit->tBuffer[i]);
	}

	RTFree(unit->mWorld, unit->tBuffer);
	RTFree(unit->mWorld, unit->bIndex);
	RTFree(unit->mWorld, unit->wrIndex);

}

void MarkovSynth_next(MarkovSynth *unit, int inNumSamples)
{
	float *in = IN(0);
	float *out = OUT(0);
	unit->isRecording = ZIN0(1);

	float bakedIn;
	int arrVal;
	int curIndex;
	for(int i = 0; i < inNumSamples; ++i)
	{
		if(unit->isRecording)
		{
			//limit to -1 1 range
			bakedIn = in[i] > 1 ? 1 : in[i];
			bakedIn = bakedIn < -1 ? -1 : bakedIn;

			//convert to an integer for indexing
			arrVal = (bakedIn + 1) * 32768;
			curIndex = unit->bIndex[unit->prevSample]; //current unused slot in the probability table for the previous sample
			unit->tBuffer[curIndex][unit->prevSample] = arrVal; //insert the new sample to the transition table

			unit->bIndex[unit->prevSample]++; //increment the unused slot index

			//since table size is limited, wrap back to zero if you reach to the end of the table
			if(unit->bIndex[unit->prevSample] == unit->trTableSize)
			{
				unit->bIndex[unit->prevSample] = 0;
				//if top is reached, all slots have values in them, so max index should stay at top
				unit->wrIndex[unit->prevSample] = unit->trTableSize - 1;
			}
			else
			{
				if( unit->wrIndex[unit->prevSample] != unit->trTableSize - 1)
				{//if end of the table isn't reached yet, current slot and max slot should be the same
					unit->wrIndex[unit->prevSample] = unit->bIndex[unit->prevSample];
				}
			}
			unit->prevSample = arrVal; //in next iteration, we will write to the table of the freshly received sample


		}

		if(unit->timePassed == false)//wait until the populating time is over
		{
			unit->currentTick++;
			if(unit->currentTick >= unit->waitTime)
				unit->timePassed = true;
		}


		if(unit->timePassed)//if the time given for populating the initial values has passed, synthesize
		{
			float nextSample;
			int nextFromArray;

			int randVal = (int)unit->mParent->mRGen->irand(unit->wrIndex[unit->nextBaseSamp]);

			//get a random transition value from the probability table of the last synthesized sample.
			nextFromArray = unit->tBuffer[randVal][unit->nextBaseSamp];

			unit->nextBaseSamp = nextFromArray;
			nextSample = (nextFromArray / 32768.f) - 1.f; //re-float the value to send to graph

			out[i] = nextSample;

		}
		else
		{
			out[i] = 0;
		}

	}

}


//////////////////////////////////////////////////

void Coyote_Ctor(Coyote* unit)
{
	SETCALC(Coyote_next);

	unit->trackFallTime = ZIN0(1);
	unit->slowLagTime = ZIN0(2);
	unit->fastLagTime = ZIN0(3);
	unit->fastLagMul = ZIN0(4);
	unit->threshArg = ZIN0(5);
	unit->minDurArg = ZIN0(6);

	unit->riseCoef = exp(log1/(0.001 * SAMPLERATE)); //this is pre-set, constant.

	unit->fallCoef = 0.0 ? 0.0 : exp(log1/(unit->trackFallTime * SAMPLERATE));
	unit->slowLagCoef = 0.f ? 0.f : exp(log001 / (unit->slowLagTime * SAMPLERATE));
	unit->fastLagCoef = 0.f ? 0.f : exp(log001 / (unit->fastLagTime * SAMPLERATE));

	unit->slowLagPrev = 0;
	unit->fastLagPrev = 0;
	unit->avgLagPrev = 0;

	unit->eTime = 0;
	unit->gate = 1;

	unit->currentAvg = 0.f;
	unit->currentIndex = 1;
	unit->avgTrig = 0;

	ZOUT0(0) = unit->prevAmp = ZIN0(0);
}

void Coyote_next(Coyote *unit, int inNumSamples)
{
	float *in = ZIN(0);

	if(ZIN0(1) != unit->trackFallTime)
	{
		unit->fallCoef = exp(log1/(ZIN0(1) * SAMPLERATE));
		unit->trackFallTime = ZIN0(1);
	}

	if(ZIN0(2) != unit->slowLagTime)
	{
		unit->slowLagCoef = 0.f ? 0.f : exp(log001 / (ZIN0(2) * SAMPLERATE));
		unit->slowLagTime = ZIN0(2);
	}

	if(ZIN0(3) != unit->fastLagTime)
	{
		unit->fastLagCoef = 0.f ? 0.f : exp(log001 / (ZIN0(3) * SAMPLERATE));
		unit->fastLagTime = ZIN0(3);
	}

	unit->fastLagMul = ZIN0(4);
	unit->threshArg = ZIN0(5);
	unit->minDurArg = ZIN0(6);

	float prev = unit->prevAmp;

	float trackerOut;
	float slowOut;
	float fastOut;

	if(unit->avgTrig)
	{

		unit->currentAvg = 0;
		unit->currentIndex = 1;
	}

	LOOP(FULLBUFLENGTH,
		 trackerOut = fabs(ZXP(in));
		 if (trackerOut < prev)
		 {
		 trackerOut = trackerOut + (prev - trackerOut) * unit->fallCoef;
		 }
		 else
		 {
		 trackerOut = trackerOut + (prev - trackerOut) * unit->riseCoef;
		 }

		 //average the incoming amplitude tracker values.
		 float64 divi = ((unit->currentAvg - trackerOut) / unit->currentIndex);
		 unit->currentAvg = unit->currentAvg - divi;
		 unit->currentIndex++;

		 prev = trackerOut;
		 );

	//smooth out the values.
	float slowVal = unit->slowLagPrev = trackerOut + (unit->slowLagCoef * (unit->slowLagPrev - trackerOut));
	float fastVal = unit->fastLagPrev = (trackerOut + (unit->fastLagCoef * (unit->fastLagPrev - trackerOut))) * unit->fastLagMul;
	float avgVal = unit->avgLagPrev = unit->currentAvg + (unit->fastLagCoef * (unit->avgLagPrev - unit->currentAvg));

	unit->slowLagPrev = zapgremlins(unit->slowLagPrev);
	unit->fastLagPrev = zapgremlins(unit->fastLagPrev);
	unit->avgLagPrev = zapgremlins(unit->avgLagPrev);

	int trig = unit->avgTrig = ((fastVal > slowVal) || (fastVal> avgVal)) * (trackerOut > unit->threshArg)  * unit->gate;

	unit->eTime++;

	ZOUT0(0) = trig;

	if((trig == 1) && (unit->gate == 1))
	{
		unit->eTime = 0;
		unit->gate = 0;
	}


	if((unit->eTime > (SAMPLERATE * unit->minDurArg)) && (unit->gate == 0))
	{
		unit->gate = 1;
		unit->eTime = 0;
	}


	unit->prevAmp = prev;

}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

void TrigAvg_Ctor(TrigAvg *unit)
{
	if(INRATE(0) == calc_FullRate)
	{
		SETCALC(TrigAvg_next);
	}
	else
	{
		SETCALC(TrigAvg_next_k);
	}
	unit->currentAvg = 0.f;
	unit->currentIndex = 1;
	unit->trig = 0;

	ZOUT0(0) = ZIN0(0);
}

void TrigAvg_next(TrigAvg *unit, int inNumSamples)
{
	float *in = ZIN(0);

	float inTrig = ZIN0(1);

	if(inTrig > 0.f && unit->trig == 0)
	{
		unit->currentAvg = 0;
		unit->currentIndex = 1;
	}

	unit->trig = inTrig > 0.f;

	float inVal;

	LOOP(FULLBUFLENGTH,
		 inVal = fabs(ZXP(in));
		 float64 divi = ((unit->currentAvg - inVal) / unit->currentIndex);
		 unit->currentAvg = unit->currentAvg - divi;
		 unit->currentIndex++;

		 );
	ZOUT0(0) = unit->currentAvg;

}

void TrigAvg_next_k(TrigAvg *unit, int inNumSamples)
{
	float inVal = fabs(IN0(0));

	float inTrig = IN0(1);
	if(inTrig > 0.f && unit->trig == 0)
	{
		unit->currentAvg = 0;
		unit->currentIndex = 1;
	}
	unit->trig = inTrig > 0.f;

	LOOP(FULLBUFLENGTH,

		 float64 divi = ((unit->currentAvg - inVal) / unit->currentIndex);
		 unit->currentAvg = unit->currentAvg - divi;
		 unit->currentIndex++;

		 );
	ZOUT0(0) = unit->currentAvg;

}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

void WAmp_Ctor(WAmp *unit)
{

	if (INRATE(0) == calc_FullRate)
	{
		SETCALC(WAmp_next);
	}
	else
	{
		SETCALC(WAmp_next_k);
	}
	unit->bufSize = ceil(SAMPLERATE * FULLBUFLENGTH * ZIN0(1));


	unit->winBuffer = (float*)RTAlloc(unit->mWorld, unit->bufSize * sizeof(float));


	for(int i = 0; i < unit->bufSize; i++)
			unit->winBuffer[i] = 0;


	unit->bufIndex = 0;
	unit->currentSum = 0;

	ZOUT0(0) = ZIN0(0);

}

void WAmp_Dtor(WAmp *unit)
{
	RTFree(unit->mWorld, unit->winBuffer);
}

void WAmp_next(WAmp *unit, int inNumSamples)
{


	float *in = ZIN(0);

	float *buf = unit->winBuffer;
	float newAvg;
	float inVal;

	LOOP(FULLBUFLENGTH,

		 inVal = fabs(ZXP(in));
		 unit->currentSum = unit->currentSum - buf[unit->bufIndex] + inVal;
		 newAvg = unit->currentSum / unit->bufSize;
		 buf[unit->bufIndex] = inVal;
		 unit->bufIndex++;
		 if(unit->bufIndex == unit->bufSize)
		 {
				unit->bufIndex = 0;

		 }
	);

	ZOUT0(0) = newAvg;

}

void WAmp_next_k(WAmp *unit, int inNumSamples)
{


	float in = IN0(0);

	float *buf = unit->winBuffer;
	float newAvg;
	float inVal;

	LOOP(FULLBUFLENGTH,

		 inVal = fabs(in);
		 unit->currentSum = unit->currentSum - buf[unit->bufIndex] + inVal;
		 newAvg = unit->currentSum / unit->bufSize;
		 buf[unit->bufIndex] = inVal;
		 unit->bufIndex++;
		 if(unit->bufIndex == unit->bufSize)
		 {
		 unit->bufIndex = 0;

		 }
		 );

	ZOUT0(0) = newAvg;

}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

void NeedleRect_Ctor(NeedleRect *unit)
{
	//rate, imgWidth, imgHeight, rectX, rectY, rectW, rectH
	SETCALC(NeedleRect_next);

	unit->imgWidth = ZIN0(1);
	unit->imgHeight = ZIN0(2);
	unit->oldX = 0;
	unit->oldY = 0;

	ZOUT0(0) = ((float)unit->imgWidth * ZIN0(4)) + ZIN0(3);
}

void NeedleRect_next(NeedleRect *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	double rate = ZIN0(0) * SAMPLEDUR;
	float rectX = ZIN0(3);
	float rectY = ZIN0(4);
	float rectW = ZIN0(5);
	float rectH = ZIN0(6);

	rectX = (rectX >= 0) ? rectX : 0.f;
	rectY = (rectY >= 0) ? rectY : 0.f;
	//rectY = (rectY < unit->imgHeight) ? rectY : (unit->imgHeight - 1);

	LOOP(inNumSamples,

		 float tempNextX = fmod((unit->oldX + rate), rectW);
		 //tempNextX = ((rectX + tempNextX) < unit->imgWidth) ? tempNextX : 0.f;

		 if(tempNextX <= unit->oldX)
		 {
			unit->oldY = fmod((unit->oldY + 1), rectH);
		 }
		 unit->oldX = tempNextX;

		 ZXP(out) = ((unit->oldY + rectY) * (float)unit->imgWidth) + rectX + tempNextX;
	);

}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

void SkipNeedle_Ctor(SkipNeedle *unit)
{
	//range, rate

	SETCALC(SkipNeedle_next);
	float temp1, temp2;
	RGen& rgen = *unit->mParent->mRGen;
	temp1 = rgen.irand(ZIN0(0));
	temp2 = rgen.irand(ZIN0(0));

	unit->sampOffset = ZIN0(2);
	unit->curStart = (temp1 >= temp2) ? temp2 : temp1;
	unit->curTarget = (temp1 < temp2) ? temp2 : temp1;

	ZOUT0(0) = unit->lastOut = unit->curStart;
}

void SkipNeedle_next(SkipNeedle *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float blockRange = ZIN0(0);
	double rate = ZIN0(1) * SAMPLEDUR;

	LOOP(inNumSamples,

		unit->lastOut = unit->lastOut + rate;

		if(unit->lastOut >= unit->curTarget)
		{
			//printf("resetting!\n");
			float temp1; float temp2;

			RGen& rgen = *unit->mParent->mRGen;
			temp1 = rgen.irand(blockRange);
			temp2 = rgen.irand(blockRange);

			unit->sampOffset = ZIN0(2);
			unit->curStart = (temp1 >= temp2) ? temp2 : temp1;
			unit->curTarget = (temp1 < temp2) ? temp2 : temp1;

			unit->lastOut = unit->curStart;

			ZXP(out) = fmod(unit->lastOut + unit->sampOffset, blockRange);
		}
		else
		{
			ZXP(out) = fmod(unit->lastOut + unit->sampOffset, blockRange);
		}
	);
}

///////////////////////////////////////////////////////////////

PluginLoad(Bat)
{
	ft = inTable;

	DefineSimpleUnit(Coyote);
	DefineDtorUnit(WAmp);
	DefineSimpleUnit(TrigAvg);
	DefineDtorUnit(MarkovSynth);
	DefineSimpleUnit(NeedleRect);
	DefineSimpleUnit(SkipNeedle);
}