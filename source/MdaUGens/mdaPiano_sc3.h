// Edited from mdaPiano.h in the VST plugin source
// See readme.license for copyright and other info

#ifndef __mdaPiano_sc3__
#define __mdaPiano_sc3__

#include <string.h>

///REMOVE #include "audioeffectx.h"

#define NVOICES 32       //max polyphony
#define SUSTAIN 128
#define SILENCE 0.0001f  //voice choking
#define WAVELEN 586348   //wave data bytes

struct VOICE  //voice state
{
  int32  delta;  //sample playback
  int32  frac;
  int32  pos;
  int32  end;
  int32  loop;
  
  float env;  //envelope
  float dec;

  float f0;   //first-order LPF
  float f1;
  float ff;

  float outl;
  float outr;
  int32  note; //remember what note triggered this
};


struct KGRP  //keygroup
{
  int32  root;  //MIDI root note
  int32  high;  //highest note
  int32  pos;
  int32  end;
  int32  loop;
};

const KGRP kgrp[] = {
	{36,  37,      0,  36275, 14774},
	{40,  41,  36278,  83135, 16268},
	{43,  45,  83137, 146756, 33541},
	{48,  49, 146758, 204997, 21156},
	{52,  53, 204999, 244908, 17191},
	{55,  57, 244910, 290978, 23286},
	{60,  61, 290980, 342948, 18002},
	{64,  65, 342950, 391750, 19746},
	{67,  69, 391752, 436915, 22253},
	{72,  73, 436917, 468807,  8852},
	{76,  77, 468809, 492772,  9693},
	{79,  81, 492774, 532293, 10596},
	{84,  85, 532295, 560192,  6011},
	{88,  89, 560194, 574121,  3414},
	{93, 999, 574123, 586343,  2399}
	};


#endif
