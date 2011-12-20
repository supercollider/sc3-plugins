BBlockerProgram {
	classvar <instructions, descs;
	var <program, <>fillUpRandom = false;
	var <collection;
	
	*new {|program|
		^super.new.init(program)	
	}


	init {|argProgram|
		this.class.initStates;
		
		program = argProgram.asArray;
	}
	
	// search in descs
	*find{|identifier|
		this.initStates;
		
		identifier = identifier.asString.toLower;
		^descs.select{|value, key|  
			key.asString.toLower.contains(identifier) ||{value.toLower.contains(identifier)}}.keys
	}
	
	*descOf{|key|
		this.descs[key]
	}
	
	*printDescs {
		this.descs.keysValuesDo{|key, val|
			"%: %\n".postf((key.asString ++ "    ")[0..3], val)
		}	
	}
	
	*descs {
		this.initStates;
		^descs;	
	}
	
	makeCollection {
		var result;
		
		result = program.collect{|instr| this.pr_translate(instr)};
		
		collection = (result ++ fillUpRandom.if({{256.rand}!(256 - program.size)}, {0!(256 - program.size)}));
		
		^collection;
	}
	
	makeBuffer {|server, action|
		server = server ?? {Server.default};
		^Buffer.loadCollection(server, this.makeCollection, 1, action) 
	}
	
	fillBuffer {|buffer|
		buffer.loadCollection(this.makeCollection);
	}

	decompile {|forceNew = false|
		forceNew.if({
			collection = this.makeCollection;
		}, {
			collection = collection ? this.makeCollection;
		});
		^(collection.collect{|val| this.pr_rTranslate(val)})
	}

	pr_translate {|val|
		^(this.class.instructions[val] ?? {val%256})	
	}
	
	pr_rTranslate {|val|
		^(this.class.instructions.findKeyForValue(val) ? val)
	}
	
	*initStates {
		instructions = instructions ?? {
			(
				\NOP:  0,
				\ORG:  1,
				\EQU:  2,
				\JMP:  3,
				\JMPZ: 4,
				\PSHL: 5,
				\PSH:  6,
				\PSHI: 7,
				\POP:  8,
				\POPI: 9,
				\ADD: 10,
				\SUB: 11,
				\INC: 12,
				\DEC: 13,
				\AND: 14,
				\OR: 15,
				\XOR: 16,
				\NOT: 17,
				\ROR: 18,
				\ROL: 19,
				\PIP: 20,
				\PDP: 21,
				\DUP: 22,
				\NOTE: 23,
				\VOX: 24,
				\STOP: 25
			)			
		};
				
		descs = descs ? (
			NOP:  "do nothing",
			ORG:  "define relative origin address",
			EQU:  "compare two topmost elements on stack, pop them, push result (0 or 1)",
			JMP:  "jump to address specified in following slot",
			JMPZ: "if stack returns 0, jump to address specified in following slot",
			PSHL: "push value of slot specified by adress in following slot",
			PSH:  "push value of slot specified by address specified by address in following slot",
			PSHI: "push value like PSH but one more encapsulation",
			POP:   "pop item from stack to slot specified by address in following slot",
			POPI:  "pop item from stack to slot specified by address specified by address in following slot",
			ADD: "perform addition (+) on first two elements on stack, pop them, push result",
			SUB: "perform subtraction (-) on first two elements on the stack, pop them, push result",
			INC: "increment value on stack",
			DEC: "decrement value on stack",
			AND: "perform bit-wise AND on first two elements on stack, pop them, push result",
			OR:  "perform bit-wise  OR on first two elements on stack, pop them, push result",
			XOR: "perform bit-wise XOR on first two elements on stack, pop them, push result",
			NOT: "perform bit-wise NOT on first element on stack, pop it, push result",
			ROR: "perform right shift  on first two elements on stack, pop them, push result",
			ROL: "perform left shift   on first two elements on stack, pop them, push result",
			PIP: "increment value specified by address in following slot (no stack involved)",
			PDP: "decrement value specified by address in following slot (no stack involved)",
			DUP: "push a copy (duplicate) of topmost value to the stack",
			NOTE: "play note (here: NOP)",
			VOX:  "play vox  (here: NOP)",
			STOP: "stop program (here: NOP)"
		);
	}
	
	ar {|rate, using, leak = false, vol = 1| // using is either Server or Buffer
		var buffer, server;

		rate = rate ? 20000;
		
		using.isKindOf(Buffer).if({
			buffer = using;
			server = buffer.server;
			
			this.fillBuffer(buffer);
		}, {
			server = using ? Server.default;
			buffer = this.makeCollection(server);
		});
		
		^{
			var bufnum = buffer.isKindOf(Buffer).if({
				buffer.bufnum;
			}, {
				buffer.as(LocalBuf)
			});
			
			var src = Demand.ar(
				Impulse.ar(rate), 
				0, 
				DetaBlockerBuf(bufnum, 0)
			);

			leak.if{
				src = LeakDC.ar(src);
			};
			
			src * vol;
			
		}
	}
	
	play {|rate, using, leak = false, vol = 0.1| // using is either Server or Buffer
		^this.ar(rate, using, leak, vol).play
	}
	
	plot {|rate, using, leak = false, duration = 1| // using is either Server or Buffer
		^this.ar(rate, using, leak).plot2(duration)
	}
	
	scope {|rate, using, leak = false, vol = 0.1| // using is either Server or Buffer
		^this.ar(rate, using, leak, vol).scope
	}
}



