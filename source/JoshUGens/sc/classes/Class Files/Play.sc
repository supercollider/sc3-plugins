Play {
	*new {arg ugenFn;
		var s, w;
		s = Server.internal;
		s.postln;
		SynthDef("simple",ugenFn).load(s);
		s.sendMsg("/s_new", "simple", 2002, 1, 0);
//		w=SCWindow("Play", Rect(128, 64, 340, 360));
//		w.view.decorator = FlowLayout(w.view.bounds);
//		SCButton(w, Rect(0,0, 75, 24))
//			.action_({s.sendMsg("/n_free", 2002); s.quit;  w.close;})
//			.states_([["Stop ", Color.black, Color.rand],["Stop ", Color.white, Color.red]])
//			.keyDownAction_({arg ... args; args.postln; s.sendMsg("/n_free", 2002); s.quit;  w.close;});
//		w.onClose_({arg ... args; args.postln; s.sendMsg("/n_free", 2002); s.quit; w.close;});
//		w.front;
	}
}