//This file is part of MLfftwUGens. Copyright (C) 2006  Nicholas M.Collins distributed under the terms of the GNU General Public License full notice in file MachineListening.license

//This file is part of The BBCut Library. Copyright (C) 2001  Nick M.Collins distributed under the terms of the GNU General Public License full notice in file BBCutLibrary.help

//could have a starttime input for accuracy but should be unnecessary

AnalyseEvents2 : UGen {

	*ar { arg in,bufnum=0,threshold=0.34, triggerid=101, circular=0, pitch=0; //, debugbufnum=0;

		^this.multiNew('audio',in,bufnum,threshold,triggerid, circular, pitch); //, debugbufnum;
	}
}
