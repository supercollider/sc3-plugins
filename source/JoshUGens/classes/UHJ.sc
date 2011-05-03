BtoUHJ {
	*ar { arg wsig, xsig, ysig;
//		L = 0.5 * (0.9397*W + 0.1856*X - j*0.342*W + j*0.5099*X + 0.655*Y)
//		R = 0.5 * (0.9397*W+ 0.1856*X + j*0.342*W - j*0.5099*X - 0.655*Y)
		var wj, xj, yj, w, x, y;
		#w, wj = Shift90.filt(wsig);
		#x, xj = Shift90.filt(xsig);
		#y, yj = Shift90.filt(ysig) * 0.655;
		w = 0.9397 * w;
		wj = 0.342 * wj;
		x = 0.1856 * x;
		xj = 0.5099 * xj;
		^[ w + x - wj + xj + y,
		   w + x + wj - xj - y] * 0.5;
	}
}



UHJtoB {
	*ar { arg leftsig, rightsig;
//		W = 0.5*(0.982*L + 0.982*R + j*0.164*L - j*0.164*R)
//		X = 0.5*(0.419*L + 0.419*R - j*0.828*L + j*0.828*R)
//		Y = 0.5*(0.763*L - 0.763*R + j*0.385*L + j*0.385*R)
		var l, r, lj, rj, w, x, y;
		#l, lj = Shift90.filt(leftsig);
		#r, rj = Shift90.filt(rightsig);
		w = (0.982*l) + (0.982*r) + (0.164*lj) - (0.164*rj);
		x = (0.419*l) + (0.419*r) - (0.828*lj) + (0.828*rj);
		y = (0.763*l) - (0.763*r) + (0.385*lj) + (0.385*rj);
		^[w,x,y]*0.5;
	}
}

Shift90 {
	*filt { arg in;
		var ay6, ay12;

		ay6 = FOS.ar( FOS.ar( FOS.ar( FOS.ar( FOS.ar( FOS.ar(in,
			-0.99922900, 1, 0.99922900),
			-0.99415879, 1, 0.99415879),
			-0.97643625, 1, 0.97643625),
			-0.90871182, 1, 0.90871182),
			-0.67793563, 1, 0.67793563),
			-0.07921014, 1, 0.07921014);

		ay12 = FOS.ar( FOS.ar( FOS.ar( FOS.ar( FOS.ar( FOS.ar(in,
			-0.99732703, 1, 0.99732703),
			-0.98817270, 1, 0.98817270),
			-0.95336481, 1, 0.95336481),
			-0.82519599, 1, 0.82519599),
			-0.43905106, 1, 0.43905106),
			0.49495651, 1, -0.49495651);

		^[ay6,ay12]
	}

	*fft { arg src, fftsize, numffts;
		// http://www.brokersys.com/~pjmac/dsp/hilbert.html
		// z(t) = ifft[ B(i) * fft(x(t)) ]
		// where:
		// B(i) = 2, for i = {0, N/2-1}
		// B(i) = 0, for i = {N/2, N-1}
		var fftoffsets, window, cosineTable, fft, ifft, filterWindow;

		fftsize = fftsize ? 2048;
		numffts = numffts ? 2;

		cosineTable = Signal.fftCosTable(fftsize);

		window = Signal.hanningWindow(fftsize);
		fftoffsets = Array.series(numffts, 0, fftsize/numffts);
		filterWindow = Signal.fill(fftsize, { arg n; (n >= (fftsize/2)).if(0,2) });

		fft =  FFT.a2sr(fftsize, fftoffsets, cosineTable, window, nil, src, 0.0);
		ifft = IFFT.s2ar(cosineTable, filterWindow, nil, fft.real, fft.imag);

		^Mix.new( ifft.imag )/(numffts/2)
	}
}