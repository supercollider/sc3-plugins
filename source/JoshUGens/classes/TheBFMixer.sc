
BFMixer {
*new {

var mixwin, play, write, setOutput, trackarray, getfile, filefunc, saveCurrentMix, loadMix;
var mixerArray, playfunc, stopfunc, playrout, trackRout, format, globStartTime, savefunc, loadfunc, savedArray, trackArray, winDict, trackDict, dictArray, globStartFunc, skipTimeFunc, trackRangeFunc, ampNumberFunc, ampSliderFunc, writefunc, sortTimes, timedArray, formatNRT, s, time, texttask, numChannels, muteFunc, fileNameFunc, scoreFile, envVals, buildEnvSig, fileName, c;

c = Condition.new;
s = Server.local;
s.boot;

SynthDef(\mixer_play_in, {arg buffer = 0, dur = 1, amp = 1, mute = 1, trackDur = 1;
	var offset;
	offset = (1 - (dur/trackDur));
	Out.ar(32, (DiskIn.ar(3, buffer) * amp * mute *
			EnvGen.kr(Env([0.001, 1, 1, 0.001], [0.01, dur - 0.02, 0.01]), doneAction: 2) *
			(BufRd.ar(1, buffer + 100,
				LFSaw.ar(trackDur.reciprocal, 1 + (offset * 2), 512, 512), 0, 4)).dbamp));
 }).load(s);

SynthDef(\mixer_playbuf_in, {arg buffer = 0, dur = 1, amp = 1, mute = 1, trackDur = 1, start = 0;
	var offset;
	offset = (1 - (dur/trackDur));
	Out.ar(32, (PlayBuf.ar(3, buffer) * amp * mute *
			EnvGen.kr(Env([0.001, 1, 1, 0.001], [0.01, dur - 0.02, 0.01]), doneAction: 2) *
			(BufRd.ar(1, buffer + 100,
				LFSaw.ar(trackDur.reciprocal, 1 + (offset * 2), 512, 512), 0, 4)).dbamp));
 }).load(s);

SynthDef(\mixer_play_stereo, {arg buffer = 0, dur = 1, amp = 1, mute = 1, trackDur = 1;
	var offset;
	offset = (1 - (dur/trackDur));
	Out.ar(0,
		LeakDC.ar(BFDecode.asStereo(In.ar(32, 3))) * amp * mute *
			EnvGen.kr(Env([0.001, 1, 1, 0.001], [0.01, dur - 0.02, 0.01]), doneAction: 2) *
			(BufRd.ar(1, buffer + 100,
				LFSaw.ar(trackDur.reciprocal, 1 + (offset * 2), 512, 512), 0, 4)).dbamp);
}).load(s);

SynthDef(\mixer_play_quad, {arg buffer = 0, dur = 1, amp = 1, mute = 1, trackDur = 1;
	var lf, rf, rr, lr;
	var offset;
	offset = (1 - (dur/trackDur));
	#lf, rf, rr, lr = BFDecode.asQuad(In.ar(32, 3));
	Out.ar(0, LeakDC.ar([lf, rf, rr, lr]) * amp * mute *
			EnvGen.kr(Env([0.001, 1, 1, 0.001], [0.01, dur - 0.02, 0.01]), doneAction: 2) *
			(BufRd.ar(1, buffer + 100,
				LFSaw.ar(trackDur.reciprocal, 1 + (offset * 2), 512, 512), 0, 4)).dbamp);
}).load(s);

SynthDef(\mixer_play_bf, {arg buffer = 0, dur = 1, amp = 1, mute = 1, trackDur = 1;
	var offset;
	offset = (1 - (dur/trackDur));
	Out.ar(0,
		LeakDC.ar(In.ar(32, 3)) * amp * mute *
			EnvGen.kr(Env([0.001, 1, 1, 0.001], [0.01, dur - 0.02, 0.01]), doneAction: 2) *
			(BufRd.ar(1, buffer + 100,
				LFSaw.ar(trackDur.reciprocal, 1 + (offset * 2), 512, 512), 0, 4)).dbamp);
	}).load(s);

SynthDef(\mixer_play_uhj, {arg buffer = 0, dur = 1, amp = 1, mute = 1, trackDur = 1;
	var offset, w, x, y;
	#w, x, y = In.ar(32, 3);
	offset = (1 - (dur/trackDur));
	Out.ar(0,
		LeakDC.ar(BtoUHJ.ar(w, x, y)) * amp * mute *
			EnvGen.kr(Env([0.001, 1, 1, 0.001], [0.01, dur - 0.02, 0.01]), doneAction: 2) *
			(BufRd.ar(1, buffer + 100,
				LFSaw.ar(trackDur.reciprocal, 1 + (offset * 2), 512, 512), 0, 4)).dbamp);
	}).load(s);

SynthDef(\mixer_play_fiveDot1, {arg buffer = 0, dur = 1, amp = 1, mute = 1, trackDur = 1;
	var offset;
	offset = (1 - (dur/trackDur));
	Out.ar(0,
		LeakDC.ar(BFDecode.fiveDot1(In.ar(32, 3))) * amp * mute *
			EnvGen.kr(Env([0.001, 1, 1, 0.001], [0.01, dur - 0.02, 0.01]), doneAction: 2) *
			(BufRd.ar(1, buffer + 100,
				LFSaw.ar(trackDur.reciprocal, 1 + (offset * 2), 512, 512), 0, 4)).dbamp);
		}).load(s);

SynthDef(\mixer_playbuf_stereo, {arg buffer = 0, dur = 1, amp = 1, mute = 1, trackDur = 1;
	var offset;
	offset = (1 - (dur/trackDur));
	Out.ar(0,
		LeakDC.ar(BFDecode.asStereo(In.ar(32, 3))) * amp * mute *
			EnvGen.kr(Env([0.001, 1, 1, 0.001], [0.01, dur - 0.02, 0.01]), doneAction: 2) *
			(BufRd.ar(1, buffer + 100,
				LFSaw.ar(trackDur.reciprocal, 1 + (offset * 2), 512, 512), 0, 4)).dbamp);
	}).load(s);

SynthDef(\mixer_playbuf_quad, {arg buffer = 0, dur = 1, amp = 1, mute = 1, trackDur = 1;
	var lf, rf, rr, lr;
	var offset;
	offset = (1 - (dur/trackDur));
	#lf, lr, rr, rf = BFDecode.asQuad(In.ar(32, 3));
	Out.ar(0, LeakDC.ar([lf, rf, rr, lr]) * amp * mute *
			EnvGen.kr(Env([0.001, 1, 1, 0.001], [0.01, dur - 0.02, 0.01]), doneAction: 2) *
			(BufRd.ar(1, buffer + 100,
				LFSaw.ar(trackDur.reciprocal, 1 + (offset * 2), 512, 512), 0, 4)).dbamp);
	}).load(s);

SynthDef(\mixer_playbuf_bf, {arg buffer = 0, dur = 1, amp = 1, mute = 1, trackDur = 1;
	var offset;
	offset = (1 - (dur/trackDur));
	Out.ar(0,
		LeakDC.ar(In.ar(32, 3)) * amp * mute *
			EnvGen.kr(Env([0.001, 1, 1, 0.001], [0.01, dur - 0.02, 0.01]), doneAction: 2) *
			(BufRd.ar(1, buffer + 100,
				LFSaw.ar(trackDur.reciprocal, 1 + (offset * 2), 512, 512), 0, 4)).dbamp);
	}).load(s);

SynthDef(\mixer_playbuf_uhj, {arg buffer = 0, dur = 1, amp = 1, mute = 1, trackDur = 1;
	var offset, w, x, y;
	#w, x, y = In.ar(32, 3);
	offset = (1 - (dur/trackDur));
	Out.ar(0,
		LeakDC.ar(BtoUHJ.ar(w, x, y)) * amp * mute *
			EnvGen.kr(Env([0.001, 1, 1, 0.001], [0.01, dur - 0.02, 0.01]), doneAction: 2) *
			(BufRd.ar(1, buffer + 100,
				LFSaw.ar(trackDur.reciprocal, 1 + (offset * 2), 512, 512), 0, 4)).dbamp);
	}).load(s);

SynthDef(\mixer_playbuf_fiveDot1, {arg buffer = 0, dur = 1, amp = 1, mute = 1, trackDur = 1;
	var offset;
	offset = (1 - (dur/trackDur));
	Out.ar(0,
		LeakDC.ar(BFDecode.fiveDot1(In.ar(32, 3))) * amp * mute *
			EnvGen.kr(Env([0.001, 1, 1, 0.001], [0.01, dur - 0.02, 0.01]), doneAction: 2) *
			(BufRd.ar(1, buffer + 100,
				LFSaw.ar(trackDur.reciprocal, 1 + (offset * 2), 512, 512), 0, 4)).dbamp);
	}).load(s);


dictArray = Array.newClear(11);

dictArray[0] = Dictionary(11);

dictArray[0].add('formatButton' -> 0);
dictArray[0].add('format' -> 0);
dictArray[0].add('formatNRT' -> 0);
dictArray[0].add('numChannels' -> 3);
dictArray[0].add('startTimeInMix' -> 0);
dictArray[0].add('totalTime' -> 0);
dictArray[0].add('env' -> 0);
dictArray[0].add('globalAmpSlider' -> 0);
dictArray[0].add('globalAmpNumber' -> 0);
dictArray[0].add('globalAmp' -> 1);
dictArray[0].add('base' -> 0);

10.do{arg i;
	var j;
	j = i+1;
	dictArray[j] = Dictionary(20);
	dictArray[j].add('file' -> nil);
	dictArray[j].add('dur' -> nil);
	dictArray[j].add('amp' -> nil); //set amp
	dictArray[j].add('sampleRate' -> nil);
	dictArray[j].add('playDur' -> nil); //duration of track after skiptime
	dictArray[j].add('globalStart' -> nil);
	dictArray[j].add('loadFile' -> nil);
	dictArray[j].add('startTime' -> nil); //for Text
	dictArray[j].add('endTime' -> nil); //for text
	dictArray[j].add('skipTime' -> nil); //set initial skip time
	dictArray[j].add('ampSlider' -> nil);
	dictArray[j].add('ampNumber' -> nil);
	dictArray[j].add('trackText' -> nil); //for text
	dictArray[j].add('trackRange' -> nil); //for text
	dictArray[j].add('shortenFile' -> nil);
	dictArray[j].add('mute' -> nil);
	dictArray[j].add('filename' -> nil);
	dictArray[j].add('env' -> nil);
	dictArray[j].add('base' -> nil);

	};

/////////////////////////////////////////////////
/////////////////////////////////////////////////
///// THESE FUNCTIONS CONTROL GLOBAL ASPECTS OF THE MIXER
/////////////////////////////////////////////////
/////////////////////////////////////////////////


format = [
	{dictArray[0].put('format', \mixer_play_bf)},
	{dictArray[0].put('format', \mixer_play_stereo)},
	{dictArray[0].put('format', \mixer_play_quad)},
	{dictArray[0].put('format', \mixer_play_uhj)},
	{dictArray[0].put('format', \mixer_play_fiveDot1)}
	];

formatNRT = [
	{dictArray[0].put('formatNRT', \mixer_playbuf_bf)},
	{dictArray[0].put('formatNRT', \mixer_playbuf_stereo)},
	{dictArray[0].put('formatNRT', \mixer_playbuf_quad)},
	{dictArray[0].put('formatNRT', \mixer_playbuf_uhj)},
	{dictArray[0].put('formatNRT', \mixer_playbuf_fiveDot1)}
	];

numChannels = [
	{dictArray[0].put('numChannels', 3)},
	{dictArray[0].put('numChannels', 2)},
	{dictArray[0].put('numChannels', 4)},
	{dictArray[0].put('numChannels', 2)},
	{dictArray[0].put('numChannels', 6)}
	];

//a function to sort and arrange an array by start times
sortTimes = {arg array;
	var output, most, pos, newArray;
	newArray = Array.newClear(array.size);
	most = 0;
	array.size.do{arg j;
		array.size.do{arg i;
			(most <= array[i][0]).if({most = array[i][0];pos = i}, {nil});
			pos;
			};
		newArray.put(j, array[pos]);
		array.removeAt(pos);
		most = 0;
		pos = 0;
		"done".postln;
		};
	newArray.reverse};

// a few useful functions for later use

// 1001 will always be track1... 1016 always track 16

playfunc = {
	var start, dur, amp, mute, globstart, offset, soundpath, startframe, writeArray, trackDur;
	var f, fileArray, startval, envArray, base, exp, envInfo;
	(dictArray[0]['totalTime'] == 0).if(
	{"No files are loaded... Nothing to play!".postln;
	play.value_(0);
	playrout.yieldAndReset;
	},
	{
	fileArray = Array.new(11);
	f = File("/tmp/scmix.sc", "w");
	f.putString("[");
	10.do{arg i;
		var j;
		j = i+1;
		(dictArray[j]['file'].notNil).if({
			soundpath = dictArray[j]['file'].asString;
			startframe =(dictArray[j]['globalStart'].value >=
				dictArray[0]['startTimeInMix'].value)
					.if({dictArray[j]['skipTime'].value},
						{(dictArray[0]['startTimeInMix'].value -
							dictArray[j]['globalStart'].value) +
							dictArray[j]['skipTime'].value});
			startval = (dictArray[j]['globalStart'].value >= dictArray[0]['startTimeInMix'].value)
				.if({dictArray[j]['skipTime'].value},
					{(dictArray[0]['startTimeInMix'].value - dictArray[j]['globalStart'].value) +
						dictArray[j]['skipTime'].value});
			f.putString(([0.1, [\b_alloc, j, 32768, 3]]).asCompileString ++ ",\n");
			f.putString(([0.1, [\b_read, j, soundpath, startframe *
				dictArray[j]['sampleRate'], 32768, 0, 1]]).asCompileString ++ ",\n");
			"Loaded" ++ dictArray[j]['file'].asString.postln;
			dur = (dictArray[j]['endTime'] - dictArray[0]['startTimeInMix'].value);
			trackDur = dictArray[j]['endTime'] - dictArray[j]['startTime'];
			amp = dictArray[j]['amp'];
			start = dictArray[j]['globalStart'].value;
			mute = dictArray[j]['mute'].value;
			offset = (start - dictArray[0]['startTimeInMix'].value).max(0);
			(dur > 0).if({
				fileArray.add([(0.2 + offset).asFloat, [\s_new, \mixer_play_in, 1001+i,
					0, 0, \buffer, j, \dur, dur - offset, \amp, amp, \mute, mute,
					\trackDur, trackDur]])}, {nil});
				},{nil});
			};
	//need to change buffer below to reflect a global env
	//f.putString
	fileArray.add([0.2, [\s_new, dictArray[0]['format'], 1000, 1, 0, \buffer, 0, \dur,
		dictArray[0]['totalTime'] - dictArray[0]['startTimeInMix'].value + 0.2, \amp, 		dictArray[0]['globalAmp'], \mute, 1, \trackDur, dictArray[0]['totalTime'] -
		dictArray[0]['startTimeInMix'].value + 0.2]]);
	writeArray = sortTimes.value(fileArray);
	writeArray.size.do{arg i;
		f.putString((writeArray[i]).asCompileString ++ ",\n");
		};
	f.putString(([dictArray[0]['totalTime'] - dictArray[0]['startTimeInMix'].value + 0.4,
		[\c_set, 0]]).asCompileString);
	f.putString("]");
	f.close;
	scoreFile = Score.newFromFile("/tmp/scmix.sc");
	scoreFile.play(s);
	texttask.play;
	});
};

stopfunc = {
	scoreFile.stop;
	s.sendBundle(0.1, [\n_free, 1000]);
	10.do{arg i;
		var j;
		j = i + 1;
		dictArray[j]['file'].notNil.if({
			s.sendBundle(0.1, [\n_free, 1000 + j],
				[\b_close, j]);
			}, {nil});
	texttask.stop;
	texttask.reset;
	SystemClock.clear;
}};

playrout = Routine({
		(playfunc.value).yield;
		(stopfunc.value).yieldAndReset;
	});

writefunc = {
	var start, dur, amp, mute, globstart, offset, soundpath, startframe, writeArray, scoreFile,
		trackDur;
	var envInfo, envArray, base, exp;
	var globEnvInfo, globEnvArray, globBase, globExp;
	(dictArray[0]['totalTime'] == 0).if(
	{"No files are loaded... Nothing to write!".postln;
	},
	{
	CocoaDialog.savePanel({arg path;
		var f, fileArray;
		fileArray = Array.new(12);
		f = File("/tmp/scmix.sc", "w");
		f.putString("[");
		10.do{arg i;
			var j;
			j = i+1;
			(dictArray[j]['file'].notNil).if({
			soundpath = dictArray[j]['file'].asString;
			startframe =(dictArray[j]['globalStart'].value >=
				dictArray[0]['startTimeInMix'].value)
				.if({dictArray[j]['skipTime'].value},
					{(dictArray[0]['startTimeInMix'].value - dictArray[j]['globalStart'].value) +
						dictArray[j]['skipTime'].value});
//			dur = (dictArray[j]['endTime'] - dictArray[0]['startTimeInMix'].value)
//				- dictArray[j]['globalStart'].value;
			trackDur = dictArray[j]['endTime'] - dictArray[j]['startTime'].value;
			dur = (dictArray[0]['startTimeInMix'].value < dictArray[j]['startTime']).if(
				{trackDur},
				{dictArray[j]['endTime'] - dictArray[0]['startTimeInMix'].value;});
			amp = dictArray[j]['amp'];
			start = dictArray[j]['globalStart'].value;
			mute = dictArray[j]['mute'].value;
			offset = (start - dictArray[0]['startTimeInMix'].value).max(0);
			(dur > 0).if({
				f.putString(([0.1, [\b_allocRead, j, soundpath, startframe *
					dictArray[j]['sampleRate'], -1, 3]])
					.asCompileString ++ ",\n");
				"Loaded" ++ dictArray[j]['file'].asString.postln;
				#envArray, base = dictArray[j]['env'].string.interpret;
				exp = (dictArray[j]['base'].value==1).if({true}, {nil});
				envInfo = Signal.newClear(1024).waveFill(
					{arg x, i; envVals.value(envArray, x, base, exp)}, 0, 1);
				f.putString(([0.1, [\b_alloc, 100 + j, 1024]]).asCompileString ++ ",\n");
				f.putString(([0.1, [\b_setn, 100 + j, 0, 1024] ++ envInfo]).asCompileString
					++ ",\n");
				fileArray.add([(0.2 + offset).asFloat, [\s_new, "mixer_playbuf_in",
					1001+i, 2, 1000, \buffer, j, \dur, dur, \amp, amp, \mute, mute,
					\trackDur, trackDur]])},
				{nil});
				},{nil});
			};
		#globEnvArray, globBase = dictArray[0]['env'].string.interpret;
		globExp = (dictArray[0]['base'].value==1).if({true}, {nil});
		globEnvInfo = Signal.newClear(1024).waveFill(
			{arg x, i; envVals.value(globEnvArray, x, globBase, globExp)}, 0, 1);
		f.putString(([0.1, [\b_alloc, 100, 1024]]).asCompileString ++ ",\n");
		f.putString(([0.1, [\b_setn, 100, 0, 1024] ++ globEnvInfo]).asCompileString ++ ",\n");
		fileArray.add([0.19, [\s_new, dictArray[0]['formatNRT'], 1000, 1, 0, \buffer, 0, \dur,
			dictArray[0]['totalTime'] - dictArray[0]['startTimeInMix'].value + 0.2, \amp,
			dictArray[0]['globalAmp'], \mute, 1, \trackDur, dictArray[0]['totalTime'] -
			dictArray[0]['startTimeInMix'].value + 0.2]]);
		writeArray = sortTimes.value(fileArray);
		writeArray.size.do{arg i;
			f.putString((writeArray[i]).asCompileString ++ ",\n");
			};
		f.putString(([dictArray[0]['totalTime'] - dictArray[0]['startTimeInMix'].value,
			[\c_set, 0]]).asCompileString);
		f.putChar($]);
		f.close;
		s.quit;
		scoreFile = Score.newFromFile("/tmp/scmix.sc");
		Score.recordNRT(scoreFile.score, "/tmp/scmix.osc", path,
			options: ServerOptions.new.
				numOutputBusChannels_(dictArray[0]['numChannels']).memSize_(32768));
		})})};

//the function for saving mixer files REWRITE TO USE DICTIONARY!

savefunc = {
	CocoaDialog.savePanel({arg path;
		var f;
		f = File(path, "w");
			f.putString("[\n");
			f.putString("//Global info\n");
			f.putString("\tDictionary[\n");
			f.putString("\t\t('formatButton' -> ");
			f.putString((dictArray[0]['formatButton'].value).asCompileString ++ "),\n");
			f.putString("\t\t('startTimeInMix' -> ");
			f.putString((dictArray[0]['startTimeInMix'].value).asCompileString ++ "),\n");			f.putString("\t\t('env' -> ");
			f.putString((dictArray[0]['env'].string).asCompileString ++ "),\n");			f.putString("\t\t('globalAmpSlider' -> ");			f.putString((dictArray[0]['globalAmpSlider'].value).asCompileString ++ "),\n");			f.putString("\t\t('globalAmpNumber' -> ");
			f.putString((dictArray[0]['globalAmpNumber'].value).asCompileString ++ "),\n");			f.putString("\t\t('globalAmp' -> ");
			f.putString((dictArray[0]['globalAmp'].value).asCompileString ++ "),\n");			f.putString("\t\t('base' -> ");
			f.putString((dictArray[0]['base'].value).asCompileString ++ "),\n");			f.putString("\t\t('totalTime' -> ");
			f.putString((dictArray[0]['totalTime']).asCompileString ++ ")\n");
			f.putString("\t],\n");
			(dictArray.size - 1).do{arg i;
				var j, dict;
				j = i + 1;
				dict = dictArray[j];
				f.putString("//Track" ++ j.asString ++ "\n");
				f.putString("\tDictionary[\n");
				f.putString("\t\t('file' -> ");
				f.putString((dict['file']).asCompileString ++ "),\n");
				f.putString("\t\t('dur' -> ");
				f.putString((dict['dur']).asCompileString ++ "),\n");
				f.putString("\t\t('amp' -> ");
				f.putString((dict['amp']).asCompileString ++ "),\n");				f.putString("\t\t('sampleRate' -> ");
				f.putString((dict['sampleRate']).asCompileString ++ "),\n");				f.putString("\t\t('playDur' -> ");
				f.putString((dict['playDur']).asCompileString ++ "),\n");				f.putString("\t\t('globalStart' -> ");
				f.putString((dict['globalStart'].value).asCompileString ++ "),\n");				f.putString("\t\t('loadFile' -> ");
				f.putString((dict['loadFile'].value).asCompileString ++ "),\n");				f.putString("\t\t('startTime' -> ");
				f.putString((dict['startTime']).asCompileString ++ "),\n");				f.putString("\t\t('endTime' -> ");
				f.putString((dict['endTime']).asCompileString ++ "),\n");				f.putString("\t\t('skipTime' -> ");
				f.putString((dict['skipTime'].value).asCompileString ++ "),\n");				f.putString("\t\t('ampSlider' -> ");
				f.putString((dict['ampSlider'].value).asCompileString ++ "),\n");				f.putString("\t\t('ampNumber' -> ");
				f.putString((dict['ampNumber'].value).asCompileString ++ "),\n");
				f.putString("\t\t('shortenFile' -> ");
				f.putString((dict['shortenFile'].value).asCompileString ++ "),\n");
				f.putString("\t\t('env' -> ");
				f.putString((dict['env'].string).asCompileString ++ "),\n");
				f.putString("\t\t('base' -> ");
				f.putString((dict['base'].value).asCompileString ++ "),\n");				f.putString("\t\t('mute' -> ");
				f.putString((dict['mute'].value).asCompileString ++ ")\n");
				//for end of Dictionary Array
				f.putString("\t],\n");
				};
		f.putString("]");
		f.close;
	}, {"cancel".postln});
};

loadfunc = {
	s.boot;
	File.openDialog("",
		{arg path;
			var a;
			mixwin.name_(fileName.value(path));
			a =thisProcess.interpreter.compileFile(path);
			dictArray[0]['formatButton'].valueAction_(a.value[0]['formatButton']);
			dictArray[0]['startTimeInMix'].value_(a.value[0]['startTimeInMix']);
			dictArray[0].put('totalTime', a.value[0]['totalTime']);
			dictArray[0]['env'].string_(a.value[0]['env']);
			dictArray[0]['globalAmpSlider'].valueAction_(a.value[0]['globalAmpSlider']);
			dictArray[0]['base'].value_(a.value[0]['base']);
			buildEnvSig.value(0);
			((a.value.size)-1).do{arg i;
				var j;
				j = i+1;
				(a.value[j]['file'].notNil).if({
					dictArray[j]['loadFile'].value_(1);
					dictArray[j].put('file', a.value[j]['file']);
					dictArray[j].put('dur', a.value[j]['dur']);
					dictArray[j].put('amp', a.value[j]['amp']);
					dictArray[j]['ampNumber'].value_(a.value[j]['ampNumber']);
					dictArray[j]['ampSlider'].value_(a.value[j]['ampSlider']);
					dictArray[j].put('sampleRate', a.value[j]['sampleRate']);
					dictArray[j].put('playDur', a.value[j]['playDur']);
					dictArray[j]['globalStart'].value_(a.value[j]['globalStart']);
					dictArray[j]['skipTime'].value_(a.value[j]['skipTime']);
					dictArray[j]['mute'].value_(a.value[j]['mute']);
					dictArray[j].put('startTime', a.value[j]['startTime']);
					dictArray[j].put('endTime', a.value[j]['endTIme']);
					dictArray[j].put('filemane', a.value[j]['filename']);
					dictArray[j]['shortenFile'].value_(a.value[j]['shortenFile']);
					dictArray[j]['env'].string_(a.value[j]['env']);
					dictArray[j]['base'].value_(a.value[j]['base']);
					dictArray[j]['trackRange'].activeLo_(dictArray[j]['globalStart']
						.value/dictArray[0]['totalTime'])
						.activeHi_((dictArray[j]['globalStart'].value+dictArray[j]['playDur'])/
							dictArray[0]['totalTime']);
					dictArray[j]['filename'].string_(fileNameFunc.value(j));
					buildEnvSig.value(j);
					},
					{});
			};
			},
		{"no".postln})
};

///////////////////////////////The tracks////////////////////////////////

/////////////////////////////////////////////////
/////////////////////////////////////////////////
///// THESE FUNCTIONS CONTROL THE INDEPENDANT TRACKS
/////////////////////////////////////////////////
/////////////////////////////////////////////////


globStartFunc = {arg track;
	((dictArray[track]['globalStart'].value + dictArray[track]['playDur'].value) >= 		dictArray[0]['totalTime'].value).if(
			{dictArray[0]['totalTime'] = dictArray[track]['globalStart'].value + 				dictArray[track]['playDur'];
				//update the other ranges
				10.do{arg i;
					var j;
					j = i + 1;
					(dictArray[j]['file'].notNil).if({
						dictArray[j]['trackRange'].activeLo_(dictArray[j]['globalStart'].value /							dictArray[0]['totalTime'])
						.activeHi_((dictArray[j]['globalStart'].value + dictArray[j]['playDur']) / 							dictArray[0]['totalTime']);
						dictArray[j]['startTime'] = dictArray[j]['globalStart'].value;
						dictArray[j]['endTime'] = dictArray[j]['globalStart'].value + 							dictArray[j]['playDur'];
						dictArray[j]['trackText'].string_("StartTime = "++  							dictArray[j]['startTime'].value.round(0.01).asString ++
							"         EndTime = " 	  ++
							dictArray[j]['endTime'].value.round(0.01).asString);
					}, {nil})
				}},
			{
			dictArray[0]['totalTime'] = 0.1;
			10.do{arg i;
				var j;
				j = i + 1;
				(dictArray[j]['file'].notNil).if({
					(dictArray[0]['totalTime'] < dictArray[j]['endTime']).if({
						dictArray[0]['totalTime'] = dictArray[j]['endTime'];
						},{nil})},
					{nil});
				};
			10.do{arg i;
				var j;
				j = i + 1;
				(dictArray[j]['file'].notNil).if({
				dictArray[j]['trackRange'].activeLo_(dictArray[j]['globalStart'].value /					dictArray[0]['totalTime'])
					.activeHi_((dictArray[j]['globalStart'].value + dictArray[j]['playDur']) / 					dictArray[0]['totalTime']);
						},{nil});
				};
			dictArray[track]['startTime'] = dictArray[track]['globalStart'].value;
			dictArray[track]['endTime'] = dictArray[track]['globalStart'].value + 				dictArray[track]['playDur'];
			dictArray[track]['trackText'].string_("StartTime = "++  				dictArray[track]['startTime'].value.round(0.01).asString ++
				"         EndTime = " 	  ++
				dictArray[track]['endTime'].value.round(0.01).asString);
			});
};

skipTimeFunc = {arg track;
	var skip, dur, shorten;
	dur = dictArray[track]['dur'].value;
	skip = dictArray[track]['skipTime'].value;
	shorten = dictArray[track]['shortenFile'].value;
	dictArray[track].put('playDur', dur - (skip + shorten));
	dictArray[track]['trackRange'].hi_(((dictArray[track]['trackRange'].lo *
		dictArray[0]['totalTime']) + dictArray[track]['playDur'].value) /
		dictArray[0]['totalTime'].value);
	dictArray[track].put('endTime', dictArray[track]['startTime'].value + 		dictArray[track]['playDur'].value);
	dictArray[track]['trackText'].string_("StartTime = "++  		dictArray[track]['startTime'].value.round(0.01).asString ++
		"         EndTime = " 		++ dictArray[track]['endTime'].value.round(0.01).asString);
};

trackRangeFunc = {	arg track;
	dictArray[track]['globalStart'].value_((dictArray[track]['trackRange'].lo *
		dictArray[0]['totalTime']).round(0.01));
	dictArray[track]['startTime'] = dictArray[track]['trackRange'].lo * dictArray[0]['totalTime'];
	dictArray[track]['endTime'] = (dictArray[track]['trackRange'].lo * dictArray[0]['totalTime']) +
		 dictArray[track]['playDur'];
	dictArray[track]['trackText'].string_("StartTime = "++
		dictArray[track]['startTime'].value.round(0.01).asString ++"         EndTime = " ++
		dictArray[track]['endTime'].value.round(0.01).asString);
};

ampSliderFunc = {arg track;
	dictArray[track]['ampNumber'].value_(
		(dictArray[track]['ampSlider'].value * 2).ampdb.round(0.1));
	dictArray[track].put('amp', dictArray[track]['ampSlider'].value * 2);
	(play.value == 1).if({
		s.sendMsg(\n_set, 1000 + track, \amp, dictArray[track]['ampSlider'].value * 2)},
		{nil});
};

ampNumberFunc = {arg track;
	dictArray[track]['ampSlider'].value_((dictArray[track]['ampNumber'].value.dbamp * 0.5));
	dictArray[track].put('amp', dictArray[track]['ampNumber'].value.dbamp);
	(play.value == 1).if({
		s.sendMsg(\n_set, 1000 + track, \amp, dictArray[track]['ampNumber'].value.dbamp)},
		{nil});
};

muteFunc = {arg track;
	var mute;
	mute = dictArray[track]['mute'].value;
	(play.value == 1).if({
		s.sendMsg(\n_set, 1000 + track, \mute, mute)},
		{nil});
	};

// a function for caluclating samples for envelopes

envVals={	arg env, index, base=1, exp;
	var inds, vals, x1, x2, y1, y2, m, b, x, ind;
	#inds, vals = env.clump(2).flop;
	inds = inds.add(index).sort;
	ind = inds.indexOf(index);
	inds.remove(index);
	vals = exp.notNil.if({ vals.log }, { vals**base.reciprocal });
	x = if( ind==0, { 1 }, { if(ind>=inds.size, { inds.size-1 }, { ind }) });
	x1 = inds.at(x-1);
	x2 = inds.at(x);
	y1 = vals.at(x-1);
	y2 = vals.at(x);
	m = (y2-y1)/(x2-x1);
	b = y2 - (x2 * m);
	exp.notNil.if( { ((m * index) + b).exp }, { ((m * index) + b)**base });
};

buildEnvSig = {arg track;
	var envArray, base, envSig, exp;
	#envArray, base = dictArray[track]['env'].string.interpret;
	exp = (dictArray[track]['base'].value==1).if({true}, {nil});
	//100+track = env for track buffer track1 = 101
	Routine.run({
		s.sync(c);
		s.sendMsgSync(c, \b_alloc, 100 + track, 1024);
		envSig = Signal.newClear(1024).waveFill({arg x, i; envVals.value(envArray, x, base, exp)},
			0, 1).postln;
		s.listSendMsg([\b_setn, 100 + track, 0, 1024] ++ envSig);
	});
};

// open a Cocoa window to choose a file

getfile = {arg track, routine, button;
	CocoaDialog.getPaths({ arg paths;
		paths.do({ arg file;
			filefunc.value(file, 0, track, routine);
			})
			},{
			button.value_(0);
			"cancelled".postln;
		});
	};

// a function to get a files vitals, and store them in an array

filefunc = {arg file, start, track, routine;
	var sf, duration, numFrames, sampleRate, sfdur;
	sf = SoundFile.new; //this gets some basic info for each track
	sf.openRead(file); //read the file
	sampleRate = sf.sampleRate; //get the sample rate
	numFrames = sf.numFrames; //numFrames
	duration = (numFrames/sampleRate); //calculate the duration
	//fill in the info in its Dictionary
	dictArray[track].put('file', file);
	dictArray[track].put('dur', duration);
	dictArray[track].put('amp', 1); //set amp
	dictArray[track].put('sampleRate', sampleRate);
	dictArray[track].put('playDur', duration); //duration of track after skiptime
	sfdur = dictArray[track]['dur'];
	(sfdur >= dictArray[0]['totalTime'].value).if({dictArray[0].put('totalTime', sfdur)},
		{nil});
	dictArray[track]['trackRange'].activeLo_(dictArray[track]['globalStart'].value/
			dictArray[0]['totalTime'])
		.activeHi_((dictArray[track]['globalStart'].value+sfdur)/dictArray[0]['totalTime']);
	dictArray[track]['mute'].value_(1.0);
	dictArray[track]['filename'].string_(fileNameFunc.value(track));
	dictArray[track]['env'].string_("[[0, 1, 1, 1], 1]");
	buildEnvSig.value(track);
	};

trackRout = [
	//clear out the array of info for this track
	{arg track;
		dictArray[track].put('file', nil);
		dictArray[track].put('sampleRate', 0);
		dictArray[track].put('playDur', 0);
		dictArray[track].put('endTime', 0);
		dictArray[track].put('dur', 0);
		dictArray[track]['ampSlider'].value_(1);
		dictArray[track]['skipTime'].value_(0.0);
		dictArray[track]['trackRange'].activeLo_(0.0);
		dictArray[track]['trackRange'].activeHi_(0.0);
		dictArray[track]['trackText'].string_("StartTime = "++  0.0.asString ++
			"         EndTime = " ++ 0.0.asString);
		dictArray[track]['mute'].value_(0.0);
		dictArray[track]['env'].string_("");
		dictArray[track]['base'].value_(0);
		dictArray[track]['filename'].string_("");
		s.sendMsg(\b_free, 100 + track);
		"cleared".postln},
	{arg track;
		var sfdur;
		getfile.value(track, trackRout, dictArray[track]['loadFile'])
	}
		];

fileNameFunc = {arg track;
	var len, delim='/', ext='.', ind1, ind2, sym, arr, ind;
	var path;
	path = dictArray[track]['file'].asString; //"/Users/monk/Desktop/test.aif";
	len = path.size;
	arr = Array.fill(len, {arg i; path[i].asSymbol });
	ind1 = len - arr.reverse.indexOf(delim);
	ind = arr.reverse.indexOf(ext);
	ind2 = ind.notNil.if({ len - ind - 2}, { len - 1});
	sym = path.copyRange( ind1, ind2 ).postln;
};

fileName = {arg path;
	var len, delim='/', ext='.', ind1, ind2, sym, arr, ind;
	len = path.size;
	arr = Array.fill(len, {arg i; path[i].asSymbol });
	ind1 = len - arr.reverse.indexOf(delim);
	ind = arr.reverse.indexOf(ext);
	ind2 = ind.notNil.if({ len - ind - 2}, { len - 1});
	sym = path.copyRange( ind1, ind2 ).postln;
};

////////////////////////////////////////////////////////////////////
////////////////////////////// THE GUI /////////////////////////////
////////////////////////////////////////////////////////////////////

mixwin = SCWindow("BFormat Mixer", Rect(200, 512, 920, 800));

play = SCButton(mixwin, Rect(10, 630, 100, 20))
	.states_([
		["play", Color.black, Color(0.5, 0.9, 0.5, 0.5)],
		["stop", Color.black, Color(0.9, 0.5, 0.5, 0.5)]
		])
	.action_({playrout.next});

write = SCButton(mixwin, Rect(10, 655, 100, 20))
	.states_([
		["write file", Color.black, Color(0.5, 0.9, 0.5, 0.5)],
		])
	.action_({writefunc.value});

SCStaticText(mixwin, Rect(120, 630, 100, 20)).string = "Set Output Format";

//dictArray[0]['format'] = format of output

dictArray[0].put('formatButton', SCButton(mixwin, Rect(120, 655, 100, 20))
	.states_([
		["BFormat", Color.black, Color(0.5, 0.9, 0.5, 0.5)],
		["Stereo", Color.black, Color(0.9, 0.5, 0.5, 0.5)],
		["Quad", Color.black, Color(0.5, 0.5, 0.9, 0.5)],
		["UHJ", Color.black, Color(0.5, 0.5, 0.5, 0.9)],
		["fiveDot1", Color.black, Color(0.7, 0.5, 0.7, 0.9)],
		])
	.action_({format[dictArray[0]['formatButton'].value].value;
		formatNRT[dictArray[0]['formatButton'].value].value;
		numChannels[dictArray[0]['formatButton'].value].value;
		dictArray[0]['numChannels'].postln}));

//SET OUTPUT FORMAT... default to BF

dictArray[0].put('format', "mixer_play_bf");
dictArray[0].put('formatNRT', "mixer_playbuf_bf");
dictArray[0].put('numChannels', 3);

SCStaticText(mixwin, Rect(230, 630, 100, 20)).string = "Start Time In Mix";

dictArray[0].put('startTimeInMix', SCNumberBox(mixwin, Rect(230, 655, 40, 20))
	.value_(0.0)
	.action_({
		(dictArray[0]['totalTime'] == 0).if(
		{"No files are loaded... Start time can't be changed!".postln;
		dictArray[0]['startTimeInMix'].value_(0);
		},
		{nil})
		}));

//dictArray[0].put('totalTime', 0.1); //default totalTime

SCButton(mixwin, Rect(340, 630, 100, 20))
	.states_([
		["Save Current Mix", Color.black, Color(0.5, 0.9, 0.5, 0.5)]
		])
	.action_(
		{
		(dictArray[0]['totalTime'] == 0).if(
			{"No files are loaded... No mix to save!".postln;
			dictArray[0]['startTimeInMix'].value_(0);
			}, {savefunc.value})});

SCButton(mixwin, Rect(340, 655, 100, 20))
	.states_([
		["Load a Mix", Color.black, Color(0.5, 0.9, 0.5, 0.5)]
		])
	.action_({loadfunc.value});

SCButton(mixwin, Rect(560, 630, 100, 20))
	.states_([
		["Close All", Color.black, Color(0.5, 0.9, 0.5, 0.5)]
		])
	.action_({stopfunc.value;
		SystemClock.clear;
		mixwin.close;});

dictArray[0].put('globalAmpSlider', SCSlider(mixwin, Rect(10, 680, 395, 20))
	.value_(0.5)
	.action_({
		dictArray[0]['globalAmpNumber'].value_(
			(dictArray[0]['globalAmpSlider'].value * 2).ampdb.round(0.1));
		dictArray[0].put('globalAmp', dictArray[0]['globalAmpSlider'].value * 2);
		(play.value == 1).if({
			s.sendMsg(\n_set, 1000, \amp, dictArray[0]['globalAmpSlider'].value * 2)},
		{nil});
})
	.step_(0.01));

//number box for amp
dictArray[0].put('globalAmpNumber', SCNumberBox(mixwin, Rect(410, 680, 35, 18))
	.value_(1.0.ampdb.round(0.1))
	.action_({
	dictArray[0]['globalAmpSlider'].value_((dictArray[0]['globalAmpNumber'].value.dbamp * 0.5));
	dictArray[0].put('globalAmp', dictArray[0]['globalAmpNumber'].value.dbamp);
	(play.value == 1).if({
		s.sendMsg(\n_set, 1000, \amp, dictArray[0]['globalAmpNumber'].value.dbamp)},
		{nil});
}));

dictArray[0].put('env', SCTextField(mixwin, Rect(10, 710, 260, 18))
	.action_({buildEnvSig.value(0)}));


dictArray[0].put('base', SCButton(mixwin, Rect(280, 710, 40, 18))
	.states_([
		["base", Color.black, Color(0.5, 0.9, 0.5, 0.5)],
		["exp", Color.black, Color(0.9, 0.5, 0.5, 0.5)]])
	.action_({buildEnvSig.value(0)}));

dictArray[0]['env'].string_("[[0, 1, 1, 1], 1]");

//a clock (in seconds)

SCStaticText(mixwin, Rect(450, 630, 120, 18)).string = "Time In Seconds";
time = SCStaticText(mixwin,Rect(450,655,120,20));
time.font = Font(\Helvetica, 20);
texttask = Task({
	var newstart;
	SystemClock.sched(dictArray[0]['totalTime']-dictArray[0]['startTimeInMix'].value,
		{texttask.stop;
			AppClock.sched(0.1, {
			play.valueAction_(0);
			time.string_(dictArray[0]['totalTime'].round(0.1));
			})});
	newstart = (dictArray[0]['startTimeInMix'].value.round(0.1));
	inf.do({ arg index;
		{ time.string = (newstart.round(0.1).asString);
		}.defer;
			0.1005.wait;
			newstart = (newstart.value) + 0.1;
		})
	}, SystemClock);

10.do{arg i;
	var j;
	j = i + 1;

//store the starttime of the track for the Slider and text... need top be init to 0
	dictArray[j].put('startTime', 0);

//store the endTime of the track for the slider and text
	dictArray[j].put('endTime', 0);

	SCStaticText(mixwin, Rect(150, (20 * j) + (40 * i), 150, 18)).string = "File Start in Mix";
	SCStaticText(mixwin, Rect(310, (20 * j) + (40 * i), 70, 20)).string_("Amp (in dB)");
	SCStaticText(mixwin, Rect(270, (20 * j) +20 + (40 * i), 120, 20))
		.string_("Skip Time Into File");
	SCStaticText(mixwin, Rect(420, (20 * j) +20 + (40 * i), 120, 20))
		.string_("Shorten File");
	SCStaticText(mixwin, Rect(530, (20 * j) + 20 + (40 * i), 120, 20))
		.string_("Env");

dictArray[j].put('loadFile', SCButton(mixwin, Rect(20, (20 * j) + (40 * i), 110, 18))
	.states_([
		["load file", Color.black, Color(0.5, 0.9, 0.5, 0.5)],
		["", Color.black, Color(0.9, 0.5, 0.5, 0.5)]
		])
	.action_({
		trackRout[(dictArray[j]['loadFile'].value)].value(j);
	}));

dictArray[j].put('filename', SCStaticText(mixwin,
		Rect(23, (20 * j) + (40 * i), 110, 18))
	.string_("").align_(\center));

//show the range of the file dictArray[1]['trackRange']

dictArray[j].put('trackRange', SCRangeSlider(mixwin, Rect(60, (20 * j) + (40 * i) + 40, 720, 10)).step_(0)
	.action_({trackRangeFunc.value(j);
		globStartFunc.value(j)}));

dictArray[j].put('globalStart', SCNumberBox(mixwin, Rect(250, (20 * j) + (40 * i), 50, 18))
	.value_(0)
	.action_({globStartFunc.value(j)}));

//slider for amp adjust
dictArray[j].put('ampSlider', SCSlider(mixwin, Rect(380, (20 * j) + (40 * i), 395, 20))
	.value_(0.5)
	.action_({ampSliderFunc.value(j);})
	.step_(0.01));

//number box for amp
dictArray[j].put('ampNumber', SCNumberBox(mixwin, Rect(785, (20 * j) + (40 * i), 35, 18))
	.value_(1.0.ampdb.round(0.1))
	.action_({ampNumberFunc.value(j)}));

// a button to mute the track

dictArray[j].put('mute', SCButton(mixwin, Rect(830, (20 * j) + (40 * i), 40, 18))
	.value_(1.0)
	.states_([
		["play", Color.black, Color(0.5, 0.9, 0.5, 0.5)],
		["mute", Color.black, Color(0.9, 0.5, 0.5, 0.5)]
		])
	.action_({muteFunc.value(j)}));

//text info for each track

dictArray[j].put('trackText', SCStaticText(mixwin, Rect(20, (20 * j) +20 + (40 * i), 400, 18))
	.string_("StartTime = "++  dictArray[j]['startTime'].value.round(0.01).asString ++"         	EndTime = " 		++ dictArray[j]['endTime'].value.round(0.01).asString));

// the amount of time, in the track, to skip in

dictArray[j].put('skipTime', SCNumberBox(mixwin, Rect(380, (20 * j) + 20 + (40 * i), 30, 18))
	.value_(0.0)
	.action_({skipTimeFunc.value(j);
		globStartFunc.value(j)}));

dictArray[j].put('shortenFile', SCNumberBox(mixwin, Rect(490, (20 * j) + 20 + (40 * i), 30, 18))
	.value_(0.0)
	.action_({skipTimeFunc.value(j);
		globStartFunc.value(j)}));

dictArray[j].put('env', SCTextField(mixwin, Rect(560, (20 * j) + 20 + (40 * i), 260, 18))
	.action_({buildEnvSig.value(j)}));

dictArray[j].put('base', SCButton(mixwin, Rect(830, (20 * j) + 20 + (40 * i), 40, 18))
	.states_([
		["base", Color.black, Color(0.5, 0.9, 0.5, 0.5)],
		["exp", Color.black, Color(0.9, 0.5, 0.5, 0.5)]])
	.action_({buildEnvSig.value(j)}));

//name of the track


//seperate the tracks
SCStaticText(mixwin, Rect(10, (60* j), 760, 18))
.string_("_______________________________________________________________________________________________________________")
	.stringColor_(Color.black);
};


//bring the window to the front
mixwin.front;

}
}
