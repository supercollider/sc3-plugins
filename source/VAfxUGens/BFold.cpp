#include "SC_PlugIn.hpp"
#include <math.h>

// InterfaceTable contains pointers to functions in the host (server).
static InterfaceTable *ft;

// declare struct to hold unit generator state
struct BFold : public SCUnit{

// Constructor usually does 3 things.
// 1. set the calculation function.
// 2. initialize the unit generator state variables.
// 3. calculate one sample of output.
public:
    BFold() {
        // 1. set the calculation function.
        set_calc_function<BFold,&BFold::next>();

        // 3. calculate one sample of output.
        next(1);
     
    }

private:
    // The calculation function executes once per control period
    // which is typically 64 samples.

    // calculation function for an audio rate frequency argument
    void next(int inNumSamples)
    {
        // get the pointer to the output buffer
        float *outBuf = out(0);

        // get the pointer to the input buffer
        const float *sig = in(0);
        const float *fold = in(1);
        const float offset = in0(2);

        // perform a loop for the number of samples in the control period.
        // If this unit is audio rate then inNumSamples will be 64 or whatever
        // the block size is. If this unit is control rate then inNumSamples will
        // be 1.
        for (int i=0; i < inNumSamples; ++i)
        {
            float sgn;
            float sigNorm;
            float sigAbs;
            float fold1 = 0.f;
            float fold2 = 0.f;
            float fold3 = 0.f;
            float fold4 = 0.f;
            float fold5 = 0.f;

            sigNorm = offset*0.6 + (fold[i]*5.4 +0.6)*sig[i];
            // sigNorm = 0.5 + 2.f*sig[i]*0.6;
            sigAbs = fabs(sigNorm);
            sgn = copysignf(1.0, sig[i]);

            if (sigAbs >= 0.6) {
               fold1 = 0.8333*sigNorm - 0.5*sgn;
               fold1 *= -12.f;
                if (sigAbs >= 1.8) {
                   fold2 = 0.5743*sigNorm - 1.0338*sgn;
                   fold2 *= 17.647;
                    if (sigAbs >= 2.994) {
                       fold3 = 0.3768*sigNorm - 1.1281*sgn;
                       fold3 *= -27.777;
                        if (sigAbs >= 4.08) {
                           fold4 = 0.2673*sigNorm - 1.0907*sgn;
                           fold4 *= 36.363;
                            if (sigAbs >= 5.46) {
                               fold5 = 0.2829*sigNorm - 1.5446*sgn;
                               fold5 *= -21.438;
                            }
                        }
                    }
                }
            }
            // out must be written last for in place operation
            outBuf[i] = (fold1 + fold2 + fold3 + fold4 + fold5 + 5.f * sigNorm)/3.f;
        }
    }

};

// the entry point is called by the host when the plug-in is loaded
PluginLoad(TimbreUGens)
{
    // InterfaceTable *inTable implicitly given as argument to the load function
    ft = inTable; // store pointer to InterfaceTable

    // registerUnit takes the place of the Define*Unit functions. It automatically checks for the presence of a
    // destructor function.
    // However, it does not seem to be possible to disable buffer aliasing with the C++ header.
    registerUnit<BFold>(ft, "BFold");
}
