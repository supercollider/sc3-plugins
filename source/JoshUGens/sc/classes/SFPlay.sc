SFPlay {
	classvar <playfunc, <buffer;

	*play {arg path, starttime = 0, outbus = 0, server;
		var c, soundfile, info, numChannels, numFrames, sampleRate, duration;
		c = Condition.new;
		server = server ? Server.default;
			path.notNil.if({soundfile = path;
				info = SoundFile.new;
				info.openRead(soundfile);
				numChannels = info.numChannels;
				numFrames = info.numFrames;
				sampleRate = info.sampleRate;
				info.close;
				duration = numFrames / sampleRate;
				(starttime > duration).if({
					("Starttime is greater than file duration of "++ duration.asString).postln},
					{
						buffer = Buffer.read(server, soundfile, starttime * sampleRate);
						playfunc = SynthDef(\sfplay, {
							Out.ar(outbus,
								PlayBuf.ar(numChannels, buffer.bufnum, BufRateScale.kr(buffer.bufnum)) *
									EnvGen.kr(Env([1,1], [(duration - starttime) +1]),
									doneAction: 2));							}).play(server);
						AppClock.sched((duration-starttime), {this.stop});
					})},
			{CocoaDialog.getPaths({ arg paths;
				paths.do({ arg p;
				soundfile = p;
				info = SoundFile.new;
				info.openRead(soundfile);
				numChannels = info.numChannels;
				numFrames = info.numFrames;
				sampleRate = info.sampleRate;
				info.close;
				duration = numFrames / sampleRate;
				(starttime > duration).if({
					("Starttime is greater than file duration of "++ duration.asString).postln},{
						buffer = Buffer.read(server, soundfile, starttime * sampleRate, -1);
						playfunc = SynthDef(\sfplay, {
							Out.ar(outbus,
								PlayBuf.ar(numChannels, buffer.bufnum, BufRateScale.kr(buffer.bufnum)) *
									EnvGen.kr(Env([1,1], [(duration - starttime) +1]),
									doneAction: 2));							}).play(server);
						AppClock.sched((duration-starttime), {this.stop});
					})
				})
				},{
			"cancelled".postln})});
		}

	*stop {
		this.playfunc.free;
		this.buffer.free;
		}
}
