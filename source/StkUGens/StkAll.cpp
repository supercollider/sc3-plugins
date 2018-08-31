#include "SC_PlugIn.h"

#define MY_FLOAT float

#include <Skini.h>  // caps
#ifdef STK_SKINIMSG
	// STK up to release version 4.5.0
	#include <SKINI.msg>
#else
	// later versions of STK
	#include <SKINImsg.h>
#endif

#include <OneZero.h>
#include <OnePole.h>
#include <DelayA.h>
#include <Noise.h>

#include <VoicForm.h>
#include <Phonemes.h>

#include <BandedWG.h>
#include <BeeThree.h>
#include <BlowHole.h>
#include <Bowed.h>
#include <Clarinet.h>
#include <Flute.h>
#include <ModalBar.h>
#include <Moog.h>
#include <Saxofony.h>
#include <Shakers.h>
#include <Mandolin.h>
#include <Sitar.h>
#include <StifKarp.h>
#include <TubeBell.h>

#include <Stk.h>
#include "STKAlloc.h"

#include <math.h>

#define RGET \
        RGen& rgen = *unit->mParent->mRGen; \
        uint32 s1 = rgen.s1; \
        uint32 s2 = rgen.s2; \
        uint32 s3 = rgen.s3;

#define RPUT \
        rgen.s1 = s1; \
        rgen.s2 = s2; \
        rgen.s3 = s3;

using namespace stk;

static InterfaceTable *ft;
 struct StkBandedWG : public Unit
{
	BandedWG *bandedWG;
	int		lastperiod;
	float   trig;
	float   instr;
	float   bowpressure;
	float   bowmotion;
	float   integration;
	float   modalresonance;
	float   bowvelocity;
	float   setstriking;

	};

 struct StkBeeThree : public Unit
{
	BeeThree *beethree;
	float op4gain;
	float op3gain;
	float lfospeed;
	float lfodepth;
	float adsrtarget;
	float trig;
	};

/*
       - Reed Stiffness = 2
       - Noise Gain = 4
       - Tonehole State = 11
       - Register State = 1
       - Breath Pressure = 128
*/
  struct StkBlowHole : public Unit
{
	BlowHole *blowhole;
	float freq;
	float reedstiffness;
	float noisegain;
	float tonehole;
	float rgister;
	float breathpressure;
	float trig;
};

  struct StkBowed : public Unit
{
	Bowed *bowed;
	float freq;
	float bowpressure;
	float bowposition;
	float vibfreq;
	float vibgain;
	float loudness;
	bool  gate;
};

/*
    Control Change Numbers:
       - Reed Stiffness = 2
       - Noise Gain = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Breath Pressure = 128
*/
  struct StkClarinet : public Unit
{
	Clarinet *clarinet;
	float freq;
	float reedstiffness;
	float noisegain;
	float vibfreq;
	float vibgain;
	float breathpressure;
	float trig;
};

  struct StkFlute : public Unit
{
	Flute *flute;
	float jetdelay;
	float noisegain;
	float vibfreq;
	float vibgain;
	float breathpressure;
	float freq;
	float trig;
};

 struct StkModalBar : public Unit
{
	ModalBar *modalBar;
	float   trig;
	float   instrument;
	float   stickhardness;
	float   stickposition;
	float   vibratogain;
	float   vibratofreq;
	float   directstickmix;
	float   volume;

	};

 struct StkMoog : public Unit
{
	Moog *moog;
	float filterQ;
	float sweeprate;
	float vibfreq;
	float vibgain;
	float gain;
	float freq;
	float trig;

	};

  struct StkPluck : public Unit
{
	uint32 length;
        float  loopGain;
	float  gain;
	DelayA *delayLine;
	OneZero *loopFilter;
	OnePole *pickFilter;
	Noise  *noise;

};

/*
    Control Change Numbers:
       - Reed Stiffness = 2
       - Reed Aperture = 26
       - Noise Gain = 4
       - Blow Position = 11
       - Vibrato Frequency = 29
       - Vibrato Gain = 1
       - Breath Pressure = 128
*/
struct StkSaxofony : public Unit
{
	Saxofony *saxofony;
	float frequency;
	float reedstiffness;
	float reedaperture;
	float noisegain;
	float blowposition;
	float vibratofrequency;
	float vibratogain;
	float breathpressure;
	float trig;
	};

 struct StkShakers : public Unit
{
	Shakers *shakers;
	float freq;
	float shakernumber;
	float energy;
	float systemdecay;
	float numobjects;
	float resfreq;
	float trigger;
	};

 struct StkVoicForm : public Unit
{
	VoicForm *voiceForm;
	float freq;
	float vuvmix;
	float vowelphon;
	float vibfreq;
	float vibgain;
	float loudness;
	float trig;

	};

struct StkMandolin : public Unit
{
	Mandolin *mandolin;
	float bodysize;
	float pickposition;
	float stringdamping;
	float stringdetune;
	float aftertouch;
	float trig;
 };

struct StkSitar : public Unit
{
	Sitar *sitar;
	float trig;
  };

/*
    Control Change Numbers:
       - Pickup Position = 4
       - String Sustain = 11
       - String Stretch = 1
*/
  struct StkStifKarp : public Unit
{
	StifKarp *stifkarp;
	float freq;
	float pickuppos;
	float stringsustain;
	float stringstretch;
 };

  struct StkTubeBell : public Unit
{
	TubeBell *tubebell;
};

 struct Sflute : public Unit
{
	uint32 delay1Length;
	uint32 delay2Length;
	float *delay1;
	float *delay2;
	float delay3;
	uint32 delay1Pos;
	uint32 delay2Pos;
	bool lenwrapped;
	bool lipwrapped;
};

extern "C"
{
        void StkBandedWG_next(StkBandedWG *unit, int inNumSamples);
        void StkBandedWG_Ctor(StkBandedWG* unit);
        void StkBandedWG_Dtor(StkBandedWG* unit);

        void StkBeeThree_next(StkBeeThree *unit, int inNumSamples);
        void StkBeeThree_Ctor(StkBeeThree* unit);
        void StkBeeThree_Dtor(StkBeeThree* unit);

        void StkBlowHole_next(StkBlowHole *unit, int inNumSamples);
        void StkBlowHole_Ctor(StkBlowHole* unit);
        void StkBlowHole_Dtor(StkBlowHole* unit);

        void StkBowed_next(StkBowed *unit, int inNumSamples);
        void StkBowed_Ctor(StkBowed* unit);
        void StkBowed_Dtor(StkBowed* unit);

        void StkClarinet_next(StkClarinet *unit, int inNumSamples);
        void StkClarinet_Ctor(StkClarinet* unit);
        void StkClarinet_Dtor(StkClarinet* unit);

        void StkFlute_next(StkFlute *unit, int inNumSamples);
        void StkFlute_Ctor(StkFlute* unit);
        void StkFlute_Dtor(StkFlute* unit);

	void StkModalBar_next(StkModalBar *unit, int inNumSamples);
        void StkModalBar_Ctor(StkModalBar* unit);
        void StkModalBar_Dtor(StkModalBar* unit);

        void StkMoog_next(StkMoog *unit, int inNumSamples);
        void StkMoog_Ctor(StkMoog* unit);
        void StkMoog_Dtor(StkMoog* unit);

        void StkPluck_next_notfull(StkPluck *unit, int inNumSamples);
        void StkPluck_Ctor(StkPluck* unit);
        void StkPluck_Dtor(StkPluck* unit);

        void StkSaxofony_next(StkSaxofony *unit, int inNumSamples);
        void StkSaxofony_Ctor(StkSaxofony* unit);
        void StkSaxofony_Dtor(StkSaxofony* unit);

        void StkShakers_next(StkShakers *unit, int inNumSamples);
        void StkShakers_Ctor(StkShakers* unit);
        void StkShakers_Dtor(StkShakers* unit);

        void StkVoicForm_next(StkVoicForm *unit, int inNumSamples);
        void StkVoicForm_Ctor(StkVoicForm* unit);
        void StkVoicForm_Dtor(StkVoicForm* unit);

        void StkMandolin_next(StkMandolin *unit, int inNumSamples);
        void StkMandolin_Ctor(StkMandolin* unit);
        void StkMandolin_Dtor(StkMandolin* unit);

        void StkSitar_next(StkSitar *unit, int inNumSamples);
        void StkSitar_Ctor(StkSitar* unit);
        void StkSitar_Dtor(StkSitar* unit);

        void StkStifKarp_next(StkStifKarp *unit, int inNumSamples);
        void StkStifKarp_Ctor(StkStifKarp* unit);
        void StkStifKarp_Dtor(StkStifKarp* unit);

        void StkTubeBell_next(StkTubeBell *unit, int inNumSamples);
        void StkTubeBell_Ctor(StkTubeBell* unit);
        void StkTubeBell_Dtor(StkTubeBell* unit);

        void Sflute_next(Sflute *unit, int inNumSamples);
        void Sflute_Ctor(Sflute* unit);
        void Sflute_Dtor(Sflute* unit);

};

//////////////////////////////////////////////////////////////////

void StkBandedWG_Ctor(StkBandedWG* unit)
{
// p0=freq,p2=bowpressure, p3=bowmotion, p4=strikeposition, p5=vibratofreq, p6=gain, p7=bowvelocity, p8=setstriking

	//unit->bandedWG = new BandedWG();
	STKAlloc0(unit->mWorld,  unit->bandedWG, BandedWG);

/******
     Control Change Numbers:
       - Bow Pressure = 2
       - Bow Motion = 4
       - Strike Position = 8 (not implemented)
       - Vibrato Frequency = 11
       - Gain = 1
       - Bow Velocity = 128
       - Set Striking = 64
       - Instrument Presets = 16
         - Uniform Bar = 0
         - Tuned Bar = 1
         - Glass Harmonica = 2
         - Tibetan Bowl = 3

  	unit->bandedWG->noteOn(IN0(0) ,1);
	unit->trig = 1;
	unit->bandedWG->controlChange((int)16,unit->instr = IN0(1));
	unit->bandedWG->controlChange((int)2,unit->bowpressure = IN0(2));// bow pressure
	unit->bandedWG->controlChange((int)4,unit->bowmotion = IN0(3)); // bow motion
	unit->bandedWG->controlChange((int)11, unit->integration = IN0(4)); // vibrato frequency?  integration
	unit->bandedWG->controlChange((int)1,unit->modalresonance = IN0(5)); // gain/ modal resonance
	unit->bandedWG->controlChange((int)128,unit->bowvelocity = IN0(6)); //bow velocity
	unit->bandedWG->controlChange((int)64,unit->setstriking = IN0(7));  // set striking
 	SETCALC(StkBandedWG_next);
	StkBandedWG_next(unit, 1);
******/

        unit->bandedWG->controlChange((int)16, IN0(1));

        unit->bandedWG->controlChange((int)2, IN0(2));// bow pressure
        unit->bandedWG->controlChange((int)4, IN0(3)); // bow motion
        unit->bandedWG->controlChange((int)11, IN0(4)); // vibrato frequency?  integration
        unit->bandedWG->controlChange((int)1, IN0(5)); // gain/ modal resonance
        unit->bandedWG->controlChange((int)128, IN0(6)); //bow velocity
        unit->bandedWG->controlChange((int)64, IN0(7));  // set striking

        unit->bandedWG->noteOn(IN0(0) ,1);
        SETCALC(StkBandedWG_next);
        StkBandedWG_next(unit, 1);

//printf("%f %f %f %f %f %f %f %f\n", IN0(0),IN0(1),IN0(2),IN0(3),IN0(4),IN0(5),IN0(6),IN0(7));
}

void StkBandedWG_next(StkBandedWG *unit, int inNumSamples)
{
	float *out = OUT(0);

 	if(IN0(8) > 0) {
		if(unit->trig < 0) {
				unit->bandedWG->noteOff( 0);
				//unit->bandedWG->clear();
			    unit->bandedWG->noteOn(IN0(0) ,1);
//printf("%f %f %f %f %f %f %f %f\n", IN0(0),IN0(1),IN0(2),IN0(3),IN0(4),IN0(5),IN0(6),IN0(7));

				if(unit->instr != IN0(1))  unit->bandedWG->controlChange((int)16,unit->instr = IN0(1));
				if(unit->bowpressure != IN0(2)) unit->bandedWG->controlChange((int)2,unit->bowpressure = IN0(2));
				if(unit->bowmotion != IN0(3)) unit->bandedWG->controlChange((int)4,unit->bowmotion = IN0(3));
				if(unit->integration != IN0(4)) unit->bandedWG->controlChange((int)11,unit->integration = IN0(4));
				if(unit->modalresonance != IN0(5)) unit->bandedWG->controlChange((int)1,unit->modalresonance = IN0(5));
				if(unit->bowvelocity != IN0(6)) unit->bandedWG->controlChange((int)128,unit->bowvelocity = IN0(6));
				if(unit->setstriking != IN0(7)) unit->bandedWG->controlChange((int)64,unit->setstriking = IN0(7));
				};
			};
	unit->trig = IN0(8);

 	for (int i=0; i < inNumSamples; ++i)
	{
		out[i] = unit->bandedWG->tick();
         }
 }

void StkBandedWG_Dtor(StkBandedWG* unit)
{
	//unit->bandedWG->clear();
	//delete unit->bandedWG;
	unit->bandedWG->~BandedWG();
	RTFree(unit->mWorld, unit->bandedWG);
}



void StkBeeThree_Ctor(StkBeeThree* unit)
{
/********

    Control Change Numbers:
       - Operator 4 (feedback) Gain = 2
       - Operator 3 Gain = 4
       - LFO Speed = 11
       - LFO Depth = 1
       - ADSR 2 & 4 Target = 128

*****/

	STKAlloc0(unit->mWorld,  unit->beethree, BeeThree);

	unit->beethree->controlChange((int)2,unit->op4gain=IN0(1));// operator4gain
	unit->beethree->controlChange((int)4,unit->op3gain=IN0(2));// operator3gain
	unit->beethree->controlChange((int)11,unit->lfospeed=IN0(3)); // lfospeed
	unit->beethree->controlChange((int)1,unit->lfodepth=IN0(4)); // lfodepth
	unit->beethree->controlChange((int)128,unit->adsrtarget=IN0(5)); // adsrtarget

 	unit->beethree->noteOn(IN0(0) ,1);
	unit->trig = IN0(6);
 	SETCALC(StkBeeThree_next);
	StkBeeThree_next(unit, 1);
}

void StkBeeThree_next(StkBeeThree *unit, int inNumSamples)
{
	float *out=OUT(0);
	float in00=IN0(0);
	float in01=IN0(1);
	float in02=IN0(2);
	float in03=IN0(3);
	float in04=IN0(4);
	float in05=IN0(5);

 	if(IN0(6) > 0) {
		if(unit->trig < 0) {
 				unit->beethree->noteOff( 0);
			    unit->beethree->noteOn(IN0(0) ,1);

	};
			};
	unit->trig = in05;

	if(in00 != unit->op4gain) unit->beethree->setFrequency(unit->op4gain = in00);
	if(in01 != unit->op3gain) unit->beethree->controlChange(2,unit->op3gain =  in01);
	if(in02 != unit->lfospeed) unit->beethree->controlChange(4,unit->lfospeed = in02);
	if(in03 != unit->lfodepth) unit->beethree->controlChange(11,unit->lfodepth =  in03);
	if(in04 != unit->adsrtarget) unit->beethree->controlChange(1, unit->adsrtarget = in04);

 	for (int i=0; i < inNumSamples; ++i)
	{
		out[i] = unit->beethree->tick();
        }
 }

void StkBeeThree_Dtor(StkBeeThree* unit)
{
	//unit->BeeThree->clear();
	//delete unit->BeeThree;
	unit->beethree->~BeeThree();
	RTFree(unit->mWorld, unit->beethree);
}

//////////////////////////////////////////////////////////////////

void StkBlowHole_Ctor(StkBlowHole* unit)
{
 	STKAlloc(unit->mWorld, unit->blowhole, BlowHole, 40);

	unit->freq = unit->reedstiffness = unit->noisegain = unit->tonehole = unit->rgister = unit->breathpressure = 0;
	unit->trig = 1;
	unit->blowhole->noteOn(IN0(0) ,1);
	SETCALC(StkBlowHole_next);
	StkBlowHole_next(unit, 1);

}

void StkBlowHole_next(StkBlowHole *unit, int inNumSamples)
{
	float *out OUT(0);
	float in00=IN0(0);
	float in01=IN0(1);
	float in02=IN0(2);
	float in03=IN0(3);
	float in04=IN0(4);
	float in05=IN0(5);

  	if(IN0(6) > 0) {
		if(unit->trig < 0) {
				unit->blowhole->noteOff( 0);
			    unit->blowhole->noteOn(IN0(0) ,1);
				};
			};
	unit->trig = IN0(6);


	if(in00 != unit->freq) {
		unit->blowhole->setFrequency(in00);
		unit->freq = in00;};

	if(in01 != unit->reedstiffness) {
		unit->blowhole->controlChange(2, in01);
		unit->reedstiffness = in01; };

	if(in02 != unit->noisegain) {
		unit->blowhole->controlChange(4, in02);
		unit->noisegain = in02; };

	if(in03 != unit->tonehole) {
		unit->blowhole->controlChange(11, in03);
		unit->tonehole = in03; };

	if(in04 != unit->rgister) {
		unit->blowhole->controlChange(1, in04);
		unit->rgister = in04; };

	if(in04 != unit->breathpressure) {
		unit->blowhole->controlChange(128, in05);
		unit->breathpressure = in05; };


 	for (int i=0; i < inNumSamples; ++i)
	{
		out[i] = unit->blowhole->tick();
        }

}

void StkBlowHole_Dtor(StkBlowHole* unit)
{
	//delete unit->blowhole;
	unit->blowhole->~BlowHole();
    RTFree(unit->mWorld, unit->blowhole);

}



void StkBowed_Ctor(StkBowed* unit)
{
	//Stk :: setRawwavePath("/Users/paul/stk-4.1.3/rawwaves");
/*
      - Bow Pressure = 2
       - Bow Position = 4
       //- Vibrato Frequency = 11
       //- Vibrato Gain = 1
       - Volume = 128
*/
	STKAlloc(unit->mWorld, unit->bowed, Bowed, 40);

	unit->freq = unit->bowposition = unit->bowpressure = unit->loudness = 0;
	unit->bowed->noteOn(IN0(0) ,1);
	SETCALC(StkBowed_next);
	StkBowed_next(unit, 1);
	unit->gate = false;

}

void StkBowed_next(StkBowed *unit, int inNumSamples)
{
	float *out = OUT(0);

	float freq  	= IN0(0);
	float bowpressure	= IN0(1);
	float bowpos	= IN0(2);

	float vibfreq	= IN0(3);
	float vibgain	= IN0(4);

	float loudness	= IN0(5);
	bool  gate		= IN0(6) > 0.f;
	/*
 	if(gate > 0) {
		if(unit->gate <= 0) {

				unit->bowed->noteOff( 0);
			    unit->bowed->noteOn(freq, 1);
				};
			};
	unit->gate = gate;
	*/
	if(gate != unit->gate){
		if(gate){
			Print("Starting\n");
		    unit->bowed->noteOn(freq, 1000.f);
//			unit->bowed->startBowing(1.f, IN0(7)); // IN0(7) is attackrate
		}else{
			Print("Stopping\n");
			unit->bowed->noteOff(1000.f);
//			unit->bowed->stopBowing(IN0(8)); // IN0(8) is decayrate
		}
		unit->gate = gate;
	}

	if(freq != unit->freq) {
		unit->bowed->setFrequency(freq);
		unit->freq = freq;};

	if(bowpressure != unit->bowpressure) {
		unit->bowed->controlChange(__SK_BowPressure_, bowpressure);
		unit->bowpressure = bowpressure; };

	if(bowpos != unit->bowposition) {
		unit->bowed->controlChange(__SK_BowPosition_, bowpos);
		unit->bowposition = bowpos; };

	if(vibfreq != unit->vibfreq) {
		unit->bowed->controlChange(__SK_ModFrequency_, vibfreq);
		unit->vibfreq = vibfreq; };

	if(vibgain != unit->vibgain) {
		unit->bowed->controlChange(__SK_ModWheel_, vibgain);
		unit->vibgain = vibgain; };

	if(loudness != unit->loudness) {
		unit->bowed->controlChange(__SK_AfterTouch_Cont_, loudness);
		unit->loudness = loudness; };

 	for (int i=0; i < inNumSamples; ++i)
	{
		out[i] = unit->bowed->tick() * 7.5f; // Scaled to approx +-1
	}

}

void StkBowed_Dtor(StkBowed* unit)
{
	//delete unit->bowed;
	unit->bowed->~Bowed();
    RTFree(unit->mWorld, unit->bowed);

}

void StkClarinet_Ctor(StkClarinet* unit)
{
 	STKAlloc(unit->mWorld, unit->clarinet, Clarinet, 40);


 	unit->clarinet->controlChange(2, unit->reedstiffness = IN0(1));
	unit->clarinet->controlChange(4, unit->noisegain = IN0(2));
	unit->clarinet->controlChange(11,unit->vibfreq = IN0(3));
	unit->clarinet->controlChange(1, unit->vibgain = IN0(4));
	unit->clarinet->controlChange(128, unit->breathpressure = IN0(5));
	unit->clarinet->noteOn(IN0(0) ,1);
	SETCALC(StkClarinet_next);
	unit->trig = 1;

	StkClarinet_next(unit, 1);
}

void StkClarinet_next(StkClarinet *unit, int inNumSamples)
{
	float *out=OUT(0);
	float in00=IN0(0);
	float in01=IN0(1);
	float in02=IN0(2);
	float in03=IN0(3);
	float in04=IN0(4);
	float in05=IN0(5);

 	if(IN0(6) > 0) {
		if(unit->trig < 0) {
 				unit->clarinet->noteOff(0);
 			    unit->clarinet->noteOn(IN0(0) ,1);
	};
			};
	unit->trig = IN0(6);

	if(in00 != unit->freq) {
		unit->clarinet->setFrequency(in00);
		unit->freq = in00; };

	if(in01 != unit->reedstiffness) {
		unit->clarinet->controlChange(2, in01);
		unit->reedstiffness = in01; };

	if(in02 != unit->noisegain) {
		unit->clarinet->controlChange(4, in02);
		unit->noisegain = in02; };

	if(in03 != unit->vibfreq) {
		unit->clarinet->controlChange(11, in03);
		unit->vibfreq = in03; };

	if(in04 != unit->vibgain) {
		unit->clarinet->controlChange(1, in04);
		unit->vibgain = in04; };

	if(in05 != unit->breathpressure) {
		unit->clarinet->controlChange(128, in05);
		unit->breathpressure = in05; };

 	for (int i=0; i < inNumSamples; ++i)
	{
		out[i] = unit->clarinet->tick();
        }

}

void StkClarinet_Dtor(StkClarinet* unit)
{
	//delete unit->clarinet;
	unit->clarinet->~Clarinet();
    RTFree(unit->mWorld, unit->clarinet);

}


//////////////////////////////////////////////////////////////////

void StkFlute_Ctor(StkFlute* unit)
{
	STKAlloc(unit->mWorld, unit->flute, Flute, 40);

	unit->flute->setFrequency( unit->freq = IN0(0) );
	unit->flute->noteOn(IN0(0),1);

//	unit->flute->setJetDelay( unit->jetdelay = IN0(1) );
	unit->flute->controlChange(2, unit->jetdelay = IN0(1));// jet delay

	unit->flute->controlChange(4, unit->noisegain = IN0(2));// noisegain // this seems to have to be some kind of midi control value??

//	unit->flute->setJetReflection( unit->jetdelay = IN0(3) );
	unit->flute->controlChange(11, unit->vibfreq = IN0(3));// vibrato freq
	unit->flute->controlChange(1, unit->vibgain = IN0(4));// vibrato gain
	unit->flute->controlChange(128, unit->breathpressure = IN0(5));	// breathpressure
	SETCALC(StkFlute_next);

	StkFlute_next(unit, 1);
	unit->trig = 1;
}

void StkFlute_next(StkFlute *unit, int inNumSamples)
{
	float *out OUT(0);

 	if(IN0(6) > 0) {
		if(unit->trig < 0) {

			unit->flute->noteOff(0);
			unit->flute->noteOn(IN0(0) ,1);

		};
			};
	unit->trig = IN0(6);

	if(unit->freq != IN0(0)) unit->flute->setFrequency(unit->freq = IN0(0));

	if(unit->jetdelay != IN0(1)) unit->flute->controlChange(2, unit->jetdelay = IN0(1));
	if(unit->noisegain != IN0(2)) unit->flute->controlChange(4, unit->noisegain = IN0(2));
	if(unit->vibfreq != IN0(3)) unit->flute->controlChange(11, unit->vibfreq = IN0(3));
	if(unit->vibgain != IN0(4)) unit->flute->controlChange(1, unit->vibgain = IN0(4));
	if(unit->breathpressure != IN0(5)) unit->flute->controlChange(128, unit->breathpressure = IN0(5));

	for (int i=0; i < inNumSamples; ++i)
	{
		out[i] = unit->flute->tick();
        }

}

void StkFlute_Dtor(StkFlute* unit)
{
	//delete unit->flute;
	unit->flute->~Flute();
    RTFree(unit->mWorld, unit->flute);
}

//////////////////////////////////////////////////////////////////

void StkModalBar_Ctor(StkModalBar* unit)
{
/*
    Control Change Numbers:
       - Stick Hardness = 2
       - Stick Position = 4
       - Vibrato Gain = 1
       - Vibrato Frequency = 11
       - Direct Stick Mix = 8
       - Volume = 128
       - Modal Presets = 16
         - Marimba = 0
         - Vibraphone = 1
         - Agogo = 2
         - Wood1 = 3
         - Reso = 4
         - Wood2 = 5
         - Beats = 6
         - Two Fixed = 7
         - Clump = 8
*/

	//unit->modalBar = new ModalBar();
	STKAlloc0(unit->mWorld,  unit->modalBar, ModalBar);

	unit->modalBar->noteOn(IN0(0) ,1);

	unit->modalBar->controlChange(16,unit->instrument = IN0(1));// instrument
	unit->modalBar->controlChange(2, unit->stickhardness = IN0(2));// stickhardness
	unit->modalBar->controlChange(4, unit->stickposition = IN0(3));// stickposition
	unit->modalBar->controlChange(1, unit->vibratogain = IN0(4));// vibratogain
	unit->modalBar->controlChange(11, unit->vibratofreq = IN0(5));// vibratofreq
	unit->modalBar->controlChange(8, unit->directstickmix = IN0(6));// directstickmix
	unit->modalBar->controlChange(128, unit->volume = IN0(7));// volume
  	SETCALC(StkModalBar_next);
	StkModalBar_next(unit, 1);
	unit->trig = 1;
}

void StkModalBar_next(StkModalBar *unit, int inNumSamples)
{
	float *out=OUT(0);


 	if(IN0(8) > 0) {
		if(unit->trig < 0) {
				unit->modalBar->clear();
				if(unit->instrument != IN0(1)) unit->modalBar->controlChange(16,unit->instrument =IN0(1));
				if(unit->stickhardness != IN0(2)) unit->modalBar->controlChange(2, unit->stickhardness =IN0(2));
				if(unit->stickposition != IN0(3)) unit->modalBar->controlChange(4,unit->stickposition =IN0(3));
				if(unit->vibratogain != IN0(4)) unit->modalBar->controlChange(11,unit->vibratogain = IN0(4));
				if(unit->vibratofreq != IN0(5)) unit->modalBar->controlChange(7,unit->vibratofreq = IN0(5));
				if(unit->directstickmix != IN0(6)) unit->modalBar->controlChange(1,unit->directstickmix =IN0(6));
				if(unit->volume != IN0(7)) unit->modalBar->controlChange(128,unit->volume  = IN0(7));

				unit->modalBar->noteOff( 0);
			    unit->modalBar->noteOn(IN0(0) ,1);
				};
			};
	unit->trig = IN0(8);


 	for (int i=0; i < inNumSamples; ++i)
	{
		out[i] = unit->modalBar->tick();
    }
 }

void StkModalBar_Dtor(StkModalBar* unit)
{
	unit->modalBar->~ModalBar();
    RTFree(unit->mWorld, unit->modalBar);
}

//////////////////////////////////////////////////////////////////

void StkMoog_Ctor(StkMoog* unit)
{
/********
    Control Change Numbers:
       - Filter Q = 2
       - Filter Sweep Rate = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Gain = 128
*****/

	STKAlloc0(unit->mWorld,  unit->moog, Moog);

	unit->moog->controlChange((int)2, unit->filterQ = IN0(1));// filter Q
	unit->moog->controlChange((int)4, unit->sweeprate = IN0(2));// sweep rate
	unit->moog->controlChange((int)11, unit->vibfreq = IN0(3)); // vibrato freq
	unit->moog->controlChange((int)1, unit->vibgain = IN0(4)); // vibrato gain
	unit->moog->controlChange((int)128, unit->gain = IN0(5)); // gain

 	unit->moog->noteOn(unit->freq = IN0(0) ,1);
 	SETCALC(StkMoog_next);
	unit->trig = 1;
	StkMoog_next(unit, 1);
}

void StkMoog_next(StkMoog *unit, int inNumSamples)
{
	float *out=OUT(0);
	float in00=IN0(0);
	float in01=IN0(1);
	float in02=IN0(2);
	float in03=IN0(3);
	float in04=IN0(4);
	float in05=IN0(5);

 	if(IN0(6) > 0) {
		if(unit->trig < 0) {
 				unit->moog->noteOff( 0);
			    unit->moog->noteOn(IN0(0) ,1);

	};
			};
	unit->trig = IN0(6);

	if(in00 != unit->freq) unit->moog->setFrequency(unit->freq = in00);

	if(in01 != unit->filterQ) unit->moog->controlChange(2,unit->filterQ =  in01);

	if(in02 != unit->sweeprate) unit->moog->controlChange(4,unit->sweeprate = in02);

	if(in03 != unit->vibfreq) unit->moog->controlChange(11,unit->vibfreq =  in03);

	if(in04 != unit->vibgain) unit->moog->controlChange(1, unit->vibgain = in04);

	if(in05 != unit->gain) unit->moog->controlChange(128,unit->gain = in05);



 	for (int i=0; i < inNumSamples; ++i)
	{
		out[i] = unit->moog->tick();
        }
 }

void StkMoog_Dtor(StkMoog* unit)
{
	//unit->Moog->clear();
	//delete unit->Moog;
	unit->moog->~Moog();
	RTFree(unit->mWorld, unit->moog);
}

//////////////////////////////////////////////////////////////////



void StkPluck_Ctor(StkPluck* unit)
{

        SETCALC(StkPluck_next_notfull);

                unit->pickFilter = new OnePole;
                unit->delayLine = new DelayA;
                unit->loopFilter = new OneZero;
                unit->noise = new Noise;
                unit->delayLine->clear();
                unit->loopFilter->clear();
                unit->pickFilter->clear();

                // set lowest frequency
                unit->length = (long) (SAMPLERATE/ 60 + 1);

                // set frequency
                MY_FLOAT delay = (SAMPLERATE/ IN0(0)) - (MY_FLOAT) 0.5;

                if (delay <= 0.0) delay = 0.3;
                        else if (delay > unit->length) delay = unit->length;
                unit->delayLine->setDelay(delay);
                unit->loopGain = IN0(1) + (IN0(0) * 0.000005);

                if ( unit->loopGain >= 1.0 ) unit->loopGain = (MY_FLOAT) 0.99999;

                MY_FLOAT gain = 0.99;
                // set noiseburst
                unit->pickFilter->setPole((MY_FLOAT) 0.999 - (gain * (MY_FLOAT) 0.15));
                unit->pickFilter->setGain(gain * (MY_FLOAT) 0.5);
                for (uint32 i=0; i< unit->length; i++)
                        // Fill delay with noise additively with current contents.
                        unit->delayLine->tick( 0.6 * unit->delayLine->lastOut() + unit->pickFilter->tick( unit->noise->tick() ) );

                StkPluck_next_notfull(unit, 1);

}

void StkPluck_next_notfull(StkPluck *unit, int inNumSamples)
{
      float *out = OUT(0);

        for (int i=0; i < inNumSamples; ++i)
        {
        float z,y;
        z = unit->delayLine->tick(
                unit->loopFilter->tick(y= unit->delayLine->lastOut() * unit->loopGain ) );

           out[i] = z;
        }
}

void StkPluck_Dtor(StkPluck* unit)
{
                delete unit->delayLine;
                delete unit->loopFilter;
                delete unit->pickFilter;
                delete unit->noise;

}

//////////////////////////////////////////////////////////////////

void StkSaxofony_Ctor(StkSaxofony* unit)
{
	//Stk :: setRawwavePath("/Users/paul/stk-4.1.3/rawwaves");
	STKAlloc(unit->mWorld,unit->saxofony, Saxofony, 40);

	//unit->saxofony = new Saxofony((MY_FLOAT) 40); // 40 is lowest frequency
	unit->reedstiffness=unit->frequency=unit->reedaperture=unit->noisegain=
		unit->blowposition=unit->vibratofrequency=unit->vibratogain=unit->breathpressure=0;
	unit->saxofony->noteOn(IN0(0) ,1);
 	SETCALC(StkSaxofony_next);
	StkSaxofony_next(unit, 1);
	unit->trig = 1;
}

void StkSaxofony_next(StkSaxofony *unit, int inNumSamples)
{
	float *out OUT(0);

	float in00=IN0(0);
	float in01=IN0(1);
	float in02=IN0(2);
	float in03=IN0(3);
	float in04=IN0(4);
	float in05=IN0(5);
 	float in06=IN0(6);
 	float in07=IN0(7);

 	if(IN0(8) > 0) {
		if(unit->trig < 0) {
			unit->saxofony->noteOff( 0);
			unit->saxofony->noteOn(IN0(0) ,1);

		};
	};
	unit->trig = IN0(8);

	if(in00 != unit->frequency) {
		unit->saxofony->setFrequency(in00);
		unit->frequency = in00;};

	if(in01 != unit->reedstiffness) {
		unit->saxofony->controlChange(2, in01);
		unit->reedstiffness = in01; };

	if(in02 != unit->reedaperture) {
		unit->saxofony->controlChange(26, in02);
		unit->reedaperture = in02; };

	if(in03 != unit->noisegain) {
		unit->saxofony->controlChange(4, in03);
		unit->noisegain = in03; };

	if(in04 != unit->blowposition) {
		unit->saxofony->controlChange(11, in04);
		unit->blowposition = in04; };

	if(in05 != unit->vibratofrequency) {
		unit->saxofony->controlChange(29, in05);
		unit->vibratofrequency = in05; };

	if(in06 != unit->vibratogain) {
		unit->saxofony->controlChange(1, in06);
		unit->vibratogain = in06; };

	if(in07 != unit->breathpressure) {
		unit->saxofony->controlChange(128, in07);
		unit->breathpressure = in07; } ;


 	for (int i=0; i < inNumSamples; ++i)
	{
		out[i] = unit->saxofony->tick();
        }
 }

void StkSaxofony_Dtor(StkSaxofony* unit)
{
	//unit->saxofony->clear();
	//delete unit->saxofony;
    unit->saxofony->~Saxofony();
    RTFree(unit->mWorld, unit->saxofony);

}


//////////////////////////////////////////////////////////////////

void StkShakers_Ctor(StkShakers* unit)
{
// p0=freq, p1=shaker number, p2=energy, p3=system decay, p4=number of objects, p5 = resonance freq
//	unit->shakers = new Shakers();
	STKAlloc0(unit->mWorld,  unit->shakers, Shakers);

/******
    Control Change Numbers:
       - Shake Energy = 2
       - System Decay = 4
       - Number Of Objects = 11
       - Resonance Frequency = 1
       - Instrument Selection = 1071
        - Maraca = 0
        - Cabasa = 1
        - Sekere = 2
        - Guiro = 3
        - Water Drops = 4
        - Bamboo Chimes = 5
        - Tambourine = 6
        - Sleigh Bells = 7
        - Sticks = 8
        - Crunch = 9
        - Wrench = 10
        - Sand Paper = 11
        - Coke Can = 12
        - Next Mug = 13
        - Penny + Mug = 14
        - Nickle + Mug = 15
        - Dime + Mug = 16
        - Quarter + Mug = 17
        - Franc + Mug = 18
        - Peso + Mug = 19
        - Big Rocks = 20
        - Little Rocks = 21
        - Tuned Bamboo Chimes = 22
*****/

 	unit->shakers->noteOn(220 ,1);
	unit->shakers->controlChange((int)1071, unit->shakernumber = IN0(0));
	unit->shakers->controlChange((int)2, unit->energy = IN0(1));
	unit->shakers->controlChange((int)4, unit->systemdecay = IN0(2));
	unit->shakers->controlChange((int)11, unit->numobjects = IN0(3));
	unit->shakers->controlChange((int)1, unit->resfreq = IN0(4));
	unit->trigger = 1;
 	SETCALC(StkShakers_next);
	StkShakers_next(unit, 1);
}

void StkShakers_next(StkShakers *unit, int inNumSamples)
{
	float *out OUT(0);


 	if(IN0(5) > 0) {
		if(unit->trigger < 0) {
			unit->shakers->noteOff( 0);
			unit->shakers->noteOn(IN0(0) ,1);

		};
	};
	unit->trigger = IN0(5);
	if(unit->shakernumber != IN0(0)) unit->shakers->controlChange((int)1071, unit->shakernumber = IN0(0));
	if(unit->energy != IN0(1)) unit->shakers->controlChange((int)2, unit->energy = IN0(1));
	if(unit->systemdecay != IN0(2)) unit->shakers->controlChange((int)4, unit->systemdecay = IN0(2));
	if(unit->numobjects != IN0(3)) unit->shakers->controlChange((int)11, unit->numobjects = IN0(3));
	if(unit->resfreq != IN0(4)) unit->shakers->controlChange((int)1, unit->resfreq = IN0(4));


 	for (int i=0; i < inNumSamples; ++i)
	{
		out[i] = unit->shakers->tick();
        }
 }

void StkShakers_Dtor(StkShakers* unit)
{
//	delete unit->shakers;
	unit->shakers->~Shakers();
    RTFree(unit->mWorld, unit->shakers);

}


//////////////////////////////////////////////////////////////////

void StkVoicForm_Ctor(StkVoicForm* unit)
{
	//unit->voiceForm = new VoicForm();
	STKAlloc0(unit->mWorld,  unit->voiceForm, VoicForm);
	//unit->voiceForm = (VoicForm *)RTAlloc(unit->mWorld, sizeof(unit->voiceForm));

/*********************************
p0=freq,p1=vuvmix,p2=vowelphonsel,p3=vibfreq, p4=vibgain, p5=loudness, p6=trig
    Control Change Numbers:
       - Voiced/Unvoiced Mix = 2
       - Vowel/Phoneme Selection = 4
       - Vibrato Frequency = 11
       - Vibrato Gain = 1
       - Loudness (Spectral Tilt) = 128
***********************************/

	unit->voiceForm->controlChange(2,unit->vuvmix = IN0(1));
	unit->voiceForm->controlChange(4, unit->vowelphon=IN0(2));
	unit->voiceForm->controlChange(11,unit->vibfreq= IN0(3));
	unit->voiceForm->controlChange(1,unit->vibgain= IN0(4));
	unit->voiceForm->controlChange(128, unit->loudness=IN0(5));

 	unit->voiceForm->noteOn(unit->freq =IN0(0) ,1);
  	SETCALC(StkVoicForm_next);
	StkVoicForm_next(unit, 1);
	unit->trig = 1;
}

void StkVoicForm_next(StkVoicForm *unit, int inNumSamples)
{
	float *out=OUT(0);
	float in00=IN0(0);
	float in01=IN0(1);
	float in02=IN0(2);
	float in03=IN0(3);
	float in04=IN0(4);
	float in05=IN0(5);
/*******
 	if(IN0(6) > 0) {
		if(unit->trig < 0) {
 				//unit->voiceForm->noteOff(0);
 			    unit->voiceForm->noteOn(IN0(0) ,1);
printf("noteon %f\n", IN0(0));
		};
	};
	unit->trig = IN0(6);
**********/
	if(in00 != unit->freq) {
		unit->voiceForm->setFrequency(in00);
		unit->freq = in00;
		};

	if(in01 != unit->vuvmix) {
		unit->voiceForm->controlChange(2, in01);
		unit->vuvmix = in01; };

	if(in02 != unit->vowelphon) {
		unit->voiceForm->controlChange(4, in02);
		unit->vowelphon = in02; };

	if(in03 != unit->vibfreq) {
		unit->voiceForm->controlChange(11, in03);
		unit->vibfreq = in03; };

	if(in04 != unit->vibgain) {
		unit->voiceForm->controlChange(1, in04);
		unit->vibgain = in04; };

	if(in04 != unit->loudness) {
		unit->voiceForm->controlChange(128, in05);
		unit->vibgain = in05; };

 	for (int i=0; i < inNumSamples; ++i)
	{
		out[i] = unit->voiceForm->tick();
        }
 }

void StkVoicForm_Dtor(StkVoicForm* unit)
{
	//unit->voiceForm->clear();
	//delete unit->voiceForm;
	unit->voiceForm->~VoicForm();
    RTFree(unit->mWorld, unit->voiceForm);

}

//////////////////////////////////////////////////////////////////

void StkMandolin_Ctor(StkMandolin* unit)
{

	//unit->mandolin = new Mandolin((MY_FLOAT) 40);
	STKAlloc(unit->mWorld, unit->mandolin, Mandolin, 40);


	unit->mandolin->controlChange(__SK_BodySize_, unit->bodysize = IN0(1));
	unit->mandolin->controlChange(__SK_PickPosition_, unit->pickposition = IN0(2));
	unit->mandolin->controlChange(__SK_StringDamping_, unit->stringdamping = IN0(3));
	unit->mandolin->controlChange(__SK_StringDetune_, unit->stringdetune = IN0(4));
	unit->mandolin->controlChange(__SK_AfterTouch_Cont_, unit->aftertouch = IN0(5));
	unit->trig = 1;
	unit->mandolin->noteOn(IN0(0) ,1);
 	SETCALC(StkMandolin_next);
	StkMandolin_next(unit, 1);

}

void StkMandolin_next(StkMandolin *unit, int inNumSamples)
{
	float *out  = OUT(0);

 	if(IN0(6) > 0) {
		if(unit->trig < 0) {
			if(unit->bodysize != IN0(1)) unit->mandolin->controlChange(__SK_BodySize_, unit->bodysize = IN0(1));
			if(unit->pickposition != IN0(2)) unit->mandolin->controlChange(__SK_PickPosition_, unit->pickposition = IN0(2));
			if(unit->stringdamping != IN0(3)) unit->mandolin->controlChange(__SK_StringDamping_, unit->stringdamping = IN0(3));
			if(unit->stringdetune != IN0(4)) unit->mandolin->controlChange(__SK_StringDetune_, unit->stringdetune = IN0(4));
			if(unit->aftertouch != IN0(5)) unit->mandolin->controlChange(__SK_AfterTouch_Cont_, unit->aftertouch = IN0(5));

			unit->mandolin->noteOff( 0);
			unit->mandolin->noteOn(IN0(0) ,1);

		};
			};
	unit->trig = IN0(6);


 	for (int i=0; i < inNumSamples; ++i)
	{
		out[i] = unit->mandolin->tick();
        }
}

void StkMandolin_Dtor(StkMandolin* unit)
{
	//delete unit->mandolin;
	unit->mandolin->~Mandolin();
    RTFree(unit->mWorld, unit->mandolin);

}

//////////////////////////////////////////////////////////////////

void StkSitar_Ctor(StkSitar* unit)
{
 	STKAlloc(unit->mWorld, unit->sitar, Sitar, 40);
	unit->sitar->clear();
	unit->sitar->noteOn(IN0(0) ,1);
	SETCALC(StkSitar_next);
	unit->trig = 1;
	StkSitar_next(unit, 1);

}

void StkSitar_next(StkSitar *unit, int inNumSamples)
{
	float *out OUT(0);

 	if(IN0(1) > 0) {
		if(unit->trig < 0) {
				unit->sitar->noteOff( 0);
			    unit->sitar->noteOn(IN0(0) ,1);
				};
			};
	unit->trig = IN0(1);

 	for (int i=0; i < inNumSamples; ++i)
	{
		out[i] = unit->sitar->tick();
	}

}

void StkSitar_Dtor(StkSitar* unit)
{
	//delete unit->sitar;
	unit->sitar->~Sitar();
    RTFree(unit->mWorld, unit->sitar);

}


//////////////////////////////////////////////////////////////////

void StkStifKarp_Ctor(StkStifKarp* unit)
{
 	STKAlloc(unit->mWorld, unit->stifkarp, StifKarp, 40);

	unit->freq = unit->pickuppos = unit->stringsustain = unit->stringstretch = 0;
	unit->stifkarp->noteOn(IN0(0) ,IN0(1));
	SETCALC(StkStifKarp_next);
	StkStifKarp_next(unit, 1);

}

void StkStifKarp_next(StkStifKarp *unit, int inNumSamples)
{
	float *out OUT(0);
	float in00=IN0(0);
	float in01=IN0(2);
	float in02=IN0(3);
	float in03=IN0(4);


	if(in00 != unit->freq) {
		unit->stifkarp->setFrequency(in00);
		unit->freq = in00;};

	if(in01 != unit->pickuppos) {
		unit->stifkarp->controlChange(4, in01);
		unit->pickuppos = in01; };

	if(in02 != unit->stringsustain) {
		unit->stifkarp->controlChange(11, in02);
		unit->stringsustain = in02; };

	if(in03 != unit->stringstretch) {
		unit->stifkarp->controlChange(1, in03);
		unit->stringstretch = in03; };

 	for (int i=0; i < inNumSamples; ++i)
	{
		out[i] = unit->stifkarp->tick();
        }

}

void StkStifKarp_Dtor(StkStifKarp* unit)
{
	//delete unit->stifkarp;
	unit->stifkarp->~StifKarp();
    RTFree(unit->mWorld, unit->stifkarp);

}

//////////////////////////////////////////////////////////////////

void StkTubeBell_Ctor(StkTubeBell* unit)
{
//	Stk :: setRawwavePath("/Users/paul/stk-4.1.3/rawwaves");

  STKAlloc0(unit->mWorld, unit->tubebell, TubeBell );

//	unit->tubebell = new TubeBell();
	unit->tubebell->noteOn(IN0(0) ,1);
	SETCALC(StkTubeBell_next);
	StkTubeBell_next(unit, 1);

}

void StkTubeBell_next(StkTubeBell *unit, int inNumSamples)
{
	float *out OUT(0);

 	for (int i=0; i < inNumSamples; ++i)
	{
		out[i] = unit->tubebell->tick();
        }

}

void StkTubeBell_Dtor(StkTubeBell* unit)
{
//	delete unit->tubebell;
	unit->tubebell->~TubeBell();
    RTFree(unit->mWorld, unit->tubebell);

}


//////////////////////////////////////////////////////////////////

void Sflute_Ctor(Sflute* unit)
{

#define EMBLEN 100
#define FLUTELEN 700
        SETCALC(Sflute_next);

	unit->delay1Length = (uint32)( SAMPLERATE/IN0(0));
	unit->delay1 = (float*)RTAlloc(unit->mWorld, FLUTELEN * sizeof(float));
	unit->delay1Pos = 0;
	unit->delay2 = (float*)RTAlloc(unit->mWorld, EMBLEN * sizeof(float)); // make larger to allow for change
	unit->delay2Pos = 0;
	unit->delay3 = 0;
	unit->lenwrapped = false;
	unit->lipwrapped = false;
	Sflute_next(unit, 1);
}

//////////////////////////////////////////////////////////////////

void Sflute_Dtor(Sflute* unit)
{
        RTFree(unit->mWorld, unit->delay1);
        RTFree(unit->mWorld, unit->delay2);
}
//////////////////////////////////////////////////////////////////

void Sflute_next(Sflute *unit, int inNumSamples)
{
	float *out = OUT(0);

	uint32 delay1Length = unit->delay1Length;
	float *delay1 = unit->delay1;
	uint32 delay1Pos = unit->delay1Pos;
	uint32 delay2Length = (uint32)IN0(4);
	float *delay2 = unit->delay2 ;
	uint32 delay2Pos = unit->delay2Pos;
	float pressure = IN0(1) ;
	float randamp = IN0(2);
	float dampcoef = IN0(3);
	float delay3 = unit->delay3;
	float jetstream = IN0(5);
	float fullwave = IN0(6);
	bool lipwrapped = unit->lipwrapped;
	bool lenwrapped = unit->lenwrapped;
	delay1Length = (uint32)( SAMPLERATE/IN0(0));

	if(delay1Length > FLUTELEN) delay1Length = FLUTELEN;
	if(delay2Length > EMBLEN) delay2Length = EMBLEN;

	RGET
	for (int i=0; i < inNumSamples; ++i)
        {
		float y,lenval;

		y = (frand(s1,s2,s3)*2-1) * randamp * pressure;

		y = y + pressure;	// breath pressure with random deviation
		lenval = lenwrapped ? delay1[delay1Pos] : 0.f;
 		y = y + (lenval * -.35);	// mix with pressure of returning wave

		delay2[delay2Pos] = y;
		if(++delay2Pos >= delay2Length) {
				delay2Pos = 0;
				lipwrapped = true;
				};

		y = lipwrapped ?  delay2[delay2Pos] : 0.f;
 		y = (y * y * y) - y;	//transfer differential across mouth hole
		y = (jetstream * y) + (fullwave * lenval);   // jet stream plus full wave

		out[i] = y;	// catch output here

		y = (dampcoef * y) + (1.-dampcoef) * delay3; // lowpass at end of tube

		delay3 = y;

		delay1[delay1Pos] = y;
                if (++delay1Pos >= delay1Length) {
                        delay1Pos = 0;
						lenwrapped = true;
                };

        }
		unit->delay1Pos = delay1Pos;
		unit->delay2Pos = delay2Pos;
		unit->delay3 = delay3;
		unit->lipwrapped = lipwrapped;
		unit->lenwrapped = lenwrapped;
		RPUT
}

PluginLoad(Stk)
{
        ft = inTable;
        DefineDtorUnit(StkBandedWG);
        DefineDtorUnit(StkBeeThree);
        DefineDtorUnit(StkBlowHole);
        DefineDtorUnit(StkBowed);
        DefineDtorUnit(StkClarinet);
        DefineDtorUnit(StkFlute);
        DefineDtorUnit(StkModalBar);
        DefineDtorUnit(StkMoog);
        DefineDtorUnit(StkPluck);
        DefineDtorUnit(StkSaxofony);
        DefineDtorUnit(StkShakers);
        DefineDtorUnit(StkVoicForm);
        DefineDtorUnit(StkMandolin);
        DefineDtorUnit(StkSitar);
        DefineDtorUnit(StkStifKarp);
	DefineSimpleUnit(StkTubeBell);
        DefineDtorUnit(Sflute);

}

