#include "dwgs.h"
#include "types.h"
#include <math.h>
#include <stdio.h>

dwgs :: dwgs(float f, float Fs, float inpos, float c1, float c3, float B, float Z, float Zb, float Zh) 
{
  float deltot = Fs/f;
  int del1 = (int)(inpos*0.5*deltot);
  if(del1 < 2)
    del1 = 1;

  if(f > 400) {
    M = 1;
    thiriandispersion(B,f,M,&(dispersion[0]));
  } else {
    M = 4;
    for(int m=0;m<M;m++)
      thiriandispersion(B,f,M,&(dispersion[m]));
  }
  float dispersiondelay = M*groupdelay(&(dispersion[0]),f,Fs);
  loss(f,Fs,c1,c3,&lowpass);
  float lowpassdelay = groupdelay(&lowpass,f,Fs);

  int del2 = (int)(0.5*(deltot-2.0*del1)-dispersiondelay);
  int del3 = (int)(0.5*(deltot-2.0*del1)-lowpassdelay-5.0);
  if(del2 < 2)
    del2 = 1;
  if(del3 < 2)
    del3 = 1;
  
  float D = (deltot-(float)(del1+del1+del2+del3+dispersiondelay+lowpassdelay));
  thirian(D,(int)D,&fracdelay);
  float tuningdelay = groupdelay(&fracdelay,f,Fs);

 // printf("total delay = %g/%g, leftdel = %d/%d, rightdel = %d/%d, dispersion delay = %g, lowpass delay = %g, fractional delay = %g/%g\n",del1+del1+del2+del3+dispersiondelay+lowpassdelay+tuningdelay,deltot, del1, del1, del2, del3, dispersiondelay, lowpassdelay, tuningdelay,D);

  d[0] = new dwg(Z,del1,del1,0,this);
  d[1] = new dwg(Z,del2,del3,1,this);
  d[2] = new dwg(Zb,0,0,0,this);
  d[3] = new dwg(Zh,0,0,0,this);

  d[0]->connectRight(d[1]->l);
  d[1]->connectLeft(d[0]->r);  
  d[1]->connectRight(d[2]->l);
  d[2]->connectLeft(d[1]->r);

  d[0]->connectRight(d[3]->l);
  d[1]->connectLeft(d[3]->l);
  d[3]->connectLeft(d[0]->r);
  d[3]->connectLeft(d[1]->l);

  d[0]->init();
  d[1]->init();
  d[2]->init();
  d[3]->init();
}

dwgs :: ~dwgs()
{
  for(int k=0;k<4;k++)
    delete d[k];
	destroy_filter(&lowpass);
	destroy_filter(&fracdelay);
	if(M==1){
		destroy_filter(&(dispersion[0]));
	}else{
		for(int k=0;k<4;k++)
			destroy_filter(&(dispersion[k]));
		
	}
}

dwg_node :: dwg_node(float z) {
  a[0] = 0;
  a[1] = 0;
  this->z = z;
  this->load = 0;
}

dwg :: dwg(float z, int del1, int del2, int commute, dwgs *parent) {
  this->parent = parent;

  if(del1>1)
    init_delay(&(d[0]), del1-1);

  if(del2>1)
    init_delay(&(d[1]), del2-1);

  this->del1 = del1;
  this->del2 = del2;
  nl = 0;
  nr = 0;
  l = new dwg_node(z);
  r = new dwg_node(z);
  this->commute = commute;
}

void dwg :: init()
{
  float ztot;
  
  ztot = l->z;
  for(int k=0;k<nl;k++) {
    ztot += cl[k]->z;
  }
  alphalthis = 2.0 * l->z / ztot;
  for(int k=0;k<nl;k++) {
    alphal[k] = 2.0 * cl[k]->z / ztot;
  }

  ztot = r->z;
  for(int k=0;k<nr;k++) {
    ztot += cr[k]->z;
  }
  alpharthis = 2.0 * r->z / ztot;
  for(int k=0;k<nr;k++) {
    alphar[k] = 2.0 * cr[k]->z / ztot;
  }
  
}

dwg :: ~dwg()
{
  delete l;
  delete r;
  if(del1>1)
    destroy_delay(&(d[0]));

  if(del2>1)
    destroy_delay(&(d[1]));
}

void dwg :: connectLeft(dwg_node *l, int polarity) {
  cl[nl] = l;
  pl[nl++] = polarity;
}

void dwg :: connectRight(dwg_node *r, int polarity) {
  cr[nr] = r;
  pr[nr++] = polarity;
}

void dwg :: connectLeft(dwg_node *l) {
  connectLeft(l,0);
}

void dwg :: connectRight(dwg_node *r) {
  connectRight(r,0);
}

void dwg :: dodelay() {
  float dar;
  if(del1==1)
    dar = r->a[0];
  else
    dar = delay(r->a[0],&(d[0]));
  
  float dal;
  if(del2==1)
    dal = l->a[1];
  else 
    dal = delay(l->a[1],&(d[1]));
  
  l->a[0] = dar;
  r->a[1] = dal;
}

void dwg :: doload() {
  if(nl == 0) 
    loadl = 0;
  else {
    loadl = alphalthis * l->a[0];
    for(int k=0;k<nl;k++) {
      int polarity = pl[k]?0:1;
      loadl += cl[k]->load;
      loadl += alphal[k] * cl[k]->a[polarity];
    }
  }
  
  if(nr == 0) 
    loadr = 0; 
  else {
    loadr = alpharthis * r->a[1];
    for(int k=0;k<nr;k++) {
      int polarity = pr[k]?1:0;
      loadr += cr[k]->load;
      loadr += alphar[k] * cr[k]->a[polarity];
    }
  }
}

void dwg :: update() {
  float a = (loadl - l->a[0]);
  if(commute) {
    for(int m=0;m<parent->M;m++) {
      a = filter(a,&(parent->dispersion[m]));
    }
  }
  l->a[1] = a;
  
  a = (loadr - r->a[1]);
  if(commute) {
    a = filter(a,&(parent->lowpass));
    a = filter(a,&(parent->fracdelay));
  }
  r->a[0] = a;
}


float dwgs :: input_velocity() {
  return d[1]->l->a[0] + d[0]->r->a[1];
}

float dwgs :: go_hammer(float load)
{
  d[3]->l->load = load;
  for(int k=0;k<2;k++) {
    d[k]->dodelay();		
  }
  
  return d[1]->r->a[1];
}

float dwgs :: go_soundboard(float load) {
  d[2]->l->load = load;
  for(int k=0;k<3;k++) {
    d[k]->doload();		
  }

  for(int k=0;k<3;k++) {
    d[k]->update();		
  }
  
  return d[2]->l->a[1];
}
