/* coded by Clayton Otey
supercollider wrapped by Victor Bombi
*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "piano.h"
#include <string.h>


long Piano :: go(float *out, int samples) 
{
  long n = 0;
  for(int i=0;i<samples;i++) {
   // if(sample > this->samples)
   //   break;
    n++;
    float vstring = 0.0;
    for(int k=0;k<nstrings;k++) {
      vstring += string[k]->input_velocity();
    }
    float hload = hammer->load(t,vstring/nstrings);
    float load = 0;
    for(int k=0;k<nstrings;k++) {
      load += (2*Z*string[k]->go_hammer(hload/(2*Z)))/(Z*nstrings+Zb);
    }
    float output = 0.0;
    for(int k=0;k<nstrings;k++) {
      output += string[k]->go_soundboard(load);
    }
    
    output = soundboard->reverb(output);
    
    output += filter(output,&shaping1);
    output = filter(output,&shaping2);
    output += filter(output,&shaping3);
    t += dt;
    sample++;
    out[i] = output * 100.0;
  }
  return n;
}
float F_NOTE_31 = 6.875 * pow(2.0,(31.0 + 3)/12.0);
float F_NOTE_41 = 6.875 * pow(2.0,(41.0 + 3)/12.0);

Piano :: Piano(float f, float Fs, float v0,World * _world):world(_world) {
  this->Fs = Fs;
  this->v0 = v0;
  //this->samples = samples;
  sample = 0;
  t = 0.0;
  dt = 1.0/Fs;
	//printf("Fs = %g,v0 = %g \n",Fs,v0);
  //float f = freqTable[note];
  this->f = f;

  float f0 = 27.5;
  float rho = 7850.0;
  float p = 2.0+1.0*log(f/f0)/log(4192/f0);
  float m = .06 - .058*pow(log(f/f0)/log(4192/f0),.1);
  float K = 40/pow(.7e-3,p);
  float L = 1.4 - 1.32*log(f/f0) / log(4192.0/f0);
  L = .04 + 1.4/(1+exp(-3.4+1.4*log(f/f0)));
  float r = .002*pow(1+.6*log(f/f0),-1.4);
  float rhoL = PI*r*r*rho;
  float T = (2*L*f)*(2*L*f)*rhoL;
  Z = sqrt(T*rhoL);
  Zb = 4000.0;
  Zh = 0;
  float E = 200e9;
  float flong = sqrt(E/rho) / (2.0*L);


  float rcore = (r<.0006)?r:.0006;
  float B = (PI*PI*PI)*E*(rcore*rcore*rcore*rcore)/(4.0*L*L*T);
  float hp = 1.0/7.0;

  //printf("f = %g, r = %g mm, L = %g, T = %g, hammer = %g, Z = %g, K = %g, B = %g\n",f,1000*r,L,T,hp,Z,K,B);

  float c1 = 0.25;
  float c3 = 5.85; 
/*  
  if(note<31)
    nstrings = 1;
  else if(note<41)
    nstrings = 2;
  else 
    nstrings = 3;
 */
	if(f < F_NOTE_31)
		nstrings = 1;
	else if(f < F_NOTE_41)
		nstrings = 2;
	else 
		nstrings = 3;
  float c1b = 20.0;
  float c3b = 20.0;
  for(int k=0;k<nstrings;k++) {
    string[k] = new dwgs(f*TUNE[k],Fs,hp,c1,c3,B,Z,Zb+(nstrings-1)*Z,Zh);
  }

  float a = -1.0/4.0;
  float mix = 1;
  float alpha = 0.1e-4*log(f/f0)/log(4192/f0);
  soundboard = new Reverb(c1b,c3b,a,mix,Fs);
  hammer = new Hammer(f,Fs,m,K,p,Z,alpha,v0);
    
  biquad(500.0,Fs,10,notch,&shaping1);
  biquad(200.0,Fs,1.0,high,&shaping2);
  biquad(800.0,Fs,1.0,low,&shaping3);

}

Piano :: ~Piano() {
  for(int k=0;k<nstrings;k++) {
    delete string[k];
  } 
	destroy_filter(&shaping1);
	destroy_filter(&shaping2);
	destroy_filter(&shaping3);
  delete hammer;
  delete soundboard;
}

struct OteyPiano : public Unit
{
	Piano * piano;
	int relcount;
	float rellevel;
	float rellevelstep;
};

extern "C"
{
	void OteyPiano_next(OteyPiano *unit, int inNumSamples);
	void OteyPiano_Ctor(OteyPiano* unit);
	void OteyPiano_Dtor(OteyPiano* unit);
}

void OteyPiano_Ctor(OteyPiano* unit) {

	float freq= ZIN0(0);
	float amp= ZIN0(1);
	unit->relcount = SAMPLERATE * ZIN0(3);
	unit->rellevel = 1.0;
	unit->rellevelstep = 1.0/(float)unit->relcount;
	//Print("mworld %p\n",unit->mWorld);
	//overloaded new operator so realtime safe
	//unit->mWorld->mFullRate.mBufLength, no need to pass blocksize now
	//unit->piano= (Piano*) new(unit->mWorld, ft) Piano(freq,SAMPLERATE,10*amp,unit->mWorld);
	gWorld = unit->mWorld;
	unit->piano= (Piano*) new Piano(freq,SAMPLERATE,10*amp,unit->mWorld);
	//unit->piano= (Piano*) new(unit->mWorld) Piano(freq,SAMPLERATE,10*amp,unit->mWorld);
	SETCALC(OteyPiano_next);

}

void OteyPiano_Dtor(OteyPiano* unit) {

	//should work via overloaded delete
	delete unit->piano;

}

void OteyPiano_next(OteyPiano *unit, int inNumSamples) {

	float * out= OUT(0);
	float gate= ZIN0(2);
//printf("gate %g\n",gate);
	unit->piano->go(out,inNumSamples);
	if(gate == 0.0){
		int relcount = unit->relcount;
		float rellevel = unit->rellevel;
		float rellevelstep = unit->rellevelstep;
		
		for(int i=0; i<inNumSamples; i++){
			if(relcount > 0){
				rellevel -= rellevelstep;
				relcount--;
			}
			out[i] *=rellevel;
		}
		if(relcount <=0)
			DoneAction(2,unit);
			
		unit->relcount = relcount;
		unit->rellevel = rellevel;
	}
}

PluginLoad(OteyPiano){
	ft = inTable;
	DefineDtorUnit(OteyPiano);
}
