#include "filter.h"
#include "reverb.h"
#include <math.h>

Reverb :: Reverb(float c1, float c3, float a, float mix, float Fs) 
{
  this->mix = mix;
  int lengths[8] = {37,87,181,271,359,592,687,721};
  float aa[8] = {a,1+a,a,a,a,a,a,a};

  for(int k=0;k<8;k++) {
    init_delay(&(d[k]),lengths[k]);
    o[k] = 0;
    b[k] = 1;
    c[k] = k<8?((k%2==0)?1.0/8.0:-1.0/8.0):0.0;
   
    loss(Fs/lengths[k],Fs,c1,c3,&(decay[k]));
  }
  for(int j=0;j<8;j++)
    for(int k=0;k<8;k++)
      A[j][k] = aa[(8+(k-j))%8];
}
Reverb::~Reverb(){
	for(int k=0;k<8;k++){
		destroy_filter(&(decay[k]));
		destroy_delay(&(d[k]));
	}
}
float Reverb :: reverb(float in) 
{
  float i[8];

  for(int j=0;j<8;j++) {
    i[j] = b[j] * in;
    for(int k=0;k<8;k++) {
      i[j] += A[j][k] * o[k];
    }
  }

  float out = 0;  
  for(int j=0;j<8;j++) {
    o[j] = filter(delay(i[j],&(d[j])),&(decay[j]));
    out += c[j] * o[j]*.5;
  }
  
  return mix*out + (1.0-mix)*in;
}

