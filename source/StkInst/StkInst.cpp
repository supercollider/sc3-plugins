#include "SC_PlugIn.h"
InterfaceTable *ft;
World * gWorld;

// STK Instrument Classes
#include <Clarinet.h>
#include <BlowHole.h>
#include <Saxofony.h>
#include <Flute.h>
#include <Brass.h>
#include <BlowBotl.h>
#include <Bowed.h>
#include <Plucked.h>
#include <StifKarp.h>
#include <Sitar.h>
#include <Mandolin.h>
#include <Rhodey.h>
#include <Wurley.h>
#include <TubeBell.h>
#include <HevyMetl.h>
#include <PercFlut.h>
#include <BeeThree.h>
#include <FMVoices.h>
#include <VoicForm.h>
#include <Moog.h>
#include <Simple.h>
#include <Drummer.h>
#include <BandedWG.h>
#include <Shakers.h>
#include <ModalBar.h>
#include <Mesh2D.h>
#include <Resonate.h>
#include <Whistle.h>

/*
void* operator new (unsigned int size)
{
    void* ptr = RTAlloc(gWorld, size);
    return ptr;
}
void operator delete(void* ptr) noexcept
{
    RTFree(gWorld, ptr);
}
*/

using namespace stk;


#define NUM_INSTS 28

// The order of the following list is important.  The location of a particular
// instrument in the list should correspond to that instrument's ProgramChange
// number (i.e. Clarinet = ProgramChange 0).
char insts[NUM_INSTS][10] = { "Clarinet", "BlowHole", "Saxofony", "Flute", "Brass",
                              "BlowBotl", "Bowed", "Plucked", "StifKarp", "Sitar", "Mandolin",
                              "Rhodey", "Wurley", "TubeBell", "HevyMetl", "PercFlut",
                              "BeeThree", "FMVoices", "VoicForm", "Moog", "Simple", "Drummer",
                              "BandedWG", "Shakers", "ModalBar", "Mesh2D", "Resonate", "Whistle" };
#define GETMEM(Inst) RTAlloc(gWorld, sizeof(Inst))
                        
void * getmem(int number){
    
  void *mem=0;
  if     (number==0)  mem = GETMEM(Clarinet);
  else if (number==1)  mem = GETMEM(BlowHole);
  else if (number==2)  mem = GETMEM(Saxofony);
  else if (number==3)  mem = GETMEM(Flute);
  else if (number==4)  mem = GETMEM(Brass);
  else if (number==5)  mem = GETMEM(BlowBotl);
  else if (number==6)  mem = GETMEM(Bowed);
  else if (number==7)  mem = GETMEM(Plucked);
  else if (number==8)  mem = GETMEM(StifKarp);
  else if (number==9)  mem = GETMEM(Sitar);
  else if (number==10) mem = GETMEM(Mandolin);

  else if (number==11) mem = GETMEM(Rhodey);
  else if (number==12) mem = GETMEM(Wurley);
  else if (number==13) mem = GETMEM(TubeBell);
  else if (number==14) mem = GETMEM(HevyMetl);
  else if (number==15) mem = GETMEM(PercFlut);
  else if (number==16) mem = GETMEM(BeeThree);
  else if (number==17) mem = GETMEM(FMVoices);

  else if (number==18) mem = GETMEM(VoicForm);
  else if (number==19) mem = GETMEM(Moog);
  else if (number==20) mem = GETMEM(Simple);
  else if (number==21) mem = GETMEM(Drummer);
  else if (number==22) mem = GETMEM(BandedWG);
  else if (number==23) mem = GETMEM(Shakers);
  else if (number==24) mem = GETMEM(ModalBar);
  else if (number==25) mem = GETMEM(Mesh2D);
  else if (number==26) mem = GETMEM(Resonate);
  else if (number==27) mem = GETMEM(Whistle);

  return mem;
}
int voiceByNumber(int number, Instrmnt **instrument)
{
  int temp = number;
  void *mem = getmem(number);
  if     (number==0)  *instrument = new(mem) Clarinet(10.0);
  else if (number==1)  *instrument = new(mem) BlowHole(10.0);
  else if (number==2)  *instrument = new(mem) Saxofony(10.0);
  else if (number==3)  *instrument = new(mem) Flute(10.0);
  else if (number==4)  *instrument = new(mem) Brass(10.0);
  else if (number==5)  *instrument = new(mem) BlowBotl;
  else if (number==6)  *instrument = new(mem) Bowed(10.0);
  else if (number==7)  *instrument = new(mem) Plucked(5.0);
  else if (number==8)  *instrument = new(mem) StifKarp(5.0);
  else if (number==9)  *instrument = new(mem) Sitar(5.0);
  else if (number==10) *instrument = new(mem) Mandolin(5.0);

  else if (number==11) *instrument = new(mem) Rhodey;
  else if (number==12) *instrument = new(mem) Wurley;
  else if (number==13) *instrument = new(mem) TubeBell;
  else if (number==14) *instrument = new(mem) HevyMetl;
  else if (number==15) *instrument = new(mem) PercFlut;
  else if (number==16) *instrument = new(mem) BeeThree;
  else if (number==17) *instrument = new(mem) FMVoices;

  else if (number==18) *instrument = new(mem) VoicForm;
  else if (number==19) *instrument = new(mem) Moog;
  else if (number==20) *instrument = new(mem) Simple;
  else if (number==21) *instrument = new(mem) Drummer;
  else if (number==22) *instrument = new(mem) BandedWG;
  else if (number==23) *instrument = new(mem) Shakers;
  else if (number==24) *instrument = new(mem) ModalBar;
  else if (number==25) *instrument = new(mem) Mesh2D(10, 10);
  else if (number==26) *instrument = new(mem) Resonate;
  else if (number==27) *instrument = new(mem) Whistle;

  else {
    printf("\nUnknown instrument or program change requested!\n");
    temp = -1;
  }

  return temp;
}


struct StkInst : public Unit
{
    Instrmnt * inst;
    bool  gate;
    float freq;
    float ampat;
    float amprel;

    int m_valueSize;
    int m_valueOffset;
    float *m_values;
    float *old_values;
    bool do_freq;
    int instNumber;
    //StkInst(Unit *unit);
    //~StkInst(){};
};

extern "C"
{
    void StkInst_next(StkInst *unit, int inNumSamples);
    void StkInst_Ctor(StkInst* unit);
    void StkInst_Dtor(StkInst* unit);
}

static void Unit_next_nop(StkInst * unit, int inNumSamples)
{}
void StkInst_Ctor(StkInst* unit) {

    gWorld = unit->mWorld;
    Stk::setSampleRate( SAMPLERATE );

    unit->gate = false;
    unit->freq = IN0(0);
    unit->ampat = 1;
    unit->amprel = 1;
    unit->m_values = 0;
    unit->old_values = 0;
    unit->do_freq = true;
    const int kVarOffset = 5;

    unit->instNumber = IN0(4);
    unit->m_valueSize = unit->mNumInputs - kVarOffset;
    //Print("StkInst\n");
    //Print("StkInst %d,%f;%f,%d,%d\n",unit->instNumber,unit->freq,unit->ampat,unit->mNumInputs,unit->m_valueSize);
    //make it pair
    unit->m_valueSize = floor(unit->m_valueSize /2.0) * 2.0;
    if(unit->m_valueSize > 0){
        const int valuesAllocSize = unit->m_valueSize * sizeof(float);
        char * chunk = (char*)RTAlloc(unit->mWorld, valuesAllocSize*2);
        if (!chunk) {
            Print("StkInst: RT memory allocation failed on values\n");
            SETCALC(Unit_next_nop);
            return;
        }
        unit->m_values  = (float*)chunk;
        unit->old_values = (float*)(chunk + valuesAllocSize);
        for (int i=0; i<unit->m_valueSize; i++)
            unit->old_values[i] = -1; //set old values to something imposible
    }

    unit->inst = NULL;
    try {
        if(voiceByNumber(unit->instNumber,&(unit->inst))==-1){
            Print("StkInst: RT memory allocation failed on instrument\n");
            SETCALC(Unit_next_nop);
            return;
        }
    }
    catch ( StkError & e) {
        Print("StkInst: Error on creation %s",e.getMessageCString());
        SETCALC(Unit_next_nop);
        return;
    }

    SETCALC(StkInst_next);
}

void StkInst_Dtor(StkInst* unit)
{
    //delete unit->m_values;
    //delete unit->inst;
    RTFree(unit->mWorld, unit->m_values);
    RTFree(unit->mWorld, unit->inst);

}

void StkInst_next(StkInst *unit, int inNumSamples)
{
    float *out = OUT(0);

    float freq      = IN0(0);
    bool  gate      = IN0(1) > 0.f;
    float  ampat        = IN0(2);
    float  amprel       = IN0(3);

    
    float *values = unit->m_values;
    float *old_values = unit->old_values;
    int valueSize = unit->m_valueSize;
    int valueOffset = 5;
    for (int i=0; i<valueSize; i++)
        values[i] = IN0(i + valueOffset);

    for(int i=0; i < valueSize ; i+=2){
        if(values[i+1] != old_values[i + 1]){
            unit->inst->controlChange(values[i], values[i+1]);
            old_values[i + 1] = values[i+1];
        }
    }
    if(unit->do_freq && freq != unit->freq){
        unit->inst->setFrequency(freq);
        unit->freq = freq;
    }
    
    if(gate != unit->gate){
        if(gate){
            //Print("Starting freq %f\n",freq);
            unit->inst->noteOn(freq, ampat);
        }else{
            //Print("Stopping\n");
            unit->inst->noteOff(amprel);
        }
        unit->gate = gate;
    }

    for (int i=0; i < inNumSamples; ++i)
    {
        out[i] = unit->inst->tick();// * 7.5f; // Scaled to approx +-1
    }

}

////////////////StkMesh2D
struct StkMesh2D : public Unit
{
    Mesh2D * inst;
    bool  gate;
    float ampat;
    float XD;
    float YD;
    float Xpos;
    float Ypos;
    float decay;

};

extern "C"
{
    void StkMesh2D_next(StkMesh2D *unit, int inNumSamples);
    void StkMesh2D_Ctor(StkMesh2D* unit);
    void StkMesh2D_Dtor(StkMesh2D* unit);
}

static void StkMesh2D_next_nop(StkMesh2D * unit, int inNumSamples)
{}
void StkMesh2D_Ctor(StkMesh2D* unit) {
    gWorld = unit->mWorld;
    Stk::setSampleRate( SAMPLERATE );
    unit->gate = false;
    unit->ampat = IN0(2);
    unit->XD = IN0(3);
    unit->YD = IN0(4);
    unit->Xpos = IN0(5);
    unit->Ypos = IN0(6);
    unit->decay = IN0(7);
    
    void* mem = RTAlloc(unit->mWorld,sizeof(Mesh2D));
    if (!mem) {
            Print("StkMesh2D: RT memory allocation failed on values\n");
            SETCALC(Unit_next_nop);
            return;
        }
    unit->inst = new(mem) Mesh2D(unit->XD,unit->YD); 
    if(unit->inst==NULL){
        Print("StkMesh2D: RT memory allocation failed on instrument\n");
        SETCALC(StkMesh2D_next_nop);
        return;
    }
    //Print("Mesh created\n");
    unit->inst->setDecay(unit->decay);
    SETCALC(StkMesh2D_next);
    //Print("Mesh created2\n");
}

void StkMesh2D_Dtor(StkMesh2D* unit)
{

    //delete unit->inst;
    RTFree(unit->mWorld, unit->inst);
}

void StkMesh2D_next(StkMesh2D *unit, int inNumSamples)
{
    float *out = OUT(0);

    float *inp = IN(0);
    bool  gate = IN0(1) > 0.f;
    unit->ampat = IN0(2);
    unit->XD = IN0(3);
    unit->YD = IN0(4);
    unit->Xpos = IN0(5);
    unit->Ypos = IN0(6);
    unit->decay = IN0(7);
    //for(int i=1;i<8;i++)
    //  Print("IN(%d) is %f",i,IN0(i));
    //Print("\n");
    unit->inst->setInputPosition(unit->Xpos,unit->Ypos);
    if(gate != unit->gate){
        if(gate){
            Print("Starting mesh %f\n",unit->ampat);
            unit->inst->noteOn(10, unit->ampat);
        }else{
            Print("Stopping\n");
            unit->inst->noteOff(0);
        }
        unit->gate = gate;
    }

    for (int i=0; i < inNumSamples; ++i)
    {
        //out[i] = unit->inst->tick();
        out[i] = unit->inst->inputTick(inp[i]);
    }

}
///////////////////////////////////////
struct StkGlobals : public Unit
{
    char *rawwavePath;
    bool showWarnings;
    bool printErrors;
    int m_valueSize;

};
extern "C"
{
    void StkGlobals_next(StkGlobals *unit, int inNumSamples);
    void StkGlobals_Ctor(StkGlobals* unit);
}

void StkGlobals_next(StkGlobals *unit, int inNumSamples){
    NodeEnd(&unit->mParent->mNode);
}
void StkGlobals_Ctor(StkGlobals* unit) {
    gWorld = unit->mWorld;
    Stk::setSampleRate( SAMPLERATE );
    unit->showWarnings = IN0(0) > 0;
    unit->printErrors = IN0(1) > 0;

    const int kVarOffset = 2;
    unit->m_valueSize = unit->mNumInputs - kVarOffset;
    
    if(unit->m_valueSize > 0){
        const int valuesAllocSize = (unit->m_valueSize +1)* sizeof(char);
        char * chunk = (char*)RTAlloc(unit->mWorld, valuesAllocSize);
        if (!chunk) {
            Print("StkGlobals: RT memory allocation failed on values\n");
            SETCALC(Unit_next_nop);
            return;
        }
        unit->rawwavePath  = chunk;
        for(int i=0; i< unit->m_valueSize; i++)
            unit->rawwavePath[i] = IN0(i + kVarOffset);
        
        unit->rawwavePath[unit->m_valueSize] = 0; //null termination
        Stk::setRawwavePath(unit->rawwavePath);
    }
    Stk::showWarnings(unit->showWarnings);
    Stk::printErrors(unit->printErrors);
    RTFree(unit->mWorld,unit->rawwavePath);
    SETCALC(StkGlobals_next);
}
////////////////////////////////////////
PluginLoad(StkUnit){
    ft = inTable;
    DefineDtorUnit(StkInst);
    DefineDtorUnit(StkMesh2D);
    DefineSimpleUnit(StkGlobals);
}
