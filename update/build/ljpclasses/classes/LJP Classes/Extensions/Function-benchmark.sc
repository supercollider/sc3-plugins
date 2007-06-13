+Function{
	// time in ms to evaluate function num times
	 benchmark{ arg num=1;
		var time1, time2, total;
		total = 0;
		num.do({ arg i;
			time1 = Main.elapsedTime;
			this.value;
			time2 = Main.elapsedTime;
			total = total + time2 - time1;
		});
		^(total*1000)
	}
}