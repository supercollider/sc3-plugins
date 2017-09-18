#include "SC_PlugIn.h"


// InterfaceTable contains pointers to functions in the host (server).
static InterfaceTable *ft;

// declare struct to hold unit generator state
struct PermModT : public Unit
{
    int buffl, chunkl, numchunks, tail, read, jump;
    bool swapped;
    float *swapbuf1, *swapbuf2;
};

struct PermMod : public Unit
{
    int  chunkl, read, index;
    bool swapped;
    float *swapbuf1, *swapbuf2;
};

struct PermModArray : public Unit
{
    int chunkl, numchunks, index, read;
    int *pattern, *array;
    bool swapped;
    float *swapbuf1, *swapbuf2;
};

void PermModT_Ctor(PermModT* unit);
void PermModT_Dtor(PermModT* unit);
void PermModT_next_even(PermModT *unit, int inNumSamples);
void PermModT_next_odd(PermModT *unit, int inNumSamples);
void PermModT_next_dono(PermModT *unit, int inNumSamples);

void PermMod_Ctor(PermMod* unit);
void PermMod_Dtor(PermMod* unit);
void PermMod_next(PermMod *unit, int inNumSamples);
void PermMod_next_dono(PermMod *unit, int inNumSamples);

void PermModArray_Ctor(PermModArray* unit);
void PermModArray_Dtor(PermModArray* unit);
void PermModArray_next(PermModArray *unit, int inNumSamples);
void PermModArray_next_dono(PermModArray *unit, int inNumSamples);


void PermModT_Ctor(PermModT* unit)
{
    float rate = IN0(1); // the frequency at wich the tail occurs
    float freq = IN0(2);
    int buffl, chunkl;
    bool errors=false;

    // input check
    if (rate>0 && freq>0 && freq<=SAMPLERATE*2 && rate<=freq){
        buffl=round(SAMPLERATE/rate);
        chunkl=round(SAMPLERATE/freq);
    }
    else{
        buffl=4;
        chunkl=1; // those values will never be used
        errors=true;
    }
    int numchunks = buffl/chunkl;

    unit->buffl = buffl;
    unit->chunkl = chunkl;
    unit->numchunks = numchunks;
    unit->tail = buffl%chunkl;
    unit->jump = chunkl;
    unit->read = 0;
    unit->swapped=false;

     //allocate buffers
    unit->swapbuf1 = (float*)RTAlloc(unit->mWorld, buffl * sizeof(float));
    unit->swapbuf2 = (float*)RTAlloc(unit->mWorld, buffl * sizeof(float));
    memset(unit->swapbuf1, 0, buffl * sizeof(float));
    memset(unit->swapbuf2, 0, buffl * sizeof(float));

    if(errors==false){
        if(numchunks%2==0){
            SETCALC(PermModT_next_even);
            PermModT_next_even(unit,1); // leaves the tail at the end
        }
        else{
            SETCALC(PermModT_next_odd);
            PermModT_next_odd(unit,1); // permutes the tail with the last chunk
        }
    }
    else{
        SETCALC(PermModT_next_dono);
        PermModT_next_dono(unit,1); // this calc function does nothing and then terminates the Ugen
    }

}

void PermModT_next_even( PermModT *unit, int inNumSamples)
{
    float *in=IN(0);
    float *out = OUT(0);

    int buffl=unit->buffl;
    int chunkl=unit->chunkl;
    int newchunkl=chunkl;
    int newbuffl=buffl;
    int numchunks=unit->numchunks;
    int read=unit->read;
    int jump=unit->jump;
    //int tail=unit->tail;
    bool swapped=unit->swapped;

    float *swapbuf1 = unit->swapbuf1;
    float *swapbuf2 = unit->swapbuf2;

    for (int i=0; i<inNumSamples; i++){


            //jump pattern
            if(read%chunkl==0 && read!=0){
                jump=jump*(-1);
            }

            //read from 2nd buffer, write on 1st
            if(swapped==false){
                //before the tail
                if(read<chunkl*numchunks){
                    swapbuf1[read+jump]=in[i];
                }
                //tail
                else{
                    swapbuf1[read]=in[i];
                }

                out[i]=swapbuf2[read];
            }
            //read from 1st buffer, write on 2nd
            else{
                //before the tail
                if(read<chunkl*numchunks){
                    swapbuf2[read+jump]=in[i];
                }
                //tail
                else{
                    swapbuf2[read]=in[i];
                }

                out[i]=swapbuf1[read];
            }

            read++;

            if(read==buffl){
                //reset read, swap buffers, reset pattern
                read=0;
                swapped=!swapped;
                jump=chunkl;
            }
        }

        //update

    if(IN0(1)>0 && IN0(2)>0 && IN0(2)<=SAMPLERATE*2 && IN0(1)<=IN0(2)){
        newchunkl=round(SAMPLERATE/IN0(2));
        newbuffl=round(SAMPLERATE/IN0(1));
    }
    else{
        Print ("%s \n", "Infreq and outfreq parameters must be >0 and <2*SampleRate, using last legal values");
    }

    if(newchunkl!=chunkl || newbuffl!=buffl){
        unit->chunkl=newchunkl;
        unit->numchunks=newbuffl/newchunkl;
        unit->tail=newbuffl%newchunkl;

        read=0;
        swapped=!swapped;
        jump=newchunkl;

        // following commented code should avoid resetting read when buffer size doesn't change, but doesn't work.
        /*int donechunks = read/newchunkl;

        if(donechunks%2==0){
             jump=newchunkl;
        }
        else{
             jump=newchunkl*(-1);
        }*/

        if (newbuffl!=buffl){
            unit->buffl=newbuffl;
            RTFree(unit->mWorld, unit->swapbuf1);
            RTFree(unit->mWorld, unit->swapbuf2);

            unit->swapbuf1 = (float*)RTAlloc(unit->mWorld, newbuffl * sizeof(float));
            unit->swapbuf2 = (float*)RTAlloc(unit->mWorld, newbuffl * sizeof(float));
            memset(unit->swapbuf1, 0, newbuffl * sizeof(float));
            memset(unit->swapbuf2, 0, newbuffl * sizeof(float));
        }

        if(unit->numchunks%2!=0){
            SETCALC(PermModT_next_odd);
            //break;
        }
     }
     unit->jump=jump;
     unit->read=read;
     unit->swapped=swapped;
}

void PermModT_next_odd( PermModT *unit, int inNumSamples)
{

    float *in= IN(0);
    float *out = OUT(0);

    int buffl=unit->buffl;
    int chunkl=unit->chunkl;
    int newchunkl=chunkl;
    int newbuffl=buffl;
    int numchunks=unit->numchunks;
    int read=unit->read;
    int jump=unit->jump;
    int tail=unit->tail;
    bool swapped=unit->swapped;

    float *swapbuf1 = unit->swapbuf1;
    float *swapbuf2 = unit->swapbuf2;

    for (int i=0; i<inNumSamples; i++){


        //jump pattern
        if(read%chunkl==0 && read!=0){
            jump=jump*(-1);
        }

        if(swapped==false){
            //before the last chunk or during tail
            if(read<chunkl*(numchunks-1) || read>=chunkl*numchunks){
                swapbuf1[read+jump]=in[i];
            }
            //last chunk
            else if (read>=chunkl*(numchunks-1) && read<chunkl*numchunks){
                swapbuf1[read+tail]=in[i];
            }

            out[i]=swapbuf2[read];
        }
        else{
            //before the last chunk or during tail
            if(read<chunkl*(numchunks-1) || read>=chunkl*numchunks){
                swapbuf2[read+jump]=in[i];
            }
            //last chunk
            else if (read>=chunkl*(numchunks-1) && read<chunkl*numchunks){
                swapbuf2[read+tail]=in[i];
                }

            out[i]=swapbuf1[read];
        }


        read++;

        if(read==buffl){
            //reset read, swap buffers, reset pattern
            read=0;
            swapped=!swapped;
            jump=chunkl;
        }

    }
    //update

    if(IN0(1)>0 && IN0(2)>0 && IN0(2)<=SAMPLERATE*2 && IN0(1)<=IN0(2)){
        newchunkl=round(SAMPLERATE/IN0(2));
        newbuffl=round(SAMPLERATE/IN0(1));
    }
    else{
        Print ("%s \n", "Infreq and outfreq parameters must be >0 and <2*SampleRate, using last legal values");
    }

    if(newchunkl!=chunkl || newbuffl!=buffl){
        unit->chunkl=newchunkl;
        unit->numchunks=newbuffl/newchunkl;
        unit->tail=newbuffl%newchunkl;

        read=0;
        swapped=!swapped;
        jump=newchunkl;
        /*int donechunks = read/newchunkl;

        if(donechunks%2==0){
                 jump=newchunkl;
        }
        else{
            jump=newchunkl*(-1);
        }*/

        if (newbuffl!=buffl){
            unit->buffl=newbuffl;
            RTFree(unit->mWorld, unit->swapbuf1);
            RTFree(unit->mWorld, unit->swapbuf2);

            unit->swapbuf1 = (float*)RTAlloc(unit->mWorld, newbuffl * sizeof(float));
            unit->swapbuf2 = (float*)RTAlloc(unit->mWorld, newbuffl * sizeof(float));
            memset(unit->swapbuf1, 0, newbuffl * sizeof(float));
            memset(unit->swapbuf2, 0, newbuffl * sizeof(float));
        }

       if(unit->numchunks%2==0){
           SETCALC(PermModT_next_even);
           //break;
       }
    }

    unit->jump=jump;
    unit->read=read;
    unit->swapped=swapped;

}

void PermMod_Ctor(PermMod* unit)
{
    float freq=IN0(1);
    int chunkl;

    if(freq>0 && freq<=SAMPLERATE*2){
        chunkl= round(SAMPLERATE/freq);
    }
    else{
        chunkl=1;
    }
      unit->chunkl = chunkl;
      unit->read = 0;
      unit->swapped=false;
      unit->index=0;

      //allocate buffers
      unit->swapbuf1 = (float*)RTAlloc(unit->mWorld, 2*chunkl * sizeof(float));
      unit->swapbuf2 = (float*)RTAlloc(unit->mWorld, 2*chunkl * sizeof(float));
      memset(unit->swapbuf1, 0, 2*chunkl * sizeof(float));
      memset(unit->swapbuf2, 0, 2*chunkl * sizeof(float));

    if(freq<=0 || freq>SAMPLERATE*2){

        SETCALC(PermMod_next_dono); // Does nothing and terminates the UGen
        PermMod_next_dono(unit,1);

    }

    else{
        SETCALC(PermMod_next);
        PermMod_next(unit,1);
    }
}

void PermMod_next( PermMod *unit, int inNumSamples)
{
    float *in= IN(0);
    float *out = OUT(0);
    int chunkl=unit->chunkl;
    //int index=unit->index;
    int newchunkl=chunkl;
    int read=unit->read;
    bool swapped=unit->swapped;

    float *swapbuf1 = unit->swapbuf1;
    float *swapbuf2 = unit->swapbuf2;

    //index++;

    for (int i=0; i<inNumSamples; i++){

        if(swapped==false){ // read 2nd buffer, write 1st

            if(read<chunkl){ // first half
                swapbuf1[read+chunkl]=in[i];
            }
            else{ // second half
                swapbuf1[read-chunkl]=in[i];
            }

            out[i]=swapbuf2[read];
        }

        else{ // read 1st buffer, write 2nd

            if(read<chunkl){
                swapbuf2[read+chunkl]=in[i];
            }
            else{
                swapbuf2[read-chunkl]=in[i];
            }

            out[i]=swapbuf1[read];

        }

        read++;

        if(read==chunkl*2){ //reset
            read=0;
            swapped=!swapped;
        }

    }
    if(IN0(1)>0 && IN0(1)<= SAMPLERATE*2){

         newchunkl=round(SAMPLERATE/IN0(1));
    }
    else{
       Print ("%s \n", "Freq parameter must be >0 and <2*SampleRate, taking last legal value");
    }

    if(newchunkl!=chunkl){

        RTFree(unit->mWorld, unit->swapbuf1);
        RTFree(unit->mWorld, unit->swapbuf2);

        unit->chunkl = newchunkl;
        read = 0;

        unit->swapbuf1 = (float*)RTAlloc(unit->mWorld, 2*newchunkl * sizeof(float));
        unit->swapbuf2 = (float*)RTAlloc(unit->mWorld, 2*newchunkl * sizeof(float));
        memset(unit->swapbuf1, 0, 2*newchunkl * sizeof(float));
        memset(unit->swapbuf2, 0, 2*newchunkl * sizeof(float));

    }

    unit->read=read;
    unit->swapped=swapped;
   //unit->index=index;
   //unit->chunkl=chunkl;

}

void PermModArray_Ctor(PermModArray* unit){

    float freq=IN0(1);
    int nfound=0;
    bool found=false;
    int chunkl;
    bool errors=false;

    if(freq>0 && freq<=SAMPLERATE*2){
        chunkl=round(SAMPLERATE/freq);
    }
    else{
        chunkl=1;
        errors=true;
    }
    int numchunks = unit->mNumInputs -2;

    unit->numchunks=numchunks;
    unit->chunkl = chunkl;
    unit->read = 0;
    unit->swapped=false;
    unit->index=0;

      //allocate buffers
    unit->swapbuf1 = (float*)RTAlloc(unit->mWorld, numchunks*chunkl * sizeof(float));
    unit->swapbuf2 = (float*)RTAlloc(unit->mWorld, numchunks*chunkl * sizeof(float));
    unit->pattern = (int*)RTAlloc(unit->mWorld, numchunks*sizeof(int)); //jump pattern
    unit->array = (int*)RTAlloc(unit->mWorld, numchunks*sizeof(int)); // input array
    memset(unit->swapbuf1, 0, numchunks*chunkl * sizeof(float));
    memset(unit->swapbuf2, 0, numchunks*chunkl * sizeof(float));

    // checks for all numbers from 0 to numchunks to be present
    for(int i=0; i<numchunks; i++) {
        unit->array[i]=(int)IN0(i+2);
        for(int j=0; j<numchunks; j++){
              if((int)IN0(j+2)==i){
                     unit->pattern[i]=(j-i); // calculates actual jumps
                     found=true;
              }
        }
        if(found==true){
             nfound++;
             found=false;
        }
    }
    if(nfound==numchunks && errors==false){
            SETCALC(PermModArray_next);
            PermModArray_next(unit,1);
    }
    else{
        SETCALC(PermModArray_next_dono); // Does nothing and terminates the UGen
        PermModArray_next_dono(unit,1);

    }
}



void PermModArray_next(PermModArray *unit, int inNumSamples){

    float *in = IN(0);
    float *out = OUT(0);
    int chunkl=unit->chunkl;
    int newchunkl=chunkl;
    int newnumchunks=unit->mNumInputs -2;
    int read=unit->read;
    bool errors=false;
    bool valuechange;
    bool swapped=unit->swapped;
    int numchunks=unit->numchunks;
    int index=unit->index;

    float *swapbuf1 = unit->swapbuf1;
    float *swapbuf2 = unit->swapbuf2;
    int *pattern = unit->pattern;


    for (int i=0; i<inNumSamples; i++){

        if (read%chunkl==0 && read!=0){
            index++;
        }


        if(swapped==false){

            swapbuf1[read+(pattern[index]*chunkl)]=in[i];
            out[i]=swapbuf2[read];
        }
        else{

            swapbuf2[read+(pattern[index]*chunkl)]=in[i];
            out[i]=swapbuf1[read];
        }

        read++;

        if(read==chunkl*numchunks){
            read=0;
            index=0;
            swapped=!swapped;
        }
    }

    if(IN0(1)>0 && IN0(1)<=SAMPLERATE*2){
        newchunkl=round(SAMPLERATE/IN0(1));
    }
    else{
        errors=true;
        Print ("%s \n", "Freq parameter must be >0 and <2*SampleRate, using last legal values");
    }

    bool found=false;
    int nfound=0;

    for(int i=0; i<numchunks; i++) {
        for(int j=0; j<numchunks; j++){
              if((int)IN0(j+2)==i){
                     found=true;
              }
        }
        if(found==true){
             nfound++;
             found=false;
        }
    }
    if(nfound!=numchunks){
        errors=true;
        Print ("%s \n", "Wrong pattern, using last legal values");
    }

    if (errors==false){
        if(newchunkl!=chunkl || numchunks!=newnumchunks){

            RTFree(unit->mWorld, unit->swapbuf1);
            RTFree(unit->mWorld, unit->swapbuf2);

            unit->chunkl = newchunkl;
            read = 0;
            index= 0;

            if(numchunks!=newnumchunks){
                unit->numchunks=newnumchunks;
                RTFree(unit->mWorld, unit->pattern);
                unit->pattern = (int*)RTAlloc(unit->mWorld, newnumchunks*sizeof(int));

                for(int i=0; i<newnumchunks; i++) {
                    unit->array[i]=(int)IN0(i+2);
                    for(int j=0; j<newnumchunks; j++){
                            if((int)IN0(j+2)==i){
                                unit->pattern[i]=(j-i);
                        }
                    }
                }

            }

            unit->swapbuf1 = (float*)RTAlloc(unit->mWorld, numchunks*newchunkl * sizeof(float));
            unit->swapbuf2 = (float*)RTAlloc(unit->mWorld, numchunks*newchunkl * sizeof(float));
            memset(unit->swapbuf1, 0, numchunks*newchunkl * sizeof(float));
            memset(unit->swapbuf2, 0, numchunks*newchunkl * sizeof(float));

        }


        if(numchunks==newnumchunks){

            for(int i=0; i<numchunks; i++){
                if(unit->array[i]!=(int)IN0(i+2)){
                    valuechange=true;
                    break;
                }
            }

            if(valuechange==true){

                for(int i=0; i<numchunks; i++) {
                    unit->array[i]=(int)IN0(i+2);
                    for(int j=0; j<numchunks; j++){
                            if((int)IN0(j+2)==i){
                                unit->pattern[i]=(j-i);
                        }
                    }
                }
            }
        }
    }

    unit->read=read;
    unit->swapped=swapped;
    unit->index=index;
}

void PermModT_next_dono( PermModT *unit, int inNumSamples){
    float *in = IN(0);
    float *out = OUT(0);

        for (int i=0; i<inNumSamples; i++){
            float val=in[i];
            out[i]=val;
        }

    Print ("%s \n", "Infreq and outfreq parameter must be >0 and <2*SampleRate");
    DoneAction(2, unit);
}

void PermMod_next_dono( PermMod *unit, int inNumSamples){
    float *in = IN(0);
    float *out = OUT(0);

        for (int i=0; i<inNumSamples; i++){
            float val=in[i];
            out[i]=val;
        }
    Print ("%s \n", "Freq parameter must be >0 and <2*SampleRate");
    DoneAction(2, unit);
}

void PermModArray_next_dono( PermModArray *unit, int inNumSamples){
    float *in = IN(0);
    float *out = OUT(0);

        for (int i=0; i<inNumSamples; i++){
            float val=in[i];
            out[i]=val;
        }
    Print ("%s \n", "Freq parameter must be >0 and <2*SampleRate, Pattern must be valid");
    DoneAction(2, unit);
}



void PermModT_Dtor (PermModT* unit) {
    RTFree(unit->mWorld, unit->swapbuf1);
    RTFree(unit->mWorld, unit->swapbuf2);
}

void PermMod_Dtor (PermMod* unit) {
    RTFree(unit->mWorld, unit->swapbuf1);
    RTFree(unit->mWorld, unit->swapbuf2);
}

void PermModArray_Dtor(PermModArray* unit){
    RTFree(unit->mWorld, unit->swapbuf1);
    RTFree(unit->mWorld, unit->swapbuf2);
    RTFree(unit->mWorld, unit->pattern);
    RTFree(unit->mWorld, unit->array);
}


// the load function is called by the host when the plug-in is loaded
PluginLoad(PermUGens)
{
        ft = inTable;

        DefineDtorUnit(PermModT);
        DefineDtorUnit(PermMod);
        DefineDtorUnit(PermModArray);
}

////////////////////////////////////////////////////////////////////
