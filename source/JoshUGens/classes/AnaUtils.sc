LPCAna {
	var <path, <sf, <anaBuf, <tmpBuf, <frames, <sr;
	var <nPoles, <frameSize, <hop, <channel;
	var <hopSamps, <data, <coefs, <>resrms, <>origrms, <>err, <>pchcps, <clock;
	var lm;
	var minFreq, maxFreq, <inc, sfDur, test;
	var rFrameSize, centerFreqs, fftSize, fftMul, real, imag, cosTable, lowBin, highBin;


	*new {arg path;
		^super.newCopyArgs(path).initLPCAna;
		}

	initLPCAna {
		sf = SoundFile.new;
		test = sf.openRead(path);
		test.if({
			sr = sf.sampleRate;
			sf.close;
			sfDur = sf.duration;
			}, {
			("Your soundfile at: "++path++" could not be opened").warn;
			});
		}

	ana {arg nPoles = 35, frameSize = 400, hop = 0.5, channel = 0, minFreq = 70, maxFreq = 200,
			check = 0, conditionSignal = 0, completion;
		var test, remOne, wait;
		clock = TempoClock.new;
		data = Array.new;
		err = Array.new;
		coefs = Array.new;
		pchcps = Array.new;
		resrms = Array.new;
		origrms = Array.new;
		lm = 0;
		inc = 0;
		wait = nPoles * 0.001;
		(check > 0).if({wait = wait * 3});
		this.initAna(nPoles, frameSize, hop, channel, minFreq, maxFreq);
		// grab the number of samples needed
		sf.openRead(path);
		Routine.run({
			((sf.numFrames / hopSamps).floor).do({
				sf.readData(tmpBuf);
				// hop BACK from where we were for the FFT pitch tracking
				sf.seek(frameSize * -1, 1);
				anaBuf = tmpBuf.clump(sf.numChannels).flop[channel];
				(anaBuf.size < frameSize).if({
					test = false;
					anaBuf = anaBuf ++ Array.fill(frameSize - anaBuf.size, {0});
					}, {
					test = true;
					inc = inc + 1;
					});
				("Analyzing Frame "++ inc).postln;
				this.analyzeWindow(check, conditionSignal);
				wait.wait;
				});
			"Closing the soundfile".postln; sf.close; clock.stop;
			inc.do({arg i;
				remOne = coefs[i].copy;
				remOne.removeAt(0);
				data = data ++ [[resrms[i], origrms[i], err[i], pchcps[i]] ++
					// for now, this is needed to make it work with the CSound based file format
					remOne.reverse.neg].flat;
				});
			completion.value;
			});
		}

	initAna {arg argNPoles, argFrameSize, argHop, argChannel, argMinFreq, argMaxFreq;
		var tmp, curCF;
		tmp = 0;
		nPoles = argNPoles;
		frameSize = argFrameSize;
		hop = argHop;
		channel = argChannel;
		tmpBuf = FloatArray.newClear(frameSize * sf.numChannels);
		hopSamps = frameSize * hop;
		minFreq = argMinFreq;
		maxFreq = argMaxFreq;
		fftSize = frameSize.nextPowerOfTwo;
		real = Signal.newClear(fftSize);
		imag = Signal.fill(fftSize, {0.0});
		cosTable = Signal.fftCosTable(fftSize);
		rFrameSize = frameSize.reciprocal;
		centerFreqs = Array.fill(fftSize, {arg i; i * rFrameSize * (sr * 0.5)});
		while({
			tmp = tmp + 1;
			curCF = centerFreqs[tmp];
			curCF < minFreq;
			});
		lowBin = tmp;
		while({
			tmp = tmp + 1;
			curCF = centerFreqs[tmp];
			curCF < maxFreq;
			});
		highBin = tmp - 2;
		fftMul = rFrameSize * pi.reciprocal * sr * 0.5
		}

	// from Moore
	// 	N - number of samples (window size)
	//	x - the window data
	//	M - number of coefs (M+1 are calculated)
	//	b - the coefs. b[0] is always 1
	//	a[0] - the residual
	//	S - check for stability - 1 - reflect poles, 2 - put poles on the unit circle

	analyzeWindow {arg check, conditionSignal;
		var x, n, b, m, s, at, a0, rx, rc, orms, pch;
		orms = this.calcOrigRMS;
		pch = this.getpch;
		m = nPoles;
		x = anaBuf.copy;
		case
			{conditionSignal == 0}
			{nil}
			{conditionSignal == 1}
			{x = x.lowpass}
			{conditionSignal == 2}
			{x = x.lowpassCS}
			{true}
			{nil};
		n = frameSize;
		b = Array.fill(m + 1, {0});
		s = check;
		rx = Array.fill(m + 2, 0);
		rc = Array.fill(m + 2, 0);
		for(0, m + 1, {arg i;
			for(0, n - i - 1, {arg j;
				rx[i] = rx[i] + (x[j] * x[i + j]);
				})
			});
		b[0] = 1;
		rc[0] = (rx[0] != 0).if({
			(rx[1] * -1) / rx[0]
			}, {
			0
			});
		b[1] = rc[0];
		// no need to zero out the rest of b!
		a0 = rx[0] + (rx[1]*rc[0]);
		for(1, m-1, {arg i;
			for(s = 0, i, {arg j;
				s = s + (rx[i - j + 1] * b[j]);
				});
			rc[i] = (a0 != 0.0).if({
				(s * -1) / a0;
				}, {
				0
				});
			for(1, (i + 1) >> 1, {arg j;
				at = b[j] + (rc[i] * b[i - j + 1]);
				b[i - j + 1] = b[i - j + 1] + (rc[i] * b[j]);
				b[j] = at;
				});
			b[i+1] = rc[i];
			a0 = a0 + (rc[i] * s);
			});
		(check > 0).if({
			b = this.checkRoots(b, check);
			});
		coefs = coefs.add(b);
		err = err.add(a0);
		pchcps = pchcps.add(pch);
		resrms = resrms.add((a0 * a0).sqrt); // not correct - figure out what this is
		origrms = origrms.add(orms);
		}

	calcOrigRMS {
		var tmp = 0;
		anaBuf.do({arg me;
			tmp = tmp + (me * me)
			});
		tmp = tmp / (anaBuf.size);
		^tmp.sqrt;
		}

	checkRoots {arg theseCoefs, check;
		var ad, roots, x, m;
		m = nPoles;
		ad = Array.newClear(nPoles + 1);
		for(0, nPoles, {arg i;
			ad[m-i] = Complex(theseCoefs[i], 0);
			});
		roots = this.findRoots(ad);
		for(0, m - 1, {arg i;
			((x = roots[i].rho) > 1.0).if({
				case
					{check == 1}
					{roots[i] = (1.0 / roots[i].conjugate)}
					{true}
					{roots[i].real = (roots[i].real / x);
						roots[i].imag = (roots[i].imag / x)}
				});
			});
		ad[0] = 0 - roots[0];
		ad[1] = Complex(1, 0);
		for(1, m - 1, {arg j;
			ad[j+1] = Complex(1, 0);
			for(j, 1, {arg i;
				ad[i] = ad[i-1] - (roots[j] * ad[i]);
				});
			ad[0] = 0 - (roots[j] * ad[0]);
		});
		for(0, m-1, {arg i;
			theseCoefs[m - i] = ad[i].real;
			});
		^theseCoefs;
		}

	findRoots {arg a; //, roots;
		var eps, ad, x, b, c, m, lm, roots;
		lm = m = nPoles;
		eps = 1e-6;
		roots = Array.newClear(a.size + 1);
		ad = Array.newClear(a.size + 1);
		a.do({arg me, i;
			ad[i] = me;
			});
		for(nPoles, 1, {arg j;
			x = this.calcLaguerre(ad, j, Complex(0, 0), eps, 0);
			(x.imag.abs <= (2 * eps).pow(2 * x.real.abs)).if({
				x.imag_(0.0);
				});
			roots[j-1] = x;
			b = ad[j];
			for(j - 1, 0, {arg jj;
				c = ad[jj];
				ad[jj] = b;
				b = (x * b) + c;
				});
			});
		for(0, m-1, {arg j;
			roots[j] = this.calcLaguerre(a, nPoles, roots[j], eps, 1);
			});
		for(0, m-2, {arg i;
			for(i + 1, m - 1, {arg j;
				(roots[j].real < roots[i].real).if({
					x = roots[i];
					roots[i] = roots[j];
					roots[j] = x;
					});
				});
			});
		^roots;
		}

	calcLaguerre {arg a, m, x, eps, p;
		var dx, x1, b, d, f, g, h, j, mh, gp, gm, g2, q, sq, npol, dxold, cdx, tiny;
		var zero;
		mh = Complex(0, 0);
		q = Complex(0, 0);
		tiny = 1e-15;
		(p > 0).if({
			dxold = x.rho;
			npol = 0;
			});
		100.do({arg i;
			b = a[m];
			d = Complex(0, 0);
			f = Complex(0, 0);
			for(m-1, 0, {arg j;
				f = d + (x * f);
				d = b + (x * d);
				b = a[j] + (x * b);
				});
			(b.rho <= tiny).if({
				dx = Complex(0, 0);
				}, {
				((d.rho <= tiny) and: {f.rho <= tiny}).if({
					q = b / a[m];
					dx = Complex(q.rho.pow(1/m), 0);
					}, {
					g = d / b;
					g2 = g * g;
					h = g2 - (2 * (f/b));
					sq = ((m-1) * ((m * h) - g2)).sqrt;
					gp = g + sq;
					gm = g - sq;
					(gp.rho < gm.rho).if({gp = gm});
					q = Complex(m, 0);
					dx = q / gp;
					})
				});
			x1 = x - dx;
			(x1 == x).if({
				^x
				});
			x = x1;
			(p > 0).if({
				npol = npol + 1;
				cdx = dx.rho;
				(npol > 9 and: {cdx >= dxold}).if({
					^x
					});
				dxold = cdx;
				}, {
				(dx.rho <= (eps * x.rho)).if({
					^x
					})
				});
			});
		"laguerre failure. This frame may not be stable (but it is probably fine).".warn;
		^x;
		}

	// lowpass the error data to smooth it!
	lperr {arg numTurns = 1;
		numTurns.do({err = err.lowpass});
		}

	lprms {arg numTurns = 1;
		numTurns.do({origrms = origrms.lowpass});
		}

	lppch {arg numTurns = 1;
		var tmp;
		numTurns.do({
			tmp = pchcps[0];
			pchcps = pchcps.lowpass;
			pchcps[0] = tmp;
			})
		}

	saveToLPCFile {arg path;
		var file, remOne;
		file = File.new(path, "wb");
		file.putInt32LE(28);
		file.putInt32LE(999);
		file.putInt32LE(nPoles);
		file.putInt32LE(nPoles + 4);
		file.putFloatLE(sf.sampleRate * 2 / frameSize);
		file.putFloatLE(sf.sampleRate);
		file.putFloatLE(sf.duration);
		data = [];
		inc.do({arg i;
			remOne = coefs[i].copy;
			remOne.removeAt(0);
			data = data ++ [[resrms[i], origrms[i], err[i], pchcps[i]] ++
				// for now, this is needed to make it work with the CSound based file format
				remOne.reverse.neg].flat;
			});
		data.do({arg me;
			file.putFloatLE(me)
			});
		file.close;
		}

	saveToFiles {arg path;
		var lpcFile;
		path = path ?? {"/tmp/trashme.lpc"};
		data.notNil.if({
			this.saveToLPCFile(path);
			lpcFile = LPCFile(path).saveToFile(path++".aif");
			}, {
			"No data to save... analyze a file first!".warn;
			});
		}

	/* this is my own hacky attempt at doing FFT pitch tracking. It actually works OK! */
	getpch {
		var frame1Polar, frame2Polar, nBins, fft, polar, rho1, theta1, rho2, theta2, anaBin,
			centerFreq, phaseDif, freq;
		nBins = highBin - lowBin;
		frame1Polar = Array.newClear(nBins);
		frame2Polar = Array.newClear(nBins);
		// get the first FFT
		sf.readData(real);
		(real.size < fftSize).if({
			real = real ++ Signal.newClear(fftSize - real.size);
			});
		sf.seek(fftSize * -0.5, 1);
		fft = real.asSignal.fft(imag, cosTable);
		polar = fft.asPolar;
		rho1 = polar.rho[lowBin..highBin];
		theta1 = polar.theta[lowBin..highBin];
		sf.readData(real);
		(real.size < fftSize).if({
			real = real ++ Signal.newClear(fftSize - real.size);
			});
		fft = real.asSignal.fft(imag, cosTable);
		polar = fft.asPolar;
		rho2 = polar.rho[lowBin..highBin];
		theta2 = polar.theta[lowBin..highBin];
		// ready for the next analysis
		sf.seek(fftSize * -1.5, 1);
		sf.seek(hopSamps, 1);
		// find the index of maxItem in rho1... this is the bin to analyze
		anaBin = rho1.indexOf(rho1.maxItem);
		phaseDif = theta2[anaBin] - theta1[anaBin];
		// wrap the phase
		(phaseDif > pi).if({
			while({
				phaseDif = phaseDif - 2pi;
				phaseDif > pi
				})
			});
		(phaseDif <= pi.neg).if({
			while({
				phaseDif = phaseDif + 2pi;
				phaseDif <= pi.neg
				})
			});
		centerFreq = centerFreqs[anaBin + lowBin];
		freq = centerFreq + (phaseDif * fftMul);
		^freq.max(minFreq).min(maxFreq);
		}
}

PVAna {
	var <sfpath, <sf, <anaBuf, <tmpBuf, <frames, <sr;
	var <nBins, <hop, <channel;
	var <hopSamps, sfDur, test;

	*new {arg sfpath;
		^super.newCopyArgs(sfpath).initPVAna;
		}

	initPVAna {
		sf = SoundFile.new;
		test = sf.openRead(sfpath);
		test.if({
			sr = sf.sampleRate;
			sf.close;
			sfDur = sf.duration;
			}, {
			("Your soundfile at: "++sfpath++" could not be opened").warn;
			});
		}

	ana {arg filPath, fftSize = 2048, winType = 1, hop = 0.25, starttime = 0, duration = nil;
		var score, sd, pathTest;
		score = Score.new;
		sd = SynthDef("pvrec", { arg recBuf=1, soundBufnum=2, dur, start;
			var in, chain, bufnum, env;
			bufnum = LocalBuf.new(fftSize, 1);
			env = EnvGen.kr(Env([0, 1, 1, 0], [0.01, dur - 0.02, 0.01], \sin), doneAction: 2);
			//Line.kr(1, 1, dur, doneAction: 2);
			in = PlayBuf.ar(1, soundBufnum, BufRateScale.kr(soundBufnum),
				startPos: start * BufSampleRate.kr(soundBufnum), loop: 0) * env;
			// note the window type and overlaps... this is important for resynth parameters
			chain = FFT(bufnum, in, hop, winType);
			chain = PV_RecordBuf(chain, recBuf, 0, 1, 0, hop, winType);
			// no ouput ... simply save the analysis to recBuf
			}).load(Server.default);
		pathTest = PathName(filPath.dirname).isFolder;
		duration = duration ?? sfDur;
		pathTest.if({
			score.add(
				[0.0, [\b_alloc, 1, duration.calcPVRecSize(fftSize, hop, sf.sampleRate)],
					[\b_allocRead, 2, sfpath],
					[\s_new, \pvrec, 1000, 0, 1, \recBuf, 1, \soundBufnum, 2, \dur, duration,
						\start, starttime]]);
			score.add(
				[duration + 0.1, [\b_write, 1, filPath, 'wav', 'float']]
				);
			score.add(
				[duration + 0.2, 0]
				);
			score.recordNRT("/tmp/trashme", "/tmp/trashme.aif");
			}, {
			("The file name you provided: " ++ filPath ++ " doesn't appear to be a valid directory. No file was written.").warn;
			});


		}
}


+ Complex {
	sqrt {
		var mag, ang;
		mag = this.rho.sqrt;
		ang = (real != 0.0).if({
			imag.atan2(real) * 0.5;
			}, {
			0.0
			});
		^Complex(mag * ang.cos, mag * ang.sin);
		}
	}

+ ArrayedCollection {
	// a VERY simple low pass function to smooth a signal in an Array
	lowpass {
		var w1, w11, lpcoll;
		w11 = 0;
		lpcoll = this.collect({arg x, i;
			w1 = (x + w11) * 0.5;
			w11 = x;
			w1;
			});
		^lpcoll;
		}

	// a MUCH more complex low pass function from CSound lpanal, to be used with the
	// actual audio signal
	lowpassCS {
		var c, a1, c1, d1, a2, c2, d2, a3, c3, d3, c4;
		var w1, w2, w3, w4, temp, y;
		var w11, w12, w21, w22, w31, w32, w41;
		var lpcoll;
		w11 = w12 = w21 = w22 = w31 = w32 = w41 = 0;
		c = 0.00048175311;
		a1 = -1.89919924;
		c1 =  -1.92324804;
		d1 = 0.985720370;
		a2 = -1.86607670;
		c2 = -1.90075003;
		d2 = 0.948444690;
		a3 = -1.66423461;
		c3 = -1.87516686;
		d3 = 0.896241023;
		c4 = -0.930449120;
		lpcoll = this.collect({arg x, i;
			w1 = (c * x) - (c1 * w11) - (d1 * w12);
			temp = w1 + (a1 * w11) + w12;
			w12 = w11;
			w11 = w1;
			w2 = temp - (c2 * w21) - (d2 * w22);
			temp = w2 + (a2 * w21) + w22;
			w22 = w21;
			w21 = w2;
			w3 = temp - (c3 * w31) - (d3 * w32);
			temp = w3 + (a3 * w31) + w32;
			w32 = w31;
			w31 = w3;
			w4 = temp - (c4 * w41);
			y = w4 + w41;
			w41 = w4;
			y;
			});
		^lpcoll;
		}
	}
