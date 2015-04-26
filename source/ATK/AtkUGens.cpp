/*
 *  AmbisonicUGens.cpp
 *  xSC3plugins
 *
 *  Created by Josh Parmenter on 2/4/05.
 *  Copyright 2005 Josh Parmenter. All rights reserved.
 *  Copyright the ATK Community, Josh Parmenter, and Joseph Anderson, 2011
 *  
 *  	J Anderson	j.anderson[at]ambisonictoolkit.net 
 *  	J Parmenter	j.parmenter[at]ambisonictoolkit.net 
 *
 */
/*
 This file is part of SuperCollider3 version of the Ambisonic Toolkit (ATK).
 
 The SuperCollider3 version of the Ambisonic Toolkit (ATK) is free software:
 you can redistribute it and/or modify it under the terms of the GNU General
 Public License as published by the Free Software Foundation, either version 3
 of the License, or (at your option) any later version.
 
 The SuperCollider3 version of the Ambisonic Toolkit (ATK) is distributed in
 the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
 implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 the GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License along with the
 SuperCollider3 version of the Ambisonic Toolkit (ATK). If not, see
 <http://www.gnu.org/licenses/>.
 */
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

/*
 ---------------------------------------------------------------------
 The Ambisonic Toolkit (ATK) is a soundfield kernel support library.
 
 The Ambisonic Toolkit (ATK) is intended to bring together a number of tools and
 methods for working with Ambisonic surround sound. The intention is for the toolset
 to be both ergonomic and comprehensive, providing both classic and novel algorithms
 to creatively manipulate and synthesise complex Ambisonic soundfields.

 The tools are framed for the user to think in terms of the soundfield kernel. By
 this, it is meant the ATK addresses the holistic problem of creatively controlling 
 a complete soundfield, allowing and encouraging the composer to think beyond the
 placement of sounds in a sound-space and instead attend to the impression and image 
 of a soundfield. This approach takes advantage of the model the Ambisonic
 technology presents, and is viewed to be the idiomatic mode for working with the 
 Ambisonic technique.

 We hope you enjoy the ATK!

 For more information visit http://ambisonictoolkit.net/ or
 email j.anderson[at]ambisonictoolkit.net OR j.parmenter[at]ambisonictoolkit.net

 ---------------------------------------------------------------------
*/


#include "SC_PlugIn.h"

const double sqrt3div6 = sqrt(3.) * 0.1666666667;
const double sqrt3div2 = sqrt(3.) * 0.5;
const double rsqrt6 = 1. / sqrt(6.);
const double sqrt6div3 = sqrt(6.) * 0.3333333333;

static InterfaceTable *ft;

typedef struct  
{
    float coefs[4][4];
} FoaMatrix;

struct FoaPanB : public Unit 
{
    float m_azimuth, m_elevation, m_W_amp, m_X_amp, m_Y_amp, m_Z_amp;
};


struct FoaDirectO : public Unit
{
    FoaMatrix matrix;
    float m_angle;
};

struct FoaDirectX : public FoaDirectO { };
struct FoaDirectY : public FoaDirectO { };
struct FoaDirectZ : public FoaDirectO { };

struct FoaRotate : public FoaDirectO { };
struct FoaTilt : public FoaDirectO { };
struct FoaTumble : public FoaDirectO { };

struct FoaFocusX : public FoaDirectO { };
struct FoaFocusY : public FoaDirectO { };
struct FoaFocusZ : public FoaDirectO { };

struct FoaPushX : public FoaDirectO { };
struct FoaPushY : public FoaDirectO { };
struct FoaPushZ : public FoaDirectO { };

struct FoaPressX : public FoaDirectO { };
struct FoaPressY : public FoaDirectO { };
struct FoaPressZ : public FoaDirectO { };

struct FoaZoomX : public FoaDirectO { };
struct FoaZoomY : public FoaDirectO { };
struct FoaZoomZ : public FoaDirectO { };

struct FoaAsymmetry : public FoaDirectO { };

struct FoaDominateX : public Unit
{
    float m_gain;
    FoaMatrix matrix;	
};

struct FoaDominateY : FoaDominateX { };
struct FoaDominateZ : FoaDominateX { };

struct FoaNFC : public Unit
{
    float m_distanceStart, m_y1x, m_y1y, m_y1z;
};

struct FoaProximity : public Unit
{
    float m_distanceStart, m_y1x, m_y1y, m_y1z;
};

struct FoaPsychoShelf : public Unit
{
	float m_freq, m_k0, m_k1;
	float m_y1w, m_y2w, m_y1x, m_y2x, m_y1y, m_y2y, m_y1z, m_y2z;
};

extern "C"
{
    void load(InterfaceTable *inTable);
    
    void FoaPanB_next_aa(FoaPanB *unit, int inNumSamples);
    void FoaPanB_next_kk(FoaPanB *unit, int inNumSamples);
    void FoaPanB_Ctor(FoaPanB *unit);

    void FoaDirectO_next_a(FoaDirectO *unit, int inNumSamples);
    void FoaDirectO_next_k(FoaDirectO *unit, int inNumSamples);
    void FoaDirectO_Ctor(FoaDirectO* unit);
    
    void FoaDirectX_next_a(FoaDirectX *unit, int inNumSamples);
    void FoaDirectX_next_k(FoaDirectX *unit, int inNumSamples);
    void FoaDirectX_Ctor(FoaDirectX* unit);
    
    void FoaDirectY_next_a(FoaDirectY *unit, int inNumSamples);
    void FoaDirectY_next_k(FoaDirectY *unit, int inNumSamples);
    void FoaDirectY_Ctor(FoaDirectY* unit);
    
    void FoaDirectZ_next_a(FoaDirectZ *unit, int inNumSamples);
    void FoaDirectZ_next_k(FoaDirectZ *unit, int inNumSamples);
    void FoaDirectZ_Ctor(FoaDirectZ* unit);
    
    void FoaRotate_next_a(FoaRotate *unit, int inNumSamples);
    void FoaRotate_next_k(FoaRotate *unit, int inNumSamples);
    void FoaRotate_Ctor(FoaRotate* unit);
    
    void FoaTilt_next_a(FoaTilt *unit, int inNumSamples);
    void FoaTilt_next_k(FoaTilt *unit, int inNumSamples);
    void FoaTilt_Ctor(FoaTilt* unit);
    
    void FoaTumble_next_a(FoaTumble *unit, int inNumSamples);
    void FoaTumble_next_k(FoaTumble *unit, int inNumSamples);
    void FoaTumble_Ctor(FoaTumble* unit);
    
    void FoaFocusX_next_a(FoaFocusX *unit, int inNumSamples);
    void FoaFocusX_next_k(FoaFocusX *unit, int inNumSamples);
    void FoaFocusX_Ctor(FoaFocusX* unit);
    
    void FoaFocusY_next_a(FoaFocusY *unit, int inNumSamples);
    void FoaFocusY_next_k(FoaFocusY *unit, int inNumSamples);
    void FoaFocusY_Ctor(FoaFocusY* unit);
    
    void FoaFocusZ_next_a(FoaFocusZ *unit, int inNumSamples);
    void FoaFocusZ_next_k(FoaFocusZ *unit, int inNumSamples);
    void FoaFocusZ_Ctor(FoaFocusZ* unit);	    
    
    void FoaPushX_next_a(FoaPushX *unit, int inNumSamples);
    void FoaPushX_next_k(FoaPushX *unit, int inNumSamples);
    void FoaPushX_Ctor(FoaPushX* unit);	
    
    void FoaPushY_next_a(FoaPushY *unit, int inNumSamples);
    void FoaPushY_next_k(FoaPushY *unit, int inNumSamples);
    void FoaPushY_Ctor(FoaPushY* unit);	
    
    void FoaPushZ_next_a(FoaPushZ *unit, int inNumSamples);
    void FoaPushZ_next_k(FoaPushZ *unit, int inNumSamples);
    void FoaPushZ_Ctor(FoaPushZ* unit);	
    
    void FoaPressX_next_a(FoaPressX *unit, int inNumSamples);
    void FoaPressX_next_k(FoaPressX *unit, int inNumSamples);
    void FoaPressX_Ctor(FoaPressX* unit);
    
    void FoaPressY_next_a(FoaPressY *unit, int inNumSamples);
    void FoaPressY_next_k(FoaPressY *unit, int inNumSamples);
    void FoaPressY_Ctor(FoaPressY* unit);
    
    void FoaPressZ_next_a(FoaPressZ *unit, int inNumSamples);
    void FoaPressZ_next_k(FoaPressZ *unit, int inNumSamples);
    void FoaPressZ_Ctor(FoaPressZ* unit);
    
    void FoaZoomX_next_a(FoaZoomX *unit, int inNumSamples);
    void FoaZoomX_next_k(FoaZoomX *unit, int inNumSamples);
    void FoaZoomX_Ctor(FoaZoomX* unit);
    
    void FoaZoomY_next_a(FoaZoomY *unit, int inNumSamples);
    void FoaZoomY_next_k(FoaZoomY *unit, int inNumSamples);
    void FoaZoomY_Ctor(FoaZoomY* unit);
    
    void FoaZoomZ_next_a(FoaZoomZ *unit, int inNumSamples);
    void FoaZoomZ_next_k(FoaZoomZ *unit, int inNumSamples);
    void FoaZoomZ_Ctor(FoaZoomZ* unit);
    
    void FoaDominateX_next_a(FoaDominateX *unit, int inNumSamples);
    void FoaDominateX_next_k(FoaDominateX *unit, int inNumSamples);
    void FoaDominateX_Ctor(FoaDominateX* unit);						
    
    void FoaDominateY_next_a(FoaDominateY *unit, int inNumSamples);
    void FoaDominateY_next_k(FoaDominateY *unit, int inNumSamples);
    void FoaDominateY_Ctor(FoaDominateY* unit);
    
    void FoaDominateZ_next_a(FoaDominateZ *unit, int inNumSamples);
    void FoaDominateZ_next_k(FoaDominateZ *unit, int inNumSamples);
    void FoaDominateZ_Ctor(FoaDominateZ* unit);
    
    void FoaAsymmetry_next_a(FoaAsymmetry *unit, int inNumSamples);
    void FoaAsymmetry_next_k(FoaAsymmetry *unit, int inNumSamples);
    void FoaAsymmetry_Ctor(FoaAsymmetry* unit);
    
    void FoaNFC_next_k(FoaNFC *unit, int inNumSamples);
    void FoaNFC_next_a(FoaNFC *unit, int inNumSamples);
    void FoaNFC_Ctor(FoaNFC* unit);
    
    void FoaProximity_next_k(FoaProximity *unit, int inNumSamples);
    void FoaProximity_next_a(FoaProximity *unit, int inNumSamples);
    void FoaProximity_Ctor(FoaProximity* unit);
    
    void FoaPsychoShelf_next_k(FoaPsychoShelf *unit, int inNumSamples);
    void FoaPsychoShelf_next_a(FoaPsychoShelf *unit, int inNumSamples);
    void FoaPsychoShelf_Ctor(FoaPsychoShelf* unit);
    
}

inline float calcmatrixval(float coef, float curval){
    return coef * curval;
}

// can perhaps optimize a bit here ... check for 0s???
#define CALC_MATRIX \
float curvals[4] = {Win[i], Xin[i], Yin[i], Zin[i]}; \
for(int j = 0; j < 4; j++){ \
float wAdd = calcmatrixval(matrix.coefs[0][j], curvals[j]); \
float xAdd = calcmatrixval(matrix.coefs[1][j], curvals[j]); \
float yAdd = calcmatrixval(matrix.coefs[2][j], curvals[j]); \
float zAdd = calcmatrixval(matrix.coefs[3][j], curvals[j]); \
float wNew = Wout[i] + wAdd; \
float xNew = Xout[i] + xAdd; \
float yNew = Yout[i] + yAdd; \
float zNew = Zout[i] + zAdd; \
Wout[i] = wNew; \
Xout[i] = xNew; \
Yout[i] = yNew; \
Zout[i] = zNew; \
} 

#define SETUP_TRANSFORMS \
float *Win = IN(0); \
float *Xin = IN(1); \
float *Yin = IN(2); \
float *Zin = IN(3); \
float *Wout = OUT(0); \
float *Xout = OUT(1); \
float *Yout = OUT(2); \
float *Zout = OUT(3); \
ClearUnitOutputs(unit, inNumSamples); \
FoaMatrix matrix = unit->matrix; \

#define ZERO_MATRIX \
for(int i = 0; i < 4; i++){ \
for(int j = 0; j < 4; j++){ \
unit->matrix.coefs[i][j] = 0.f; \
} \
} \

#define SIN_COS \
sina = sin(azimuth); \
sinb = sin(elevation); \
\
cosa = cos(azimuth); \
cosb = cos(elevation); \

// this could be optimised
#define UNWRAPANGLE(next,prev) \
if (fabs(fmod(next, twopi) - fmod(prev, twopi)) <= pi) \
next = prev + fmod(next, twopi) - fmod(prev, twopi); \
else \
if ((fmod(next, twopi) - fmod(prev, twopi)) < 0) \
next = prev +  fmod(next, twopi) - fmod(prev, twopi) + twopi; \
else \
next = prev + fmod(next, twopi) - fmod(prev, twopi) - twopi; \


/* FoaPanB - basic encoder (places sound on the sphere) */

void FoaPanB_Ctor(FoaPanB *unit)
{
    if((INRATE(1) == calc_FullRate) && (INRATE(2) == calc_FullRate)){
	SETCALC(FoaPanB_next_aa);//aa
    } else {
	SETCALC(FoaPanB_next_kk);//ak
    }
    
    float azimuth = unit->m_azimuth = IN0(1);
    float elevation = unit->m_elevation = IN0(2);
    float sina, sinb, cosa, cosb;
    
    SIN_COS
    
    unit->m_W_amp = rsqrt2;
    unit->m_X_amp = cosa * cosb;
    unit->m_Y_amp = sina * cosb;
    unit->m_Z_amp = sinb;
    
    FoaPanB_next_kk(unit, 1);
}

void FoaPanB_next_kk(FoaPanB *unit, int inNumSamples)
{
    float azimuth = IN0(1);
    float elevation = IN0(2);
    float *in = IN(0);
    float *Wout = OUT(0); 
    float *Xout = OUT(1); 
    float *Yout = OUT(2); 
    float *Zout = OUT(3);
    float Wamp = unit->m_W_amp;
    float Xamp = unit->m_X_amp;
    float Yamp = unit->m_Y_amp;
    float Zamp = unit->m_Z_amp;
    float sina, sinb, cosa, cosb;
    
    if((unit->m_azimuth == azimuth) && (unit->m_elevation == elevation)){
	for(int i = 0; i < inNumSamples; i++){
	    Wout[i] = in[i] * Wamp;
	    Xout[i] = in[i] * Xamp;
	    Yout[i] = in[i] * Yamp;
	    Zout[i] = in[i] * Zamp;
	}
    } else {
	
	SIN_COS
	
	float nextXamp = cosa * cosb;
	float nextYamp = sina * cosb;
	float nextZamp = sinb;
	
	float xSlope = CALCSLOPE(nextXamp, Xamp);
	float ySlope = CALCSLOPE(nextYamp, Yamp);
	float zSlope = CALCSLOPE(nextZamp, Zamp);
	
	for(int i = 0; i < inNumSamples; i++){
	    Wout[i] = in[i] * Wamp;
	    Xout[i] = in[i] * Xamp;
	    Yout[i] = in[i] * Yamp;
	    Zout[i] = in[i] * Zamp;
	    
	    Xamp += xSlope;
	    Yamp += ySlope;  
	    Zamp += zSlope;
	}	
	
	unit->m_X_amp = Xamp;
	unit->m_Y_amp = Yamp;
	unit->m_Z_amp = Zamp;
	unit->m_azimuth = azimuth;
	unit->m_elevation = elevation;
    }
}

void FoaPanB_next_aa(FoaPanB *unit, int inNumSamples)
{
    float *pazimuth = IN(1);
    float *pelevation = IN(2);
    float *in = IN(0);
    float *Wout = OUT(0); 
    float *Xout = OUT(1); 
    float *Yout = OUT(2); 
    float *Zout = OUT(3);
    float Wamp = unit->m_W_amp;
    float Xamp = unit->m_X_amp;
    float Yamp = unit->m_Y_amp;
    float Zamp = unit->m_Z_amp;
    float sina, sinb, cosa, cosb, azimuth, elevation;
    
    for(int i = 0; i < inNumSamples; i++){
	if((unit->m_azimuth == pazimuth[i]) && (unit->m_elevation == pelevation[i])){
	    Wout[i] = in[i] * Wamp;
	    Xout[i] = in[i] * Xamp;
	    Yout[i] = in[i] * Yamp;
	    Zout[i] = in[i] * Zamp;
	} else {
	    
	    azimuth = pazimuth[i];
	    elevation = pelevation[i];
	    
	    SIN_COS
	    
	    Xamp = cosa * cosb;
	    Yamp = sina * cosb;
	    Zamp = sinb;
	    
	    Wout[i] = in[i] * Wamp;
	    Xout[i] = in[i] * Xamp;
	    Yout[i] = in[i] * Yamp;
	    Zout[i] = in[i] * Zamp;
	    
	    unit->m_azimuth = azimuth;
	    unit->m_elevation = elevation;	    
	}
    }
    unit->m_X_amp = Xamp;
    unit->m_Y_amp = Yamp;
    unit->m_Z_amp = Zamp;
}



////////////////////// FoaRotate ///////////////////////
// uses 'angle' var. 

#define FILL_ROTATE_MATRIX \
matrix.coefs[0][0] = matrix.coefs[3][3] = 1.; \
double cosa = cos(unit->m_angle); \
double sina = sin(unit->m_angle); \
matrix.coefs[1][1] = matrix.coefs[2][2] = cosa; \
matrix.coefs[1][2] = -sina; \
matrix.coefs[2][1] = sina;

void FoaRotate_Ctor(FoaRotate* unit)
{
    ZERO_MATRIX
    unit->m_angle = IN0(4);
    FoaMatrix matrix = unit->matrix;
    FILL_ROTATE_MATRIX;
    unit->matrix = matrix;
    if(INRATE(4) == calc_FullRate)
	SETCALC(FoaRotate_next_a);
    else
	SETCALC(FoaRotate_next_k);
    FoaRotate_next_k(unit, 1); 
}

void FoaRotate_next_a(FoaRotate *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float *angle = IN(4);
    for(int i = 0; i < inNumSamples; i++){
	if(angle[i] != unit->m_angle){
	    unit->m_angle = angle[i];
	    FILL_ROTATE_MATRIX
	}
	CALC_MATRIX
    }
    unit->matrix = matrix;
    
}

void FoaRotate_next_k(FoaRotate *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float angle = IN0(4);
    float angleslope;
    UNWRAPANGLE(angle, unit->m_angle);
    if(angle != unit->m_angle){
        angleslope = CALCSLOPE(angle, unit->m_angle);
        for(int i = 0; i < inNumSamples; i++){
            CALC_MATRIX
            unit->m_angle += angleslope;
            FILL_ROTATE_MATRIX
	}	
    } else {
        for(int i = 0; i < inNumSamples; i++){
            CALC_MATRIX
        }
    }
    unit->matrix = matrix;
    unit->m_angle = angle;	
}

////////////////////// FoaTilt ///////////////////////
// uses 'angle' var. 

#define FILL_TILT_MATRIX \
matrix.coefs[0][0] = matrix.coefs[1][1] = 1.; \
double cosa = cos(unit->m_angle); \
double sina = sin(unit->m_angle); \
matrix.coefs[2][2] = matrix.coefs[3][3] = cosa; \
matrix.coefs[2][3] = -sina; \
matrix.coefs[3][2] = sina;

void FoaTilt_Ctor(FoaTilt* unit)
{
    ZERO_MATRIX
    unit->m_angle = IN0(4);
    FoaMatrix matrix = unit->matrix;
    FILL_TILT_MATRIX;
    unit->matrix = matrix;
    if(INRATE(4) == calc_FullRate)
	SETCALC(FoaTilt_next_a);
    else
	SETCALC(FoaTilt_next_k);
    FoaTilt_next_k(unit, 1); 
}

void FoaTilt_next_a(FoaTilt *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float *angle = IN(4);
    for(int i = 0; i < inNumSamples; i++){
	if(angle[i] != unit->m_angle){
	    unit->m_angle = angle[i];
	    FILL_TILT_MATRIX
	}
	CALC_MATRIX
    }
    unit->matrix = matrix;
    
}

void FoaTilt_next_k(FoaTilt *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float angle = IN0(4);
    float angleslope;
    UNWRAPANGLE(angle, unit->m_angle);
    if(angle != unit->m_angle){
	angleslope = CALCSLOPE(angle, unit->m_angle);
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	    unit->m_angle += angleslope;
	    FILL_TILT_MATRIX
	}	
    } else {
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	}
    }
    unit->matrix = matrix;
    unit->m_angle = angle;	
}

////////////////////// FoaTumble ///////////////////////
// uses 'angle' var. 

#define FILL_TUMBLE_MATRIX \
matrix.coefs[0][0] = matrix.coefs[2][2] = 1.; \
double cosa = cos(unit->m_angle); \
double sina = sin(unit->m_angle); \
matrix.coefs[1][1] = matrix.coefs[3][3] = cosa; \
matrix.coefs[1][3] = -sina; \
matrix.coefs[3][1] = sina;

void FoaTumble_Ctor(FoaTumble* unit)
{
    ZERO_MATRIX
    unit->m_angle = IN0(4);
    FoaMatrix matrix = unit->matrix;
    FILL_TUMBLE_MATRIX;
    unit->matrix = matrix;
    if(INRATE(4) == calc_FullRate)
	SETCALC(FoaTumble_next_a);
    else
	SETCALC(FoaTumble_next_k);
    FoaTumble_next_k(unit, 1); 
}

void FoaTumble_next_a(FoaTumble *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float *angle = IN(4);
    for(int i = 0; i < inNumSamples; i++){
	if(angle[i] != unit->m_angle){
	    unit->m_angle = angle[i];
	    FILL_TUMBLE_MATRIX
	}
	CALC_MATRIX
    }
    unit->matrix = matrix;
    
}

void FoaTumble_next_k(FoaTumble *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float angle = IN0(4);
    float angleslope;
    UNWRAPANGLE(angle, unit->m_angle);
    if(angle != unit->m_angle){
	angleslope = CALCSLOPE(angle, unit->m_angle);
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	    unit->m_angle += angleslope;
	    FILL_TUMBLE_MATRIX
	}	
    } else {
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	}
    }
    unit->matrix = matrix;
    unit->m_angle = angle;	
}

////////////////////// FoaFocusX ///////////////////////
// uses 'angle' var. 

#define FILL_FOCUSX_MATRIX \
double sina = sin(unit->m_angle); \
double roneplussinaa = 1. / (1. + sin(fabs(unit->m_angle))); \
double cosa = cos(unit->m_angle); \
double sina1sina = sina * roneplussinaa; \
matrix.coefs[0][0] = matrix.coefs[1][1] = roneplussinaa; \
matrix.coefs[2][2] = matrix.coefs[3][3] = cosa * roneplussinaa; \
matrix.coefs[0][1] = rsqrt2 * sina1sina; \
matrix.coefs[1][0] = sqrt2 * sina1sina; \

void FoaFocusX_Ctor(FoaFocusX* unit)
{
    ZERO_MATRIX
    unit->m_angle = IN0(4);
    FoaMatrix matrix = unit->matrix;
    
    FILL_FOCUSX_MATRIX
    
    unit->matrix = matrix;
    if(INRATE(4) == calc_FullRate)
	SETCALC(FoaFocusX_next_a);
    else
	SETCALC(FoaFocusX_next_k);
    FoaFocusX_next_k(unit, 1); 
}

void FoaFocusX_next_a(FoaFocusX *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float *angle = IN(4);
    for(int i = 0; i < inNumSamples; i++){
	if(angle[i] != unit->m_angle){
	    unit->m_angle = angle[i];	    
	    
	    FILL_FOCUSX_MATRIX
	    
	}
	CALC_MATRIX
    }
    unit->matrix = matrix;
    
}

void FoaFocusX_next_k(FoaFocusX *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float angle = IN0(4);
    float angleslope;
    UNWRAPANGLE(angle, unit->m_angle);
    if(angle != unit->m_angle){
	angleslope = CALCSLOPE(angle, unit->m_angle);
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	    unit->m_angle += angleslope;
	    FILL_FOCUSX_MATRIX
	}	
    } else {
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	}
    }
    unit->matrix = matrix;
    unit->m_angle = angle;	
}

////////////////////// FoaFocusY ///////////////////////
// uses 'angle' var. 

#define FILL_FOCUSY_MATRIX \
double sina = sin(unit->m_angle); \
double roneplussinaa = 1. / (1. + sin(fabs(unit->m_angle))); \
double cosa = cos(unit->m_angle); \
double sina1sina = sina * roneplussinaa; \
matrix.coefs[0][0] = matrix.coefs[2][2] = roneplussinaa; \
matrix.coefs[1][1] = matrix.coefs[3][3] = cosa * roneplussinaa; \
matrix.coefs[0][2] = rsqrt2 * sina1sina; \
matrix.coefs[2][0] = sqrt2 * sina1sina; \


void FoaFocusY_Ctor(FoaFocusY* unit)
{
    ZERO_MATRIX
    unit->m_angle = IN0(4);
    FoaMatrix matrix = unit->matrix;
    
    FILL_FOCUSY_MATRIX
    
    unit->matrix = matrix;
    if(INRATE(4) == calc_FullRate)
	SETCALC(FoaFocusY_next_a);
    else
	SETCALC(FoaFocusY_next_k);
    FoaFocusY_next_k(unit, 1); 
}

void FoaFocusY_next_a(FoaFocusY *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float *angle = IN(4);
    for(int i = 0; i < inNumSamples; i++){
	if(angle[i] != unit->m_angle){
	    unit->m_angle = angle[i];
	    FILL_FOCUSY_MATRIX
	}
	CALC_MATRIX
    }
    unit->matrix = matrix;
    
}

void FoaFocusY_next_k(FoaFocusY *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float angle = IN0(4);
    float angleslope;
    UNWRAPANGLE(angle, unit->m_angle);
    if(angle != unit->m_angle){
	angleslope = CALCSLOPE(angle, unit->m_angle);
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	    unit->m_angle += angleslope;
	    FILL_FOCUSY_MATRIX
	}	
    } else {
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	}
    }
    unit->matrix = matrix;
    unit->m_angle = angle;	
}

////////////////////// FoaFocusZ ///////////////////////
// uses 'angle' var. 

#define FILL_FOCUSZ_MATRIX \
double sina = sin(unit->m_angle); \
double roneplussinaa = 1. / (1. + sin(fabs(unit->m_angle))); \
double cosa = cos(unit->m_angle); \
double sina1sina = sina * roneplussinaa; \
matrix.coefs[0][0] = matrix.coefs[3][3] = roneplussinaa; \
matrix.coefs[1][1] = matrix.coefs[2][2] = cosa * roneplussinaa; \
matrix.coefs[0][3] = rsqrt2 * sina1sina; \
matrix.coefs[3][0] = sqrt2 * sina1sina; \

void FoaFocusZ_Ctor(FoaFocusZ* unit)
{
    ZERO_MATRIX
    unit->m_angle = IN0(4);
    FoaMatrix matrix = unit->matrix;
    FILL_FOCUSZ_MATRIX;
    unit->matrix = matrix;
    if(INRATE(4) == calc_FullRate)
	SETCALC(FoaFocusZ_next_a);
    else
	SETCALC(FoaFocusZ_next_k);
    FoaFocusZ_next_k(unit, 1); 
}

void FoaFocusZ_next_a(FoaFocusZ *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float *angle = IN(4);
    for(int i = 0; i < inNumSamples; i++){
	if(angle[i] != unit->m_angle){
	    unit->m_angle = angle[i];
	    FILL_FOCUSZ_MATRIX
	}
	CALC_MATRIX
    }
    unit->matrix = matrix;
    
}

void FoaFocusZ_next_k(FoaFocusZ *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float angle = IN0(4);
    float angleslope;
    UNWRAPANGLE(angle, unit->m_angle);
    if(angle != unit->m_angle){
	angleslope = CALCSLOPE(angle, unit->m_angle);
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	    unit->m_angle += angleslope;
	    FILL_FOCUSZ_MATRIX
	}	
    } else {
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	}
    }
    unit->matrix = matrix;
    unit->m_angle = angle;	
}

////////////////////// FoaDirectX /////////////////////
// uses 'angle' var. 

#define FILL_DIRECTX_MATRIX \
matrix.coefs[0][0]= matrix.coefs[2][2] = matrix.coefs[3][3]  = sqrt(1.0 + sin(unit->m_angle)); \
matrix.coefs[1][1] = sqrt(1.0 - sin(unit->m_angle)); \


void FoaDirectX_Ctor(FoaDirectX* unit)
{
    ZERO_MATRIX
    unit->m_angle = IN0(4);
    FoaMatrix matrix = unit->matrix;
    FILL_DIRECTX_MATRIX;
    unit->matrix = matrix;
    if(INRATE(4) == calc_FullRate)
	SETCALC(FoaDirectX_next_a);
    else
	SETCALC(FoaDirectX_next_k);
    FoaDirectX_next_k(unit, 1); 
}

void FoaDirectX_next_a(FoaDirectX *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float *angle = IN(4);
    for(int i = 0; i < inNumSamples; i++){
	if(angle[i] != unit->m_angle){
	    unit->m_angle = angle[i];
	    FILL_DIRECTX_MATRIX
	}
	CALC_MATRIX
    }
    unit->matrix = matrix;
    
}

void FoaDirectX_next_k(FoaDirectX *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float angle = IN0(4);
    float angleslope;
    UNWRAPANGLE(angle, unit->m_angle);
    if(angle != unit->m_angle){
	angleslope = CALCSLOPE(angle, unit->m_angle);
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	    unit->m_angle += angleslope;
	    FILL_DIRECTX_MATRIX
	}	
    } else {
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	}
    }
    unit->matrix = matrix;
    unit->m_angle = angle;	
}

////////////////////// FoaDirectY /////////////////////
// uses 'angle' var. 

#define FILL_DIRECTY_MATRIX \
matrix.coefs[0][0]= matrix.coefs[1][1] = matrix.coefs[3][3]  = sqrt(1.0 + sin(unit->m_angle)); \
matrix.coefs[2][2] = sqrt(1.0 - sin(unit->m_angle)); \

void FoaDirectY_Ctor(FoaDirectY* unit)
{
    ZERO_MATRIX
    unit->m_angle = IN0(4);
    FoaMatrix matrix = unit->matrix;
    FILL_DIRECTY_MATRIX;
    unit->matrix = matrix;
    if(INRATE(4) == calc_FullRate)
	SETCALC(FoaDirectY_next_a);
    else
	SETCALC(FoaDirectY_next_k);
    FoaDirectY_next_k(unit, 1); 
}

void FoaDirectY_next_a(FoaDirectY *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float *angle = IN(4);
    for(int i = 0; i < inNumSamples; i++){
	if(angle[i] != unit->m_angle){
	    unit->m_angle = angle[i];
	    FILL_DIRECTY_MATRIX
	}
	CALC_MATRIX
    }
    unit->matrix = matrix;
    
}

void FoaDirectY_next_k(FoaDirectY *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float angle = IN0(4);
    float angleslope;
    UNWRAPANGLE(angle, unit->m_angle);
    if(angle != unit->m_angle){
	angleslope = CALCSLOPE(angle, unit->m_angle);
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	    unit->m_angle += angleslope;
	    FILL_DIRECTY_MATRIX
	}	
    } else {
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	}
    }
    unit->matrix = matrix;
    unit->m_angle = angle;	
}

////////////////////// FoaDirectZ /////////////////////
// uses 'angle' var. 

#define FILL_DIRECTZ_MATRIX \
matrix.coefs[0][0]= matrix.coefs[1][1] = matrix.coefs[2][2]  = sqrt(1.0 + sin(unit->m_angle)); \
matrix.coefs[3][3] = sqrt(1.0 - sin(unit->m_angle)); \

void FoaDirectZ_Ctor(FoaDirectZ* unit)
{
    ZERO_MATRIX
    unit->m_angle = IN0(4);
    FoaMatrix matrix = unit->matrix;
    FILL_DIRECTZ_MATRIX;
    unit->matrix = matrix;
    if(INRATE(4) == calc_FullRate)
	SETCALC(FoaDirectZ_next_a);
    else
	SETCALC(FoaDirectZ_next_k);
    FoaDirectZ_next_k(unit, 1); 
}

void FoaDirectZ_next_a(FoaDirectZ *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float *angle = IN(4);
    for(int i = 0; i < inNumSamples; i++){
	if(angle[i] != unit->m_angle){
	    unit->m_angle = angle[i];
	    FILL_DIRECTZ_MATRIX
	}
	CALC_MATRIX
    }
    unit->matrix = matrix;
    
}

void FoaDirectZ_next_k(FoaDirectZ *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float angle = IN0(4);
    float angleslope;
    UNWRAPANGLE(angle, unit->m_angle);
    if(angle != unit->m_angle){
	angleslope = CALCSLOPE(angle, unit->m_angle);
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	    unit->m_angle += angleslope;
	    FILL_DIRECTZ_MATRIX
	}	
    } else {
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	}
    }
    unit->matrix = matrix;
    unit->m_angle = angle;	
}

////////////////////// FoaPushX ///////////////////////
// uses 'angle' var. 

#define FILL_PUSHX_MATRIX \
double cosa = cos(unit->m_angle); \
double cosa2 = cosa * cosa; \
double sina = sin(unit->m_angle); \
double asina = sin(fabs(unit->m_angle)); \
double sqrt2asinasina = sqrt2 * (asina * sina); \
matrix.coefs[0][0] = 1.f; \
matrix.coefs[1][1] = matrix.coefs[2][2] = matrix.coefs[3][3] = cosa2; \
matrix.coefs[1][0] = sqrt2asinasina; \

void FoaPushX_Ctor(FoaPushX* unit)
{
    ZERO_MATRIX
    unit->m_angle = IN0(4);
    FoaMatrix matrix = unit->matrix;
    FILL_PUSHX_MATRIX;
    unit->matrix = matrix;
    if(INRATE(4) == calc_FullRate)
	SETCALC(FoaPushX_next_a);
    else
	SETCALC(FoaPushX_next_k);
    FoaPushX_next_k(unit, 1); 
}

void FoaPushX_next_a(FoaPushX *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float *angle = IN(4);
    for(int i = 0; i < inNumSamples; i++){
	if(angle[i] != unit->m_angle){
	    unit->m_angle = angle[i];
	    FILL_PUSHX_MATRIX
	}
	CALC_MATRIX
    }
    unit->matrix = matrix;
    
}

void FoaPushX_next_k(FoaPushX *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float angle = IN0(4);
    float angleslope;
    UNWRAPANGLE(angle, unit->m_angle);
    if(angle != unit->m_angle){
	angleslope = CALCSLOPE(angle, unit->m_angle);
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	    unit->m_angle += angleslope;
	    FILL_PUSHX_MATRIX
	}	
    } else {
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	}
    }
    unit->matrix = matrix;
    unit->m_angle = angle;	
}

////////////////////// FoaPushY ///////////////////////
// uses 'angle' var.

#define FILL_PUSHY_MATRIX \
double cosa = cos(unit->m_angle); \
double cosa2 = cosa * cosa; \
double sina = sin(unit->m_angle); \
double asina = sin(fabs(unit->m_angle)); \
double sqrt2asinasina = sqrt2 * (asina * sina); \
matrix.coefs[0][0] = 1.f; \
matrix.coefs[1][1] = matrix.coefs[2][2] = matrix.coefs[3][3] = cosa2; \
matrix.coefs[2][0] = sqrt2asinasina; \



void FoaPushY_Ctor(FoaPushY* unit)
{
    ZERO_MATRIX
    unit->m_angle = IN0(4);
    FoaMatrix matrix = unit->matrix;
    FILL_PUSHY_MATRIX;
    unit->matrix = matrix;
    if(INRATE(4) == calc_FullRate)
	SETCALC(FoaPushY_next_a);
    else
	SETCALC(FoaPushY_next_k);
    FoaPushY_next_k(unit, 1); 
}

void FoaPushY_next_a(FoaPushY *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float *angle = IN(4);
    for(int i = 0; i < inNumSamples; i++){
	if(angle[i] != unit->m_angle){
	    unit->m_angle = angle[i];
	    FILL_PUSHY_MATRIX
	}
	CALC_MATRIX
    }
    unit->matrix = matrix;
    
}

void FoaPushY_next_k(FoaPushY *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float angle = IN0(4);
    float angleslope;
    UNWRAPANGLE(angle, unit->m_angle);
    if(angle != unit->m_angle){
	angleslope = CALCSLOPE(angle, unit->m_angle);
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	    unit->m_angle += angleslope;
	    FILL_PUSHY_MATRIX
	}	
    } else {
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	}
    }
    unit->matrix = matrix;
    unit->m_angle = angle;	
}

////////////////////// FoaPushZ ///////////////////////
// uses 'angle' var. 

#define FILL_PUSHZ_MATRIX \
double cosa = cos(unit->m_angle); \
double cosa2 = cosa * cosa; \
double sina = sin(unit->m_angle); \
double asina = sin(fabs(unit->m_angle)); \
double sqrt2asinasina = sqrt2 * (asina * sina); \
matrix.coefs[0][0] = 1.f; \
matrix.coefs[1][1] = matrix.coefs[2][2] = matrix.coefs[3][3] = cosa2; \
matrix.coefs[3][0] = sqrt2asinasina; \

void FoaPushZ_Ctor(FoaPushZ* unit)
{
    ZERO_MATRIX
    unit->m_angle = IN0(4);
    FoaMatrix matrix = unit->matrix;
    FILL_PUSHZ_MATRIX;
    unit->matrix = matrix;
    if(INRATE(4) == calc_FullRate)
	SETCALC(FoaPushZ_next_a);
    else
	SETCALC(FoaPushZ_next_k);
    FoaPushZ_next_k(unit, 1); 
}

void FoaPushZ_next_a(FoaPushZ *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float *angle = IN(4);
    for(int i = 0; i < inNumSamples; i++){
	if(angle[i] != unit->m_angle){
	    unit->m_angle = angle[i];
	    FILL_PUSHZ_MATRIX
	}
	CALC_MATRIX
    }
    unit->matrix = matrix;
    
}

void FoaPushZ_next_k(FoaPushZ *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float angle = IN0(4);
    float angleslope;
    UNWRAPANGLE(angle, unit->m_angle);
    if(angle != unit->m_angle){
	angleslope = CALCSLOPE(angle, unit->m_angle);
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	    unit->m_angle += angleslope;
	    FILL_PUSHZ_MATRIX
	}	
    } else {
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	}
    }
    unit->matrix = matrix;
    unit->m_angle = angle;	
}

////////////////////// FoaPressX ///////////////////////
// uses 'angle' var. 

#define FILL_PRESSX_MATRIX \
double cosa = cos(unit->m_angle); \
double cosa2 = cosa * cosa; \
double sina = sin(unit->m_angle); \
double asina = sin(fabs(unit->m_angle)); \
double sqrt2asinasina = sqrt2 * (asina * sina); \
matrix.coefs[0][0] = 1.f; \
matrix.coefs[1][0] = sqrt2asinasina; \
matrix.coefs[1][1] = cosa2; \
matrix.coefs[2][2] = matrix.coefs[3][3] = cosa; \


void FoaPressX_Ctor(FoaPressX* unit)
{
    ZERO_MATRIX
    unit->m_angle = IN0(4);
    FoaMatrix matrix = unit->matrix;
    FILL_PRESSX_MATRIX;
    unit->matrix = matrix;
    if(INRATE(4) == calc_FullRate)
	SETCALC(FoaPressX_next_a);
    else
	SETCALC(FoaPressX_next_k);
    FoaPressX_next_k(unit, 1); 
}

void FoaPressX_next_a(FoaPressX *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float *angle = IN(4);
    for(int i = 0; i < inNumSamples; i++){
	if(angle[i] != unit->m_angle){
	    unit->m_angle = angle[i];
	    FILL_PRESSX_MATRIX
	}
	CALC_MATRIX
    }
    unit->matrix = matrix;
    
}

void FoaPressX_next_k(FoaPressX *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float angle = IN0(4);
    float angleslope;
    UNWRAPANGLE(angle, unit->m_angle);
    if(angle != unit->m_angle){
	angleslope = CALCSLOPE(angle, unit->m_angle);
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	    unit->m_angle += angleslope;
	    FILL_PRESSX_MATRIX
	}	
    } else {
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	}
    }
    unit->matrix = matrix;
    unit->m_angle = angle;	
}

////////////////////// FoaPressY ///////////////////////
// uses 'angle' var. 


#define FILL_PRESSY_MATRIX \
double cosa = cos(unit->m_angle); \
double cosa2 = cosa * cosa; \
double sina = sin(unit->m_angle); \
double asina = sin(fabs(unit->m_angle)); \
double sqrt2asinasina = sqrt2 * (asina * sina); \
matrix.coefs[0][0] = 1.f; \
matrix.coefs[2][0] = sqrt2asinasina; \
matrix.coefs[2][2] = cosa2; \
matrix.coefs[1][1] = matrix.coefs[3][3] = cosa; \

void FoaPressY_Ctor(FoaPressY* unit)
{
    ZERO_MATRIX
    unit->m_angle = IN0(4);
    FoaMatrix matrix = unit->matrix;
    FILL_PRESSY_MATRIX;
    unit->matrix = matrix;
    if(INRATE(4) == calc_FullRate)
	SETCALC(FoaPressY_next_a);
    else
	SETCALC(FoaPressY_next_k);
    FoaPressY_next_k(unit, 1); 
}

void FoaPressY_next_a(FoaPressY *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float *angle = IN(4);
    for(int i = 0; i < inNumSamples; i++){
	if(angle[i] != unit->m_angle){
	    unit->m_angle = angle[i];
	    FILL_PRESSY_MATRIX
	}
	CALC_MATRIX
    }
    unit->matrix = matrix;
    
}

void FoaPressY_next_k(FoaPressY *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float angle = IN0(4);
    float angleslope;
    UNWRAPANGLE(angle, unit->m_angle);
    if(angle != unit->m_angle){
	angleslope = CALCSLOPE(angle, unit->m_angle);
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	    unit->m_angle += angleslope;
	    FILL_PRESSY_MATRIX
	}	
    } else {
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	}
    }
    unit->matrix = matrix;
    unit->m_angle = angle;	
}

////////////////////// FoaPressZ ///////////////////////
// uses 'angle' var. 

#define FILL_PRESSZ_MATRIX \
double cosa = cos(unit->m_angle); \
double cosa2 = cosa * cosa; \
double sina = sin(unit->m_angle); \
double asina = sin(fabs(unit->m_angle)); \
double sqrt2asinasina = sqrt2 * (asina * sina); \
matrix.coefs[0][0] = 1.f; \
matrix.coefs[3][0] = sqrt2asinasina; \
matrix.coefs[3][3] = cosa2; \
matrix.coefs[1][1] = matrix.coefs[2][2] = cosa; \

void FoaPressZ_Ctor(FoaPressZ* unit)
{
    ZERO_MATRIX
    unit->m_angle = IN0(4);
    FoaMatrix matrix = unit->matrix;
    FILL_PRESSZ_MATRIX;
    unit->matrix = matrix;
    if(INRATE(4) == calc_FullRate)
	SETCALC(FoaPressZ_next_a);
    else
	SETCALC(FoaPressZ_next_k);
    FoaPressZ_next_k(unit, 1); 
}

void FoaPressZ_next_a(FoaPressZ *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float *angle = IN(4);
    for(int i = 0; i < inNumSamples; i++){
	if(angle[i] != unit->m_angle){
	    unit->m_angle = angle[i];
	    FILL_PRESSZ_MATRIX
	}
	CALC_MATRIX
    }
    unit->matrix = matrix;
    
}

void FoaPressZ_next_k(FoaPressZ *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float angle = IN0(4);
    float angleslope;
    UNWRAPANGLE(angle, unit->m_angle);
    if(angle != unit->m_angle){
	angleslope = CALCSLOPE(angle, unit->m_angle);
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	    unit->m_angle += angleslope;
	    FILL_PRESSZ_MATRIX
	}	
    } else {
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	}
    }
    unit->matrix = matrix;
    unit->m_angle = angle;	
}

////////////////////// FoaZoomX ///////////////////////
// uses 'angle' var. 

#define FILL_ZOOMX_MATRIX \
double cosa = cos(unit->m_angle); \
double sina = sin(unit->m_angle); \
matrix.coefs[0][0] = matrix.coefs[1][1] = 1.; \
matrix.coefs[2][2] = matrix.coefs[3][3] = cosa; \
matrix.coefs[0][1] = rsqrt2 * sina; \
matrix.coefs[1][0] = sqrt2 * sina;

void FoaZoomX_Ctor(FoaZoomX* unit)
{
    ZERO_MATRIX
    unit->m_angle = IN0(4);
    FoaMatrix matrix = unit->matrix;
    FILL_ZOOMX_MATRIX;
    unit->matrix = matrix;
    if(INRATE(4) == calc_FullRate)
	SETCALC(FoaZoomX_next_a);
    else
	SETCALC(FoaZoomX_next_k);
    FoaZoomX_next_k(unit, 1); 
}

void FoaZoomX_next_a(FoaZoomX *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float *angle = IN(4);
    for(int i = 0; i < inNumSamples; i++){
	if(angle[i] != unit->m_angle){
	    unit->m_angle = angle[i];
	    FILL_ZOOMX_MATRIX
	}
	CALC_MATRIX
    }
    unit->matrix = matrix;
    
}

void FoaZoomX_next_k(FoaZoomX *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float angle = IN0(4);
    float angleslope;
    UNWRAPANGLE(angle, unit->m_angle);
    if(angle != unit->m_angle){
	angleslope = CALCSLOPE(angle, unit->m_angle);
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	    unit->m_angle += angleslope;
	    FILL_ZOOMX_MATRIX
	}	
    } else {
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	}
    }
    unit->matrix = matrix;
    unit->m_angle = angle;	
}

////////////////////// FoaZoomY ///////////////////////
// uses 'angle' var. 

#define FILL_ZOOMY_MATRIX \
double cosa = cos(unit->m_angle); \
double sina = sin(unit->m_angle); \
matrix.coefs[0][0] = matrix.coefs[2][2] = 1.; \
matrix.coefs[1][1] = matrix.coefs[3][3] = cosa; \
matrix.coefs[0][2] = rsqrt2 * sina; \
matrix.coefs[2][0] = sqrt2 * sina; \


void FoaZoomY_Ctor(FoaZoomY* unit)
{
    ZERO_MATRIX
    unit->m_angle = IN0(4);
    FoaMatrix matrix = unit->matrix;
    FILL_ZOOMY_MATRIX;
    unit->matrix = matrix;
    if(INRATE(4) == calc_FullRate)
	SETCALC(FoaZoomY_next_a);
    else
	SETCALC(FoaZoomY_next_k);
    FoaZoomY_next_k(unit, 1); 
}

void FoaZoomY_next_a(FoaZoomY *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float *angle = IN(4);
    for(int i = 0; i < inNumSamples; i++){
	if(angle[i] != unit->m_angle){
	    unit->m_angle = angle[i];
	    FILL_ZOOMY_MATRIX
	}
	CALC_MATRIX
    }
    unit->matrix = matrix;
    
}

void FoaZoomY_next_k(FoaZoomY *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float angle = IN0(4);
    float angleslope;
    UNWRAPANGLE(angle, unit->m_angle);
    if(angle != unit->m_angle){
	angleslope = CALCSLOPE(angle, unit->m_angle);
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	    unit->m_angle += angleslope;
	    FILL_ZOOMY_MATRIX
	}	
    } else {
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	}
    }
    unit->matrix = matrix;
    unit->m_angle = angle;	
}

////////////////////// FoaZoomZ ///////////////////////
// uses 'angle' var. 

#define FILL_ZOOMZ_MATRIX \
double cosa = cos(unit->m_angle); \
double sina = sin(unit->m_angle); \
matrix.coefs[0][0] = matrix.coefs[3][3] = 1.; \
matrix.coefs[1][1] = matrix.coefs[2][2] = cosa; \
matrix.coefs[0][3] = rsqrt2 * sina; \
matrix.coefs[3][0] = sqrt2 * sina; \

void FoaZoomZ_Ctor(FoaZoomZ* unit)
{
    ZERO_MATRIX
    unit->m_angle = IN0(4);
    FoaMatrix matrix = unit->matrix;
    FILL_ZOOMZ_MATRIX;
    unit->matrix = matrix;
    if(INRATE(4) == calc_FullRate)
	SETCALC(FoaZoomZ_next_a);
    else
	SETCALC(FoaZoomZ_next_k);
    FoaZoomZ_next_k(unit, 1); 
}

void FoaZoomZ_next_a(FoaZoomZ *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float *angle = IN(4);
    for(int i = 0; i < inNumSamples; i++){
	if(angle[i] != unit->m_angle){
	    unit->m_angle = angle[i];
	    FILL_ZOOMZ_MATRIX
	}
	CALC_MATRIX
    }
    unit->matrix = matrix;
    
}

void FoaZoomZ_next_k(FoaZoomZ *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float angle = IN0(4);
    float angleslope;
    UNWRAPANGLE(angle, unit->m_angle);
    if(angle != unit->m_angle){
	angleslope = CALCSLOPE(angle, unit->m_angle);
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	    unit->m_angle += angleslope;
	    FILL_ZOOMZ_MATRIX
	}	
    } else {
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	}
    }
    unit->matrix = matrix;
    unit->m_angle = angle;	
}

////////////////////// FoaDominateX ///////////////////////
// uses 'gain' var (in dB).

#define FILL_DOMINATEX_MATRIX \
double dominate = pow(10, unit->m_gain * 0.05); \
double rsqrt8 = 1./sqrt(8.); \
double rdom = 1./dominate; \
double hdomprdom = 0.5 * (dominate + rdom); \
double dommrdom = dominate - rdom; \
matrix.coefs[0][0] = matrix.coefs[1][1] = hdomprdom; \
matrix.coefs[0][1] = rsqrt8 * dommrdom; \
matrix.coefs[1][0] = rsqrt2 * dommrdom; \
matrix.coefs[2][2] = matrix.coefs[3][3] = 1.; \

void FoaDominateX_Ctor(FoaDominateX* unit)
{
    ZERO_MATRIX
    unit->m_gain = IN0(4);
    FoaMatrix matrix = unit->matrix;
    FILL_DOMINATEX_MATRIX;
    unit->matrix = matrix;
    if(INRATE(4) == calc_FullRate)
	SETCALC(FoaDominateX_next_a);
    else
	SETCALC(FoaDominateX_next_k);
    FoaDominateX_next_k(unit, 1); 
}

void FoaDominateX_next_a(FoaDominateX *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float *gain = IN(4);
    for(int i = 0; i < inNumSamples; i++){
	if(gain[i] != unit->m_gain){
	    unit->m_gain = gain[i];
	    FILL_DOMINATEX_MATRIX
	}
	CALC_MATRIX
    }
    unit->matrix = matrix;
    
}

void FoaDominateX_next_k(FoaDominateX *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float gain = IN0(4);
    float gainslope;
    if(gain != unit->m_gain){
	gainslope = CALCSLOPE(gain, unit->m_gain);
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	    unit->m_gain += gainslope;
	    FILL_DOMINATEX_MATRIX
	}	
    } else {
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	}
    }
    unit->matrix = matrix;
    unit->m_gain = gain;	
}

////////////////////// FoaDominateY ///////////////////////
// uses 'angle' var. 

#define FILL_DOMINATEY_MATRIX \
double dominate = pow(10, unit->m_gain * 0.05); \
double rsqrt8 = 1./sqrt(8.); \
double rdom = 1./dominate; \
double hdomprdom = 0.5 * (dominate + rdom); \
double dommrdom = dominate - rdom; \
matrix.coefs[0][0] = matrix.coefs[2][2] = hdomprdom; \
matrix.coefs[0][2] = rsqrt8 * dommrdom; \
matrix.coefs[2][0] = rsqrt2 * dommrdom; \
matrix.coefs[1][1] = matrix.coefs[3][3] = 1.; \

void FoaDominateY_Ctor(FoaDominateY* unit)
{
    ZERO_MATRIX
    unit->m_gain = IN0(4);
    FoaMatrix matrix = unit->matrix;
    FILL_DOMINATEY_MATRIX;
    unit->matrix = matrix;
    if(INRATE(4) == calc_FullRate)
	SETCALC(FoaDominateY_next_a);
    else
	SETCALC(FoaDominateY_next_k);
    FoaDominateY_next_k(unit, 1); 
}

void FoaDominateY_next_a(FoaDominateY *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float *gain = IN(4);
    for(int i = 0; i < inNumSamples; i++){
	if(gain[i] != unit->m_gain){
	    unit->m_gain = gain[i];
	    FILL_DOMINATEY_MATRIX
	}
	CALC_MATRIX
    }
    unit->matrix = matrix;
    
}

void FoaDominateY_next_k(FoaDominateY *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float gain = IN0(4);
    float gainslope;
    if(gain != unit->m_gain){
	gainslope = CALCSLOPE(gain, unit->m_gain);
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	    unit->m_gain += gainslope;
	    FILL_DOMINATEY_MATRIX
	}	
    } else {
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	}
    }
    unit->matrix = matrix;
    unit->m_gain = gain;	
}

////////////////////// FoaDominateZ ///////////////////////
// uses 'angle' var. 

#define FILL_DOMINATEZ_MATRIX \
double dominate = pow(10, unit->m_gain * 0.05); \
double rsqrt8 = 1./sqrt(8.); \
double rdom = 1./dominate; \
double hdomprdom = 0.5 * (dominate + rdom); \
double dommrdom = dominate - rdom; \
matrix.coefs[0][0] = matrix.coefs[3][3] = hdomprdom; \
matrix.coefs[0][3] = rsqrt8 * dommrdom; \
matrix.coefs[3][0] = rsqrt2 * dommrdom; \
matrix.coefs[1][1] = matrix.coefs[2][2] = 1.; \

void FoaDominateZ_Ctor(FoaDominateZ* unit)
{
    ZERO_MATRIX
    unit->m_gain = IN0(4);
    FoaMatrix matrix = unit->matrix;
    FILL_DOMINATEZ_MATRIX;
    unit->matrix = matrix;
    if(INRATE(4) == calc_FullRate)
	SETCALC(FoaDominateZ_next_a);
    else
	SETCALC(FoaDominateZ_next_k);
    FoaDominateZ_next_k(unit, 1); 
}

void FoaDominateZ_next_a(FoaDominateZ *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float *gain = IN(4);
    for(int i = 0; i < inNumSamples; i++){
	if(gain[i] != unit->m_gain){
	    unit->m_gain = gain[i];
	    FILL_DOMINATEZ_MATRIX
	}
	CALC_MATRIX
    }
    unit->matrix = matrix;
    
}

void FoaDominateZ_next_k(FoaDominateZ *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float gain = IN0(4);
    float gainslope;
    if(gain != unit->m_gain){
	gainslope = CALCSLOPE(gain, unit->m_gain);
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	    unit->m_gain += gainslope;
	    FILL_DOMINATEZ_MATRIX
	}	
    } else {
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	}
    }
    unit->matrix = matrix;
    unit->m_gain = gain;	
}



////////////////////// FoaDirectO ///////////////////////
// uses 'angle' var

#define FILL_DIRECT_MATRIX \
matrix.coefs[0][0] = sqrt(1.0 + sin(unit->m_angle)); \
matrix.coefs[1][1] = matrix.coefs[2][2] = matrix.coefs[3][3] = sqrt(1.0 - sin(unit->m_angle)); \

void FoaDirectO_Ctor(FoaDirectO* unit)
{
    ZERO_MATRIX
    unit->m_angle = IN0(4);
    FoaMatrix matrix = unit->matrix;
    FILL_DIRECT_MATRIX;
    unit->matrix = matrix;
    if(INRATE(4) == calc_FullRate)
	SETCALC(FoaDirectO_next_a);
    else
	SETCALC(FoaDirectO_next_k);
    FoaDirectO_next_k(unit, 1); 
}

void FoaDirectO_next_a(FoaDirectO *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float *angle = IN(4);
    
    for(int i = 0; i < inNumSamples; i++){
	if(angle[i] != unit->m_angle){
	    unit->m_angle = angle[i];
	    FILL_DIRECT_MATRIX
	}
	CALC_MATRIX
    }
    unit->matrix = matrix;
}


void FoaDirectO_next_k(FoaDirectO *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float angle = IN0(4);
    float angleslope;
    
    UNWRAPANGLE(angle, unit->m_angle);
    if(angle != unit->m_angle){
	angleslope = CALCSLOPE(angle, unit->m_angle);
	FILL_DIRECT_MATRIX	
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	    unit->m_angle += angleslope;
	    FILL_DIRECT_MATRIX
	}	
    } else {
	for(int i = 0; i < inNumSamples; i++){
	    CALC_MATRIX
	}
    }
    
    unit->matrix = matrix;
    unit->m_angle = angle;	
}


////////////////////// FoaAsymmetry ///////////////////////
// uses 'angle' var. 

#define FILL_ASYMMETRY_MATRIX \
double cosa = cos(unit->m_angle); \
double sina = sin(unit->m_angle); \
matrix.coefs[0][0] = 1.; \
matrix.coefs[1][1] = cosa * cosa; \
matrix.coefs[2][2] = matrix.coefs[3][3] = cosa; \
matrix.coefs[0][2] = -rsqrt2 * sina; \
matrix.coefs[1][0] = sqrt2 * sina * sina; \
matrix.coefs[1][2] = -sina; \
matrix.coefs[2][0] = -sqrt2 * cosa * sina; \
matrix.coefs[2][1] = cosa * sina;

void FoaAsymmetry_Ctor(FoaAsymmetry* unit)
{
    ZERO_MATRIX
    unit->m_angle = IN0(4);
    FoaMatrix matrix = unit->matrix;
    FILL_ASYMMETRY_MATRIX;
    unit->matrix = matrix;
    if(INRATE(4) == calc_FullRate)
		SETCALC(FoaAsymmetry_next_a);
    else
		SETCALC(FoaAsymmetry_next_k);
    FoaAsymmetry_next_k(unit, 1); 
}

void FoaAsymmetry_next_a(FoaAsymmetry *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float *angle = IN(4);
    for(int i = 0; i < inNumSamples; i++){
		if(angle[i] != unit->m_angle){
			unit->m_angle = angle[i];
			FILL_ASYMMETRY_MATRIX
		}
		CALC_MATRIX
    }
    unit->matrix = matrix;
    
}

void FoaAsymmetry_next_k(FoaAsymmetry *unit, int inNumSamples)
{
    SETUP_TRANSFORMS
    float angle = IN0(4);
    float angleslope;
    UNWRAPANGLE(angle, unit->m_angle);
    if(angle != unit->m_angle){
		angleslope = CALCSLOPE(angle, unit->m_angle);
		for(int i = 0; i < inNumSamples; i++){
			CALC_MATRIX
			unit->m_angle += angleslope;
			FILL_ASYMMETRY_MATRIX
		}	
    } else {
		for(int i = 0; i < inNumSamples; i++){
			CALC_MATRIX
		}
    }
    unit->matrix = matrix;
    unit->m_angle = angle;	
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// FoaNFC - 
void FoaNFC_Ctor(FoaNFC* unit)
{
    unit->m_y1x = 0.0f;
    unit->m_y1y = 0.0f;
    unit->m_y1z = 0.0f;
    unit->m_distanceStart = IN0(4);
    if (INRATE(4) == calc_FullRate) {
	SETCALC(FoaNFC_next_a);
    } else {
	SETCALC(FoaNFC_next_k);
    };
    ClearUnitOutputs(unit, 1);
}

void FoaNFC_next_k(FoaNFC *unit, int inNumSamples)
{       
    float *Wout = OUT(0);
    float *Xout = OUT(1);
    float *Yout = OUT(2);
    float *Zout = OUT(3);
    
    float *Win = IN(0);
    float *Xin = IN(1);
    float *Yin = IN(2);
    float *Zin = IN(3);
    float distanceEnd = IN0(4);
    float distanceStart = unit->m_distanceStart;
    
    float distanceInc = CALCSLOPE(distanceEnd, distanceStart);
    
    float y1x = unit->m_y1x;
    float y1y = unit->m_y1y;
    float y1z = unit->m_y1z;
    
    for(int i = 0; i < inNumSamples; i++){
		float freq = 53.0 / distanceStart;
		float wc = (twopi * freq) * SAMPLEDUR;

		//	a0 = (1 + (wc.cos.neg * 2 + 2).sqrt).reciprocal;
		float a0 = 1 / (sqrt((cos(wc) * -2) + 2) + 1);

		// W is passed straight out
		Wout[i] = Win[i];
		
		// filter 1st order
		float y0x = Xin[i] + a0 * y1x;
		Xout[i] = a0 * y0x + -a0 * y1x;
		y1x = y0x;

		float y0y = Yin[i] + a0 * y1y;
		Yout[i] = a0 * y0y + -a0 * y1y;
		y1y = y0y;

		float y0z = Zin[i] + a0 * y1z;
		Zout[i] = a0 * y0z + -a0 * y1z;
		y1z = y0z;

		distanceStart += distanceInc;
    }
    
    unit->m_y1x = zapgremlins(y1x);
    unit->m_y1y = zapgremlins(y1y);
    unit->m_y1z = zapgremlins(y1z);
    unit->m_distanceStart = distanceEnd;
    
}

void FoaNFC_next_a(FoaNFC *unit, int inNumSamples)
{       
    float *Wout = OUT(0);
    float *Xout = OUT(1);
    float *Yout = OUT(2);
    float *Zout = OUT(3);
    
    float *Win = IN(0);
    float *Xin = IN(1);
    float *Yin = IN(2);
    float *Zin = IN(3);
    float *distance = IN(4);
    
    float y1x = unit->m_y1x;
    float y1y = unit->m_y1y;
    float y1z = unit->m_y1z;
    
    for(int i = 0; i < inNumSamples; i++){
		float freq = 53.0 / distance[i];
		float wc = (twopi * freq) * SAMPLEDUR;
		float a0 = 1 / (sqrt((cos(wc) * -2) + 2) + 1);

		// W is passed straight out
		Wout[i] = Win[i];

		// filter 1st order
		float y0x = Xin[i] + a0 * y1x;
		Xout[i] = a0 * y0x + -a0 * y1x;
		y1x = y0x;

		float y0y = Yin[i] + a0 * y1y;
		Yout[i] = a0 * y0y + -a0 * y1y;
		y1y = y0y;

		float y0z = Zin[i] + a0 * y1z;
		Zout[i] = a0 * y0z + -a0 * y1z;
		y1z = y0z;
    }
    
    unit->m_y1x = zapgremlins(y1x);
    unit->m_y1y = zapgremlins(y1y);
    unit->m_y1z = zapgremlins(y1z);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// FoaProximity

void FoaProximity_Ctor(FoaProximity* unit)
{
    unit->m_y1x = 0.0f;
    unit->m_y1y = 0.0f;
    unit->m_y1z = 0.0f;
    unit->m_distanceStart = IN0(4);
    if (INRATE(4) == calc_FullRate) {
	SETCALC(FoaProximity_next_a);
    } else {
	SETCALC(FoaProximity_next_k);
    };
    ClearUnitOutputs(unit, 1);
}

void FoaProximity_next_k(FoaProximity *unit, int inNumSamples)
{       
    float *Wout = OUT(0);
    float *Xout = OUT(1);
    float *Yout = OUT(2);
    float *Zout = OUT(3);
    
    float *Win = IN(0);
    float *Xin = IN(1);
    float *Yin = IN(2);
    float *Zin = IN(3);
    float distanceEnd = IN0(4);
    float distanceStart = unit->m_distanceStart;
    
    float distanceInc = CALCSLOPE(distanceEnd, distanceStart);
    
    float y1x = unit->m_y1x;
    float y1y = unit->m_y1y;
    float y1z = unit->m_y1z;
    
    for(int i=0; i<inNumSamples;i++){
		float freq = 53.0 / distanceStart;
		float wc = (twopi * freq) * SAMPLEDUR;

		//	a0 = 1 + (wc.cos.neg * 2 + 2).sqrt;
		float a0 = 1 + sqrt((cos(wc) * -2) + 2);

		// W is passed straight out
		Wout[i] = Win[i];

		// filter 1st order
		float y0x = Xin[i] + y1x;
		Xout[i] = a0 * y0x - y1x;
		y1x = y0x;

		float y0y = Yin[i] + y1y;
		Yout[i] = a0 * y0y - y1y;
		y1y = y0y;

		float y0z = Zin[i] + y1z;
		Zout[i] = a0 * y0z - y1z;
		y1z = y0z;

		distanceStart += distanceInc;
    }
    
    unit->m_y1x = zapgremlins(y1x);
    unit->m_y1y = zapgremlins(y1y);
    unit->m_y1z = zapgremlins(y1z);
    unit->m_distanceStart = distanceEnd;
    
}

void FoaProximity_next_a(FoaProximity *unit, int inNumSamples)
{       
    float *Wout = OUT(0);
    float *Xout = OUT(1);
    float *Yout = OUT(2);
    float *Zout = OUT(3);
    
    float *Win = IN(0);
    float *Xin = IN(1);
    float *Yin = IN(2);
    float *Zin = IN(3);
    float *distance = IN(4);
    
    float y1x = unit->m_y1x;
    float y1y = unit->m_y1y;
    float y1z = unit->m_y1z;
    
    for(int i = 0; i<inNumSamples; i++){
		float freq = 53.0 / distance[i];
		float wc = (twopi * freq) * SAMPLEDUR;
		float a0 = 1 + sqrt((cos(wc) * -2) + 2);

		// W is passed straight out
		Wout[i] = Win[i];

		// filter 1st order
		float y0x = Xin[i] + y1x;
		Xout[i] = a0 * y0x - y1x;
		y1x = y0x;
		
		float y0y = Yin[i] + y1y;
		Yout[i] = a0 * y0y - y1y;
		y1y = y0y;
		
		float y0z = Zin[i] + y1z;
		Zout[i] = a0 * y0z - y1z;
		y1z = y0z;
    }
    
    unit->m_y1x = zapgremlins(y1x);
    unit->m_y1y = zapgremlins(y1y);
    unit->m_y1z = zapgremlins(y1z);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// FoaPsychoShelf

void FoaPsychoShelf_Ctor(FoaPsychoShelf* unit)
{
    unit->m_y1w = 0.0f;
    unit->m_y2w = 0.0f;
    unit->m_y1x = 0.0f;
    unit->m_y2x = 0.0f;
    unit->m_y1y = 0.0f;
    unit->m_y2y = 0.0f;
    unit->m_y1z = 0.0f;
    unit->m_y2z = 0.0f;
    unit->m_freq = IN0(4);
    unit->m_k0 = IN0(5);
    unit->m_k1 = IN0(6);
    if (INRATE(4) == calc_FullRate || INRATE(5) == calc_FullRate || INRATE(6) == calc_FullRate) {
		SETCALC(FoaPsychoShelf_next_a);
    } else {
		SETCALC(FoaPsychoShelf_next_k);
    };
    ClearUnitOutputs(unit, 1);
}

void FoaPsychoShelf_next_k(FoaPsychoShelf *unit, int inNumSamples)
{       
    float *Wout = OUT(0);
    float *Xout = OUT(1);
    float *Yout = OUT(2);
    float *Zout = OUT(3);
    
    float *Win = IN(0);
    float *Xin = IN(1);
    float *Yin = IN(2);
    float *Zin = IN(3);
	float freqEnd = IN0(4);
	float k0End = IN0(5);
	float k1End = IN0(6);

	float freqStart = unit->m_freq;
	float k0Start = unit->m_k0;
	float k1Start = unit->m_k1;

	float freqInc = CALCSLOPE(freqEnd, freqStart);
	float k0Inc = CALCSLOPE(k0End, k0Start);
	float k1Inc = CALCSLOPE(k1End, k1Start);
    
	float y0w;
    float y1w = unit->m_y1w;
    float y2w = unit->m_y2w;
	float y0x;
    float y1x = unit->m_y1x;
    float y2x = unit->m_y2x;
	float y0y;
    float y1y = unit->m_y1y;
    float y2y = unit->m_y2y;
	float y0z;
    float y1z = unit->m_y1z;
    float y2z = unit->m_y2z;
    
    for(int i=0; i<inNumSamples;i++){

		// wc = (pi * frequency / Server.default.sampleRate).tan
		// c = (wc - 1) / (wc + 1)
		float wc = tan(pi * freqStart * SAMPLEDUR);
		float c = (wc - 1.0) / (wc + 1.0);

		// a0 = (((1 - k2)/4) * (1 + (c**2))) + (((1 + k2)/2) * c);
		// a1 = ((1 - k2) * c) + (((1 + k2)/2) * (1 + (c**2)));
		// a2 = a0;
		//
		// b1 = Array.fill( k2.size, { (2*c).neg } );
		// b2 = Array.fill( k2.size, { (c**2).neg } );
		
		// 0 order coefficients
		float a0w = (((1.0 - k0Start) * 0.25) * (1.0 + (c*c))) + (((1.0 + k0Start) * 0.5) * c);
		float a1w = ((1.0 - k0Start) * c) + (((1.0 + k0Start) * 0.5) * (1.0 + (c*c)));
		float a2w = a0w;
		float b1w = -2.0 * c;
		float b2w = -c * c;

		// 1st order coefficients
		float a0xyz = (((1.0 - k1Start) * 0.25) * (1.0 + (c*c))) + (((1.0 + k1Start) * 0.5) * c);
		float a1xyz = ((1.0 - k1Start) * c) + (((1.0 + k1Start) * 0.5) * (1.0 + (c*c)));
		float a2xyz = a0xyz;
		float b1xyz = -2.0 * c;
		float b2xyz = -c * c;
		

		// filter 0 order
		y0w = Win[i] + b1w * y1w + b2w * y2w;
		Wout[i] = a0w * y0w + a1w * y1w + a2w * y2w;
		y2w = y1w;
		y1w = y0w;
		
		
		// filter 1st order
		y0x = Xin[i] + b1xyz * y1x + b2xyz * y2x;
		Xout[i] = a0xyz * y0x + a1xyz * y1x + a2xyz * y2x;
		y2x = y1x;
		y1x = y0x;

		y0y = Yin[i] + b1xyz * y1y + b2xyz * y2y;
		Yout[i] = a0xyz * y0y + a1xyz * y1y + a2xyz * y2y;
		y2y = y1y;
		y1y = y0y;
		
		y0z = Zin[i] + b1xyz * y1z + b2xyz * y2z;
		Zout[i] = a0xyz * y0z + a1xyz * y1z + a2xyz * y2z;
		y2z = y1z;
		y1z = y0z;
		
		
		freqStart += freqInc;
		k0Start += k0Inc;
		k1Start += k1Inc;
    }
    
    unit->m_y1w = zapgremlins(y1w);
    unit->m_y2w = zapgremlins(y2w);
    unit->m_y1x = zapgremlins(y1x);
    unit->m_y2x = zapgremlins(y2x);
    unit->m_y1y = zapgremlins(y1y);
    unit->m_y2y = zapgremlins(y2y);
    unit->m_y1z = zapgremlins(y1z);
    unit->m_y2z = zapgremlins(y2z);

    unit->m_freq = freqEnd;
    unit->m_k0 = k0End;
    unit->m_k1 = k1End;
    
}

void FoaPsychoShelf_next_a(FoaPsychoShelf *unit, int inNumSamples)
{       
    float *Wout = OUT(0);
    float *Xout = OUT(1);
    float *Yout = OUT(2);
    float *Zout = OUT(3);
    
    float *Win = IN(0);
    float *Xin = IN(1);
    float *Yin = IN(2);
    float *Zin = IN(3);
	float *freq = IN(4);
	float *k0 = IN(5);
	float *k1 = IN(6);
    
	float y0w;
    float y1w = unit->m_y1w;
    float y2w = unit->m_y2w;
	float y0x;
    float y1x = unit->m_y1x;
    float y2x = unit->m_y2x;
	float y0y;
    float y1y = unit->m_y1y;
    float y2y = unit->m_y2y;
	float y0z;
    float y1z = unit->m_y1z;
    float y2z = unit->m_y2z;
    
    for(int i = 0; i<inNumSamples; i++){
		// wc = (pi * frequency / Server.default.sampleRate).tan
		// c = (wc - 1) / (wc + 1)
		float wc = tan(pi * freq[i] * SAMPLEDUR);
		float c = (wc - 1.0) / (wc + 1.0);
		
		// a0 = (((1 - k2)/4) * (1 + (c**2))) + (((1 + k2)/2) * c);
		// a1 = ((1 - k2) * c) + (((1 + k2)/2) * (1 + (c**2)));
		// a2 = a0;
		//
		// b1 = Array.fill( k2.size, { (2*c).neg } );
		// b2 = Array.fill( k2.size, { (c**2).neg } );
		
		// 0 order coefficients
		float a0w = (((1.0 - k0[i]) * 0.25) * (1.0 + (c*c))) + (((1.0 + k0[i]) * 0.5) * c);
		float a1w = ((1.0 - k0[i]) * c) + (((1.0 + k0[i]) * 0.5) * (1.0 + (c*c)));
		float a2w = a0w;
		float b1w = -2.0 * c;
		float b2w = -c * c;
		
		// 1st order coefficients
		float a0xyz = (((1.0 - k1[i]) * 0.25) * (1.0 + (c*c))) + (((1.0 + k1[i]) * 0.5) * c);
		float a1xyz = ((1.0 - k1[i]) * c) + (((1.0 + k1[i]) * 0.5) * (1.0 + (c*c)));
		float a2xyz = a0xyz;
		float b1xyz = -2.0 * c;
		float b2xyz = -c * c;
		
		
		// filter 0 order
		y0w = Win[i] + b1w * y1w + b2w * y2w;
		Wout[i] = a0w * y0w + a1w * y1w + a2w * y2w;
		y2w = y1w;
		y1w = y0w;
		
		
		// filter 1st order
		y0x = Xin[i] + b1xyz * y1x + b2xyz * y2x;
		Xout[i] = a0xyz * y0x + a1xyz * y1x + a2xyz * y2x;
		y2x = y1x;
		y1x = y0x;
		
		y0y = Yin[i] + b1xyz * y1y + b2xyz * y2y;
		Yout[i] = a0xyz * y0y + a1xyz * y1y + a2xyz * y2y;
		y2y = y1y;
		y1y = y0y;
		
		y0z = Zin[i] + b1xyz * y1z + b2xyz * y2z;
		Zout[i] = a0xyz * y0z + a1xyz * y1z + a2xyz * y2z;
		y2z = y1z;
		y1z = y0z;
    }
    
    unit->m_y1w = zapgremlins(y1w);
    unit->m_y2w = zapgremlins(y2w);
    unit->m_y1x = zapgremlins(y1x);
    unit->m_y2x = zapgremlins(y2x);
    unit->m_y1y = zapgremlins(y1y);
    unit->m_y2y = zapgremlins(y2y);
    unit->m_y1z = zapgremlins(y1z);
    unit->m_y2z = zapgremlins(y2z);
}




////////////////////////////////////////////////////////////////////////////////////////////////////////

PluginLoad(ATK)
{
	ft = inTable;
    
    DefineSimpleCantAliasUnit(FoaPanB);
    
    DefineSimpleCantAliasUnit(FoaDirectO);
    DefineSimpleCantAliasUnit(FoaDirectX);
    DefineSimpleCantAliasUnit(FoaDirectY);
    DefineSimpleCantAliasUnit(FoaDirectZ);
    DefineSimpleCantAliasUnit(FoaRotate);
    DefineSimpleCantAliasUnit(FoaTilt);
    DefineSimpleCantAliasUnit(FoaTumble);
    DefineSimpleCantAliasUnit(FoaFocusX);	
    DefineSimpleCantAliasUnit(FoaFocusY);
    DefineSimpleCantAliasUnit(FoaFocusZ);
    DefineSimpleCantAliasUnit(FoaPushX);
    DefineSimpleCantAliasUnit(FoaPushY);
    DefineSimpleCantAliasUnit(FoaPushZ);
    DefineSimpleCantAliasUnit(FoaPressX);
    DefineSimpleCantAliasUnit(FoaPressY);
    DefineSimpleCantAliasUnit(FoaPressZ);
    DefineSimpleCantAliasUnit(FoaZoomX);
    DefineSimpleCantAliasUnit(FoaZoomY);
    DefineSimpleCantAliasUnit(FoaZoomZ);
    DefineSimpleCantAliasUnit(FoaDominateX);
    DefineSimpleCantAliasUnit(FoaDominateY);
    DefineSimpleCantAliasUnit(FoaDominateZ);
    DefineSimpleCantAliasUnit(FoaAsymmetry);
    
    
    DefineSimpleCantAliasUnit(FoaNFC);
    DefineSimpleCantAliasUnit(FoaProximity);
    DefineSimpleCantAliasUnit(FoaPsychoShelf);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

