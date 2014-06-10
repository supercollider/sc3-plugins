/*
 *
 *    Copyright (C) 2013 Victor Bombi
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "dwglib/DWG.cpp"
#define MAXDELAY 1024
InterfaceTable *ft;

//DWGPlucked////////////////////////////////////////////////////
struct DWGPlucked : public Unit
{
	LagrangeT<MAXDELAY> DWGF[2];
	FilterC1C3 Loss;
	DWGPlucked(Unit* unit);
	void Release(float trig,float *out,int NumSamples);
	float m_trig;
	int relcount;
	float rellevel;
	float rellevelstep;
};
SCWrapClass(DWGPlucked);
DWGPlucked::DWGPlucked(Unit* unit)
{
	m_trig = 0.0;
	float release = ZIN0(7);
	relcount = SAMPLERATE * release;
	rellevel = 1.0;
	rellevelstep = 1.0/(float)relcount;
	SETCALC(DWGPlucked_next);
}
void DWGPlucked::Release(float trig,float *out,int NumSamples){
	
	if(this->m_trig <=0 && trig > 0){
		this->m_trig = trig;
	}
	if((this->m_trig >0 && trig <= 0)){
		int relcount = this->relcount;
		float rellevel = this->rellevel;
		float rellevelstep = this->rellevelstep;
		
		for(int i=0; i<NumSamples; i++){
			if(relcount > 0){
				rellevel -= rellevelstep;
				relcount--;
			}
			out[i] *=rellevel;
		}
		if(relcount <=0)
			DoneAction(2,this);
			
		this->relcount = relcount;
		this->rellevel = rellevel;
	}
}
void DWGPlucked_next(DWGPlucked *unit, int inNumSamples)
{

	float *out = OUT(0);
	float freq = ZIN0(0);
	float amp = ZIN0(1);
	float trig = ZIN0(2);
	float pos = ZIN0(3);

	float c1 = ZIN0(4);
	float c3 = std::max(ZIN0(5),(float)1e-9);
	float *in = IN(6);
	
	unit->Loss.setcoeffs(freq,c1,c3);
	float lossdelay = unit->Loss.groupdelay(freq,SAMPLERATE);
	float deltot = SAMPLERATE/freq;
	float del1 = (deltot - lossdelay )*0.5 - 1;

	float PMAS,PMAS2;
	float PMENOS;
	for (int i=0; i < inNumSamples; ++i)
	{
		unit->DWGF[0].add(in[i],pos*del1);
		unit->DWGF[1].add(in[i],del1*(1-pos));
		
		PMAS = unit->DWGF[0].delay(del1);
		PMAS2 = unit->Loss.filter(PMAS);
		PMENOS = unit->DWGF[1].delay(del1);
		
		unit->DWGF[1].push(-PMAS2);
		unit->DWGF[0].push(-PMENOS);
		
		out[i] = PMAS + PMAS2;
			
	}
	unit->Release(trig,out,inNumSamples);
}

/////////////////////////////////////////
//DWGPluckedStiff////////////////////////////////////////////////////

struct DWGPluckedStiff : public DWGPlucked
{
	ThirianDispersion<4> disper;
	DWGPluckedStiff(Unit* unit);
};
SCWrapClass(DWGPluckedStiff);

DWGPluckedStiff::DWGPluckedStiff(Unit* unit):DWGPlucked(unit)
{
    SETCALC(DWGPluckedStiff_next);
}

void DWGPluckedStiff_next(DWGPluckedStiff *unit, int inNumSamples)
{
	float *out = OUT(0);
	float freq = ZIN0(0);
	float amp = ZIN0(1);
	float trig = ZIN0(2);
	float pos = ZIN0(3);

	float c1 = ZIN0(4);
	float c3 = std::max(ZIN0(5),(float)1e-9);
	float *in = IN(6);
	float B = ZIN0(8)/100000;

	unit->disper.setcoeffs(freq,B);
	float disperdelay = unit->disper.groupdelay(SAMPLERATE);
	
	unit->Loss.setcoeffs(freq,c1,c3);
	float lossdelay = unit->Loss.groupdelay(freq,SAMPLERATE);
	float deltot = SAMPLERATE/freq;
	float del1 = (deltot - lossdelay - disperdelay)*0.5 - 1;

	float PMAS,PMAS2;
	float PMENOS;
	for (int i=0; i < inNumSamples; ++i)
	{
		unit->DWGF[0].add(in[i],pos*del1);
		unit->DWGF[1].add(in[i],del1*(1-pos));
		
		PMAS = unit->DWGF[0].delay(del1);
		PMAS2 = unit->Loss.filter(PMAS);
		PMAS2 = unit->disper.filter(PMAS2);
		PMENOS = unit->DWGF[1].delay(del1);
		
		unit->DWGF[1].push(-PMAS2);
		unit->DWGF[0].push(-PMENOS);
		
		out[i] = PMAS + PMAS2;
			
	}
	unit->Release(trig,out,inNumSamples);
}

/////////////////////////////////////////
//DWGPlucked2////////////////////////////////////////////////////

struct DWGPlucked2 : public DWGPlucked
{
	LagrangeT<MAXDELAY> DWGF2[2];
	FilterC1C3 Loss2;
	DWGPlucked2(Unit* unit);
};
SCWrapClass(DWGPlucked2);

DWGPlucked2::DWGPlucked2(Unit* unit):DWGPlucked(unit)
{
    SETCALC(DWGPlucked2_next);
}

void DWGPlucked2_next(DWGPlucked2 *unit, int inNumSamples)
{

	float *out = OUT(0);
	float freq = ZIN0(0);
	float amp = ZIN0(1);
	float trig = ZIN0(2);
	float pos = ZIN0(3);

	float c1 = ZIN0(4);
	float c3 = std::max(ZIN0(5),(float)1e-9);
	float *in = IN(6);
	float mistune = ZIN0(8);
	float mp = sc_clip(ZIN0(9),0,1);
	float gc = ZIN0(10);
	
	unit->Loss.setcoeffs(freq,c1,c3);
	float lossdelay = unit->Loss.groupdelay(freq,SAMPLERATE);
	float deltot = SAMPLERATE/freq;
	float del1 = (deltot - lossdelay )*0.5 - 1;

	float freq2 = freq * mistune;
	unit->Loss2.setcoeffs(freq2,c1,c3);
	lossdelay = unit->Loss2.groupdelay(freq2,SAMPLERATE);
	deltot = SAMPLERATE/freq2;
	float del2 = (deltot - lossdelay )*0.5 - 1;


	float PMAS,PMAS2;
	float PMENOS,OUT1,OUT2;
	for (int i=0; i < inNumSamples; ++i)
	{
		unit->DWGF[0].add(in[i]*mp,pos*del1);
		unit->DWGF[1].add(in[i]*mp,del1*(1-pos));
		
		PMAS = unit->DWGF[0].delay(del1);
		PMAS2 = unit->Loss.filter(PMAS);
		PMENOS = unit->DWGF[1].delay(del1);
		
		unit->DWGF[1].push(-PMAS2);
		unit->DWGF[0].push(-PMENOS);
		
		OUT1 = PMAS + PMAS2;
		
		unit->DWGF2[0].add(in[i]*(1-mp)+OUT1*gc,pos*del2);
		unit->DWGF2[1].add(in[i]*(1-mp)+OUT1*gc,del2*(1-pos));
		
		PMAS = unit->DWGF2[0].delay(del2);
		PMAS2 = unit->Loss2.filter(PMAS);
		PMENOS = unit->DWGF2[1].delay(del2);
		
		unit->DWGF2[1].push(-PMAS2);//*0.9999);
		unit->DWGF2[0].push(-PMENOS);
		
		OUT2 = PMAS + PMAS2;
		
		out[i] =  OUT1 + OUT2;
		
	}
	unit->Release(trig,out,inNumSamples);
}

/////////////////////////////////////////
PluginLoad(DWGPlucked)
{
	ft = inTable;
	DefineDtorUnit(DWGPlucked);
	DefineDtorUnit(DWGPluckedStiff);
	DefineDtorUnit(DWGPlucked2);
}
