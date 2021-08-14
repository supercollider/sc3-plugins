#include "SC_PlugIn.h"

static InterfaceTable *ft;

struct TrigPhasor : public Unit {
    float prevInTrig;
    float level;
};

static void TrigPhasor_Ctor(TrigPhasor* unit);
static void TrigPhasor_next_a(TrigPhasor* unit, int inNumSamples);
static void TrigPhasor_next_k(TrigPhasor* unit, int inNumSamples);

void TrigPhasor_Ctor(TrigPhasor* unit) {
    unit->prevInTrig = 0;
    unit->level = 0;

    if (INRATE(0) == calc_FullRate) {
        SETCALC(TrigPhasor_next_a);
    } else {
        SETCALC(TrigPhasor_next_k);
    }
    TrigPhasor_next_k(unit, 1);
}

void TrigPhasor_next_a(TrigPhasor* unit, int inNumSamples) {
    float *inTrig   = IN(0);
    float resetPos  = IN0(1);
    float start     = IN0(2);
    float end       = IN0(3);
    float step      = IN0(4);

    float *out      = OUT(0);
    float *outTrig  = OUT(1);

    for (int i = 0; i < inNumSamples; i++) {
        float slope = end - start;

        if (unit->prevInTrig <= 0 && inTrig[i] > 0) {
            unit->level = resetPos;
        } else {
            unit->level += copysignf(step, slope);
        }

        if (slope <= 0) {
            if (unit->level < end) {
                outTrig[i] = 1;
                unit->level = start;
            } else {
                outTrig[i] = 0;
            }
        } else {
            if (unit->level > end) {
                outTrig[i] = 1;
                unit->level = start;
            } else {
                outTrig[i] = 0;
            }
        }

        out[i] = unit->level;
        unit->prevInTrig = inTrig[i];
    }
}

void TrigPhasor_next_k(TrigPhasor* unit, int inNumSamples) {
    float inTrig    = IN0(0);
    float resetPos  = IN0(1);
    float start     = IN0(2);
    float end       = IN0(3);
    float step      = IN0(4);

    float *out      = OUT(0);
    float *outTrig  = OUT(1);

    for (int i = 0; i < inNumSamples; i++) {
        float slope = end - start;

        if (unit->prevInTrig <= 0 && inTrig > 0) {
            unit->level = resetPos;
        } else {
            unit->level += copysignf(step, slope);
        }

        if (slope <= 0) {
            if (unit->level < end) {
                outTrig[i] = 1;
                unit->level = start;
            } else {
                outTrig[i] = 0;
            }
        } else {
            if (unit->level > end) {
                outTrig[i] = 1;
                unit->level = start;
            } else {
                outTrig[i] = 0;
            }
        }

        out[i] = unit->level;
        unit->prevInTrig = inTrig;
    }
}

PluginLoad(TrigPhasorUGens) {
    ft = inTable;
    DefineSimpleUnit(TrigPhasor);
}
