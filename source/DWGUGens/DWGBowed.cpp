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

float repair_input(float val,float min,float max){
	if( std::fpclassify(val)== FP_INFINITE ||  std::fpclassify(val)== FP_NAN )
		return min;
	else
		return sc_clip(val,min,max);
	
}
//////////////////////////////////////////////////////////
struct DWGBowedSimple:public Unit
{
	LagrangeT<MAXDELAY> DWGF[2];
	FilterC1C3 Loss;
	DWGBowedSimple(Unit* unit);
	void Release(float trig,float *out,int NumSamples);
	float m_trig;
	int relcount;
	float rellevel;
	float rellevelstep;
};
SCWrapClass(DWGBowedSimple);
DWGBowedSimple::DWGBowedSimple(Unit* unit){
		m_trig = 0.0;
		float release = ZIN0(5);
		relcount = SAMPLERATE * release;
		rellevel = 1.0;
		rellevelstep = 1.0/(float)relcount;
		SETCALC(DWGBowedSimple_next);
}
void DWGBowedSimple::Release(float trig,float *out,int NumSamples){
	
	if(this->m_trig <=0.0f && trig > 0.0f){
		this->m_trig = trig;
	}
	if((this->m_trig >0.0f && trig <= 0.0f)){
		int relcount = this->relcount;
		float rellevel = this->rellevel;
		float rellevelstep = this->rellevelstep;
		
		for(int i=0; i<NumSamples; i++){
			//if(relcount > 0){
				rellevel -= rellevelstep;
				rellevel = std::max(0.0f, rellevel);
				relcount--;
			//}
			out[i] *=rellevel;
		}
		if(relcount <=0 && rellevel == 0.0f)
			DoneAction(2,this);
			
		this->relcount = relcount;
		this->rellevel = rellevel;
	}
}
////////////////////////////////////////////////////
struct DWGBowedStk : public DWGBowedSimple
{
	DWGBowedStk(Unit* unit);
};
SCWrapClass(DWGBowedStk);
DWGBowedStk::DWGBowedStk(Unit* unit):DWGBowedSimple(unit){ SETCALC(DWGBowedStk_next);}
////////////////////////////////////////////////////
struct DWGBowed : public DWGBowedSimple
{

	DCBlocker dcblock;
	ThirianDispersion<4> disper;
	DWGBowed(Unit* unit);

	int SolveHyperbolicm1(float vdeltap,float fb,float &sol);
	int SolveHyperbolicM1(float vdeltap,float fb,float &sol);
	float Bow2(float vb,float fb,float vsr_plus_vsl);
	const static float mus ;//= 0.8;
	const static float mud ;//= 0.3;
	const static float mue ;//= 0.2;
	int stickslip;
	float Z;
};
const float DWGBowed::mus = 0.8;
const float DWGBowed::mud = 0.3;
const float DWGBowed::mue = 0.2;

SCWrapClass(DWGBowed);
DWGBowed::DWGBowed(Unit* unit):DWGBowedSimple(unit),stickslip(0){ SETCALC(DWGBowed_next);}
////////////////////////////////////////////////////
struct DWGBowedTor:public DWGBowed{
	DCBlocker dcblock2;
	LagrangeT<MAXDELAY> DWGF2[2];
	FilterC1C3 Loss2;
	DWGBowedTor(Unit* unit);
	float Ztra,Ztor;
};
SCWrapClass(DWGBowedTor);
DWGBowedTor::DWGBowedTor(Unit* unit):DWGBowed(unit){ SETCALC(DWGBowedTor_next);}
/////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
struct DWGSoundBoard : public Unit
{
	FDNC1C3<8,1024> fdn;
	float c1b;
	float c3b;
    void getargs(Unit* unit,bool force=false);
	DWGSoundBoard(Unit *unit);
	~DWGSoundBoard(){};
};
SCWrapClass(DWGSoundBoard);
DWGSoundBoard :: DWGSoundBoard(Unit *unit){
    getargs(unit,true);
	SETCALC(DWGSoundBoard_next);
}
void DWGSoundBoard::getargs(Unit *unit,bool force)
{
    c1b = ZIN0(1);
	c3b = ZIN0(2);
	float mix = ZIN0(3);
	float len[8];
	for(int i=0;i<8;i++)
		len[i] = ZIN0(4+i);
    //optimize when values don change?
	fdn.setlengths(len);
	fdn.setcoeffs(c1b,c3b,mix,SAMPLERATE);
}
void DWGSoundBoard_next(DWGSoundBoard *unit, int inNumSamples){
	float *out = OUT(0);
	float *in = IN(0);
    unit->getargs(unit);
    unit->fdn.go(in,out,inNumSamples);
    /*
	for(int i=0; i < inNumSamples; i++){
		out[i] = unit->fdn.go(in[i]);
	}*/
}

/////////////////////////////////////////////////////
float BowTable(float vd,float f){
	float val;
	if (abs(vd) < f ){
		return 1.0;
	}else{
		
		if(vd > 0){
			val = 1.0 - (vd - f)/f;
		}else{
			val = 1.0 - (-vd - f)/f;
		}
		return sc_max(0,val);
	}
}

float Bow(float vb,float fb,float vsr_plus_vsl){
	float vdeltap = vb - (vsr_plus_vsl);
	float ref = BowTable(vdeltap,fb);
	return ref * vdeltap;
}

float BowStk(float vb,float fb,float vsr_plus_vsl){
	float vdeltap = vb - (vsr_plus_vsl);
	float slope = ( 5.0 - (4.0 * fb) );
	float sample = vdeltap + 0.001; //offset
	sample *= slope;
	sample = fabs( sample ) + 0.75;
	sample =  pow( sample, -4.0 );
	return sample;
}
/////////////////////////////////////////
float hyperbolicbow(float deltav,float fb){
	float mus = 0.8;
	float mud = 0.3;
	float mue = 0.2;
	if (deltav > 0){
		return -fb*(mud + (mus-mud)*mue/(mue + deltav));
	}else{
		return fb*(mud + (mus-mud)*mue/(mue - deltav));
	}
}

int DWGBowed::SolveHyperbolicm1(float vdeltap,float fb,float &sol){
	//Z=Z*0.5;
	float Zx2 = 2*Z;
	float c2 = -Zx2;
	float c1 = -Zx2*(vdeltap - mue) + fb*mud;
	float c0 = 0.2*(Zx2*vdeltap - fb*mus);
	float discrim = c1*c1 - 4*c2*c0;
	if(discrim < 0)
		return 0;
	else{
		float sqr = sqrt(discrim);
		float sol1 = (-c1 + sqr)/(2*c2);
		float sol2 = (-c1 - sqr)/(2*c2);
		sol = sc_min(sol1,sol2);
		if(sol < 0)
			return 1;
		else
			return 0;
	}
}

int DWGBowed::SolveHyperbolicM1(float vdeltap,float fb,float &sol){
	//Z=Z*0.5;
	float Zx2 = Z*2;
	float c2 = Zx2;
	float c1 = Zx2*(vdeltap + mue) + fb*mud;
	float c0 = 0.2*(Zx2*vdeltap + fb*mus);
	float discrim = c1*c1 - 4*c2*c0;
	if(discrim < 0)
		return 0;
	else{
		float sqr = sqrt(discrim);
		float sol1 = (-c1 + sqr)/(2*c2);
		float sol2 = (-c1 - sqr)/(2*c2);
		sol = sc_max(sol1,sol2);
		if(sol > 0)
			return 1;
		else
			return 0;
	}
}
#define show(aa) Print("%s Z %g vdeltap %g fb %g\n",aa,Z,vdeltap,fb)
//#define show(aa) ((void)0)
float DWGBowed::Bow2(float vb,float fb,float vsr_plus_vsl){
	float vdeltap = vb - (vsr_plus_vsl);
	float ret=0;
	if (this->stickslip == 0){
		if(mus*fb >= abs(2*this->Z * vdeltap)){
			return vdeltap;
		}else{ 
			if(vb > vsr_plus_vsl){
				if(SolveHyperbolicm1(vdeltap,fb,ret)){
					this->stickslip = -1;
					return ret + vdeltap;
				}else{
					if(SolveHyperbolicM1(vdeltap,fb,ret)){
						this->stickslip = 1;
						return ret + vdeltap;
					}else{
						show("stick failed ");
						return 0;
					}
				}
			}else{
				if(SolveHyperbolicM1(vdeltap,fb,ret)){
					this->stickslip = 1;
					return ret + vdeltap;
				}else{
					if(SolveHyperbolicm1(vdeltap,fb,ret)){
						this->stickslip = -1;
						return ret + vdeltap;
					}else{
						show("stick failed2 ");
						return 0;
					}
				}
			}
		}
	}
	
	if (this->stickslip == 1){
		if(SolveHyperbolicM1(vdeltap,fb,ret)){
			return ret + vdeltap;
		}else{
			if(mus*fb >= abs(2*this->Z * vdeltap)){
				this->stickslip = 0;
				return vdeltap;
			}else{
				if(SolveHyperbolicm1(vdeltap,fb,ret)){
					this->stickslip = -1;
					return ret + vdeltap;
				}else{
					show("slip 1 failed2 ");
					return 0;
				}
			}
		}
	}
	
	if (this->stickslip == -1){
		if(SolveHyperbolicm1(vdeltap,fb,ret)){
			return ret + vdeltap;
		}else{
			if(mus*fb >= abs(2*this->Z * vdeltap)){
				this->stickslip = 0;
				return vdeltap;
			}else{
				if(SolveHyperbolicM1(vdeltap,fb,ret)){
					this->stickslip = 1;
					return ret + vdeltap;
				}else{
					show("slip -1 failed2 ");
					return 0;
				}
			}
		}
	}
	show("imposible state");
	return 0;
}

void DWGBowed_next(DWGBowed *unit, int inNumSamples)
{

	float *out = OUT(0);
	float freq = ZIN0(0);
	float bowvelocity = ZIN0(1);
	float bowforce = ZIN0(2);
	float trig = ZIN0(3);
	float pos = ZIN0(4);

	float c1 = ZIN0(6);
	float c3 = std::max(ZIN0(7),(float)1e-9);
	unit->Z = ZIN0(8);
	float B = ZIN0(9)/100000;

	unit->disper.setcoeffs(freq,B);
	float disperdelay = unit->disper.groupdelay(SAMPLERATE);
	//Print("disperdellay %g\n",disperdelay);
	
	unit->Loss.setcoeffs(freq,c1,c3);
	float lossdelay = unit->Loss.groupdelay(freq,SAMPLERATE);
	float deltot = SAMPLERATE/freq;
	float del1 = (deltot - lossdelay - disperdelay)*0.5 - 1;


	float PMAS,PMAS2;
	float PMENOS;
	for (int i=0; i < inNumSamples; ++i)
	{
		float vel = unit->DWGF[0].get(pos*del1) + unit->DWGF[1].get(del1*(1-pos));
		//vel = Bow(bowvelocity,bowforce,vel);
		vel = unit->Bow2(bowvelocity,bowforce,vel);
		unit->DWGF[0].add(vel,pos*del1);
		unit->DWGF[1].add(vel,del1*(1-pos));
		
		PMAS = unit->DWGF[0].delay(del1);
		PMAS2 = unit->Loss.filter(PMAS);
		PMAS2 = unit->disper.filter(PMAS2);
		PMAS2 = unit->dcblock.filter(PMAS2);
		PMENOS = unit->DWGF[1].delay(del1);
		
		unit->DWGF[1].push(-PMAS2);
		unit->DWGF[0].push(-PMENOS);
		
		out[i] =  PMAS ;//+ PMAS2;
		
	}
	unit->Release(trig,out,inNumSamples);

}
void DWGBowedTor_next(DWGBowedTor *unit, int inNumSamples)
{

	float *out = OUT(0);
	float freq = ZIN0(0);
	float bowvelocity = ZIN0(1);
	float bowforce = ZIN0(2);
	float trig = ZIN0(3);
	float pos = ZIN0(4);

	float c1 = ZIN0(6);
	float c3 = std::max(ZIN0(7),(float)1e-9);

	int inslot = 8;
	unit->Ztra = ZIN0(inslot++);
	float B = ZIN0(inslot++)/100000;
	float mistune = ZIN0(inslot++);
	float c1tor = ZIN0(inslot++);
	float c3tor = ZIN0(inslot++);
	unit->Ztor = ZIN0(inslot++);

	unit ->Z = unit->Ztra*unit->Ztor/(unit->Ztra+unit->Ztor);
	float factra = unit->Z/unit->Ztra;
	float factor = unit->Z/unit->Ztor;
	
	unit->disper.setcoeffs(freq,B);
	float disperdelay = unit->disper.groupdelay(SAMPLERATE);
	//Print("disperdellay %g\n",disperdelay);
	
	unit->Loss.setcoeffs(freq,c1,c3);
	float lossdelay = unit->Loss.groupdelay(freq,SAMPLERATE);
	float deltot = SAMPLERATE/freq;
	float del1 = (deltot - lossdelay - disperdelay)*0.5 - 1;

	float freq2 = freq * mistune;
	unit->Loss2.setcoeffs(freq2,c1tor,c3tor);
	lossdelay = unit->Loss2.groupdelay(freq2,SAMPLERATE);
	deltot = SAMPLERATE/freq2;
	float del2 = (deltot - lossdelay )*0.5 - 1;
	del2 = sc_max(del2,1);
	//Print("del2 %g \n",del2); 
	float PMAS,PMAS2;
	float PMENOS,OUT1,OUT2;
	for (int i=0; i < inNumSamples; ++i)
	{
		float vel = unit->DWGF[0].get(pos*del1) + unit->DWGF[1].get(del1*(1-pos))
		+ unit->DWGF2[0].get(pos*del2) + unit->DWGF2[1].get(del2*(1-pos));
		
		vel = unit->Bow2(bowvelocity,bowforce,vel);
		
		unit->DWGF[0].add(vel*factra,pos*del1);
		unit->DWGF[1].add(vel*factra,del1*(1-pos));
		
		PMAS = unit->DWGF[0].delay(del1);
		PMAS2 = unit->Loss.filter(PMAS);
		PMAS2 = unit->disper.filter(PMAS2);
		PMAS2 = unit->dcblock.filter(PMAS2);
		PMENOS = unit->DWGF[1].delay(del1);
		
		unit->DWGF[1].push(-PMAS2);
		unit->DWGF[0].push(-PMENOS);
		
		OUT1 = PMAS ;//+ PMAS2;
		
		unit->DWGF2[0].add(vel*factor,pos*del2);
		unit->DWGF2[1].add(vel*factor,del2*(1-pos));
		
		PMAS = unit->DWGF2[0].delay(del2);
		PMAS2 = unit->Loss2.filter(PMAS);
		PMAS2 = unit->dcblock2.filter(PMAS2);
		PMENOS = unit->DWGF2[1].delay(del2);
		
		unit->DWGF2[1].push(-PMAS2);
		unit->DWGF2[0].push(-PMENOS);
		
		OUT2 = PMAS ;//+ PMAS2;
		
		out[i] =  OUT1 + OUT2;
		
	}
	unit->Release(trig,out,inNumSamples);
}
void DWGBowedSimple_next(DWGBowedSimple *unit, int inNumSamples)
{

	float *out = OUT(0);
	float freq = ZIN0(0);
	float bowvelocity = ZIN0(1);
	float bowforce = ZIN0(2);
	float trig = ZIN0(3);
	float pos = ZIN0(4);

	float c1 = ZIN0(6);
	float c3 = std::max(ZIN0(7),(float)1e-9);

	
	unit->Loss.setcoeffs(freq,c1,c3);
	float lossdelay = unit->Loss.groupdelay(freq,SAMPLERATE);
	float deltot = SAMPLERATE/freq;
	float del1 = (deltot - lossdelay)*0.5 - 1;


	float PMAS,PMAS2;
	float PMENOS;
	for (int i=0; i < inNumSamples; ++i)
	{
		float vel = unit->DWGF[0].get(pos*del1) + unit->DWGF[1].get(del1*(1-pos));
		vel = Bow(bowvelocity,bowforce,vel);
		//vel = unit->Bow2(bowvelocity,bowforce,vel);
		unit->DWGF[0].add(vel,pos*del1);
		unit->DWGF[1].add(vel,del1*(1-pos));
		
		PMAS = unit->DWGF[0].delay(del1);
		PMAS2 = unit->Loss.pushConvol(PMAS);
		PMENOS = unit->DWGF[1].delay(del1);
		
		unit->DWGF[1].push(-PMAS2);
		unit->DWGF[0].push(-PMENOS);
		
		out[i] = PMAS;// + PMAS2;

	}
	unit->Release(trig,out,inNumSamples);
}

void DWGBowedStk_next(DWGBowedStk *unit, int inNumSamples)
{

	float *out = OUT(0);
	float freq = ZIN0(0);
	float bowvelocity = ZIN0(1);
	float bowforce = ZIN0(2);
	float trig = ZIN0(3);
	float pos = ZIN0(4);

	float c1 = ZIN0(6);
	float c3 = std::max(ZIN0(7),(float)1e-9);

	
	unit->Loss.setcoeffs(freq,c1,c3);
	float lossdelay = unit->Loss.groupdelay(freq,SAMPLERATE);
	float deltot = SAMPLERATE/freq;
	float del1 = (deltot - lossdelay)*0.5 - 1;


	float PMAS,PMAS2;
	float PMENOS;
	for (int i=0; i < inNumSamples; ++i)
	{
		float vel = unit->DWGF[0].get(pos*del1) + unit->DWGF[1].get(del1*(1-pos));
		vel = BowStk(bowvelocity,bowforce,vel);
		//vel = unit->Bow2(bowvelocity,bowforce,vel);
		unit->DWGF[0].add(vel,pos*del1);
		unit->DWGF[1].add(vel,del1*(1-pos));
		
		PMAS = unit->DWGF[0].delay(del1);
		PMAS2 = unit->Loss.pushConvol(PMAS);
		PMENOS = unit->DWGF[1].delay(del1);
		
		unit->DWGF[1].push(-PMAS2);
		unit->DWGF[0].push(-PMENOS);
		
		out[i] = PMAS;// + PMAS2;

	}
	unit->Release(trig,out,inNumSamples);
}

/////////////////////////////////////////
PluginLoad(DWGBowed)
{
	ft = inTable;
	DefineDtorUnit(DWGBowed);
	DefineDtorUnit(DWGBowedTor);
	DefineDtorUnit(DWGBowedSimple);
	DefineDtorUnit(DWGBowedStk);
	DefineDtorUnit(DWGSoundBoard);
}
