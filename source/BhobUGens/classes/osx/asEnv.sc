+ EnvelopeView {
	asEnv {|curves, timeScale=1, levelScale=1, levelBias=0, releaseNode, loopNode=0|
		var val, times, levels;
		times=Array.new;
		val=this.value;
		levels=val[1];
		curves=curves ? Array.fill(val[0].size, 0);
		curves=curves.asArray;
		(val[0].size-1).do({|i|
			times=times.add(val[0][i+1] - val[0][i]);
		});
		(timeScale != 1).if({ times=times * timeScale });
		(levelScale != 1).if({ levels=levels * levelScale });
		(levelBias != 0).if({ levels=levels + levelBias });
		^Env(levels, times, curves, releaseNode, loopNode);
	}
}