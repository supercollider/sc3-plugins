+ SimpleNumber{

	// receiver is a time in seconds
	// returns string "hh:mm:ss:ttt" where t is milliseconds
	asTimeStringLJP{
		var decimal, hours, minutes, seconds, mseconds, string;
		decimal = this.asInteger;

		hours = (decimal.div(3600)).asString;
		if(hours.size < 2, { hours = "0" ++ hours });

		minutes = (decimal.div(60) % 60).asString;
		if(minutes.size < 2, { minutes = "0" ++ minutes });

		seconds = (decimal % 60).asString;
		if(seconds.size < 2, { seconds = "0" ++ seconds });

		mseconds = (this.frac*1000).round.asInteger.asString;
		if(mseconds.size < 3, {
			mseconds = "0" ++ mseconds;
			if(mseconds.size < 3, { mseconds = "0" ++ mseconds });
		});

		^(hours ++ ":" ++ minutes ++ ":" ++ seconds ++ ":" ++ mseconds).asString
	}
}
