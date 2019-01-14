/*
This package contains a number of ports of the
Stk physical models.  They are not highly optimized
and initializations can be very expensive.  To
get around this triggers are supplied in all
units so that they can be fired up in paused states
and subsequently triggered and paused, thus greatly reducing
initialization cost.  The downside of this is that
you have to keep track of the nodes, but that's not
a big deal.  I've been initializing a bunch of instruments
at the start, pausing for a freq seconds and then starting
to play.

Installation instructions

1) The rawwaves folder has to be parallel
to the SuperCollider.app folder.  i.e. the
units will look for ../rawwaves/etc.  If you
get 'realtime errors' it is in the wrong place.
notice: when building from source you will need to get these from the library itself (or the prebuilt lib package download from sc3-plugins sourceforge site).
The prebuilt UGens packages for Mac OS X come with this included.

2) Copy all the .scx files to the plugins dir.

3) These were compiled under 10.3.5 so they will
only work under XCode systems.

4) Copy the classdefs file to the SCClassLibrary
and recompile the Library.

5) tests.rtf will quickly test each of the units.
If you get sound they're working.

6) The classdefs.sc file contains minimal information
about each of the units.  When it says that arguments
are modulateable it means that they can also be changed
when retriggering.

7) Triggers
When the trigger changes from a negative to a positive
value the unit will do an STK noteOn().  In a number of
these models the attack portion has particular and 
noticeable qualities so this can make a big difference.
It also allows you to avoid the expensive initializations.

Here is an example using the Clarinet model
*/
(
SynthDef("StkClarinet", {arg  freq=440,reedstiffness=64,noisegain=10,vibfreq=64,
        vibgain=10,breathpressure=64, gain=0.2,gate=1,bus=0, 
        lag = 0.1,sloc=0,riset=0.2,decayt=0.2 ;

    var z,env;
    env  =  EnvGen.kr(Env.adsr(attackTime:riset, decayTime:0,sustainLevel:1,
            releaseTime:0.1,peakLevel:1),gate:gate, doneAction:0);

    z = StkClarinet.ar(
      freq:Lag.kr(freq, lag),
      reedstiffness:reedstiffness,
      noisegain:noisegain,
      vibfreq:vibfreq,
      vibgain:vibgain,
      breathpressure:breathpressure,
      trig:gate);
    Out.ar(bus, Pan2.ar(z,sloc)*env*gain);
  }).load(s);
)

// test synthdef
Synth("StkClarinet", [\riset, 12]).play

// start up in paused state (usually necessary to slightly defer
//  the pause for some reason

{s.sendMsg("/s_new", "StkClarinet", n = s.nextNodeID ,0,0, \freq, 440, \gate, -1, \reedstiffness, 64,\i_bus, 0);  
  {s.sendMsg(\n_run, n, 0)}.defer(0.01); }
// trigger a note
  s.sendBundle(nil, [\n_set,n,  \gate, 1, \freq, 440, \gain, 0.1], [\n_run, n,1]);
 // trigger another note 
  s.sendBundle(nil, [\n_set,n,  \gate, 1, \freq, 540, \lag, 0.1], [\n_run, n,1]);
 // turn it off
  s.sendMsg(\n_set, n, \gate, -1.2);
 // pause the unit and free up the CPU
  s.sendMsg(\n_run, n, 0);
 // start up another note
  s.sendBundle(nil, [\n_set,n,  \gate, 1, \freq, 100, \vibgain, 50, \breathpressure, 80, \lag,0], [\n_run, n,1]);
// etc ( any parameters you change will apply to all subsequent notes)

