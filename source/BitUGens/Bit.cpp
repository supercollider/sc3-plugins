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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

//Bit by redFrik 070714
//updated for windows 080607 - changed from uint to uint8 for unsignedRightShift

#include <math.h>
#include "SC_PlugIn.h"
static InterfaceTable *ft;


struct BitAnd : public Unit {};
struct BitOr : public Unit {};
struct BitXor : public Unit {};
struct BitNot : public Unit {};
struct BitLeftShift : public Unit {};
struct BitRightShift : public Unit {};
struct BitUnsignedRightShift : public Unit {};

extern "C" {
	void BitAnd_Ctor(BitAnd *unit);
	void BitAnd_next_kk(BitAnd *unit, int inNumSamples);
	void BitAnd_next_ak(BitAnd *unit, int inNumSamples);
	void BitAnd_next_aa(BitAnd *unit, int inNumSamples);
	void BitOr_Ctor(BitOr *unit);
	void BitOr_next_kk(BitOr *unit, int inNumSamples);
	void BitOr_next_ak(BitOr *unit, int inNumSamples);
	void BitOr_next_aa(BitOr *unit, int inNumSamples);
	void BitXor_Ctor(BitXor *unit);
	void BitXor_next_kk(BitXor *unit, int inNumSamples);
	void BitXor_next_ak(BitXor *unit, int inNumSamples);
	void BitXor_next_aa(BitXor *unit, int inNumSamples);
	void BitNot_Ctor(BitNot *unit);
	void BitNot_next_k(BitNot *unit, int inNumSamples);
	void BitNot_next_a(BitNot *unit, int inNumSamples);
	void BitLeftShift_Ctor(BitLeftShift *unit);
	void BitLeftShift_next_kk(BitLeftShift *unit, int inNumSamples);
	void BitLeftShift_next_ak(BitLeftShift *unit, int inNumSamples);
	void BitLeftShift_next_aa(BitLeftShift *unit, int inNumSamples);
	void BitRightShift_Ctor(BitRightShift *unit);
	void BitRightShift_next_kk(BitRightShift *unit, int inNumSamples);
	void BitRightShift_next_ak(BitRightShift *unit, int inNumSamples);
	void BitRightShift_next_aa(BitRightShift *unit, int inNumSamples);
	void BitUnsignedRightShift_Ctor(BitUnsignedRightShift *unit);
	void BitUnsignedRightShift_next_kk(BitUnsignedRightShift *unit, int inNumSamples);
	void BitUnsignedRightShift_next_ak(BitUnsignedRightShift *unit, int inNumSamples);
	void BitUnsignedRightShift_next_aa(BitUnsignedRightShift *unit, int inNumSamples);
}

PluginLoad(Bit)
{
	ft= inTable;
	DefineSimpleUnit(BitAnd);
	DefineSimpleUnit(BitOr);
	DefineSimpleUnit(BitXor);
	DefineSimpleUnit(BitNot);
	DefineSimpleUnit(BitLeftShift);
	DefineSimpleUnit(BitRightShift);
	DefineSimpleUnit(BitUnsignedRightShift);
}

//--
void BitAnd_Ctor(BitAnd *unit) {
	if(unit->mCalcRate==calc_FullRate) {
		if(INRATE(0)==calc_FullRate) {
			if(INRATE(1)==calc_FullRate) {
				SETCALC(BitAnd_next_aa);
				BitAnd_next_aa(unit, 1);
			} else {
				SETCALC(BitAnd_next_ak);
				BitAnd_next_ak(unit, 1);
			}
		} else {
			SETCALC(BitAnd_next_kk);
			BitAnd_next_kk(unit, 1);
		}
	} else {
		SETCALC(BitAnd_next_kk);
		BitAnd_next_kk(unit, 1);
	}
}
void BitAnd_next_kk(BitAnd *unit, int inNumSamples) {
	float *out= ZOUT(0);
	int inA= ZIN0(0);
	int inB= ZIN0(1);
	int res= inA & inB;
	LOOP(inNumSamples,
		ZXP(out)= res;
	);
}
void BitAnd_next_ak(BitAnd *unit, int inNumSamples) {
	float *out= ZOUT(0);
	float *inA= ZIN(0);
	int inB= ZIN0(1);
	LOOP(inNumSamples,
		ZXP(out)= (int)ZXP(inA) & inB;
	);
}
void BitAnd_next_aa(BitAnd *unit, int inNumSamples) {
	float *out= ZOUT(0);
	float *inA= ZIN(0);
	float *inB= ZIN(1);
	LOOP(inNumSamples,
		ZXP(out)= (int)ZXP(inA) & (int)ZXP(inB);
	);
}

//--
void BitOr_Ctor(BitOr *unit) {
	if(unit->mCalcRate==calc_FullRate) {
		if(INRATE(0)==calc_FullRate) {
			if(INRATE(1)==calc_FullRate) {
				SETCALC(BitOr_next_aa);
				BitOr_next_aa(unit, 1);
			} else {
				SETCALC(BitOr_next_ak);
				BitOr_next_ak(unit, 1);
			}
		} else {
			SETCALC(BitOr_next_kk);
			BitOr_next_kk(unit, 1);
		}
	} else {
		SETCALC(BitOr_next_kk);
		BitOr_next_kk(unit, 1);
	}
}
void BitOr_next_kk(BitOr *unit, int inNumSamples) {
	float *out= ZOUT(0);
	int inA= ZIN0(0);
	int inB= ZIN0(1);
	int res= inA | inB;
	LOOP(inNumSamples,
		ZXP(out)= res;
	);
}
void BitOr_next_ak(BitOr *unit, int inNumSamples) {
	float *out= ZOUT(0);
	float *inA= ZIN(0);
	int inB= ZIN0(1);
	LOOP(inNumSamples,
		ZXP(out)= (int)ZXP(inA) | inB;
	);
}
void BitOr_next_aa(BitOr *unit, int inNumSamples) {
	float *out= ZOUT(0);
	float *inA= ZIN(0);
	float *inB= ZIN(1);
	LOOP(inNumSamples,
		ZXP(out)= (int)ZXP(inA) | (int)ZXP(inB);
	);
}

//--
void BitXor_Ctor(BitXor *unit) {
	if(unit->mCalcRate==calc_FullRate) {
		if(INRATE(0)==calc_FullRate) {
			if(INRATE(1)==calc_FullRate) {
				SETCALC(BitXor_next_aa);
				BitXor_next_aa(unit, 1);
			} else {
				SETCALC(BitXor_next_ak);
				BitXor_next_ak(unit, 1);
			}
		} else {
			SETCALC(BitXor_next_kk);
			BitXor_next_kk(unit, 1);
		}
	} else {
		SETCALC(BitXor_next_kk);
		BitXor_next_kk(unit, 1);
	}
}
void BitXor_next_kk(BitXor *unit, int inNumSamples) {
	float *out= ZOUT(0);
	int inA= ZIN0(0);
	int inB= ZIN0(1);
	int res= inA ^ inB;
	LOOP(inNumSamples,
		ZXP(out)= res;
	);
}
void BitXor_next_ak(BitXor *unit, int inNumSamples) {
	float *out= ZOUT(0);
	float *inA= ZIN(0);
	int inB= ZIN0(1);
	LOOP(inNumSamples,
		ZXP(out)= (int)ZXP(inA) ^ inB;
	);
}
void BitXor_next_aa(BitXor *unit, int inNumSamples) {
	float *out= ZOUT(0);
	float *inA= ZIN(0);
	float *inB= ZIN(1);
	LOOP(inNumSamples,
		ZXP(out)= (int)ZXP(inA) ^ (int)ZXP(inB);
	);
}

//--
void BitNot_Ctor(BitNot *unit) {
	if(unit->mCalcRate==calc_FullRate) {
		if(INRATE(0)==calc_FullRate) {
			SETCALC(BitNot_next_a);
			BitNot_next_a(unit, 1);
		} else {
			SETCALC(BitNot_next_k);
			BitNot_next_k(unit, 1);
		}
	} else {
		SETCALC(BitNot_next_k);
		BitNot_next_k(unit, 1);
	}
}
void BitNot_next_k(BitNot *unit, int inNumSamples) {
	float *out= ZOUT(0);
	int inA= ZIN0(0);
	int res= ~inA;
	LOOP(inNumSamples,
		ZXP(out)= res;
	);
}
void BitNot_next_a(BitNot *unit, int inNumSamples) {
	float *out= ZOUT(0);
	float *inA= ZIN(0);
	LOOP(inNumSamples,
		ZXP(out)= ~(int)ZXP(inA);
	);
}

//--
void BitLeftShift_Ctor(BitLeftShift *unit) {
	if(unit->mCalcRate==calc_FullRate) {
		if(INRATE(0)==calc_FullRate) {
			if(INRATE(1)==calc_FullRate) {
				SETCALC(BitLeftShift_next_aa);
				BitLeftShift_next_aa(unit, 1);
			} else {
				SETCALC(BitLeftShift_next_ak);
				BitLeftShift_next_ak(unit, 1);
			}
		} else {
			SETCALC(BitLeftShift_next_kk);
			BitLeftShift_next_kk(unit, 1);
		}
	} else {
		SETCALC(BitLeftShift_next_kk);
		BitLeftShift_next_kk(unit, 1);
	}
}
void BitLeftShift_next_kk(BitLeftShift *unit, int inNumSamples) {
	float *out= ZOUT(0);
	int inA= ZIN0(0);
	int inB= ZIN0(1);
	int bits= pow(2, ZIN0(2));
	int res= (inA << inB) % bits;
	LOOP(inNumSamples,
		ZXP(out)= res;
	);
}
void BitLeftShift_next_ak(BitLeftShift *unit, int inNumSamples) {
	float *out= ZOUT(0);
	float *inA= ZIN(0);
	int inB= ZIN0(1);
	int bits= pow(2, ZIN0(2));
	LOOP(inNumSamples,
		ZXP(out)= ((int)ZXP(inA) << inB) % bits;
	);
}
void BitLeftShift_next_aa(BitLeftShift *unit, int inNumSamples) {
	float *out= ZOUT(0);
	float *inA= ZIN(0);
	float *inB= ZIN(1);
	int bits= pow(2, ZIN0(2));
	LOOP(inNumSamples,
		ZXP(out)= ((int)ZXP(inA) << (int)ZXP(inB)) % bits;
	);
}

//--
void BitRightShift_Ctor(BitRightShift *unit) {
	if(unit->mCalcRate==calc_FullRate) {
		if(INRATE(0)==calc_FullRate) {
			if(INRATE(1)==calc_FullRate) {
				SETCALC(BitRightShift_next_aa);
				BitRightShift_next_aa(unit, 1);
			} else {
				SETCALC(BitRightShift_next_ak);
				BitRightShift_next_ak(unit, 1);
			}
		} else {
			SETCALC(BitRightShift_next_kk);
			BitRightShift_next_kk(unit, 1);
		}
	} else {
		SETCALC(BitRightShift_next_kk);
		BitRightShift_next_kk(unit, 1);
	}
}
void BitRightShift_next_kk(BitRightShift *unit, int inNumSamples) {
	float *out= ZOUT(0);
	int inA= ZIN0(0);
	int inB= ZIN0(1);
	int res= inA >> inB;
	LOOP(inNumSamples,
		ZXP(out)= res;
	);
}
void BitRightShift_next_ak(BitRightShift *unit, int inNumSamples) {
	float *out= ZOUT(0);
	float *inA= ZIN(0);
	int inB= ZIN0(1);
	LOOP(inNumSamples,
		ZXP(out)= (int)ZXP(inA) >> inB;
	);
}
void BitRightShift_next_aa(BitRightShift *unit, int inNumSamples) {
	float *out= ZOUT(0);
	float *inA= ZIN(0);
	float *inB= ZIN(1);
	LOOP(inNumSamples,
		ZXP(out)= (int)ZXP(inA) >> (int)ZXP(inB);
	);
}

//--
void BitUnsignedRightShift_Ctor(BitUnsignedRightShift *unit) {
	if(unit->mCalcRate==calc_FullRate) {
		if(INRATE(0)==calc_FullRate) {
			if(INRATE(1)==calc_FullRate) {
				SETCALC(BitUnsignedRightShift_next_aa);
				BitUnsignedRightShift_next_aa(unit, 1);
			} else {
				SETCALC(BitUnsignedRightShift_next_ak);
				BitUnsignedRightShift_next_ak(unit, 1);
			}
		} else {
			SETCALC(BitUnsignedRightShift_next_kk);
			BitUnsignedRightShift_next_kk(unit, 1);
		}
	} else {
		SETCALC(BitUnsignedRightShift_next_kk);
		BitUnsignedRightShift_next_kk(unit, 1);
	}
}
void BitUnsignedRightShift_next_kk(BitUnsignedRightShift *unit, int inNumSamples) {
	float *out= ZOUT(0);
	int inA= ZIN0(0);
	int inB= ZIN0(1);
	int bits= pow(2, ZIN0(2));
	int res= ((uint8)inA % bits) >> inB;
	LOOP(inNumSamples,
		ZXP(out)= res;
	);
}
void BitUnsignedRightShift_next_ak(BitUnsignedRightShift *unit, int inNumSamples) {
	float *out= ZOUT(0);
	float *inA= ZIN(0);
	int inB= ZIN0(1);
	int bits= pow(2, ZIN0(2));
	LOOP(inNumSamples,
		ZXP(out)= ((uint8)(int)ZXP(inA) % bits) >> inB;
	);
}
void BitUnsignedRightShift_next_aa(BitUnsignedRightShift *unit, int inNumSamples) {
	float *out= ZOUT(0);
	float *inA= ZIN(0);
	float *inB= ZIN(1);
	int bits= pow(2, ZIN0(2));
	LOOP(inNumSamples,
		ZXP(out)= ((uint8)(int)ZXP(inA) % bits) >> (int)ZXP(inB);
	);
}
