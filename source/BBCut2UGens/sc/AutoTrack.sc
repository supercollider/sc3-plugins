//This file is part of The BBCut Library. Copyright (C) 2001  Nick M.Collins distributed under the terms of the GNU General Public License full notice in file BBCutLibrary.help

//synonym for BeatTrack these days, since BeatTrack went in core
AutoTrack {

	*kr { arg in, lock=0;

		^BeatTrack.kr(in, lock);
	}

}
