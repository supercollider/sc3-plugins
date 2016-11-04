/* Oversamp.h
   Copyright (C) 2007 by Bjoern Erlach. */

/* Macros and structs for oversampled SuperCollider Ugens */
/* For an example of how to use this macros see the file Oversamp8.cpp */

#define OVERSAMPLE_INIT(n,off)\
  int domemsize = (n) * BUFLENGTH + (off);\
  unit->m_domem = (float*)RTAlloc(unit->mWorld, sizeof(float)*domemsize);\
  memset(unit->m_domem, 0, sizeof(float)*domemsize);\
  memset(unit->m_upmem, 0, sizeof(float)*8)

#define OVERSAMPLE_DTOR\
  RTFree(unit->mWorld, unit->m_domem)

#define UPMEM(i) (upmem[i])
#define DOMEMPUT(i) (domemoff[oversampidx+(i)])
#define DOMEMGET(i) (domemoff[(oversampidx-(i))])

struct Oversamp3 : public Unit
{
	float* m_domem;
	float m_upmem[8];
};

#define OVERSAMPLE3_INIT OVERSAMPLE_INIT(3, 46)



#define UPSAMPLE3\
  float *upmem = unit->m_upmem; \
  float *domemoff = unit->m_domem+46; \
  int oversampidx = 0;\
  LOOP(inNumSamples, \
     float x;\
     UPMEM(0) = x = ZXP(in);\
     DOMEMPUT(0) = UPMEM(4);\
     DOMEMPUT(1) = -0.008551916f*x+0.038633489f*UPMEM(1)-0.115727938f*UPMEM(2)+\
                   0.391427368f*UPMEM(3)+0.816087019f*UPMEM(4)-\
                   0.165084609f*UPMEM(5)+0.056872437f*UPMEM(6)-\
                   0.015422813f*UPMEM(7);\
     DOMEMPUT(2) = -0.015422813f*x+0.056872437f*UPMEM(1)-0.165084609f*UPMEM(2)+ \
                    0.816087019f*UPMEM(3)+0.391427368f*UPMEM(4)-\
                    0.115727938f*UPMEM(5)+0.038633489f*UPMEM(6)-\
                    0.008551916f*UPMEM(7);\
     UPMEM(7) = UPMEM(6); UPMEM(6) = UPMEM(5); UPMEM(5) = UPMEM(4);\
     UPMEM(4) = UPMEM(3); UPMEM(3) = UPMEM(2); UPMEM(2) = UPMEM(1);\
     UPMEM(1) = UPMEM(0);\
     oversampidx+=3;\
     )



#define DOWNSAMPLE3\
  oversampidx = 0;\
  LOOP(inNumSamples,\
       ZXP(out) = -0.003138415f * (DOMEMGET(0)+DOMEMGET(46)) +\
                  -0.003798740f * (DOMEMGET(1)+DOMEMGET(45)) +\
		  0.005366905f  * (DOMEMGET(3)+DOMEMGET(43)) +\
		  0.006292481f  * (DOMEMGET(4)+DOMEMGET(42)) +\
		  -0.008484863f * (DOMEMGET(6)+DOMEMGET(40)) +\
		  -0.009783173f * (DOMEMGET(7)+DOMEMGET(39)) +\
		  0.012895758f  * (DOMEMGET(9)+DOMEMGET(37)) +\
		  0.014775299f  * (DOMEMGET(10)+DOMEMGET(36)) +\
		  -0.019433890f * (DOMEMGET(12)+DOMEMGET(34)) +\
		  -0.022375618f * (DOMEMGET(13)+DOMEMGET(33)) +\
		  0.030199310f  * (DOMEMGET(15)+DOMEMGET(31)) +\
		  0.035614857f  * (DOMEMGET(16)+DOMEMGET(30)) +\
		  -0.052397427f * (DOMEMGET(18)+DOMEMGET(28)) +\
		  -0.066713896f * (DOMEMGET(19)+DOMEMGET(27)) +\
		  0.136721817f  * (DOMEMGET(21)+DOMEMGET(25)) +\
		  0.275108076f  * (DOMEMGET(22)+DOMEMGET(24)) +\
		  0.333333333f  * DOMEMGET(23);\
       oversampidx+=3;\
       );\
  memmove((domemoff-46), domemoff+(inNumSamples*3-46), sizeof(float)*46)



struct Oversamp4 : public Unit
{
	float m_upmem[8];
	float* m_domem;
};

#define OVERSAMPLE4_INIT OVERSAMPLE_INIT(4, 46)



#define UPSAMPLE4\
  float *upmem = unit->m_upmem; \
  float *domemoff = unit->m_domem+46; \
  int oversampidx = 0;\
  LOOP(inNumSamples, \
     float x;\
       UPMEM(0) = x = ZXP(in);\
	  DOMEMPUT(0) = UPMEM(4);\
	  DOMEMPUT(1) = -0.0042426311691388f*x+ 0.0248294827511144f*UPMEM(1) -\
                 0.0823400298177909f*UPMEM(2)+ 0.2773229763200462f*UPMEM(3)+ \
                 0.8925067048294461f*UPMEM(4) -0.1441502708734832f*UPMEM(5) +\
                 0.0466933953083191f*UPMEM(6) -0.0116341035822361f*UPMEM(7);\
	  DOMEMPUT(2) = -0.0103948826638472f*(x+UPMEM(7)) +\
                        0.0487048015914403f*(UPMEM(1)+UPMEM(6)) -\
                        0.1535393011057635f*(UPMEM(2)+UPMEM(5)) +\
		        0.6147615761636365f*(UPMEM(3)+UPMEM(4));\
	  DOMEMPUT(3)= -0.0116341035822361f*x+ 0.0466933953083191f*UPMEM(1) -\
                 0.1441502708734832f*UPMEM(2)+ 0.8925067048294461f*UPMEM(3) + \
                 0.2773229763200462f*UPMEM(4) -0.0823400298177909f*UPMEM(5) +\
		  0.0248294827511144f*UPMEM(6) -0.0042426311691388f*UPMEM(7);\
     UPMEM(7) = UPMEM(6); UPMEM(6) = UPMEM(5); UPMEM(5) = UPMEM(4);\
     UPMEM(4) = UPMEM(3); UPMEM(3) = UPMEM(2); UPMEM(2) = UPMEM(1);\
     UPMEM(1) = UPMEM(0);\
     oversampidx+=4;\
     )

#define UPSAMPLE4LININTERP\
  float *upmem = unit->m_upmem; \
  float *domemoff = unit->m_domem+46; \
  int oversampidx = 0;\
  LOOP(inNumSamples, \
     float x;\
       float dif;\
       UPMEM(0) = x = ZXP(in);\
       dif = x - UPMEM(1);\
       DOMEMPUT(0) = UPMEM(1);\
       DOMEMPUT(1) = UPMEM(1)+dif/4;\
       DOMEMPUT(2) = UPMEM(1)+(2*dif)/4;\
       DOMEMPUT(3) = UPMEM(1)+(3*dif)/4;\
     UPMEM(1) = UPMEM(0);\
     oversampidx+=4;\
     )

#define DOWNSAMPLE4\
  oversampidx = 0;\
  LOOP(inNumSamples,\
 ZXP(out)= (-0.012589000000000f * (DOMEMGET(0) + DOMEMGET(46)) +\
       -0.020990000000000f * (DOMEMGET(1)  + DOMEMGET(45))+\
       -0.017339000000000f * (DOMEMGET(2)  + DOMEMGET(44)) +\
        0.023171000000000f * (DOMEMGET(4)  + DOMEMGET(42)) +\
        0.037581000000000f * (DOMEMGET(5)  + DOMEMGET(41)) +\
        0.030358000000000f * (DOMEMGET(6)  + DOMEMGET(40)) +\
       -0.039313000000000f * (DOMEMGET(8)  + DOMEMGET(38)) +\
       -0.063143000000000f * (DOMEMGET(9)  + DOMEMGET(37)) +\
       -0.050708000000000f * (DOMEMGET(10) + DOMEMGET(36)) +\
        0.065704000000000f * (DOMEMGET(12) + DOMEMGET(34)) +\
        0.106310000000000f * (DOMEMGET(13) + DOMEMGET(33)) +\
        0.086513000000000f * (DOMEMGET(14) + DOMEMGET(32)) +\
       -0.117900000000000f * (DOMEMGET(16) + DOMEMGET(30)) +\
       -0.199120000000000f * (DOMEMGET(17) + DOMEMGET(29)) +\
       -0.172300000000000f * (DOMEMGET(18) + DOMEMGET(28)) +\
        0.295400000000000f * (DOMEMGET(20) + DOMEMGET(26)) +\
        0.632170000000000f * (DOMEMGET(21) + DOMEMGET(25)) +\
        0.898740000000000f * (DOMEMGET(22) + DOMEMGET(24)) +\
        1.000000000000000f * DOMEMGET(23)) * 0.25f;\
       oversampidx+=4;\
       );\
       memmove((domemoff-46), domemoff+(inNumSamples*4-46), sizeof(float)*46)



// (+ (* 64 4) 46)
// (+ (* 64 8) 72)
struct Oversamp8 : public Unit
{
	float m_upmem[8];
	float* m_domem;
};

#define OVERSAMPLE8_INIT OVERSAMPLE_INIT(8, 72)



#define UPSAMPLE8\
  float *upmem = unit->m_upmem; \
  float *domemoff = unit->m_domem+72; \
  int oversampidx = 0;\
  LOOP(inNumSamples, \
     float x;\
 UPMEM(0) = x = ZXP(in);\
 DOMEMPUT(0) = UPMEM(4);\
 DOMEMPUT(1)=\
-0.0037931994f*x \
+0.0158813611f*UPMEM(1)\
-0.0442545016f*UPMEM(2)\
+0.1287107838f*UPMEM(3)\
+0.9753976635f*UPMEM(4)\
-0.0948656533f*UPMEM(5)\
+0.0347190922f*UPMEM(6)\
-0.0117955463f*UPMEM(7);\
 DOMEMPUT(2)=\
-0.0088142121f *x \
+0.0338077224f *UPMEM(1)\
-0.0924474127f *UPMEM(2)\
+0.2840130427f *UPMEM(3)\
+0.8982382508f *UPMEM(4)\
-0.1530045572f *UPMEM(5)\
+0.0568189369f *UPMEM(6)\
-0.0186117707f *UPMEM(7);\
 DOMEMPUT(3)=\
-0.0141879675f *x \
+0.0506097695f *UPMEM(1)\
-0.1365747821f *UPMEM(2)\
+0.4539031668f *UPMEM(3)\
+0.7767189331f *UPMEM(4)\
-0.1754713746f *UPMEM(5)\
+0.0655667940f *UPMEM(6)\
-0.0205645392f *UPMEM(7);\
 DOMEMPUT(4)=\
-0.0186127440f *(x+UPMEM(7)) \
+0.0624728497f *(UPMEM(1)+UPMEM(6))\
-0.1673546124f *(UPMEM(2)+UPMEM(5))\
+0.6234945067f *(UPMEM(3)+UPMEM(4));\
 DOMEMPUT(5)=\
-0.0205645392f *x \
+0.0655667940f *UPMEM(1)\
-0.1754713746f *UPMEM(2)\
+0.7767189331f *UPMEM(3)\
+0.4539031668f *UPMEM(4)\
-0.1365747821f *UPMEM(5)\
+0.0506097695f *UPMEM(6)\
-0.0141879675f *UPMEM(7);\
 DOMEMPUT(6)=\
-0.0186117707*x \
+0.0568189369f*UPMEM(1)\
-0.1530045572f*UPMEM(2)\
+0.8982382508f*UPMEM(3)\
+0.2840130427f*UPMEM(4)\
-0.0924474127f*UPMEM(5)\
+0.0338077224f*UPMEM(6)\
-0.0088142121f*UPMEM(7);\
 DOMEMPUT(7)=\
-0.0117955463f*x \
+0.0347190922f*UPMEM(1)\
-0.0948656533f*UPMEM(2)\
+0.9753976635f*UPMEM(3)\
+0.1287107838f*UPMEM(4)\
-0.0442545016f*UPMEM(5)\
+0.0158813611f*UPMEM(6)\
-0.0037931994f*UPMEM(7);\
     UPMEM(7) = UPMEM(6); UPMEM(6) = UPMEM(5); UPMEM(5) = UPMEM(4);\
     UPMEM(4) = UPMEM(3); UPMEM(3) = UPMEM(2); UPMEM(2) = UPMEM(1);\
     UPMEM(1) = UPMEM(0);\
     oversampidx+=8;\
       )


#define DOWNSAMPLE8\
  oversampidx = 0;\
  LOOP(inNumSamples,\
   ZXP(out) = 0.00248839632f * (DOMEMGET(0) + DOMEMGET(72)) +\
       0.00258871550f * (DOMEMGET(1) + DOMEMGET(71)) +\
       0.00221903196f * (DOMEMGET(2) + DOMEMGET(70)) + \
       0.00133875226f * (DOMEMGET(3) + DOMEMGET(69)) +\
       -0.00164433851f * (DOMEMGET(5) + DOMEMGET(67)) + \
       -0.00335091872f * (DOMEMGET(6) + DOMEMGET(66)) +\
       -0.00481555469f * (DOMEMGET(7) + DOMEMGET(65)) +\
       -0.00571956013f * (DOMEMGET(8) + DOMEMGET(64)) +\
       -0.00578670062f * (DOMEMGET(9) + DOMEMGET(63)) +\
       -0.00484180568f * (DOMEMGET(10) + DOMEMGET(62)) +\
       -0.00286054003f * (DOMEMGET(11) + DOMEMGET(61)) +\
       0.00339815750f * (DOMEMGET(13) + DOMEMGET(59)) +\
       0.00683659191f * (DOMEMGET(14) + DOMEMGET(58)) +\
       0.00972271787f * (DOMEMGET(15) + DOMEMGET(57)) +\
       0.01145479470f * (DOMEMGET(16)  + DOMEMGET(56)) +\
       0.01152270030f * (DOMEMGET(17) + DOMEMGET(55)) +\
       0.00960868457f * (DOMEMGET(18) + DOMEMGET(54)) +\
       0.00567172832f * (DOMEMGET(19) + DOMEMGET(53)) +\
       -0.00678062348f * (DOMEMGET(21) + DOMEMGET(51)) +\
       -0.01374827838f * (DOMEMGET(22) + DOMEMGET(50)) +\
       -0.01977615927f * (DOMEMGET(23) + DOMEMGET(49)) +\
       -0.02366429666f * (DOMEMGET(24) + DOMEMGET(48)) +\
       -0.02429654885f * (DOMEMGET(25) + DOMEMGET(47)) +\
       -0.02080225311f * (DOMEMGET(26) + DOMEMGET(46)) +\
       -0.01269974211f * (DOMEMGET(27) + DOMEMGET(45)) +\
       0.01674687665f * (DOMEMGET(29) + DOMEMGET(43)) +\
       0.03647254519f * (DOMEMGET(30) + DOMEMGET(42)) +\
       0.05767969442f * (DOMEMGET(31) + DOMEMGET(41)) +\
       0.07859121686f * (DOMEMGET(32) + DOMEMGET(40)) +\
       0.09734202792f * (DOMEMGET(33) + DOMEMGET(39)) +\
       0.11218975207f * (DOMEMGET(34) + DOMEMGET(38)) +\
       0.12171719386f * (DOMEMGET(35) + DOMEMGET(37)) +\
       0.12500000000f * DOMEMGET(36);\
       oversampidx+=8;\
       );\
       memmove((domemoff-72), domemoff+(inNumSamples*8-72), sizeof(float)*72)
