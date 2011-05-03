// A phrase is an array that behaves like a stream, but allows
// you to manipulate its data on the fly.

// Every phrase has a region defined that determines both what
// array data responds to the operation methods and what section of
// the phrase will loop when it's streamed.

Phrase{

	var <initialPhrase, <>phrase;
	var <returnProbs, <returnBools, <>loopReturns;
	var <regionStart, <regionLength, <pos, <seed;
	var counter;

	*new{ arg phrase, returnProbs, loopReturns = true;
		if(returnProbs.isNil, {
			returnProbs = Array.fill(phrase.size, 1);
		});

		^this.newCopyArgs(
			phrase.copy, phrase,
			returnProbs, Array.newClear(phrase.size), loopReturns,
			0, phrase.size, 0, Date.seed,
			0
		).calcReturns
	}

	// randomly choose element from region
	choose{
		var index, returnItem;
		index = Array.newClear(regionLength);
		regionLength.do({ arg i;
			index[i] = (regionStart + i) % phrase.size;
		});
		pos = index.choose;

		if(loopReturns, {
			returnItem = returnBools[pos];
		},{
			returnItem = returnProbs[pos % returnProbs.size].coin;
		});

		if(returnItem, {
			^phrase[pos]
		},{
			^nil
		});
	}

	next{
		var prevPos, returnItem;
		prevPos = pos;
		//pos = (pos + 1) % length;
		// odd loop lengths don't work!!!
		//pos = ((pos + 1).wrap(regionStart, regionStart+regionLength-1)) % phrase.size;

		pos = (pos + 1) % phrase.size;
		counter = counter + 1;

		if(counter >= regionLength, {
			counter = 0;
			pos = regionStart;
		});

		if(loopReturns, {
			returnItem = returnBools[prevPos];
		},{
			returnItem = returnProbs[prevPos % returnProbs.size].coin;
		});

		if(returnItem, {
			^phrase[prevPos]
		},{
			^nil
		});
	}

	calcReturns{ arg anotherSeed;

		if(anotherSeed.isNil, {
			thisThread.randSeed = seed;
		},{
			thisThread.randSeed = anotherSeed;
		});

		phrase.size.do({ arg i;
			returnBools[i] = returnProbs[i % returnProbs.size].coin;
		});
	}

	// A function used for doing operations on current region
	regionFunc{ arg func ... sourceArrays;
		var temp;
		temp = Array.newClear(regionLength);

		sourceArrays.do({ arg arr;
			// extract region from source array
			regionLength.do({ arg i;
				temp[i] = arr[(regionStart + i)%(arr.size)];
			});

			// perform function on extracted region
			temp = func.value(temp);

			// copy modified region back into the source array
			regionLength.do({ arg j;
				arr[(regionStart + j)%(phrase.size)] = temp[j];
			});
		});
	}


	// Phrase operations
	// Note: these work on the region [regionStart, regionStart + regionLength]

	// change order of elements
	ascend{
		this.regionFunc({arg array; array.quickSort({arg a, b; a <= b}) }, phrase);
	}
	descend{
		this.regionFunc({arg array; array.quickSort({arg a, b; a >= b}) }, phrase);
	}
	permute{ arg nthPermutation;
		this.regionFunc({arg array; array.permute(nthPermutation) }, phrase, returnBools);
	}
	reverse{
		this.regionFunc({arg array; array.reverse }, phrase, returnBools);
	}
	rotate{ arg n=1;
		this.regionFunc({arg array; array.rotate(n) }, phrase, returnBools);
	}

	// change values of elements
	gaussian{ arg dev;
		thisThread.randSeed = seed;
		this.regionFunc({arg array; array.gaussian(dev) }, phrase);
//		phrase.size.do({ arg i;
//			phrase[i] = initialPhrase[i] * (2.pow(0.gaussian(octaveDev)));
//		});
	}
	invert{
		this.regionFunc({arg array; array.invert }, phrase);
	}
	revert{
		phrase = initialPhrase.copy;
	}
	scale{ arg scalar;
		this.regionFunc({arg array; array * scalar }, phrase);
	}
	shift{ arg constant;
		this.regionFunc({arg array; array + constant }, phrase);
	}


	// changing parameters
	regionLength_{ arg newLength;
		//if(newLength <= phrase.size, {
			regionLength = newLength;
		//});
	}

	regionStart_{ arg newLoopStart;
		regionStart = newLoopStart % phrase.size;
	}

	returnProbs_{ arg probs, seed;

		// If input probs is just a number, then fill the array
		// with that number. If it's an array, lace it.
		if(probs.class.superclass == SimpleNumber, {
			returnProbs = returnProbs.fill(probs);
		},{
			returnProbs = probs;
		});

		this.calcReturns(seed);
	}

	seed_{ arg newSeed;
		if(newSeed != seed, {
			seed = newSeed;
			//phrase = this.gaussify(initialPhrase, 0.5, );
		});
	}
}