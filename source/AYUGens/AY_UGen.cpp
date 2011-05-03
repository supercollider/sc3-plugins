#include "SC_PlugIn.h"


#include "ayemu.h"



static InterfaceTable *ft;

struct AY : public Unit
{
	ayemu_ay_t * m_ay;
	unsigned char * m_audiobuf; // The integer audiobuf that the emulator will write to.
	size_t m_audiobufsize;
};

// declare unit generator functions
extern "C"
{
	void load(InterfaceTable *inTable);
	void AY_next(AY *unit, int inNumSamples);
	void AY_Ctor(AY* unit);
	void AY_Dtor(AY* unit);
};

//////////////////////////////////////////////////////////////////

void AY_Ctor(AY* unit)
{
	SETCALC(AY_next);

	// initialize the emulator etc.

	ayemu_chip_t chiptype = ((int) ZIN0(10)) == 1 ? AYEMU_YM : AYEMU_AY;

	// the emulator's output settings
	int freq = SAMPLERATE;
	int chans = 1;
	int bits = 8;//16;

	// The audiobufsize is the number of bytes to store the 16-bit mono integer output for one control block.
	int audiobufsize = BUFLENGTH * chans * (bits >> 3);
	unsigned char * audiobuf = (unsigned char *) RTAlloc(unit->mWorld, audiobufsize);

	ayemu_ay_t * ay = (ayemu_ay_t *) RTAlloc(unit->mWorld, sizeof(ayemu_ay_t));
	memset(ay, 0, sizeof(ayemu_ay_t));

	ayemu_init(ay);

	ayemu_set_chip_type(ay, chiptype, NULL);
	//ayemu_set_chip_freq(ay, freq);
	ayemu_set_sound_format(ay, freq, chans, bits);



	unit->m_audiobuf		= audiobuf;
	unit->m_ay				= ay;
	unit->m_audiobufsize	= audiobufsize;

	// calculate one sample of output.
	AY_next(unit, 1);
}

void AY_Dtor(AY* unit)
{
	RTFree(unit->mWorld, unit->m_ay);
	RTFree(unit->mWorld, unit->m_audiobuf);
}

void AY_next(AY *unit, int inNumSamples)
{
	unsigned char regs[14];

	float *out = OUT(0);

	// Retrieve state
	ayemu_ay_t * ay	= unit->m_ay;
	unsigned char * audiobuf	= unit->m_audiobuf;
	size_t audiobufsize	= unit->m_audiobufsize;

	// The chip's inputs.
	// The hard limits are applied here because the library wastes CPU
	//  by outputting lots of warning messages if we exceed the limits.
	int tonea		= sc_max(0, sc_min((int)ZIN0(0), 4095));
	int toneb		= sc_max(0, sc_min((int)ZIN0(1), 4095));
	int tonec		= sc_max(0, sc_min((int)ZIN0(2), 4095));
	int noise		= (int)ZIN0(3);
	int control		= (int)ZIN0(4);
	int vola		= sc_max(0, sc_min((int)ZIN0(5), 31));
	int volb		= sc_max(0, sc_min((int)ZIN0(6), 31));
	int volc		= sc_max(0, sc_min((int)ZIN0(7), 31));
	int envfreq		= sc_max(0, sc_min((int)ZIN0(8), 4095));
	int envstyle	= (int)ZIN0(9);

	// Translate the values into the raw registers
	regs[0]  = (unsigned char)(tonea & 0xff);
	regs[1]  = (unsigned char)(tonea >> 8);
	regs[2]  = (unsigned char)(toneb & 0xff);
	regs[3]  = (unsigned char)(toneb >> 8);
	regs[4]  = (unsigned char)(tonec & 0xff);
	regs[5]  = (unsigned char)(tonec >> 8);
	regs[6]  = (unsigned char)(noise);
	regs[7]  = (unsigned char)((~control) & 0x3f); 	/* invert bits 0-5 */
	regs[8]  = (unsigned char)(vola); 		/* included bit 4 */
	regs[9]  = (unsigned char)(volb);
	regs[10] = (unsigned char)(volc);
	regs[11] = (unsigned char)(envfreq & 0xff);
	regs[12] = (unsigned char)(envfreq >> 8);
	regs[13] = (unsigned char)(envstyle);

//	Print("vola: %d\n", regs[8]);
//	Print("volb: %d\n", regs[9]);
//	Print("volc: %d\n", regs[10]);

	// Now let's pump that chip
	ayemu_set_regs(ay, regs);
	ayemu_gen_sound(ay, audiobuf, audiobufsize);

	//Print("AY:::    ay->Amp_Global is %d\n", ay->Amp_Global);


	// Convert the output from 16-bit to float, and write to output
	int bufreadpos=0;
	for (int i=0; i < BUFLENGTH; ++i)
	{
//		out[i] = ((audiobuf[bufreadpos++] & 0x00FF) | (audiobuf[bufreadpos++] << 8)) * 3.0517578125e-05f;
//		Print(" {%u}", audiobuf[bufreadpos]);
		out[i] = (audiobuf[bufreadpos++] - 128) * 0.0078125f; // 1/128
	}

}

////////////////////////////////////////////////////////////////////

PluginLoad(AY)
{
	ft = inTable;

	DefineDtorUnit(AY);
}
