//Tendency {
//	var <high, <low;
//	
//	*new {|high, low|
//		^super.newCopyArgs(high, low);
//		}
//		
//	at {|time|
//		^(high.isKindOf(Env).if({high[time]}, {high})).rrand
//			(low.isKindOf(Env).if({low[time]}, {low}))
//	}
//			
//}
//
///*
//
//a = Tendency.new(Env([0, 1], [10]), 1);
//
//a[9.5]
//
//*/
