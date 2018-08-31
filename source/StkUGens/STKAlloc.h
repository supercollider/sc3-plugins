
#ifndef _STKAlloc_H_
#define _STKAlloc_H_

/*
    C.A. Hresko 2004
    in Ctor.
	STKAlloc(unit->mWorld, unit->stkPool, unit->instrument, Instrument, 80);

    simple macro which allocates memory for use with STK instruments
    to delete allocated memory, you must:
    
    
    unit->instrument->~Instrument();
    RTFree(unit->mWorld, unit->instrument);
    
    inside the Unit Generator Dtor function
*/

#define STKAlloc(_World_, _STKPtr_, _STKInstrument_, _Arg_)             \
        void * _STKMem_ = RTAlloc(_World_, sizeof(_STKInstrument_));    \
        _STKPtr_ = new(_STKMem_) _STKInstrument_(_Arg_);

#define STKAlloc0(_World_, _STKPtr_, _STKInstrument_)             \
        void * _STKMem_ = RTAlloc(_World_, sizeof(_STKInstrument_));    \
        _STKPtr_ = new(_STKMem_) _STKInstrument_();

#endif

