#ifndef FILTER_H
#define FILTER_H
#define PI 3.14159265358979
#define SINC_COEFFS 128

struct Filter {
  float *x, *y;
  float *a, *b;
  int n;
};

struct Delay {
  int di; 
  int d1;
  int size;
  int mask;
  int cursor;
  float *x;
  //float *y;
};
/*
enum biqaudtype {
  pass = 0,
  low,
  high,
  notch
};
*/
float Db(float B, float f, int M);
long choose(long n, long k);
float probe_delay(Delay *c, int pos);
float groupdelay(Filter *c, float f, float Fs);
float phasedelay(Filter *c, float f, float Fs);
void thirian(float D, int N, Filter *c);
void thiriandispersion(float B, float f, int M, Filter *c);
void resonator(float f, float Fs, float tau, Filter *c);
void differentiator(Filter *c);
void loss(float f0, float fs, float c1, float c3, Filter *c);
void biquad(float f0, float fs, float Q, int type, Filter *c);
void destroy_filter(Filter *c);
float filter(float in, Filter *c);
void init_delay(Delay *c, int di);
void destroy_delay(Delay *c);
float delay(float in, Delay *c);


#endif
