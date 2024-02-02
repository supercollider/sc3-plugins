// PluginLFSRNoise.cpp
// josiah sytsma (sytsma dot music at icloud dot com)

#include "SC_PlugIn.h"

static InterfaceTable *ft;

struct LFSRNoise : public Unit {
    uint32_t    mState; //lfsr state
    uint32_t    mCounter; //sample counter
    uint32_t    mSeed; //seed 1 or UINT32_MAX
    float       mPrevTrig;
    float       mLevel; //output
    bool        triggered(float inTrig);
};

void LFSRNoise_Ctor(LFSRNoise *unit);
void LFSRNoise_next(LFSRNoise *unit, int inNumSamples);

void LFSRNoise_Ctor(LFSRNoise *unit) {
    //set calc function
    SETCALC(LFSRNoise_next);
    
    //initialize member variables
    unit->mSeed = UINT32_MAX;
    unit->mState = UINT32_MAX;
    unit->mCounter = 0;
    unit->mLevel = 0.f;
    
    //calculate one sample
    LFSRNoise_next(unit, 1);
}

inline bool LFSRNoise::triggered(float trigIn) {
    return trigIn > 0.f && mPrevTrig <= 0.f;
}

void LFSRNoise_next(LFSRNoise *unit, int inNumSamples) {
    float* out = ZOUT(0);
    float freq = IN0(0);
    int fbPos = sc_clip(static_cast<int>(IN0(1)), 1, 31);
    float curTrig = IN0(2);
    float seedType = IN0(3);
    
    uint32 seed = unit->mSeed;
    uint32 state = unit->mState;
    uint32 counter = unit->mCounter;
    float level = unit->mLevel;
    
    int remain = inNumSamples;
    do {
        if (counter <= 0) {
            counter = static_cast<int>(SAMPLERATE / sc_max(freq, .001f));
            counter = sc_max(1, counter);
            
            //check seed type
            if (seedType < 0) {
                seed = UINT32_MAX;  // All 1's
            } else if (seedType == 0) {
                seed = ~(~0u << fbPos);  // Up to fbIndex 1's
            } else {
                seed = 1;  // 1
            }
            
            //reset
            if (state == 0 || unit->triggered(curTrig)) {
                state = seed;
            }
            unit->mPrevTrig = curTrig;
            
            //step
            int newbit = state & 1;
            state >>= 1;
            newbit ^= state & 1;
            
            int mask = (1 << fbPos);
            state = ((state & ~mask) | (newbit << fbPos));
            
            //level value
            level = (state & 1) ? 1.0f : -1.0f;
        }
        int nsmps = sc_min(remain, counter);
        remain -= nsmps;
        counter -= nsmps;
        for (int i = 0; i < nsmps; i++) {
            ZXP(out) = level;
        }
    } while (remain);
    
    //update member variables
    unit->mSeed = seed;
    unit->mState = state;
    unit->mLevel = level;
    unit->mCounter = counter;
}


PluginLoad(InterfaceTable *inTable) {
    ft = inTable;
    DefineSimpleUnit(LFSRNoise);
}
