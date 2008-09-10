//This file is part of MLfftwUGens. Copyright (C) 2006  Nicholas M.Collins distributed under the terms of the GNU General Public License full notice in file MachineListening.license

//This file is part of The BBCut Library. Copyright (C) 2001  Nick M.Collins distributed under the terms of the GNU General Public License full notice in file BBCutLibrary.help

//4 outs 
AutoTrack : MultiOutUGen {

	*kr { arg in, lock=0;
	
		^this.multiNew('control',in, lock);
	}
	
	init { arg ... theInputs;
		inputs = theInputs;
		^this.initOutputs(4, rate);
	}
}

