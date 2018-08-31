// Some of these UGens are rather buggy and don't behave right...
// look up the class files to see which arguments each of the synths have

s =Server.local.boot

{StkPluck.ar}.play(s)


{StkMandolin.ar(mul:3)}.play(s)  // prob


{StkSaxofony.ar(mul:0.1)}.play(s)


{StkBandedWG.ar(3000, instr:1, mul:3) }.play(s)


{StkVoicForm.ar}.play(s)  // prob
 // don't know why there is a glitch at start

{StkClarinet.ar(mul:0.2)}.play(s)

{StkBlowHole.ar(mul:7)}.play(s)

{StkMoog.ar}.play(s) // prob

{StkBeeThree.ar}.play(s)   // prob

// FIXED: undefined control number 7???
// But doesn't sound right
{StkModalBar.ar(40, 5, mul:6)}.play(s)  // prob

// This one throws an error that seems to need fixing!
{StkFlute.ar(440, 35, 0.5, mul:1) }.play(s)

// This one just posts "Starting" twice and doesn't do much afterwards
{StkBowed.ar(vibgain:1) }.play(s)

// NOT FIXED: this one corrupts memory!!
// doesn't post the warning anymore, but still quits scsynth, when freed
{StkShakers.ar(instr:11, mul:4)}.play(s)
