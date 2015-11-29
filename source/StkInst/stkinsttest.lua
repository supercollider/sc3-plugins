--UGen definitions
Stknames ={ "Clarinet", "BlowHole", "Saxofony", "Flute", "Brass",
                              "BlowBotl", "Bowed", "Plucked", "StifKarp", "Sitar", "Mandolin",
                              "Rhodey", "Wurley", "TubeBell", "HevyMetl", "PercFlut",
                              "BeeThree", "FMVoices", "VoicForm", "Moog", "Simple", "Drummer",
                              "BandedWG", "Shakers", "ModalBar", "Mesh2D", "Resonate", "Whistle" }

Stk = {}
for i,v in ipairs(Stknames) do
	Stk[v] = i - 1
end

StkInst = UGen:new{name="StkInst"}
function StkInst.ar(instNumber,freq,gate,ampat,amprel,args)
	local freq = freq or 231;gate = gate or 1;ampat = ampat or 1;amprel=amprel or 0;instNumber =instNumber or 6;args = args or {};
	return StkInst:MultiNew(concatTables({2,freq, gate,ampat,amprel,instNumber},args))
end

StkGlobals = UGen:new{name="StkGlobals"}
function StkGlobals.ar(showWarnings, printErrors, rawfilepath)
	showWarnings = showWarnings or 1;printErrors = printErrors or 1;rawfilepath = rawfilepath or ""
	local ascii = {rawfilepath:byte(1,-1)}
	return StkGlobals:MultiNew(concatTables({2,showWarnings, printErrors},ascii))
end

------------------------some synthdefs
SynthDef("help_stkBowed",{out=0,freq=200,gate=1,amp=1,bowvel=65,pres=94,pos=16,vf=50,vg=1},function()

	local env = EnvGen.kr{Env.asr(0,1,0.1),gate,doneAction=2}
	local sig = StkInst.ar(Stk.Bowed,freq,gate,amp,0.5,{2,pres,4,pos,1,vg,11,vf})
	Out.ar(out,sig:dup())
end):store()

SynthDef("help_stkClarinet",{out=0,freq=200,gate=1,stiff=50,pres= 111,noise=24,vf=60,vg=1},function()
	local sig = StkInst.ar(Stk.Clarinet,freq,gate,1,0.05,{2,stiff,128,pres,4,noise,1,vg,11,vf})
	Out.ar(out,sig:dup())
end):store()

SynthDef("help_stkBrass",{out=0,freq=200,gate=1,amp=1,stiff=50,pres= 111,noise=24,vf=60,vg=1},function()
	local env = EnvGen.kr{Env.asr(0,1,0.1),gate,doneAction=2}
	local sig = StkInst.ar(Stk.Brass,freq,gate,amp,0.05)--,{2,stiff,128,pres,4,noise,1,vg,11,vf})
	Out.ar(out,sig:dup())
end):store()


SynthDef("help_VoicForm",{out=0,freq=200,gate=1,amp=1,phon=8,loud=50,mix=65,vf=60,vg=20},function()
	StkGlobals.ar(1,1,[[C:\SupercolliderRepos\Mios\stk-4.5.0\stk-4.5.0\rawwaves\]])
	local env = EnvGen.kr{Env.asr(0.3,1,0.1),gate,doneAction=2}
	local sig = StkInst.ar(Stk.VoicForm,freq,gate,amp,0.05,{4,phon,2,mix,128,loud,1,vg,11,vf})*env--,{2,stiff,128,pres,4,noise,1,vg,11,vf})
	Out.ar(out,sig:dup())
end):store()

SynthDef("help_stkS",{out=0,freq=200,gate=1,amp=1,vowel=120,tilt= 50,targ=50,noise=24,vf=60,vg=1},function()
	--StkGlobals.ir(1,1,[[C:\SupercolliderRepos\Mios\stk-4.5.0\stk-4.5.0\rawwaves\]])
	local env = EnvGen.kr{Env.asr(0,1,0.1),gate,doneAction=2}
	local sig = StkInst.ar(Stk.Mesh2D,freq,gate,amp,0.5)--,{2,vowel,4,tilt,128,targ})*env--,{16,3})--,{2,stiff,128,pres,4,noise,1,vg,11,vf})
	Out.ar(out,sig:dup())
end):store()

SynthDef("stkglobals",{},function()
		StkGlobals.ar(1,1,[[C:/SupercolliderRepos/Mios/stk-4.5.0/stk-4.5.0/rawwaves/]])
end):play()
------------------------------------use it
local sclua = require "sclua.Server"
local s = sclua.Server()

--sinte = s.Synth("help_stkS",{freq=200})

instgui=InstrumentsGUI("help_stkS",false)
MidiToOsc.AddChannel(0,instgui,{0.1},mmm)--,{{"bowsoundboard"}},false)

--[[
sinte:set{freq = 300}
sinte:set{pos = 10}
sinte:set{gate = 0}

sinte:set{bowvel=0}
sinte:set{pres=0}
sinte:set{gate = 1}
--]]