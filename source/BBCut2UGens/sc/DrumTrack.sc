//This file is part of The BBCut Library. Copyright (C) 2001  Nick M.Collins distributed under the terms of the GNU General Public License full notice in file BBCutLibrary.help

//4 outs
DrumTrack : MultiOutUGen {

    *kr { arg in,lock=0,dynleak=0.0,tempowt=0.0,phasewt=0.0,basswt=0.0,patternwt=1.0, prior, kicksensitivity=1.0,snaresensitivity=1.0, debugmode=0;

        ^this.multiNew('control',in,lock, dynleak,tempowt,phasewt,basswt,patternwt,prior ? (-10),kicksensitivity,snaresensitivity,debugmode);
    }

    init { arg ... theInputs;
        inputs = theInputs;
        ^this.initOutputs(4, rate);
    }
}
