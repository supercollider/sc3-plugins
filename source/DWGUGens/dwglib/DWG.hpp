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
#ifndef __dwg_sc3__
#define __dwg_sc3__

#include "SC_PlugIn.h"
extern  InterfaceTable *ft;
#include <float.h>
#include <math.h>
#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif
void volcar(const char *_Message, const char *_File, unsigned _Line);

#define NOASSERTS

#ifndef NOASSERTS
#define assertv(_Expression) \
 (void) \
 ((!!(_Expression)) || \
  (volcar(#_Expression,__FILE__,__LINE__),0))
#define assertv2(_Expression,msg) \
 (void) \
 ((!!(_Expression)) || \
  (volcar(#_Expression#msg,__FILE__,__LINE__),0))
#else
#define assertv(aa) ((void)0)
#define assertv2(aa,bb) ((void)0)
#endif

#define XSTR(x) STR(x)
#define STR(x) #x

#define NumElements(_BUF) (sizeof(_BUF)/sizeof(_BUF[0]))
#define SNumElements(_BUF) (sizeof(_BUF)/sizeof(_BUF[0]))

#define b2(x)   (   (x) | (   (x) >> 1) )
#define b4(x)   ( b2(x) | ( b2(x) >> 2) )
#define b8(x)   ( b4(x) | ( b4(x) >> 4) )
#define b16(x)  ( b8(x) | ( b8(x) >> 8) )
#define b32(x)  (b16(x) | (b16(x) >>16) )
#define next_power_of_2(x)      (b32(x-1) + 1)

#define ispowerof2(x) ((x != 0) && ((x & (x - 1)) == 0))

#define SCWrapClass(classname) extern "C" {\
	void classname##_Ctor(classname* unit);\
	void classname##_next(classname *unit, int inNumSamples);\
	void classname##_Dtor(classname *unit);\
}\
void classname##_Ctor(classname* unit){new(unit) classname(unit);}\
void classname##_Dtor(classname* unit){unit->~classname();}


inline bool approximatelyEqual(float a, float b, float epsilon = 1e-7f)
{
    float absa = fabs(a);
	float absb = fabs(b);
	return fabs(a - b) <= ( (absa < absb ? absb : absa) * epsilon);
}
float PhaseDelay(float f,float* B,int sizeB,float* A,int sizeA,float FS);
float PhaseDelayDerive(float omega,float* B,int sizeB,float* A,int sizeA,float delta=0.0005);
float groupdelay(float f,float *B,int sizeB,float *A,int sizeA,float FS);
long Nchoose(long n, long k);
float ValimakiDispersion(float B, float f, int M);
void kill_denormals(float &val);
void kill_denormals(double &val);
///////////////////////////
//Circular BufferT
template<int size>
class CircularBufferTBase
{
	public:
	float Buffer[size];
	int buffer_size;
	int pointer;
	CircularBufferTBase(){
		pointer = 0;
		buffer_size = size;
		memset(Buffer, 0, size * sizeof(float));
		//Print("CircularBufferTBase\n");
	}
	void push(float a){
		pointer--;
		if(pointer < 0)
			pointer = size - 1;
		Buffer[pointer] = a;
	}
	void push(float a,int pos){
		Buffer[pointerInRange(pointer + pos)] = a;
	}
	void add(float a,int pos){
		int posr = pointerInRange(pointer + pos);
		Buffer[posr] += a;
	}
	void set(float a,int pos){
		int posr = pointerInRange(pointer + pos);
		Buffer[posr] = a;
	}
	virtual int pointerInRange(int p)=0;
	float get()
	{
		return Buffer[pointer];
	}
	float get(int pos)
	{
		return Buffer[pointerInRange(pointer + pos)];
	}

	virtual float delay(int pos)=0;
};
template<int size>
class CircularBufferT : public CircularBufferTBase<size>
{
	public:
	int pointerInRange(int p){
		p = p%size;
		if( p < 0 ){ p += size;}
		return p;
	}

	float delay(int pos)
	{
		assertv(size >= pos);
		int pAl2 = (this->pointer + pos);
		pAl2 = pAl2%size;
		if( pAl2 < 0 ){ pAl2 += size;}
		return this->Buffer[pAl2];
	}

};
//((x & (x - 1)) == 0)
///////////////////////////
//Circular Buffer with size power of two
template<int size>
class CircularBuffer2POWSizedT : public CircularBufferTBase<size>
{
	public:
	int mask;
	//static_assert(ispowerof2(size),"Size mus be power of two!!!");
	CircularBuffer2POWSizedT(){mask = size - 1;//Print("CircularBuffer2POWSizedT\n");
		//assertv(ispowerof2(size));

	}
	int pointerInRange(int p){
		return p & mask;
	}
	float delay(int pos)
	{
		assertv(size >= pos);
		return this->Buffer[(this->pointer + pos) & mask];
	}
};

//3rd order lagrange fractional delay
template<int size>
class LagrangeT : public CircularBuffer2POWSizedT<size>
{

	public:
	float lastdelay;
	float h[4];
	int ptL;
	float actdelay;
	LagrangeT()
	{
		//this->buffer_size = size;
		//this->mask = size - 1;
		//this->pointer = 0;
		//memset(this->Buffer, 0, size * sizeof(float));
		actdelay = 0;
		lastdelay = 0;
		ptL = CalcCoeffs(0);
		//Print("LAGRANGE construc\n");
	}
	int CalcCoeffs(float delay)
	{
		int intd =(int) delay;
		float Dm1 = delay - (float)intd;
		intd -= 1.;
		float D = Dm1 + 1;
		float Dm2 = Dm1 - 1;
		float Dm3 = Dm1 - 2;
		float DxDm1 = D * Dm1;
		//float Dm1xDm2 = Dm1 * Dm2;
		float Dm2xDm3 = Dm2 *Dm3;
		h[0] = (-1/6.)* Dm1 * Dm2xDm3;
		h[1] = 0.5 * D * Dm2xDm3;
		h[2] = -0.5 * DxDm1 * Dm3;
		h[3] = (1/6.) * DxDm1 * Dm2;
		//Print("LagrangeT CalcCoefs %d coefs %.34g %.39g %.39g %.39g delay %g\n",intd - 1,h[0],h[1],h[2],h[3],delay);
		return intd ;
	}
	float delay(float pos)
	{
		assertv(size >= pos);
		if (pos != lastdelay){
			ptL = CalcCoeffs(pos);
			lastdelay = pos;
		}
		//return this->Buffer[(this->pointer - (int)pos) & this->mask];
		float sum = 0;
		for(int i=0; i < 4; i++){
			sum += this->Buffer[(this->pointer + ptL + i) & this->mask]*h[i];
		}
		//DUMPONNAN(sum);
		return sum;
	}
	void setdelay(float del){actdelay = del;}
	float delay(){return delay(actdelay);}
};
//3rd order lagrange fractional multitap delay
template<int size,int taps>
class LagrangeMtapT : public CircularBuffer2POWSizedT<size>
{

	public:
	float lastdelay[taps];
	float h[taps][4];
	int ptL[taps];
	//float actdelay;
	LagrangeMtapT()
	{
		//actdelay = 0;
        for(int i=0;i<taps;i++){
            lastdelay[i] = 0.0;
            ptL[i] = CalcCoeffs(0.0,i);
        }
	}
	int CalcCoeffs(float delay,int tap)
	{
		int intd =(int) delay;
		float Dm1 = delay - (float)intd;
		intd -= 1.;
		float D = Dm1 + 1;
		float Dm2 = Dm1 - 1;
		float Dm3 = Dm1 - 2;
		float DxDm1 = D * Dm1;
		//float Dm1xDm2 = Dm1 * Dm2;
		float Dm2xDm3 = Dm2 *Dm3;
		h[tap][0] = (-1/6.)* Dm1 * Dm2xDm3;
		h[tap][1] = 0.5 * D * Dm2xDm3;
		h[tap][2] = -0.5 * DxDm1 * Dm3;
		h[tap][3] = (1/6.) * DxDm1 * Dm2;
		//Print("LagrangeT CalcCoefs %d coefs %.34g %.39g %.39g %.39g delay %g\n",intd - 1,h[0],h[1],h[2],h[3],delay);
        //printf("LagrangeMtap calc %f, %d\n",delay,tap);
		return intd ;
	}
	float delay(float pos,int tap)
	{
		assertv(size >= pos);
		if (pos != lastdelay[tap]){
			ptL[tap] = CalcCoeffs(pos,tap);
			lastdelay[tap] = pos;
		}
		//return this->Buffer[(this->pointer - (int)pos) & this->mask];
		float sum = 0;
		for(int i=0; i < 4; i++){
			sum += this->Buffer[(this->pointer + ptL[tap] + i) & this->mask]*h[tap][i];
		}
		//DUMPONNAN(sum);
		return sum;
	}
	//void setdelay(float del){actdelay = del;}
	//float delay(){return delay(actdelay);}
};

//1st order lagrange fractional delay
template<int size>
class Lagrange1T : public CircularBuffer2POWSizedT<size>
{

	public:
	float lastdelay;
	float h[2];
	int ptL;
	float actdelay;
	Lagrange1T()
	{
		actdelay = 0;
		lastdelay = 0;
		ptL = CalcCoeffs(0);
		//Print("LAGRANGE construc\n");
	}
	int CalcCoeffs(float delay)
	{
		int intd =(int) delay;
		float Dm1 = delay - (float)intd;
		h[0] = (1.0 - Dm1);
		h[1] = Dm1;
		return intd ;
	}
	float delay(float pos)
	{
		assertv(size >= pos);
		if (pos != lastdelay){
			ptL = CalcCoeffs(pos);
			lastdelay = pos;
		}
		//return this->Buffer[(this->pointer - (int)pos) & this->mask];
		float sum = 0;
		for(int i=0; i < 2; i++){
			sum += this->Buffer[(this->pointer + ptL + i) & this->mask]*h[i];
		}
		//DUMPONNAN(sum);
		return sum;
	}
	void setdelay(float del){actdelay = del;}
	float delay(){return delay(actdelay);}
};

template<float * Kernel,int kernel_size>
class ConvolverT
{
	public:
	template<typename T>
	float Convol(T &buf)
	{
		int AlSize = buf.buffer_size;
		assertv(AlSize >= kernel_size);
		float sum=0.;

		//int pAl2 = (buf.pointer - kernel_size + 1);
		//pAl2 = pAl2%AlSize;
		//if( pAl2 < 0 ){ pAl2 += AlSize;}
		int pAl2 = buf.pointerInRange(buf.pointer + kernel_size - 1);
		int howmany = sc_min(AlSize - pAl2,kernel_size);
		for(int i=0; i < howmany; i++)
			sum += Kernel[i]*buf.Buffer[pAl2 + i];
		int howmany2 = kernel_size - howmany;
		for(int i=0; i < howmany2; i++)
			sum += Kernel[howmany + i]*buf.Buffer[i];
		return sum;
	}

};

/////////////
template<float* KernelB,int kernel_sizeB,float* KernelA,int kernel_sizeA>
class LTIT
{

	CircularBufferT<kernel_sizeB> cbuf;
	CircularBufferT<kernel_sizeA>  cbufout;
	public:
	LTIT(){//Print("LTIT\n");
	};
	void push(float a){
		cbuf.push(a);
	}
    float filter(float a){
		push(a);
		return Convol();
	}
	float pushConvol(float a){
		push(a);
		return Convol();
	}
	float Convol()
	{
		float sum=0.;
		//get oldest sample from buffer
		int pAl2 = cbuf.pointer ;//(cbuf.pointer + 1);
		//pAl2 = pAl2%kernel_sizeB;
		//if(pAl2 >= kernel_sizeB) {pAl2 = 0;}

		int howmany = kernel_sizeB - pAl2;
		for(int i=0; i < howmany; i++)
			sum += KernelB[i]*cbuf.Buffer[pAl2 + i];
		for(int i=0; i < pAl2; i++)
			sum += KernelB[howmany + i]*cbuf.Buffer[i];

		pAl2 = cbufout.pointer ;
		//pAl2 = pAl2%kernel_sizeA;
		//if(pAl2 >= kernel_sizeA) {pAl2 = 0;}
		howmany = kernel_sizeA - pAl2;
		for(int i=0; i < howmany; i++)
			sum -= KernelA[i]*cbufout.Buffer[pAl2 + i];
		for(int i=0; i < pAl2; i++)
			sum -= KernelA[howmany + i]*cbufout.Buffer[i];

		cbufout.push(sum);
		return sum;
	}
	float groupdelay(float f,float FS){
		return ::groupdelay(f,KernelB,kernel_sizeB,KernelA,kernel_sizeA,FS);
	}
};

/////////////
template<int kernel_sizeB,int kernel_sizeA>
class LTITv
{
	public:
	float KernelB[kernel_sizeB];
	float KernelA[kernel_sizeA];
	CircularBufferT<kernel_sizeB> cbuf;
	CircularBufferT<kernel_sizeA>  cbufout;
	bool dirty_grdel;
	float grdel;

	LTITv():dirty_grdel(true){//Print("LTITv\n");
	};
	void push(float a){
		cbuf.push(a);
	}
	void setKernel(float KB[],float KA[]){
		for(int i=0;i<kernel_sizeB;i++)
			KernelB[i] = KB[i];
		for(int i=0;i<kernel_sizeA;i++)
			KernelA[i] = KA[i];
		dirty_grdel = true;
	}
	float pushConvol(float a){
		push(a);
		return Convol();
	}
	float filter(float a){
		push(a);
		return Convol();
	}
	float Convol()
	{
		float sum=0.;
		//get oldest sample from buffer
		int pAl2 = cbuf.pointer ;
		//pAl2 = pAl2%kernel_sizeB;
		//if(pAl2 >= kernel_sizeB) {pAl2 = 0;}

		int howmany = kernel_sizeB - pAl2;
		for(int i=0; i < howmany; i++)
			sum += KernelB[i]*cbuf.Buffer[pAl2 + i];
		for(int i=0; i < pAl2; i++)
			sum += KernelB[howmany + i]*cbuf.Buffer[i];

		pAl2 = cbufout.pointer ;
		//pAl2 = pAl2%kernel_sizeA;
		//if(pAl2 >= kernel_sizeA) {pAl2 = 0;}
		howmany = kernel_sizeA - pAl2;
		for(int i=0; i < howmany; i++)
			sum -= KernelA[i]*cbufout.Buffer[pAl2 + i];
		for(int i=0; i < pAl2; i++)
			sum -= KernelA[howmany + i]*cbufout.Buffer[i];

		//sum = zapgremlins(sum);
		cbufout.push(sum);
		return sum;
	}
	float groupdelay(float f,float FS){
		if(dirty_grdel){
			grdel = ::groupdelay(f,KernelB,kernel_sizeB,KernelA,kernel_sizeA,FS);
			dirty_grdel = false;
		}
		return grdel;
	}
	float phasedelay(float f,float FS){
		//return ::PhaseDelay(f,KernelB,kernel_sizeB,KernelA,kernel_sizeA,FS);
		//if(dirty_phdel){
			float grpdel = groupdelay(f,FS);
			float omega = 2.0*M_PI*f/FS;
			float phdel = grpdel - omega*::PhaseDelayDerive(omega,KernelB,kernel_sizeB,KernelA,kernel_sizeA);
			//dirty_phdel = false;
		//}
		return phdel;
	}
};
//////////////specialization
template<>
class LTITv<1,1>
{
	public:
	float KernelB;
	float KernelA;
	float cbuf;
	float  cbufout;
	bool dirty_grdel;
	float grdel;
	LTITv():cbuf(0),cbufout(0),KernelB(0),KernelA(0),dirty_grdel(true){//Print("LTITv especialized\n");
	};
	void push(float a){
		cbuf = a;
	}
	void setKernel(float KB[],float KA[]){
			KernelB = KB[0];
			KernelA = KA[0];
			dirty_grdel = true;
	}
	float pushConvol(float a){
		push(a);
		return Convol();
	}
	float filter(float a){
		push(a);
		return Convol();
	}
	float Convol()
	{
		//float sum=0.;
		float sum = KernelB*cbuf;
		sum -= KernelA*cbufout;
		//sum = zapgremlins(sum);
		cbufout = sum;
		return sum;
	}
	float groupdelay(float f,float FS){
		if(dirty_grdel){
			grdel = ::groupdelay(f,&KernelB,1,&KernelA,1,FS);
			dirty_grdel = false;
		}
		return grdel;
	}
	float phasedelay(float f,float FS){
		//return ::PhaseDelay(f,&KernelB,1,&KernelA,1,FS);
		//if(dirty_phdel){
			float grpdel = groupdelay(f,FS);
			float omega = 2.0*M_PI*f/FS;
			float phdel = grpdel - omega*::PhaseDelayDerive(omega,&KernelB,1,&KernelA,1);
			//dirty_phdel = false;
		//}
		return phdel;
	}
};
//////////////////////////////////////////////////////////
template<int kernel_sizeB>
class LTITBv
{
	float KernelB[kernel_sizeB];
	CircularBufferT<kernel_sizeB> cbuf;

	public:
	LTITBv(){//Print("LTITv\n");
	};
	void push(float a){
		cbuf.push(a);
	}
	void setKernel(float KB[]){
		for(int i=0;i<kernel_sizeB;i++)
			KernelB[i] = KB[i];
	}
	float pushConvol(float a){
		push(a);
		return Convol();
	}
	float Convol()
	{
		float sum=0.;
		//get oldest sample from buffer
		int pAl2 = cbuf.pointer ;
		//pAl2 = pAl2%kernel_sizeB;
		//if(pAl2 >= kernel_sizeB) {pAl2 = 0;}

		int howmany = kernel_sizeB - pAl2;
		for(int i=0; i < howmany; i++)
			sum += KernelB[i]*cbuf.Buffer[pAl2 + i];
		for(int i=0; i < pAl2; i++)
			sum += KernelB[howmany + i]*cbuf.Buffer[i];
		return sum;
	}
	float groupdelay(float f,float FS){
		return ::groupdelay(f,KernelB,kernel_sizeB,0,0,FS);
	}
};
///////////////////////////
//Circular Buffer
struct CircularBufferBase
{
	public:
	Unit *unit;
	float *Buffer;
	int size;
	int pointer;
	CircularBufferBase(Unit * unit,int size);
	~CircularBufferBase();
	void push(float a);
	void push(float a,int pos);
	void add(float a,int pos);
	void set(float a,int pos);
	virtual int pointerInRange(int p)=0;
	virtual void allocate(int size)=0;
	float get();
	float get(int pos);
	virtual float delay(int pos)=0;
};

struct CircularBuffer : public CircularBufferBase
{
	public:
	CircularBuffer(Unit *unit,int size):CircularBufferBase(unit,size){};
	int pointerInRange(int p);
	void allocate(int size);
	float delay(int pos);
};

struct LTIv
{
	public:
	Unit *unit;
	float *KernelB;
	float *KernelA;
	int kernel_sizeB;
	int kernel_sizeA;

	CircularBuffer cbuf;
	CircularBuffer cbufout;

	LTIv(Unit* unit,int sizeB,int sizeA):cbuf(unit,sizeB),cbufout(unit,sizeA){
		//Print("LTIv construct\n");
		// for(int i=0;i<sizeB-1;i++){
			// Print("KernelB %d %g\n",i,KernelB[i]);
		// }
		this->unit = unit;
		kernel_sizeB = sizeB;
		kernel_sizeA = sizeA;
		KernelB = (float *)RTAlloc(unit->mWorld,sizeof(float)*sizeB);
		KernelA = (float *)RTAlloc(unit->mWorld,sizeof(float)*sizeA);
	};
	~LTIv(){
		RTFree(unit->mWorld,KernelB);
		RTFree(unit->mWorld,KernelA);
	};

	void* operator new(size_t sz, Unit* unit) {
		//Print("LTIv new\n");
		return RTAlloc(unit->mWorld, sizeof(LTIv));
	}
	void operator delete(void* pObject) {
		//Print("LTIv delete\n");
		LTIv * obj = (LTIv*)pObject;
		RTFree(obj->unit->mWorld, pObject);
	}

	void push(float a){
		cbuf.push(a);
	}
	void setKernel(float KB[],float KA[]){
		for(int i=0;i<kernel_sizeB;i++)
			KernelB[i] = KB[i];
		for(int i=0;i<kernel_sizeA;i++)
			KernelA[i] = KA[i];
	}
	void setKernelB(float val,int i){
		assertv2(i<kernel_sizeB,"bigger than kernelB");
		KernelB[i] = val;
	}
	void setKernelA(float val,int i){
		assertv2(i<kernel_sizeA,"bigger than kernelA");
		KernelA[i] = val;
	}
	float pushConvol(float a){
		push(a);
		return Convol();
	}
	float filter(float a){
		push(a);
		return Convol();
	}
	float Convol()
	{
		float sum=0.;
		//get oldest sample from buffer
		int pAl2 = cbuf.pointer ;
		//pAl2 = pAl2%kernel_sizeB;
		//if(pAl2 >= kernel_sizeB) {pAl2 = 0;}

		int howmany = kernel_sizeB - pAl2;
		for(int i=0; i < howmany; i++)
			sum += KernelB[i]*cbuf.Buffer[pAl2 + i];
		for(int i=0; i < pAl2; i++)
			sum += KernelB[howmany + i]*cbuf.Buffer[i];

		pAl2 = cbufout.pointer ;
		//pAl2 = pAl2%kernel_sizeA;
		//if(pAl2 >= kernel_sizeA) {pAl2 = 0;}
		howmany = kernel_sizeA - pAl2;
		for(int i=0; i < howmany; i++)
			sum -= KernelA[i]*cbufout.Buffer[pAl2 + i];
		for(int i=0; i < pAl2; i++)
			sum -= KernelA[howmany + i]*cbufout.Buffer[i];

		//sum = zapgremlins(sum);
		cbufout.push(sum);
		return sum;
	}
	float groupdelay(float f,float FS){
		return ::groupdelay(f,KernelB,kernel_sizeB,KernelA,kernel_sizeA,FS);
	}
};

struct Thirian:public LTIv
{
	float D;
	int N;
	Thirian(Unit* unit,float D,int N):LTIv(unit,N+1,N){
		this->N = N;
		this->D = 0;
		//Print("Thirian construct N %d\n",N);
		//for(int i=0;i<N-1;i++){
			//Print("KernelB %d %g\n",i,KernelB[i]);
		//}
		setcoeffs(D);
	}
	void* operator new(size_t sz, Unit* unit) {
		//Print("Thirian new\n");
		return RTAlloc(unit->mWorld, sizeof(Thirian));
	}
	void operator delete(void* pObject) {
		//Print("Thirian delete\n");
		Thirian * obj = (Thirian*)pObject;
		RTFree(obj->unit->mWorld, pObject);
	}
	void setcoeffs(float D){
		if(this->D != D){
			this->D = D;
			this->KernelB[N] = 1;
			for(int k=1;k<=N;k++) {
				double ak = (float)Nchoose((long)N,(long)k);
				if(k%2==1)
					ak = -ak;
					/*
				for(int n=0;n<=N;n++) {
					ak *= ((double)D-(double)(N-n));
					ak /= ((double)D-(double)(N-k-n));
				}
				*/
				for(int n=0;n<k;n++)
					ak *= ((double)D-(double)(N-n));
				for(int i=0;i<k;i++)
					ak /= ((double)D-(double)(-k+i));
				this->KernelA[k-1] = (float)ak;
				this->KernelB[N-k] = (float)ak;
			}
			/*
			for(int i=0;i<N+1;i++)
				Print("KernelB %d %g\n",i,KernelB[i]);
			for(int i=0;i<N;i++)
				Print("KernelA %d %g\n",i,KernelA[i]);
				*/
			//Print("D es %g\n",D);
		}
	}
};
struct Biquad : public LTITv<3,2>
{
	enum biquadtype {
		pass = 0,
		low,
		high,
		notch
	};
	void setcoeffs(float f0, float fs, float Q, int type){
		float a = 1/(2*tan(M_PI*f0/fs));
		float a2 = a*a;
		float aoQ = a/Q;
		float d = (4*a2+2*aoQ+1);

		KernelA[0] = -(8*a2-2) / d;
		KernelA[1] = (4*a2 - 2*aoQ + 1) / d;

		switch(type) {
		case pass:
			KernelB[0] = 2*aoQ/d;
			KernelB[1] = 0;
			KernelB[2] = -2*aoQ/d;
			break;
		case low:
			KernelB[0] = 1/d;
			KernelB[1] = 2/d;
			KernelB[2] = 1/d;
			break;
		case high:
			KernelB[0] = 4*a2/d;
			KernelB[1] = -8*a2/d;
			KernelB[2] = 4*a2/d;
			break;
		case notch:
			KernelB[0] = (1+4*a2)/d;
			KernelB[1] = (2-8*a2)/d;
			KernelB[2] = (1+4*a2)/d;
			break;
		}
		this->dirty_grdel = true;
	}
};
template<int N>
struct ThirianT : public LTITv<N+1,N>
{
	float D;
	ThirianT(){};//setcoeffs(0);};
	void setcoeffs(float D){
		//if(this->D != D){
			if(D < N-1)
				D = N-1;
			this->D = D;
			this->KernelB[N] = 1;
			for(int k=1;k<=N;k++) {
				double ak = (float)Nchoose((long)N,(long)k);
				if(k%2==1)
					ak = -ak;
					/*
				for(int n=0;n<=N;n++) {
					ak *= ((double)D-(double)(N-n));
					ak /= ((double)D-(double)(N-k-n));
				}
				*/
				for(int n=0;n<k;n++)
					ak *= ((double)D-(double)(N-n));
				for(int i=0;i<k;i++)
					ak /= ((double)D-(double)(-k+i));
				this->KernelA[k-1] = (float)ak;
				this->KernelB[N-k] = (float)ak;
			}
			this->dirty_grdel = true;
		//}
	}
};
struct FilterC1C3 : public LTITv<1,1>
{
	float freq;
	float c1;
	float c3;
	FilterC1C3(){c1 = 0;c3 = 0; freq = 0;};
	void setcoeffs(float freq,float c1,float c3){
		if(this->freq == freq && this->c1 == c1 && this->c3 == c3)
			return;
		//if(approximatelyEqual(this->freq,freq) && approximatelyEqual(this->c1,c1) && approximatelyEqual(this->c3,c3))
			//return;
			float g = 1.0 - c1/freq;
			float b = 4.0*c3+freq;
			float a1 = (-b+sqrt(b*b-16.0*c3*c3))/(4.0*c3);
			KernelB = g*(1+a1);
			KernelA = a1;
			//Print("freq %g c1 %g c3 %g\n",freq,c1,c3);
			//Print("this freq %g c1 %g c3 %g\n",this->freq - freq,this->c1-c1,this->c3-c3);
			this->freq = freq;
			this->c1 = c1;
			this->c3 = c3;
			this->dirty_grdel = true;
		//}
	}
};
struct FilterT0TN : public FilterC1C3
{
	float t0;
	float ftN;
	FilterT0TN():t0(0),ftN(0){};
	void setcoeffs(float freq,float t0,float ftN){
		//if(this->freq == freq && this->t0 == t0 && this->ftN == ftN)
		if(approximatelyEqual(this->freq,freq) && approximatelyEqual(this->t0,t0) && approximatelyEqual(this->ftN,ftN))
			return;
		//if(std::islessgreater(this->freq,freq) || std::islessgreater(this->t0,t0) || std::islessgreater(this->ftN,ftN)){
			Print("this freq %g c1 %g c3 %g\n",this->freq - freq,this->t0-t0,this->ftN-ftN);
			this->t0 = t0;
			this->ftN = ftN;
			float c1 = 1/t0;
			float c3 = (1.0 - ftN)/(ftN*t0*M_PI*M_PI);
			Print("freq %g c1 %g c3 %g\n",freq,c1,c3);
			Print("this freq %g c1 %g c3 %g\n",this->freq - freq,this->t0-t0,this->ftN-ftN);
			FilterC1C3::setcoeffs(freq,c1,c3);
		//}
	}
};

//////////////////////////////////////////////////////
//Householder Feedback Matrix

template<typename Tdecay,int size,int sizedel>
struct FDN_HH_Base{
	Tdecay decay[size];
	CircularBuffer2POWSizedT<sizedel> delay[size];
	//void setcoeffs(float c1, float c3, float mix, float Fs){};
	float mix,fac;
	float o[size];
    int o_perm[size];
	float c[size],cR[size];
	int lengths[size];// = {37,87,181,271,359,593,688,721};//{37, 87, 181,271, 359, 492, 687, 721};//{37,87,181,271,359,592,687,721};
    FDN_HH_Base(){
        fac = 2.0/(float)size;
        mix = 1.0;
        for(int k=0;k<size;k++) {
            o[k] = 0.0;
            o_perm[k] = (k+1)%size;
            c[k] = (k%2==0)?1.0:-1.0;
            cR[k] = ((k/2)%2==0)?1.0:-1.0;
        }
    }
    void setlengths(float len[size]){
        for(int i=0;i<size;i++)
            lengths[i] = sc_min(len[i],sizedel);
    }

    float go(float in)
    {
        float i[size];
        float sumo = 0;
        for(int j=0;j<size;j++)
            sumo += o[j];
        sumo *= fac;
        sumo -= in;
        for(int j=0;j<size;j++)
            i[j] = o[o_perm[j]] - sumo;

        float out = 0.0;
        for(int j=0;j<size;j++) {
            delay[j].push(i[j]);
            o[j] = decay[j].filter(delay[j].delay(lengths[j]));
            out += c[j] * o[j];
        }
        out *= fac;
        return mix*out + (1.0-mix)*in;
    }
    void go(float *in,float *outA, int N)
    {
        float i[size];
        for(int k=0; k<N; k++){
            float sumo = 0;
            for(int j=0;j<size;j++)
                sumo += o[j];
            sumo *= fac;
            sumo -= in[k];
            for(int j=0;j<size;j++)
                i[j] = o[o_perm[j]] - sumo;

            float out = 0.0;
            for(int j=0;j<size;j++) {
                delay[j].push(i[j]);
                o[j] = decay[j].filter(delay[j].delay(lengths[j]));
                out += c[j] * o[j];
            }
            out *= fac;
            outA[k] = mix*out + (1.0-mix)*in[k];
        }
    }
    void go_st(float *in,float *outA[2], int N)
    {
        float i[size];
        for(int k=0; k<N; k++){
            float sumo = 0;
            for(int j=0;j<size;j++)
                sumo += o[j];
            sumo *= fac;
            sumo -= in[k];
            for(int j=0;j<size;j++)
                i[j] = o[o_perm[j]] - sumo;

            float out[2];out[0]=0;out[1]=0;
            for(int j=0;j<size;j++) {
                delay[j].push(i[j]);
                o[j] = decay[j].filter(delay[j].delay(lengths[j]));
                out[0] += c[j] * o[j];
                out[1] += cR[j] * o[j];
            }
            out[0] *=fac;
            out[1] *=fac;

            outA[0][k] =  mix*out[0] + (1.0-mix)*in[k];
            outA[1][k] =  mix*out[1] + (1.0-mix)*in[k];
        }
    }
};

template<int size,int sizedel>
struct FDNC1C3: public FDN_HH_Base<FilterC1C3,size,sizedel>{
    float c1,c3;
    void setcoeffs(float c1, float c3, float mix, float Fs){
        this->mix = mix;
        this->c1 = c1;
        this->c3 = c3;
        for(int k=0;k<size;k++) {
            this->decay[k].setcoeffs(Fs/this->lengths[k],c1,c3);
        }
    }
};

///////////////////////////////////

template<int M>
struct ThirianDispersion{

	float freq;
	float B;
	ThirianT<2> dispersion[M];
	void setcoeffs(float freq,float B){
		if(B==0){
			this->B = 0.0;
			return;
		}

		if(this->freq==freq && this->B==B)
			return;
		float D = ValimakiDispersion(B,freq,M);
		//if(D <=1)
			//Print("D es %g\n",D);
		for(int i=0; i<M ;i++)
			dispersion[i].setcoeffs(D);
		this->freq = freq;
		this->B = B;

	}
	float groupdelay(float FS){
		if(B==0)
			return 0;
		return M*dispersion[0].groupdelay(freq,FS);
	}
	float phasedelay(float FS){
		if(B==0)
			return 0;
		return M*dispersion[0].phasedelay(freq,FS);
	}
	float filter(float a){
		if(B==0)
			return a;
		float out = a;
		for(int i=0; i<M ;i++)
			out = dispersion[i].filter(out);
		return out;
	}
};
struct DCBlocker:public LTITv<2,1>{
	float R;
	DCBlocker():R(0){setcoeffs(0.995);};
	void setcoeffs(float R){
		if(this->R == R)
			return;
		float g = (1 + R)/2;
		KernelB[0] = g;
		KernelB[1] = -g;
		KernelA[0] = -R;
		this->R = R;
		this->dirty_grdel = true;
	}
};

struct KL_Junction{
    Unit * unit;
    float k,one_plus_k,one_minus_k;
    float inR;
    float inL;
    float outR;
    float outL;
    KL_Junction(Unit * unit):inR(0),inL(0),outR(0),outL(0),k(0){this->unit = unit;};
    void* operator new(size_t sz, Unit* unit) {
        //Print("KL_Junction new\n");
		void * ptr = NULL;
        ptr = RTAlloc(unit->mWorld,sz);
        /*
        if(ptr==NULL)
            Print("Null pointer\n");
        else
            Print("not Null pointer\n");*/
        return ptr;
	}
    void operator delete(void* pObject) {
		//Print("KL_Junction delete\n");
		KL_Junction * obj = (KL_Junction*)pObject;
		RTFree(obj->unit->mWorld, pObject);
	}
    void set_areas(float a1,float a2){
        float num = a1 - a2;
        if (num == 0.0)
            k = 0.0;
        else
            k = num/(a1 + a2);
        one_plus_k = 1.0 + k;
        one_minus_k = 1.0 - k;
        /*
        if(a1 < 1e-18)
            lossR = 0.0;
        else
            lossR = 1.0 - std::min(lossF/sqrt(a1),1.0);
        if(a2 < 1e-18)
            lossL = 0.0;
        else
            lossL = 1.0 - std::min(lossF/sqrt(a2),1.0);
        */
    }
    void go(){
        //inR *= lossR;
        //inL *= lossL;
        outR = inR*(one_plus_k) - inL*k;
        outL = inL*(one_minus_k) + inR*k;
        kill_denormals(outR);
        kill_denormals(outL);
    }
};

struct KL_JunctionU{
    Unit * unit;
    float k;
    float inR;
    float inL;
    float outR;
    float outL;
    KL_JunctionU(Unit * unit):inR(0),inL(0),outR(0),outL(0),k(0){this->unit = unit;};
    void* operator new(size_t sz, Unit* unit) {
        //Print("KL_Junction new\n");
		void * ptr = NULL;
        ptr = RTAlloc(unit->mWorld,sz);
        /*
        if(ptr==NULL)
            Print("Null pointer\n");
        else
            Print("not Null pointer\n");*/
        return ptr;
	}
    void operator delete(void* pObject) {
		//Print("KL_Junction delete\n");
		KL_Junction * obj = (KL_Junction*)pObject;
		RTFree(obj->unit->mWorld, pObject);
	}
    void set_areas(float a1,float a2){
        float num = a1 - a2;
        if (num == 0.0)
            k = 0.0;
        else
            k = num/(a1 + a2);
        /*
        if(a1 < 1e-18)
            lossR = 0.0;
        else
            lossR = 1.0 - std::min(lossF/sqrt(a1),1.0);
        if(a2 < 1e-18)
            lossL = 0.0;
        else
            lossL = 1.0 - std::min(lossF/sqrt(a2),1.0);
        */
    }
    void go(){
        //inR *= lossR;
        //inL *= lossL;
        outR = inR*(1.0 - k) - inL*k;
        outL = inL*(1.0 + k) + inR*k;
        kill_denormals(outR);
        kill_denormals(outL);
    }
};
template<int N>
class NJunction{
    public:
    float in[N];
    float out[N];
    float alpha[N];
    /*
    void* operator new(size_t sz, Unit* unit) {
        return RTAlloc(unit->mWorld,sz);
	}
    void operator delete(void* pObject) {
		//Print("KL_Junction delete\n");
		NJunction * obj = (NJunction*)pObject;
		RTFree(obj->unit->mWorld, pObject);
	}*/
    NJunction(){
        //Print("NJunction const\n");
        for(int i=0;i<N;i++){
            in[i]=0;
            out[i]=0;
        }
    }
    void Init(){
        //Print("NJunction init\n");
        for(int i=0;i<N;i++){
            in[i]=0;
            out[i]=0;
        }
    }
    void calc_alpha(float *areas){
        float Sa = 0;
        for(int i=0;i<N;i++)
            Sa += areas[i];
        for(int i=0;i<N;i++)
            alpha[i] = 2.0*areas[i]/Sa;
    }
    void go(){
        float PJ=0;
        for(int i=0;i<N;i++)
            PJ += alpha[i]*in[i];
        for(int i=0;i<N;i++)
            out[i] = PJ - in[i];
    }
};
template<int N>
class NJunctionU{
    public:
    float in[N];
    float out[N];
    float alpha[N];
    /*
    void* operator new(size_t sz, Unit* unit) {
        return RTAlloc(unit->mWorld,sz);
	}
    void operator delete(void* pObject) {
		//Print("KL_Junction delete\n");
		NJunction * obj = (NJunction*)pObject;
		RTFree(obj->unit->mWorld, pObject);
	}*/
    NJunctionU(){
        //Print("NJunctionU const\n");
        for(int i=0;i<N;i++){
            in[i]=0;
            out[i]=0;
        }
    }
    void Init(){
        //Print("NJunctionU init\n");
        for(int i=0;i<N;i++){
            in[i]=0;
            out[i]=0;
        }
    }
    void calc_alpha(float *areas){
        float Sa = 0;
        for(int i=0;i<N;i++)
            Sa += areas[i];
        for(int i=0;i<N;i++)
            alpha[i] = 2.0*areas[i]/Sa;
    }
    void go(){
        float UJ=0;
        for(int i=0;i<N;i++)
            UJ += in[i];
        for(int i=0;i<N;i++)
            out[i] = in[i] - alpha[i] * UJ;
    }
};
class TUBE{
    Unit * unit;
    public:
    Lagrange1T<1024> Rline;
    Lagrange1T<1024> Lline;
    float inR;
    float inL;
    float outR;
    float outL;
    float del;
    float loss;
    void* operator new(size_t sz, Unit* unit) {
        //Print("TUBE new\n");
		void * ptr = NULL;
        ptr = RTAlloc(unit->mWorld,sz);

        return ptr;
	}
    void operator delete(void* pObject) {
		TUBE * obj = (TUBE*)pObject;
		RTFree(obj->unit->mWorld, pObject);
	}
    TUBE(Unit * unit):inR(0),inL(0),outR(0),outL(0),del(0.0),loss(1){
        //Print("TUBE constructor\n");
        this->unit = unit;
    }
    void set_area(float a1,float lossF)
    {
        if(a1 < 1e-18)
            loss = 0.0;
        else
            loss = 1.0 - sc_min(lossF/sqrt(a1),1.0f);

    }
    void go(){
        Rline.push(inR);
        Lline.push(inL);
        outR = loss*Rline.delay(del);
        outL = loss*Lline.delay(del);
    }
};
#endif