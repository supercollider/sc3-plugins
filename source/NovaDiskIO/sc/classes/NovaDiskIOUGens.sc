NovaDiskOut : UGen {
	*ar { arg signal, filename;
		var args = [signal.size] ++ signal ++ [filename.size] ++ filename.asString.collectAs(_.ascii, Array);
		^this.multiNew('audio', *args)
	}
}

NovaDiskIn : MultiOutUGen {
	classvar readerIDAllocator;

	*initClass {
		readerIDAllocator = StackNumberAllocator(0, 1073741824)
	}

	*cueSoundfile {|filename, completionMessage, server|
		var cueID = readerIDAllocator.alloc;

		if (server.isNil) {
			server = Server.default
		};

		server.sendMsg(\cmd, \NovaDiskIn, 0 /* open */, cueID, filename, completionMessage);
		^cueID
	}

	*close {|cueID, completionMessage, server|
		if (server.isNil) {
			server = Server.default
		};

		server.sendMsg(\cmd, \NovaDiskIn, 2 /* close */, cueID, "", completionMessage);
		readerIDAllocator.free( cueID )
	}

	*ar { arg numberOfChannels, cueID;
		^this.multiNew('audio', numberOfChannels, cueID)
	}

	init { arg numberOfChannels, cueID;
		inputs = [numberOfChannels, cueID];
		^this.initOutputs(numberOfChannels, rate)
	}
}
