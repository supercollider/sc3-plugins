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

//This file is part of MachineListening. Copyright (C) 2006  Nicholas M.Collins distributed under the terms of the GNU General Public License full notice in file MachineListening.license

//MachineListening.h  Created by Nick Collins on 06/12/2006.

#include "SC_PlugIn.h"
#include <fftw3.h>

//global data

//helpful constants
#define PI 3.1415926535898f
#define TWOPI 6.28318530717952646f 


extern float hanning256[256]; 
extern float hanning1024[1024];


extern InterfaceTable *ft;

#include "AutoTrackfftw.h"
#include "AnalyseEvents2fftw.h"
#include "Concatfftw.h"
#include "Tartini.h"
#include "Qitchfftw.h"



