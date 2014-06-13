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
#include "DWG.hpp"
//#include "simd_binary_arithmetic.hpp"
//#include "simd_horizontal_functions.hpp"

void volcar(const char *_Message, const char *_File, unsigned _Line)
{
	Print("assertion %s ,%s:%d\n",_Message,_File,_Line);
}

void kill_denormals(float &val)
{
	static const float anti_denormal = 1e-18;
	val += anti_denormal;
	val -= anti_denormal;
}

float ValimakiDispersion(float B, float f, int M) {
  float C1,C2,k1,k2,k3;
  if(M==4) {
    C1 = .069618;
    C2 = 2.0427;
    k1 = -.00050469;
    k2 = -.0064264;
    k3 = -2.8743;
  } else {
    C1 = .071089;
    C2 = 2.1074;
    k1 = -.0026580;
    k2 = -.014811;
    k3 = -2.9018;
  }

  float logB = log(B);
  float kd = exp(k1*logB*logB + k2*logB + k3);
  float Cd = exp(C1*logB+C2);
  float halfstep = pow(2.0,1.0/12.0);
  float Ikey = log(f*halfstep/27.5) / log(halfstep);
  float D = exp(Cd - Ikey*kd);
  return D;
}
////////groupdelay

void evalpolyB(float omega,float* h,int N,float H[2]){
	H[0]=0;H[1]=0;
	for(int i=0;i<N;i++){
		H[0] += cos(float(i)*omega)*h[i];
		H[1] -= sin(float(i)*omega)*h[i];
	}
}
void evalpolyA(float omega,float* h,int N,float H[2]){
	H[0]=1;H[1]=0;
	for(int i=0;i<N;i++){
		H[0] += cos(float(i + 1)*omega)*h[i];
		H[1] -= sin(float(i + 1)*omega)*h[i];
	}
}
void evalrampedB(float omega,float* h,int N,float H[2]){
	H[0]=0;H[1]=0;
	for(int i=1;i<N;i++){
		H[0] += float(i)*cos(float(i)*omega)*h[i];
		H[1] -= float(i)*sin(float(i)*omega)*h[i];
	}
}
void evalrampedA(float omega,float* h,int N,float H[2]){
	H[0]=0;H[1]=0;
	for(int i=0;i<N;i++){
		float j = float(i) + 1.0;
		H[0] += j*cos(j*omega)*h[i];
		H[1] -= j*sin(j*omega)*h[i];
	}
}
void complex_divide(float A[2],float B[2],float C[2]){
	float magd2 = B[0]*B[0]+B[1]*B[1];
	C[0] = (A[0] * B [0] + A[1] * B[1])/magd2;
	C[1] = (A[1] * B [0] - A[0] * B[1])/magd2;
}
void complex_multiply(float A[2],float B[2],float C[2]){
	C[0] = A[0]*B[0]-A[1]*B[1];
	C[1] = A[0]*B[1]+A[1]*B[0];
}
float groupdelay(float f,float *B,int sizeB,float *A,int sizeA,float FS){
	float omega = 2.0*M_PI*f/FS;
	float Br[2] ; evalrampedB(omega,B,sizeB,Br);
	float Be[2] ; evalpolyB(omega,B,sizeB,Be);
	float Ar[2] ; evalrampedA(omega,A,sizeA,Ar);
	float Ae[2] ; evalpolyA(omega,A,sizeA,Ae);
	float BrxAe[2] ; complex_multiply(Br,Ae,BrxAe);
	float BexAr[2] ; complex_multiply(Be,Ar,BexAr);
	float AxB[2] ; complex_multiply(Ae,Be,AxB);
	float Num[2]; Num[0] = BrxAe[0]-BexAr[0]; Num[1] = BrxAe[1]-BexAr[1];
	float c[2] ; complex_divide(Num,AxB,c);
	return c[0];
}
////
void evalpoly(float omega,float* B,int sizeB,float* A,int sizeA,float H[2]){
	float HB[2];
	float HA[2];
	evalpolyB(omega,B,sizeB,HB);
	evalpolyA(omega,A,sizeA,HA);
	complex_divide(HB,HA,H);
}
float PhaseIIR(float omega,float* B,int sizeB,float* A,int sizeA){
	float C[2];
	evalpoly(omega,B,sizeB,A,sizeA,C);
	return atan2(C[1],C[0]);
}
float PhaseDelayDerive(float omega,float* B,int sizeB,float* A,int sizeA,float delta){
	float omega1 = omega - delta;
	float omega2 = omega + delta;
	float p1 = PhaseIIR(omega1,B,sizeB,A,sizeA);
	float p2 = PhaseIIR(omega2,B,sizeB,A,sizeA);
	return (-omega1*p2 + omega2*p1)/(2*delta*omega1*omega2);
}
float PhaseDelay(float f,float* B,int sizeB,float* A,int sizeA,float FS){
	float grpdel = ::groupdelay(f,B,sizeB,A,sizeA,FS);
	float omega = 2.0*M_PI*f/FS;
	return grpdel - omega*PhaseDelayDerive(omega,B,sizeB,A,sizeA);
}
///
long Nchoose(long n, long k) {
  long divisor = 1;
  long multiplier = n;
  long answer = 1;
  k = std::min(k,n-k);
  while(divisor <= k)
    {
      answer = (answer * multiplier) / divisor;
      multiplier--;
      divisor++;
    }
  return answer;
}
/*
///////////////////////////////////////////
//returns Sum(i=0,n-1) A[i]*B[i]
//Buf, A and B must have same memory alignement
float convolve_reversed(float *Buf,float *A,float *B,int n,int aligj)
{
	const unsigned int per_loop = nova::vec<float>::objects_per_cacheline;
	//int precount = ((intptr_t)A % (per_loop * sizeof(float)))/sizeof(float);

	int vsize = nova::vec<float>::size;
    int alig = ((intptr_t)A /sizeof(float))% vsize;
    int precount = (vsize - alig) % vsize;
    precount = sc_min(precount,n);
    int nmultiple = ((n - precount)/ per_loop)*per_loop;
	int nrest = (n - precount) - nmultiple;
	float sum = 0;

	//from beginig until first memory aligned
	for( int i = 0; i < precount; i++){
		sum += A[i]*B[i];
	}
	//nmultiple/objects_per_cacheline cache lines from first aligned
    if(nmultiple > 0){
        nova::times_vec_simd(Buf,A + precount,B + precount,nmultiple);
        sum += nova::horizontal_sum_vec_simd(Buf,nmultiple);
    }
	//rest
	if( nrest > 0){
		nova::times_vec(Buf,A + nmultiple + precount,B + nmultiple + precount, nrest);
		sum += nova::horizontal_sum_vec(Buf, nrest);
	}
	return sum;
}
*/
//////////////////////////////////////////
//include local buffer test in one place
static SndBuf * ConvGetBuffer(Unit * unit, uint32 bufnum, const char * ugenName, int inNumSamples)
{
	SndBuf *buf;
	World *world = unit->mWorld;

	if (bufnum >= world->mNumSndBufs) {
		int localBufNum = bufnum - world->mNumSndBufs;
		Graph *parent = unit->mParent;
		if (localBufNum <= parent->localMaxBufNum) {
			buf = parent->mLocalSndBufs + localBufNum;
		} else {
			if (unit->mWorld->mVerbosity > -1)
				Print("%s: invalid buffer number (%d).\n", ugenName, bufnum);
			goto handle_failure;
		}
	} else {
		buf = world->mSndBufs + bufnum;
	}

	if (buf->data == NULL) {
		if (unit->mWorld->mVerbosity > -1)
			Print("%s: uninitialized buffer (%i).\n", ugenName, bufnum);
		goto handle_failure;
	}

	return buf;

handle_failure:
	SETCALC(*ClearUnitOutputs);
	ClearUnitOutputs(unit, inNumSamples);
	unit->mDone = true;
	return NULL;
}

///////////////////////////
//Circular Buffer

CircularBufferBase::CircularBufferBase(Unit * unit,int size){
		this->unit = unit;
		pointer = 0;
		this->size = size;
		//Print("CircularBufferBase construct size %d\n",size);
		Buffer = (float *)RTAlloc(unit->mWorld,sizeof(float)*size);
		//Print("Buffer %p\n",Buffer);
		//allocate(size);
		memset(Buffer, 0, size * sizeof(float));
	}
CircularBufferBase::~CircularBufferBase(){
		RTFree(unit->mWorld,Buffer);
	}
	void CircularBufferBase::push(float a){
		pointer--;
		if(pointer < 0)
			pointer = size - 1;
		Buffer[pointer] = a;
	}
	void CircularBufferBase::push(float a,int pos){
		Buffer[pointerInRange(pointer + pos)] = a;
	}
	void CircularBufferBase::add(float a,int pos){
		int posr = pointerInRange(pointer + pos);
		Buffer[posr] += a;
	}
	void CircularBufferBase::set(float a,int pos){
		int posr = pointerInRange(pointer + pos);
		Buffer[posr] = a;
	}

	float CircularBufferBase::get()
	{
		return Buffer[pointer];
	}
	float CircularBufferBase::get(int pos)
	{
		return Buffer[pointerInRange(pointer + pos)];
	}
	
	int CircularBuffer::pointerInRange(int p){
		p = p%size;
		if( p < 0 ){ p += size;}
		return p;
	}
	void CircularBuffer::allocate(int size){
		Unit * unit = this->unit;
		Buffer = (float *)RTAlloc(unit->mWorld,sizeof(float)*size);
	}
	float CircularBuffer::delay(int pos)
	{
		assertv(size >= pos);
		int pAl2 = (this->pointer + pos);
		pAl2 = pAl2%size;
		if( pAl2 < 0 ){ pAl2 += size;}
		return this->Buffer[pAl2];
	}
////////////////////////////////////////


void FDN8::setlengths(float len[8]){
	for(int i=0;i<8;i++)
		lengths[i] = sc_min(len[i],1024);
}
//Householder Feedback Matrix
void FDN8::setcoeffs(float c1, float c3, float mix, float Fs){
	float a = -1.0/4.0;
	float aa[8] = {a,1+a,a,a,a,a,a,a};
	this->mix = mix;

  for(int k=0;k<8;k++) {
    o[k] = 0;
    b[k] = 1;
    c[k] = k<8?((k%2==0)?1.0/8.0:-1.0/8.0):0.0;
    decay[k].setcoeffs(Fs/lengths[k],c1,c3);
  }
  for(int j=0;j<8;j++)
    for(int k=0;k<8;k++)
      A[j][k] = aa[(8+(k-j))%8];
}

float FDN8 :: go(float in) 
{
  float i[8];

  for(int j=0;j<8;j++) {
    //i[j] = b[j] * in;
	i[j] = in;
    for(int k=0;k<8;k++) {
      i[j] += A[j][k] * o[k];
    }
  }

  float out = 0;  
  for(int j=0;j<8;j++) {
	delay[j].push(i[j]);
    o[j] = decay[j].filter(delay[j].delay(lengths[j]));
	o[j] = zapgremlins(o[j]);
    out += c[j] * o[j]*.5;
  }
  
  return mix*out + (1.0-mix)*in;
}
