
s.reboot;

/*

left - right channels are the outputs of the two neurons.
inputs are:
in1, in2, x1, x2, w11, w12, w21, w22

in1 and in2 are theta1 / theat2

*/



(
{
	var module =  Dneuromodule(2, [MouseX.kr(-2, 3), MouseY.kr(-2, 3)], [0.1, 0.3], [-3, 2, -2, 0]);
	var sig = Duty.ar(0.2, 0, module.dpoll);
	Blip.ar(sig * 30, 150) * 0.1;

}.play
)
(
{
	var module =  Dneuromodule(2, [MouseX.kr(-2, 3), MouseY.kr(-2, 3)], [0.1, 0.3], [-3, 2, -2, 0]);
	var sig = Duty.kr(0.2, 0, module);
	0.0

}.play
)

/////////////////////

(
{
	var args = [[-2, 2], [0.1, 0.3], [-3, 2, -2, 0]];
	var module =  Dneuromodule(2, *args);
	var module2 =  Dneuromodule2(*args);
	var sig = Duty.kr(0.2, 0, module.dpoll("module 1"));
	var sig2 = Duty.kr(0.2, 0, module2.dpoll("module 2"));

	0.0

}.play
)

////////////////////

(
{
	var module =  Dneuromodule2(MouseX.kr(-2, 3), MouseY.kr(-2, 3), 0.1, 0.3, -3, 2, -2, 0);
	var sig = Duty.ar(SampleDur.ir, 0, module);
	LPF.ar(sig * 0.1, 15000)  * 0.03;

}.play
)
(
{
	var module =  Dneuromodule(2, [MouseX.kr(-2, 3), MouseY.kr(-2, 3)], [0.1, 0.3], [-3, 2, -2, 0]);
	var sig = Duty.ar(SampleDur.ir, 0, module);
	LPF.ar(sig * 0.1, 15000) * 0.03;

}.play
)


{ SinOsc.ar(Dneuromodule.ar(1/100, 2, [MouseX.kr(-2, 3), MouseY.kr(-2, 3)], [0.1, 0.3], [-3, 2, -2, 0]).lag(0.01) * 700 + 800) * 0.1 }.play;

(
{
	var dt = 0.001;
	var m = Duty.ar(dt, 0, Dneuromodule2(MouseX.kr(-2, 2), MouseY.kr(-2, 2), 0.1, 0.3, -3, 2, -2, 0)).lag3(dt);
	SinOsc.ar(m * 400 + 600) * 0.1
}.play
)

// modulate w11 and w21 slowly and randomly
(
{
	var module =  Dneuromodule2(MouseX.kr(-2, 3), MouseY.kr(-2, 3), 0.1, 0.3, LFNoise1.kr(0.1).range(-3, -2), 2, LFNoise1.kr(0.1).range(-2, -1), 0);
	var sig = Duty.ar(SampleDur.ir, 0, module);
	LPF.ar(sig * 0.1, 15000);

}.play
)
// modulate everything slowly and randomly

(
{
	var module =  Dneuromodule2(MouseX.kr(-2, 3), MouseY.kr(-2, 3), 0.1, 0.3, LFNoise1.kr(0.1).range(-3, -2),  LFNoise1.kr(0.1).range(1, 2), LFNoise1.kr(0.1).range(-2, -1),  LFNoise1.kr(0.1).range(-1, 1));
	var sig = Duty.ar(SampleDur.ir, 0, module);
	LPF.ar(sig * 0.1, 15000);

}.play
)

// sonified:
(
{

	var module =  Dneuromodule2(
		MouseX.kr(-2, 3),
		MouseY.kr(-2, 3),
		0.1,
		0.3,
		LFNoise1.kr(0.1).range(-3, -2),
		LFNoise1.kr(0.1).range(1, 2),
		LFNoise1.kr(0.1).range(-2, -1),
		LFNoise1.kr(0.1).range(-1, 1)
	);

	var dt = 0.001;
	var m = Duty.ar(dt, 0, module).lag3(dt);
	SinOsc.ar(m * 800 + 1100) * 0.1
}.play
)

(
{
	var w1 = LFNoise1.kr(0.1);
	var w2 = LFNoise1.kr(0.1);

	var module =  Dneuromodule2(
		MouseX.kr(-2, 3),
		MouseY.kr(-2, 3),
		0.1,
		0.3,
		w1.range(-3, -2),
		w2.range(1, 2),
		w1.range(-2, -1),
		w2.range(-1, 1)
	);

	var dt = 0.0001;
	var m = Duty.ar(dt, MouseButton.kr(0, 1), module).lag3(dt);
	SinOsc.ar(m * 800 + 1100) * 0.1
}.play
)




// tests

{ Duty.ar(0.2, 0, Dneuromodule2(2, -1, 0.1, 0.3, -3, 2, -2, 0).dpoll); 0.0 }.play
{ Duty.ar(0.2, 0, Dneuromodule2(2, -1, 0.1, 0.3, -3, 2, -2, 0)); 0.0 }.play
{ Duty.ar(0.2, 0, Dneuromodule2(MouseX.kr(-2, 3), -1, 0.1, 0.3, -3, 2, -2, 0)); 0.0 }.play
{ Duty.ar(0.1, 0, Dneuromodule2(MouseX.kr(-2, 3), MouseY.kr(-2, 3), 0.1, 0.3, -3, 2, -2, 0)) * 0.03 }.play
{ Duty.ar(SampleDur.ir, 0, Dneuromodule2(MouseX.kr(-2, 3), MouseY.kr(-2, 3), 0.1, 0.3, -3, 2, -2, 0)) * 0.03 }.play

{ Duty.ar(0.2, 0, Dneuromodule2(2, Dseq([-1, -2, -3]), 0.1, 0.3, -3, 2, -2, 0).dpoll, doneAction:2); 0.0 }.play
{ Duty.ar(0.2, 0, Dneuromodule2(2, -1, 0.1, 0.3, -3, 2, -2, 0).dpoll, MouseButton.kr); 0.0 }.play







// fig. 82
t = [2, -1]; w = [[-3, 2], [-2, 0]];
w = w + [0.2, -0.1];
w = w + [-0.2, 0.1];

