/* -------------------------------------------------------------------------------
 * DFM-1 Digital Filter Module (SuperCollider UGen)
 * Copyright (C) 2010 Jonny Stutters
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 * ------------------------------------------------------------------------------ */

#include "SC_PlugIn.h"
#include "Dfm1.h"
#include <iostream>

static InterfaceTable *ft;

struct DFM1 : public Unit
{
  double m_l;
  double m_h;
  double m_a;
  double m_b;
  double m_r;
  double m_s;
  
  double m_za;
  double m_zb;
  double m_zh;
  double m_zr;
  double m_zy;
  
  uint32_t m_x;
  uint32_t m_y;
  uint32_t m_z;
  uint32_t m_w;
  
  float filterFreq;
  float resonance;
};

extern "C"
{
  void DFM1_next(DFM1 *unit, int inNumSamples);
  void DFM1_Ctor(DFM1 *unit);
};

void DFM1_Ctor(DFM1* unit)
{
  SETCALC(DFM1_next);
    
  Dfm1State df;
  Dfm1::initialize(&df);
  
  unit->m_l = df.l;
  unit->m_h = df.h;
  unit->m_a = df.a;
  unit->m_b = df.b;
  unit->m_r = df.r;
  unit->m_s = df.s;
  
  unit->m_za = df.za;
  unit->m_zb = df.zb;
  unit->m_zh = df.zh;
  unit->m_zr = df.zr;
  unit->m_zy = df.zy;
  
  unit->m_x = df.ng.x;
  unit->m_y = df.ng.y;
  unit->m_z = df.ng.z;
  unit->m_w = df.ng.w;
}

void DFM1_next(DFM1 *unit, int inNumSamples)
{
  float *in = IN(0);
  float *out = OUT(0);
  
  float frequency = IN0(1);
  float resonance = IN0(2);
  float inputGain = IN0(3);
  float type = IN0(4);
  float noiseLevel = IN0(5);

  Dfm1State df;
  df.l = unit->m_l;
  df.h = unit->m_h;
  df.a = unit->m_a;
  df.b = unit->m_b;
  df.r = unit->m_r;
  df.s = unit->m_s;
  
  df.za = unit->m_za;
  df.zb = unit->m_zb;
  df.zh = unit->m_zh;
  df.zr = unit->m_zr;
  df.zy = unit->m_zy;
  
  df.ng.x = unit->m_x;
  df.ng.y = unit->m_y;
  df.ng.z = unit->m_z;
  df.ng.w = unit->m_w;

  Dfm1::filter(inputGain, frequency, resonance, type, noiseLevel, in, out, inNumSamples, SAMPLERATE, &df);
  
  unit->m_l = df.l;
  unit->m_h = df.h;
  unit->m_a = df.a;
  unit->m_b = df.b;
  unit->m_r = df.r;
  unit->m_s = df.s;
  
  unit->m_za = df.za;
  unit->m_zb = df.zb;
  unit->m_zh = df.zh;
  unit->m_zr = df.zr;
  unit->m_zy = df.zy;
  
  unit->m_x = df.ng.x;
  unit->m_y = df.ng.y;
  unit->m_z = df.ng.z;
  unit->m_w = df.ng.w;
}

PluginLoad(TJUgens)
{
  ft = inTable;
  DefineSimpleUnit(DFM1);
}
