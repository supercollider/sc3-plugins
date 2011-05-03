
Sequencer {
	var <>func, <target, <>id, <>count=0;
	var <resp;
	*new { arg func, target, id;
		^super.newCopyArgs(func, target.asTarget, id).rinit

	}
	rinit {
		resp = OSCresponder(target.server.addr, '/tr', { arg time, addr, msg;

			if(id.isNil or: { msg.at(0) == id }, { func.value(count,time,msg); count = count + 1 });
		});
		resp.add;
	}

	free {
		resp.remove
	}


}