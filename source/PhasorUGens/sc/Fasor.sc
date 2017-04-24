//was RedPhasor and RedPhasor2
Fasor : UGen {
	*ar {|trig= 0.0, rate= 1, start= 0, end= 1, loop= 0, loopstart= 0, loopend= 1|
		^this.multiNew('audio', trig, rate, start, end, loop, loopstart, loopend)
	}
	*kr {|trig= 0.0, rate= 1, start= 0, end= 1, loop= 0, loopstart= 0, loopend= 1|
		^this.multiNew('control', trig, rate, start, end, loop, loopstart, loopend)
	}
	*categories {^#["UGens>Triggers", "UGens>Buffer"]}
}

Fasor2 : UGen {
	*ar {|trig= 0.0, rate= 1, start= 0, end= 1, loop= 0, loopstart= 0, loopend= 1|
		^this.multiNew('audio', trig, rate, start, end, loop, loopstart, loopend)
	}
	*kr {|trig= 0.0, rate= 1, start= 0, end= 1, loop= 0, loopstart= 0, loopend= 1|
		^this.multiNew('control', trig, rate, start, end, loop, loopstart, loopend)
	}
	*categories {^#["UGens>Triggers", "UGens>Buffer"]}
}
