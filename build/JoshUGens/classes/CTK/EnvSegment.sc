+ Env {
	segment {arg start, end, normalize = false;
		var newenv, temp, inc, origsize, newtimes, newlevels, newcurves, tempcurve;
		temp = inc = 0;
		newenv = this.deepCopy;
		origsize = newenv.times.size;
		// check if endtime is before the end of the env... re-calculate the end
		if(end < newenv.times.sum, {
			while({
				temp < end
				}, {
				temp = temp + newenv.times[inc];
				inc = inc + 1;
				});
			newlevels = newenv.levels[0..inc];
			newtimes = newenv.times[0..(inc-1)];
			newcurves = if(newenv.curves.isArray, {
				newenv.curves[0..(inc-1)]
				}, {
				newenv.curves
				});
			// recalculate the last value
			newlevels[inc] = this[end];
			// recalculate last time
			newtimes[inc-1] = newtimes[inc-1] - (newtimes.sum - end);
			}, {
			newlevels = newenv.levels ++ this[end];
			newtimes = newenv.times ++ (end - newenv.times.sum);
			newcurves = if(newenv.curves.isArray, {
				newenv.curves ++ \lin
				}, {
				newenv.curves
				}); 
			});
		// reset temp and inc
		temp = inc = 0;
		// check if starttime is greater then 0... recalculate first part of env
		if(start > 0, {
			temp = newtimes[inc]; //newenv.times[inc];
//			("Temp = " ++ temp).postln;
			while ({
				temp < start
				}, {
				inc = inc + 1;
				temp = temp + newenv.times[inc];
				});
			(inc+1).do({
				newlevels.removeAt(0);
				newtimes.removeAt(0);
				newcurves.isArray.if({tempcurve = newcurves.removeAt(0)});
				});
			newlevels = [newenv[start]] ++ newlevels;
			newtimes = [(start - temp).abs] ++ newtimes;
			newcurves = if(newcurves.isArray, {
				[tempcurve] ++ newcurves
				}, {
				newcurves
				});
			});
		normalize.if({newtimes = newtimes.normalizeSum});
		^Env(newlevels, newtimes, newcurves);
	}
}
/*
a = Env([0, 1, 0], [5, 5], [\sin, \lin]);
b = a.segment(2, 7, true);
b.postcs
*/
