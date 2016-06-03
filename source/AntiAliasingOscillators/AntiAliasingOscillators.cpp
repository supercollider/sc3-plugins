//SuperCollider is under GNU GPL version 3, http://supercollider.sourceforge.net/
//these extensions released under the same license

/*
 *  AntiAliasingOscillators.cpp
 *  AntiAliasingOscillators
 *
 *  Created by Nicholas Collins on 07/08/2010.
 *  Copyright 2010 Nicholas M Collins. All rights reserved.
 *
 */



#include "SC_PlugIn.h"

//Juhan Nam, Vesa Valimaki, Jonathan S. Abel, and Julius O. Smith
//Efficient Antialiasing Oscillator Algorithms Using Low-Order Fractional Delay Filters
//IEEE Transactions on Audio, Speech, and Language Processing 18(4) May 2010, pp 773--785

//and

//Vesa Valimaki, Juhan Nam, Julius O. Smith and Jonathan S. Abel
//Alias-Suppressed Oscillators Based on Differentiated Polynomial Waveforms
//IEEE Transactions on Audio, Speech, and Language Processing 18(4) May 2010, pp 786--798


InterfaceTable *ft;


struct BlitB3 : public Unit
{
	float phase;
};



struct BlitB3Saw : public Unit
{
	float phase_;
	//float leakycoefficient_;
	float lastoutput_;
	float dcoffset_; //compensation for DC offset

};


struct BlitB3Square : public Unit
{
	float phase_;
	//float leakycoefficient_;
	float lastoutput_;
	float bipolar_;

};

struct BlitB3Tri : public Unit
{
	float phase_;
	//float leakycoefficient_;
	float lastoutput_;
	float lastoutput2_;
	float bipolar_;
	float scalefactor_;

};


//4th order differentiated polynomial waveform for sawtooth generation
//requires double precision for sufficient accuracy
struct DPW4Saw : public Unit
{
	//	float phase_;
	//	float scalefactor_;
	//	float differentiations_[3];
	double phase_;
	//double scalefactor_;
	double differentiations_[3];
	double differentiations2_[2]; //for DPW2 algorithm covering lower frequencies

};


struct DPW3Tri : public Unit
{
//	float phase_;
//	float differentiations_[2];
	double phase_;
	double differentiations_[2];
};


extern "C" {
	void BlitB3_next(BlitB3 *unit, int inNumSamples);
	void BlitB3_Ctor(BlitB3* unit);

	void BlitB3Saw_next(BlitB3Saw *unit, int inNumSamples);
	void BlitB3Saw_Ctor(BlitB3Saw* unit);

	void BlitB3Square_next(BlitB3Square *unit, int inNumSamples);
	void BlitB3Square_Ctor(BlitB3Square* unit);

	void BlitB3Tri_next(BlitB3Tri *unit, int inNumSamples);
	void BlitB3Tri_Ctor(BlitB3Tri* unit);

	void DPW4Saw_next(DPW4Saw *unit, int inNumSamples);
	void DPW4Saw_Ctor(DPW4Saw* unit);

	void DPW3Tri_next(DPW3Tri *unit, int inNumSamples);
	void DPW3Tri_Ctor(DPW3Tri* unit);
}


// BlitB3 revised Nathan Ho 2016.
// Phase tracking was modified to prevent freezing with lower frequencies.

void BlitB3_Ctor(BlitB3* unit) {
	unit->phase = 0.0f;
	SETCALC(BlitB3_next);
	BlitB3_next(unit, 1);
	// We have to set this to 0 again because the above line disrupts the phase
	unit->phase = 0.0f;
}

void BlitB3_next(BlitB3 *unit, int inNumSamples) {
	float *out = OUT(0);
	float freq = ZIN0(0);

	// Clip ludicrous frequencies
	if (freq < 0.000001f) {
		freq = 0.000001f;
	}
	// period in samples
	float period = SAMPLERATE / freq;

	// The phase is in range [0,1).
	float phase = fmod(unit->phase, 1.f);
	// Scale up to [0,period).
	float t = phase * period;

	// Temporary variables to save some arithmetic ops
	float x, y;
	for (int i = 0; i < inNumSamples; i++) {

		// 3rd order Lagrange interpolator
		if (t >= 4.0f) {
			out[i] = 0.0f;
		} else if (t >= 3.0f) {
			x = 4.0f - t;
			out[i] = 0.16666666666667f*x*x*x;
		} else if (t >= 2.0f) {
			x = t - 2.0;
			y = x*x;
			out[i] = 0.66666666666666f - y + (0.5f*y*x);
		} else if (t >= 1.0f) {
			x = t - 2.0;
			y = x*x;
			out[i] = 0.66666666666666f - y - (0.5f*y*x);
		} else {
			out[i] = 0.16666666666667f*t*t*t;
		}

		t += 1.f;

		// wrap
		if (t >= period) {
			t -= period;
		}
	}

	// Divide by period (without dividing)
	// t / (SAMPLERATE / freq) = t * freq * SAMPLEDUR
	unit->phase = t * freq * SAMPLEDUR;
}




void BlitB3Saw_Ctor( BlitB3Saw* unit ) {

	unit->phase_ = 3.0f;
	//implies initial period is 5.0f

	//unit->leakycoefficient_ = 0.99;

	unit->lastoutput_ = 3.0f/5.0f- 0.5f;

	unit->dcoffset_ = -1.0f/5.0f;

	//float freq = ZIN0(0);

	SETCALC(BlitB3Saw_next);
}



//void BlitB3_Dtor(BlitB3 *unit)
//{
//
//
//}




void BlitB3Saw_next( BlitB3Saw *unit, int inNumSamples ) {

	//int numSamples = unit->mWorld->mFullRate.mBufLength;

	//float *input = IN(0);
	float *output = OUT(0);

	float dcoffset= unit->dcoffset_;
	//unit->period_ = period;

	float phase= unit->phase_;

	float temp;

	float lastoutput = unit->lastoutput_;
	float leakycoefficient = ZIN0(1);
	float calc;

	for (int i=0; i<inNumSamples; ++i) {

		phase -= 1.0f;

		if (phase>=2.0f) {

			calc = dcoffset;

		} else if (phase>=1.0f) {

			temp= 2.0f-phase;

			calc = 0.16666666666667f*temp*temp*temp + dcoffset;

		} else if (phase>=0.0f) {

			temp= phase*phase;

			calc = 0.66666666666666f - temp + (0.5f*temp*phase) + dcoffset;

		} else if (phase>= -1.0f) {

			temp= phase*phase;

			calc = 0.66666666666666f - temp - (0.5f*temp*phase) + dcoffset;

		} else if (phase>= - 2.0f) {

			temp= 2.0f+phase;

			calc = 0.16666666666667f*temp*temp*temp + dcoffset;

		} else {

			calc= dcoffset; //0.0f;

			//recalculate parameters
			float freq = ZIN0(0);

			if(freq<0.000001f) freq= 0.000001f;

			//need period in samples
			float period = SAMPLERATE/freq;

			if (period<=4.0f) period = 4.0f; //otherwise would have runaway fall off of phase value and incorrect integration

			//won't the jump of offset cause discontinuities?

			dcoffset= -1.0f/period;	//compensation, see eqn (30) in the paper

			unit->dcoffset_ = dcoffset;

			//printf("hello phase %f period %f dc %f leaky %f \n",phase, period, dcoffset, leakycoefficient);

			phase += period; //(period-2.0);
		}

		calc += (lastoutput*leakycoefficient);

		output[i]= calc;
		lastoutput = calc;

	}


	unit->phase_ = phase;
	unit->lastoutput_ = lastoutput;
}















void BlitB3Square_Ctor( BlitB3Square* unit ) {

	unit->phase_ = 3.0f;
	//implies initial period is 10.0f

	//unit->leakycoefficient_ = 0.99;

	unit->lastoutput_ = -0.5f;

	unit->bipolar_ = 1.0f; //start positive, will flip each time


	//float freq = ZIN0(0);

	SETCALC(BlitB3Square_next);
}



void BlitB3Square_next( BlitB3Square *unit, int inNumSamples ) {

	//int numSamples = unit->mWorld->mFullRate.mBufLength;

	//float *input = IN(0);
	float *output = OUT(0);

	float phase= unit->phase_;

	float temp;

	float lastoutput = unit->lastoutput_;
	float leakycoefficient = ZIN0(1);
	float calc;

	float bipolar = unit->bipolar_;

	for (int i=0; i<inNumSamples; ++i) {

		phase -= 1.0f;

		if (phase>=2.0f) {

			calc = 0.0f;

		} else if (phase>=1.0f) {

			temp= 2.0f-phase;

			calc = 0.16666666666667f*temp*temp*temp*bipolar;

		} else if (phase>=0.0f) {

			temp= phase*phase;

			calc = (0.66666666666666f - temp + (0.5f*temp*phase))*bipolar;

		} else if (phase>= -1.0f) {

			temp= phase*phase;

			calc = (0.66666666666666f - temp - (0.5f*temp*phase))*bipolar;

		} else if (phase>= - 2.0f) {

			temp= 2.0f+phase;

			calc = 0.16666666666667f*temp*temp*temp*bipolar;

		} else {

			calc= 0.0f;

			//recalculate parameters
			float freq = ZIN0(0);

			if(freq<0.000001f) freq= 0.000001f;

			//need period in samples
			float period = (SAMPLERATE/freq)*0.5f; //always half period because of bipolarism

			//no dc offset issues here hopefully
			if (period<=1.0f) period = 1.0f; //otherwise would have runaway fall off of phase value and incorrect integration

			//printf("hello phase %f period %f dc %f leaky %f \n",phase, period, dcoffset, leakycoefficient);

			bipolar *= -1.0f; //swap sign

			phase += period; //(period-2.0);
		}

		calc += (lastoutput*leakycoefficient);

		output[i]= calc;
		lastoutput = calc;

	}


	unit->phase_ = phase;
	unit->lastoutput_ = lastoutput;
	unit->bipolar_ = bipolar;
}




void BlitB3Tri_Ctor( BlitB3Tri* unit ) {

	unit->phase_ = 3.0f;
	//implies initial period is 20.0f

	//unit->leakycoefficient_ = 0.99;
	//unit->leakycoefficient2_ = 0.99;

	unit->lastoutput_ = -0.5f;
	unit->lastoutput2_ = 0.0f;

	unit->bipolar_ = 1.0f; //start positive, will flip each time

	unit->scalefactor_ = 4.0f/20.0f; //start positive, will flip each time


	//float freq = ZIN0(0);

	SETCALC(BlitB3Tri_next);
}



void BlitB3Tri_next( BlitB3Tri *unit, int inNumSamples ) {

	//int numSamples = unit->mWorld->mFullRate.mBufLength;

	//float *input = IN(0);
	float *output = OUT(0);

	float phase= unit->phase_;

	float temp;

	float lastoutput = unit->lastoutput_;
	float lastoutput2 = unit->lastoutput2_;
	float leakycoefficient = ZIN0(1);
	float leakycoefficient2 = ZIN0(2);
	float calc;

	float bipolar = unit->bipolar_;
	float scalefactor = unit->scalefactor_;

	for (int i=0; i<inNumSamples; ++i) {

		phase -= 1.0f;

		if (phase>=2.0f) {

			calc = 0.0f;

		} else if (phase>=1.0f) {

			temp= 2.0f-phase;

			calc = 0.16666666666667f*temp*temp*temp*bipolar;

		} else if (phase>=0.0f) {

			temp= phase*phase;

			calc = (0.66666666666666f - temp + (0.5f*temp*phase))*bipolar;

		} else if (phase>= -1.0f) {

			temp= phase*phase;

			calc = (0.66666666666666f - temp - (0.5f*temp*phase))*bipolar;

		} else if (phase>= - 2.0f) {

			temp= 2.0f+phase;

			calc = 0.16666666666667f*temp*temp*temp*bipolar;

		} else {

			calc= 0.0f;

			//recalculate parameters
			float freq = ZIN0(0);

			if(freq<0.000001f) freq= 0.000001f;

			//need period in samples
			float period = (SAMPLERATE/freq)*0.5f; //always half period because of bipolarism

			//no dc offset issues here hopefully
			if (period<=1.0) period = 1.0; //otherwise would have runaway fall off of phase value and incorrect integration

			//printf("hello phase %f period %f dc %f leaky %f \n",phase, period, dcoffset, leakycoefficient);

			//some compensation, pretty uniform, better than having higher frequencies louder as paper has it
			scalefactor = 0.25f; //2.0/period;

			bipolar *= -1.0f; //swap sign

			phase += period; //(period-2.0);
		}

		calc += (lastoutput*leakycoefficient);

		lastoutput = calc;

		calc += (lastoutput2*leakycoefficient2);

		lastoutput2 = calc;

		output[i]= calc*scalefactor;

	}


	unit->phase_ = phase;
	unit->lastoutput_ = lastoutput;
	unit->lastoutput2_ = lastoutput2;
	unit->bipolar_ = bipolar;
	unit->scalefactor_ = scalefactor;
}








/*
 void DPW4Saw_Ctor( DPW4Saw* unit ) {

 unit->phase_ = 0.0f;
 unit->scalefactor_ = 1.0f;

 for (int i=0; i<3; ++i)
 unit->differentiations_[i] = 0.0f;

 SETCALC(DPW4Saw_next);
 }



 //void BlitB3_Dtor(BlitB3 *unit)
 //{
 //
 //
 //}




 void DPW4Saw_next( DPW4Saw *unit, int inNumSamples ) {

 float *output = OUT(0);

 //can change frequency at control rate once per block
 float freq = ZIN0(0);

 if(freq<0.000001f) freq= 0.000001f;

 //need period in samples too
 float phasestep = freq/SAMPLERATE;
 float period = 1.0f/phasestep;

 //4th order
 float ampcompensation = 0.0052083333333333f*period*period*period; //basic formula, other formula involves sine

 //3rd order
 //float ampcompensation = 0.041666666666667*period*period; //basic formula, other formula involves sine

 float phase= unit->phase_;

 float * differentiations= unit->differentiations_;
 float diff1 = differentiations[0];
 float diff2 = differentiations[1];
 float diff3 = differentiations[2];

 float temp, temp2;


 //if frequency less than 500Hz, use DPW2, else DPW4

 for (int i=0; i<inNumSamples; ++i) {

 phase += phasestep;
 if (phase >= 1.0f) phase -= 1.0f;

 temp= 2.f*phase- 1.f;

 temp = temp*temp;

 temp = temp*temp - (2.f*temp);

 //approximation works for x^2 and one differentiation

 //now three differentiations to return it to linear from x^4- 2x^2

 //		temp2 = temp*period;
 //		temp = temp2 - diff1;
 //		diff1 = temp2;
 //
 //		temp2 = temp*period;
 //		temp = temp2 - diff2;
 //		diff2= temp2;
 //
 //		temp2 = temp*period;
 //		temp = temp2 - diff3;
 //		diff3= temp2;
 //
 temp2 = temp;
 temp = temp2 - diff1;
 diff1 = temp2;

 temp2 = temp;
 temp = temp2 - diff2;
 diff2= temp2;

 temp2 = temp;
 temp = temp2 - diff3;
 diff3= temp2;

 //output[i]= 0.0052083333333333f*temp; //ampcompensation*temp;
 output[i]= ampcompensation*temp;
 //output[i]= temp;
 }

 //printf("hello phase %f period %f\n",phase, period);

 unit->phase_ = phase;
 differentiations[0] = diff1;
 differentiations[1] = diff2;
 differentiations[2] = diff3;
 }
 */


void DPW4Saw_Ctor( DPW4Saw* unit ) {

	unit->phase_ = 0.5;
	//unit->scalefactor_ = 1.0;

	for (int i=0; i<3; ++i)
		unit->differentiations_[i] = 0.0;

	for (int i=0; i<2; ++i)
		unit->differentiations2_[i] = 0.0;

	SETCALC(DPW4Saw_next);
}



//void BlitB3_Dtor(BlitB3 *unit)
//{
//
//
//}




void DPW4Saw_next( DPW4Saw *unit, int inNumSamples ) {

	float *output = OUT(0);

	//can change frequency at control rate once per block
	double freq = ZIN0(0);

	if(freq<0.000001) freq= 0.000001;

	//need period in samples too
	double phasestep = freq/SAMPLERATE;
	double period = 1.0/phasestep;

	//4th order
	double ampcompensation = 0.0052083333333333*period*period*period; //basic formula, other formula involves sine
	//if split up normalisation factors, get more artefacts on transients!

	double ampcompensation2 = 0.25*period; //basic formula, other formula involves sine

	//3rd order
	//float ampcompensation = 0.041666666666667*period*period; //basic formula, other formula involves sine

	double phase= unit->phase_;

	double * differentiations= unit->differentiations_;
	double diff1 = differentiations[0];
	double diff2 = differentiations[1];
	double diff3 = differentiations[2];

	//for DPW2
	double * differentiations2= unit->differentiations2_;
	double diff4 = differentiations2[0];
	double diff5 = differentiations2[1];

	double temp, temp2;


	//if frequency less than 500Hz, use DPW2, else DPW4

//	if(freq>600.0) {
//
//		for (int i=0; i<inNumSamples; ++i) {
//
//			phase += phasestep;
//			if (phase > 1.0) phase -= 1.0;
//
//			temp= 2.0*phase- 1.0;
//
//			temp = temp*temp;
//
//			temp = temp*temp - (2.0*temp);
//
//			//now three differentiations to return it to linear from x^4- 2x^2
//			temp2 = temp;
//			temp = temp2 - diff1;
//			diff1 = temp2;
//
//			temp2 = temp;
//			temp = temp2 - diff2;
//			diff2= temp2;
//
//			temp2 = temp;
//			temp = temp2 - diff3;
//			diff3= temp2;
//
//			//output[i]= 0.0052083333333333f*temp; //ampcompensation*temp;
//			output[i]= ampcompensation*temp;
//			//output[i]= temp;
//		}
//
//	} else if (freq>400.0) {

	//crossfade region between 400 and 600 Hz, linear for simplicity, could use g *= g
//	double g= (freq-400.0)*0.005;
//	g *= g;
//	double oneminusg = 1.0-g;
//
	//must calculate both and crossfade; need to calculate both all the time to avoid click issues from phase mismatch, thankyou Jussi Pekonen
	double g;
	double oneminusg;

	if(freq>600.0) {
		g=1.0;
		oneminusg = 0.0;

	} else if (freq>400.0) {
		//crossfade region between 400 and 600 Hz, linear for simplicity, could use g *= g
		g= (freq-400.0)*0.005;
		g *= g;
		oneminusg = 1.0-g;
	} else {

		g=0.0;
		oneminusg = 1.0;

	}

		for (int i=0; i<inNumSamples; ++i) {

			phase += phasestep;
			if (phase > 1.0) phase -= 1.0;

			temp= 2.0*phase- 1.0;

			temp = temp*temp;

			//calculate DPW2 here
			//delay of one sample required
			double dpw2= diff5;

			//now one differentiation to return it to linear from x^2
			temp2 = temp;
			diff5 = temp2 - diff4;
			diff4 = temp2;

			//output[i]= 0.0052083333333333f*temp; //ampcompensation*temp;

			diff5*= ampcompensation2;

			temp = temp*temp - (2.0*temp);

			//now three differentiations to return it to linear from x^4- 2x^2
			temp2 = temp;
			temp = temp2 - diff1;
			diff1 = temp2;

			temp2 = temp;
			temp = temp2 - diff2;
			diff2= temp2;

			temp2 = temp;
			temp = temp2 - diff3;
			diff3= temp2;

			temp *= ampcompensation;

			//output[i]= 0.0052083333333333f*temp; //ampcompensation*temp;
			output[i]= (g*temp) + (oneminusg*dpw2);
			//output[i]= temp;
		}

//	} else {
//
//		//just DPW2
//
//		for (int i=0; i<inNumSamples; ++i) {
//
//			phase += phasestep;
//			if (phase > 1.0) phase -= 1.0;
//
//			temp= 2.0*phase- 1.0;
//
//			temp = temp*temp;
//
//			//now one differentiation to return it to linear from x^2
//			temp2 = temp;
//			temp = temp2 - diff4;
//			diff4 = temp2;
//
//			//output[i]= 0.0052083333333333f*temp; //ampcompensation*temp;
//			output[i]= diff5;
//			//delay of one sample required
//			diff5= ampcompensation2*temp;
//			//output[i]= temp;
//		}
//	}


	//printf("hello phase %f period %f\n",phase, period);

	unit->phase_ = phase;
	differentiations[0] = diff1;
	differentiations[1] = diff2;
	differentiations[2] = diff3;
	differentiations2[0] = diff4;
	differentiations2[1] = diff5;
}










/*

void DPW3Tri_Ctor( DPW3Tri* unit ) {

	unit->phase_ = 0.75f; //want it to start at (0.5-|2*phase-1|), ie zero

	for (int i=0; i<2; ++i)
		unit->differentiations_[i] = 0.0f;

	SETCALC(DPW3Tri_next);
}





void DPW3Tri_next( DPW3Tri *unit, int inNumSamples ) {

	float *output = OUT(0);

	//can change frequency at control rate once per block
	float freq = ZIN0(0);

	if(freq<0.000001f) freq= 0.000001f;

	//need period in samples too
	float phasestep = freq/SAMPLERATE;
	float period = 1.0f/phasestep;

	//3rd order: 2c for triangle, where c is scale factor for DPW sawtooth
	float ampcompensation = 0.083333333333333f*period*period; //basic formula, other formula involves sine

	//sine formula, too much sapping of amplitude at low frequencies
	//float ampcompensation = 2.0f*sin(pi/period);
	//ampcompensation = 1.6449340668482f/(ampcompensation*ampcompensation); //basic formula, other formula involves sine


	float phase= unit->phase_;

	float * differentiations= unit->differentiations_;
	float diff1 = differentiations[0];
	float diff2 = differentiations[1];

	float temp, temp2;


	//if frequency less than 500Hz, use DPW2, else DPW4

	for (int i=0; i<inNumSamples; ++i) {

		phase += phasestep;
		if (phase >= 1.0f) phase -= 1.0f;

		temp= 2.f*phase- 1.f;

		//absolute value
		temp2 = temp>0.0f?(0.5f-temp):(0.5f+temp);

		temp = temp2*temp2;

		temp = (temp-0.75f)*temp2;

		//now two differentiations

		temp2 = temp;
		temp = temp2 - diff1;
		diff1 = temp2;

		temp2 = temp;
		temp = temp2 - diff2;
		diff2= temp2;

		output[i]= ampcompensation*temp;

	}

	//printf("hello phase %f period %f\n",phase, period);

	unit->phase_ = phase;
	differentiations[0] = diff1;
	differentiations[1] = diff2;

}

*/


void DPW3Tri_Ctor( DPW3Tri* unit ) {

	unit->phase_ = 0.75; //want it to start at (0.5-|2*phase-1|), ie zero

	for (int i=0; i<2; ++i)
		unit->differentiations_[i] = 0.0;

	SETCALC(DPW3Tri_next);
}





void DPW3Tri_next( DPW3Tri *unit, int inNumSamples ) {

	float *output = OUT(0);

	//can change frequency at control rate once per block
	double freq = ZIN0(0);

	if(freq<0.000001) freq= 0.000001;

	//need period in samples too
	double phasestep = freq/SAMPLERATE;
	double period = 1.0/phasestep;

	//3rd order: 2c for triangle, where c is scale factor for DPW sawtooth
	double ampcompensation = 0.083333333333333*period*period; //basic formula, other formula involves sine

	//sine formula, too much sapping of amplitude at low frequencies
	//float ampcompensation = 2.0f*sin(pi/period);
	//ampcompensation = 1.6449340668482f/(ampcompensation*ampcompensation); //basic formula, other formula involves sine


	double phase= unit->phase_;

	double * differentiations= unit->differentiations_;
	double diff1 = differentiations[0];
	double diff2 = differentiations[1];

	double temp, temp2;


	//if frequency less than 500Hz, use DPW2, else DPW4

	for (int i=0; i<inNumSamples; ++i) {

		phase += phasestep;
		if (phase >= 1.0) phase -= 1.0;

		temp= 2.0*phase- 1.0;

		//absolute value
		temp2 = temp>0.0?(0.5-temp):(0.5+temp);

		temp = temp2*temp2;

		temp = (temp-0.75)*temp2;

		//now two differentiations

		temp2 = temp;
		temp = temp2 - diff1;
		diff1 = temp2;

		temp2 = temp;
		temp = temp2 - diff2;
		diff2= temp2;

		output[i]= ampcompensation*temp;

	}

	//printf("hello phase %f period %f\n",phase, period);

	unit->phase_ = phase;
	differentiations[0] = diff1;
	differentiations[1] = diff2;

}


PluginLoad(AntiAliasingOscillators) {

	ft = inTable;

	DefineSimpleUnit(BlitB3);

	DefineSimpleUnit(BlitB3Saw);

	DefineSimpleUnit(BlitB3Square);

	DefineSimpleUnit(BlitB3Tri);

	DefineSimpleUnit(DPW4Saw);

	DefineSimpleUnit(DPW3Tri);

}
