
// supercollider 3 ugens
// blackrain at realizedsound dot net - 11 2006

#include "SC_PlugIn.h"

static InterfaceTable *ft;

static float gaintable[199] = {
0.999969, 0.990082, 0.980347, 0.970764, 0.961304, 0.951996, 0.94281, 0.933777,  0.924866, 0.916077,
0.90741,  0.898865, 0.890442, 0.882141, 0.873962, 0.865906, 0.857941, 0.850067, 0.842346, 0.834686,
0.827148, 0.819733, 0.812378, 0.805145, 0.798004, 0.790955, 0.783997, 0.77713,  0.770355, 0.763672,
0.75708 , 0.75058,  0.744141, 0.737793, 0.731537, 0.725342, 0.719238, 0.713196, 0.707245, 0.701355,
0.695557, 0.689819, 0.684174, 0.678558, 0.673035, 0.667572, 0.66217,  0.65686,  0.651581, 0.646393,
0.641235, 0.636169, 0.631134, 0.62619,  0.621277, 0.616425, 0.611633, 0.606903, 0.602234, 0.597626,
0.593048, 0.588531, 0.584045, 0.579651, 0.575287, 0.570953, 0.566681, 0.562469, 0.558289, 0.554169,
0.550079, 0.546051, 0.542053, 0.538116, 0.53421,  0.530334, 0.52652,  0.522736, 0.518982, 0.515289,
0.511627, 0.507996, 0.504425, 0.500885, 0.497375, 0.493896, 0.490448, 0.487061, 0.483704, 0.480377,
0.477081, 0.473816, 0.470581, 0.467377, 0.464203, 0.46109,  0.457977, 0.454926, 0.451874, 0.448883,
0.445892, 0.442932, 0.440033, 0.437134, 0.434265, 0.431427, 0.428619, 0.425842, 0.423096, 0.42038,
0.417664, 0.415009, 0.412354, 0.409729, 0.407135, 0.404572, 0.402008, 0.399506, 0.397003, 0.394501,
0.392059, 0.389618, 0.387207, 0.384827, 0.382477, 0.380127, 0.377808, 0.375488, 0.37323,  0.370972,
0.368713, 0.366516, 0.364319, 0.362122, 0.359985, 0.357849, 0.355713, 0.353607, 0.351532, 0.349457,
0.347412, 0.345398, 0.343384, 0.34137,  0.339417, 0.337463, 0.33551,  0.333588, 0.331665, 0.329773,
0.327911, 0.32605,  0.324188, 0.322357, 0.320557, 0.318756, 0.316986, 0.315216, 0.313446, 0.311707,
0.309998, 0.308289, 0.30658,  0.304901, 0.303223, 0.301575, 0.299927, 0.298309, 0.296692, 0.295074,
0.293488, 0.291931, 0.290375, 0.288818, 0.287262, 0.285736, 0.284241, 0.282715, 0.28125,  0.279755,
0.27829,  0.276825, 0.275391, 0.273956, 0.272552, 0.271118, 0.269745, 0.268341, 0.266968, 0.265594,
0.264252, 0.262909, 0.261566, 0.260223, 0.258911, 0.257599, 0.256317, 0.255035, 0.25375 };

/*
 *	SVF - a 12dB/Oct state variable filter
 *	SC3 port from musicdsp dot org - blackrain.
 */
struct SVF : public Unit
{
    float m_cutoff;		// Normalised frequency
    float m_resonance;		// Resonance (0 - 1.f)
    float m_freq;		// Frequency coefficient
    float m_damp;		// Damping factor
    float m_notch;		// Notch output
    float m_low;		// Low output
    float m_high;		// High output
    float m_band;		// Band output
};


struct AmplitudeMod : public Unit
{
	float m_previn, m_clampcoef, m_relaxcoef, m_clamp, m_relax;;
};

// declare struct to hold unit generator state
struct BMoog : public Unit
{
	float cutoff, res;
	float p, q, state[4], output;
};

typedef struct {
    int length;			// size of filter
    float coef[16];		// pointer to coefficients of filter
    float history[8];		// pointer to history in filter
} FILTER;

#define FILTER_SECTIONS   2	// 2 filter sections for 24 db/oct filter

typedef struct {
        float a0, a1, a2;	// numerator coefficients
        float b0, b1, b2;	// denominator coefficients
} BIQUAD;

BIQUAD ProtoCoef[FILTER_SECTIONS];	// Filter prototype coefficients,
					// 1 for each filter section
struct IIRFilter : public Unit
{
	float fc;
	float rq;
	FILTER iir;
};


extern "C"
{
    void load(InterfaceTable *inTable);
    void SVF_Ctor(SVF *unit);
    void SVF_next(SVF *unit, int inNumSamples);

    void AmplitudeMod_next(AmplitudeMod *unit, int inNumSamples);
    void AmplitudeMod_Ctor(AmplitudeMod* unit);

    void BMoog_Ctor(BMoog *unit);
    void BMoog_next(BMoog *unit, int inNumSamples);

    void IIRFilter_Ctor(IIRFilter* unit);
    void IIRFilter_next_a(IIRFilter *unit, int inNumSamples);
};


void SVF_Ctor(SVF *unit)
{
    SETCALC(SVF_next);
    unit->m_cutoff = 0.f;
    unit->m_resonance = 0.f;
    unit->m_freq  = 0.f;
    unit->m_damp  = 0.f;
    unit->m_notch = 0.f;
    unit->m_low   = 0.f;
    unit->m_high  = 0.f;
    unit->m_band  = 0.f;
    SVF_next(unit, 1);
}

void SVF_next(SVF *unit, int inNumSamples)
{
    float *out = OUT(0);
    float *in  = IN(0);
    float cutoff     = IN0(1);
    float resonance  = IN0(2);
    float lowlevel   = IN0(3);
    float bandlevel  = IN0(4);
    float highlevel  = IN0(5);
    float notchlevel = IN0(6);
    float peaklevel  = IN0(7);
//    float drive      = IN0(8);

    cutoff	= sc_clip(cutoff, 20.f, SAMPLERATE);
    resonance	= sc_clip(resonance, 0.f, 1.f);
    lowlevel	= sc_clip(lowlevel, 0.f, 1.f);
    bandlevel	= sc_clip(bandlevel, 0.f, 1.f);
    highlevel	= sc_clip(highlevel, 0.f, 1.f);
    notchlevel	= sc_clip(notchlevel, 0.f, 1.f);
    peaklevel 	= sc_clip(peaklevel, 0.f, 1.f);
//    drive	= sc_clip(drive, 0.f, 1.f) * 0.1;

    float m_freq, m_damp;

    if (cutoff != unit->m_cutoff || resonance != unit->m_resonance) {
	unit->m_cutoff = cutoff;
	unit->m_resonance = resonance;
	unit->m_freq = m_freq = (float)(2.f * sin(pi * sc_min(0.25, cutoff / (SAMPLERATE * 2.f))));
	unit->m_damp = m_damp = sc_min(2.0 * (1.0 - pow(resonance, 0.25f)), (double)sc_min(2.f, 2.f / m_freq - m_freq * 0.5f));
    } else {
	m_freq = unit->m_freq;
	m_damp = unit->m_damp;
    }

    float input;
    float low;
    float band;
    float high;
    float notch;

    float outPeak;

    float m_notch = unit->m_notch;
    float m_low   = unit->m_low;
    float m_high  = unit->m_high;
    float m_band  = unit->m_band;

    for (long i = 0; i < inNumSamples; ++i)
    {
	    input = in[i];

	    // First iteration
	    m_notch	= input - m_damp * m_band;
	    m_low	= m_low + m_freq * m_band;
	    m_high	= m_notch - m_low;
	    m_band	= m_freq * m_high + m_band;

	    low   = 0.5f * m_low;
	    band  = 0.5f * m_band;
	    high  = 0.5f * m_high;
	    notch = 0.5f * m_notch;
	    outPeak = 0.5f * (m_low - m_high);

	    // Double sampled...
	    m_notch 	= input - m_damp * m_band;
	    m_low 	= m_low + m_freq * m_band;
	    m_high 	= m_notch - m_low;
	    m_band 	= m_freq * m_high + m_band;

	    low   += 0.5f * m_low;
	    band  += 0.5f * m_band;
	    high  += 0.5f * m_high;
	    notch += 0.5f * m_notch;
	    outPeak += 0.5f * (m_low - m_high);

	    out[i] = (low * lowlevel) + (band * bandlevel) + (high * highlevel) + (notch * notchlevel) + (outPeak * peaklevel);
    }

    unit->m_notch = m_notch;
    unit->m_low   = m_low;
    unit->m_high  = m_high;
    unit->m_band  = m_band;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void AmplitudeMod_Ctor(AmplitudeMod* unit)
{
	//printf("AmplitudeMod_Reset\n");
	SETCALC(AmplitudeMod_next);

	float clamp = ZIN0(1);
	unit->m_clampcoef  = clamp == 0.0 ? 0.0 : exp(log1/(clamp * SAMPLERATE));

	float relax = ZIN0(2);
	unit->m_relaxcoef = relax == 0.0 ? 0.0 : exp(log1/(relax * SAMPLERATE));

	ZOUT0(0) = unit->m_previn = ZIN0(0);
}

void AmplitudeMod_next(AmplitudeMod* unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float clamp = ZIN0(1);
	float relax = ZIN0(2);

	if (unit->m_clamp != clamp) {
		unit->m_clamp = clamp;
		unit->m_clampcoef = clamp == 0.0 ? 0.0 : exp(log1/(clamp * SAMPLERATE));
	}

	if (unit->m_relax != relax) {
		unit->m_relax = relax;
		unit->m_relaxcoef = relax == 0.0 ? 0.0 : exp(log1/(relax * SAMPLERATE));
	}

	float relaxcoef = unit->m_relaxcoef;
	float clampcoef = unit->m_clampcoef;
	float previn = unit->m_previn;

	float val;
	LOOP(inNumSamples,
		val = fabs(ZXP(in));
		if (val < previn) {
			val = val + (previn - val) * relaxcoef;
		} else {
			val = val + (previn - val) * clampcoef;
		}
		ZXP(out) = previn = val;
	);

	unit->m_previn = previn;
}

static inline float saturate(float input) {
#define _limit 0.95
  float x1 = fabsf( input + _limit );
  float x2 = fabsf( input - _limit );
  return 0.5 * (x1 - x2);
}

static inline float crossfade( float amount, float a, float b ) {
  return (1. - amount) * a + amount * b;
}

void BMoog_Ctor(BMoog* unit)
{
	SETCALC(BMoog_next);
	unit->cutoff = 0.f;
	unit->res = 0.f;
	unit->p = 0.f;
	unit->q = 0.f;
	unit->output = 0.f;

	for(int i = 0; i < 4; i++) {
	    unit->state[i] = 0.f;
	}

	ClearUnitOutputs(unit, 1);
}

void BMoog_next(BMoog *unit, int inNumSamples)
{
	float *out   = OUT(0);
	float *in    =  IN(0);
	float cutoff = IN0(1);
	float res    = IN0(2);
	float mode   = IN0(3);
//	float sat    = IN0(4);
	float p = unit->p;
	float q = unit->q;
	float output = unit->output;
	float *state = unit->state;

	for (int i=0; i < inNumSamples; ++i ) {
	    if ( cutoff != unit->cutoff || res != unit->res ) {
		// pole coefficient based on frequency
		float fc = 2 * cutoff * SAMPLEDUR;
		float fc2 = fc * fc;
		float fc3 = fc * fc2;

		// cubic fit by DFL, not 100% accurate but better than nothing...
		p = -0.69346 * fc3 - 0.59515 * fc2 + 3.2937 * fc - 1.0072;

		// setting q
		float ix = p * 99.;
		int ixint = (int)floorf(ix);
		float ixfrac = ix - ixint;
		float q = res * crossfade( ixfrac, gaintable[ ixint + 99 ], gaintable[ ixint + 100 ] );
		unit->cutoff = cutoff;
		unit->res = res;
		unit->p = p;
		unit->q = q;
	    }

	    float temp, input;

	    input = in[i];
	    output = 0.25 * ( input - output ); // negative feedback

	    for(int pole = 0; pole < 4; pole++) {
                  temp = state[pole];
                  output = saturate(output + p * (output - temp));
                  state[pole] = output;
                  output = saturate(output + temp);
	    }

	    if ( mode < 1.f )		// lowpass
		    out[i] = output;
	    else if ( mode < 2.f )	// highpass
		    out[i] = input - output;
	    else if ( mode < 3.f )	// bandpass
		    out[i] = 3.f * state[2] - output;
	    else
		out[i] = output;	// default to lowpass

	    output *= q;  //scale the feedback
	}
	unit->output = output;
}


void IIRFilter_Ctor(IIRFilter* unit)
{
    SETCALC(IIRFilter_next_a);

    // Setup filter s-domain coefficients
    ProtoCoef[0].a0 = 1.0;	// Section 1
    ProtoCoef[0].a1 = 0.;
    ProtoCoef[0].a2 = 0.;
    ProtoCoef[0].b0 = 1.0;
    ProtoCoef[0].b1 = 0.765367;
    ProtoCoef[0].b2 = 1.0;

    ProtoCoef[1].a0 = 1.0;	// Section 2
    ProtoCoef[1].a1 = 0.;
    ProtoCoef[1].a2 = 0.;
    ProtoCoef[1].b0 = 1.0;
    ProtoCoef[1].b1 = 1.847759;
    ProtoCoef[1].b2 = 1.0;

    unit->iir.length = FILTER_SECTIONS;	// Number of filter sections

    for (int i = 0; i < unit->iir.length * 2; i++) { // clear filter history
	unit->iir.history[i] = 0.f;
    }

    unit->fc = 0.f;
    unit->rq = 0.f;

    IIRFilter_next_a(unit, 1);
}

void IIRFilter_next_a(IIRFilter *unit, int inNumSamples)
{
    float *out = OUT(0); // get the pointer to the output buffer
    float *in = IN(0);	// get the pointer to the input buffer
    float  fc = *IN(1);
    float  rq = *IN(2);

    for (int n = 0; n < inNumSamples; ++n)
    {
	if (fc != unit->fc || rq != unit->rq) {
	    rq = sqrt(rq); // compensate q value for 2 sections
	    unit->fc = fc; // save new params
	    unit->rq = rq;

	    // precalc
	    float fs2 = SAMPLERATE * SAMPLERATE;
	    float wp = 2.0 * SAMPLERATE * tan(pi * fc * SAMPLEDUR);
	    float wp2 = wp * wp;
	    float k = 1.0;			// Set overall filter gain
	    float *coef;
	    coef = unit->iir.coef + 1;	// Skip k, or gain

	    // Compute z-domain coefficients for each biquad section
	    // for new Cutoff Frequency and Resonance
	    float a0, a1, a2, b0, b1, b2;
	    for (int i = 0; i < unit->iir.length; i++)
	    {
		a0 = ProtoCoef[i].a0;
		a1 = ProtoCoef[i].a1;
		a2 = ProtoCoef[i].a2;

		b0 = ProtoCoef[i].b0;
		b1 = ProtoCoef[i].b1 * rq; // res
		b2 = ProtoCoef[i].b2;

		// prewarp
		a1 = a1 / wp; a2 = a2 / wp2;
		b1 = b1 / wp; b2 = b2 / wp2;

		// bilinear
		float ad, bd;
			    // alpha (Numerator in s-domain)
		ad = 4. * a2 * fs2 + 2. * a1 * SAMPLERATE + a0;
			    // beta (Denominator in s-domain)
		bd = 4. * b2 * fs2 + 2. * b1 * SAMPLERATE + b0;

			    // update gain constant for this section
		k *= ad/bd;

			    // Denominator
		*coef++ = (2. * b0 - 8. * b2 * fs2)
				    / bd; // beta1
		*coef++ = (4. * b2 * fs2 - 2. * b1 * SAMPLERATE + b0)
				    / bd; // beta2

			    // Nominator
		*coef++ = (2. * a0 - 8. * a2 * fs2)
				    / ad; // alpha1
		*coef++ = (4. * a2 * fs2 - 2. * a1 * SAMPLERATE + a0)
				    / ad; // alpha2
	    }

	    // Update overall filter gain in coef array
	    unit->iir.coef[0] = k;
	}

	float *hist1_ptr, *hist2_ptr, *coef_ptr;
	float output, new_hist, history1, history2;

	coef_ptr = unit->iir.coef;	// coefficient pointer
	hist1_ptr = unit->iir.history;	// first history
	hist2_ptr = hist1_ptr + 1;	// next history

	// 1st number of coefficients array is overall input scale factor,
	// or filter gain
	output = in[n] * (*coef_ptr++);

	for (int i = 0 ; i < unit->iir.length; ++i)
	{
	    history1 = *hist1_ptr;	// history values
	    history2 = *hist2_ptr;

	    output = output - history1 * (*coef_ptr++);
	    new_hist = output - history2 * (*coef_ptr++); // poles

	    output = new_hist + history1 * (*coef_ptr++);
	    output = output + history2 * (*coef_ptr++); // zeros

	    *hist2_ptr++ = *hist1_ptr;
	    *hist1_ptr++ = new_hist;
	    hist1_ptr++;
	    hist2_ptr++;
	}
	out[n] = output;
    }
}



PluginLoad(Blackrain)
{
	ft = inTable;
	DefineSimpleUnit(SVF);
	DefineSimpleUnit(AmplitudeMod);
	DefineSimpleUnit(BMoog);
	DefineSimpleUnit(IIRFilter);

}

// EOF
