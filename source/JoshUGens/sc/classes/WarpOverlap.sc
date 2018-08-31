WarpOverlap {
	*ar {arg numChans = 1, buffer, overlaps = 4, starttime = 0, stretch = 1, harm = 1,
			windowsize = 6, step = 1, warpwindowsize = 0.11, warpwindowoverlaps = 8,
			warpwindowran = 0.1, mul = 1, add = 0;
		var tris, saws, counter, trigger, pointers, warps, warpharm, harmarray,
			warpwindowsizearray, warpwindowoverlapsarray, warpwindowranarray;
		trigger = Impulse.kr(windowsize.reciprocal,
			Array.fill(overlaps, {arg i; (overlaps - i) / overlaps})
			);
		tris = SinOsc.kr(windowsize.reciprocal,
			Array.fill(overlaps, {arg i; (((overlaps - i) * 2pi) / overlaps) + 1.5pi}))
			.range(-0.1, 0.9).max(0); // sin envs
		counter = Stepper.kr(trigger, 0,
			Array.fill(overlaps, {arg i; (i == 0).if({0},
				{(overlaps-i) * ((-1*windowsize*step)/overlaps)})}) + starttime,
			BufDur.kr(buffer), (windowsize * step));
		saws = LFSaw.kr(windowsize.reciprocal,
			Array.fill(overlaps, {arg i; (((overlaps - i) * 2) / overlaps)} + 1)			).range(0, windowsize / stretch);
		pointers = (counter + saws) / BufDur.kr(buffer);
		harmarray = harm.isKindOf(Ref).if({
			Array.fill(overlaps, {arg i; harm.dereference.wrapAt(i)})
			}, {
			harm
			});
		warpwindowsizearray = warpwindowsize.isKindOf(Ref).if({
			Array.fill(overlaps, {arg i; warpwindowsize.dereference.wrapAt(i)})
			}, {
			warpwindowsize
			});
		warpwindowoverlapsarray = warpwindowoverlaps.isKindOf(Ref).if({
			Array.fill(overlaps, {arg i; warpwindowoverlaps.dereference.wrapAt(i)})
			}, {
			warpwindowoverlaps
			});
		warpwindowranarray = warpwindowran.isKindOf(Ref).if({
			Array.fill(overlaps, {arg i; warpwindowran.dereference.wrapAt(i)})
			}, {
			warpwindowran
			});
		warps = Warp1.ar(numChans, buffer, pointers, harmarray, warpwindowsizearray,
			-1, warpwindowoverlapsarray, warpwindowranarray);
		^(warps * tris).sum.madd(mul, add);
		}
}
