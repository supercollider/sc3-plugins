# SC3-plugins


This repository contains the community collection of unit generator plugins for [SuperCollider](http://supercollider.github.io/).
An installation extends the functionality of SuperCollider by additional UGens that run on `scsynth`, the SuperCollider audio synthesis server.

Please note that the UGens in this repository are, on average, less stable and well-maintained than the core collection. Use at your own risk!

**Note:** Extensions for the SuperCollider programming language are different. They are collected within the [Quarks](https://github.com/supercollider-quarks/) packaging system.

To learn how to write your own plugins, see [example-plugins](https://github.com/supercollider/example-plugins) and the "[Writing UGens](http://doc.sccode.org/Guides/WritingUGens.html)" helpfile.

For community discussion and support see the [SuperCollider mailing lists](http://www.birmingham.ac.uk/facilities/BEAST/research/supercollider/mailinglist.aspx) and the [github issue tracker](https://github.com/supercollider/sc3-plugins/issues).

## Installation

Releases are available from the [GitHub release page](https://github.com/supercollider/sc3-plugins/releases). For older versions (2013), see the [Sourceforge project page](http://sourceforge.net/projects/sc3-plugins/files/).

Unzip the release and move it to your SuperCollider extensions folder. You can find it by evaluating

```supercollider
Platform.userExtensionDir
```

in SuperCollider. To evaluate code in SuperCollder, put your cursor on the line of code and press `Cmd+Enter`
(macOS) or `Ctrl+Enter`.  Alternatively, you may install the extensions system-wide by copying to

```supercollider
Platform.systemExtensionDir
```

The folder might not exist, so you may need to create it yourself. You can do this in your operating system's file
explorer or from within SuperCollider by evaluating:

```supercollider
File.mkdir(Platform.userExtensionDir)
```

On some operating systems, these directories may be hard to find because they're in hidden folders.  You can open
the user app support directory (where the Extensions folder is) with the menu item
"File->Open user support directory". On macOS, you can open a finder window and press `Cmd+Shift+G` and enter the
name of the directory.

## Compile from source

Download both the SuperCollider source and the source for this repository:

```shell
git clone --recursive https://github.com/supercollider/sc3-plugins.git
git clone --recursive https://github.com/supercollider/supercollider.git
```

Be sure to use `--recursive`, or to initialize the submodules after cloning, otherwise you will not have files
that are necessary to build the project.

Like SuperCollider, sc3-plugins uses `cmake`. Execute the following commands in the directory where you cloned sc3-plugins;
replace `/path/to/sc/` with the path to the SuperCollider source directory. That will probably be
`../../supercollider` if you cloned both repositories in the same working directory.

```shell
mkdir build && cd build
cmake -DSC_PATH=/path/to/sc/ ..
cmake --build . --config Release
# to install the plugins
cmake --build . --config Release --target install
```

If no `SC_PATH` is provided the build system assumes the SuperCollider include files are in `/usr/include/SuperCollider/`.

On macOS, the plugins will end up in `sc3-plugins/build/SC3plugins`.
Copy the `SC3plugins` folder to your Extensions folder (evaluate `Platform.userExtensionDir` in SuperCollider to find it).

NOTE: on macOS, if you want to install into `CMAKE_INSTALL_PREFIX`, you have to specify it by disabling the `IN_PLACE_BUILD` cmake option which defaults to ON (see below).

### Cmake Options

+ Set install target
    * (default on linux `/usr/local`)
    * `cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..`
+ Install in cmake build folder instead of `CMAKE_INSTALL_PREFIX`
    * (OSX ONLY, default=ON)
    * `cmake -DIN_PLACE_BUILD=ON`
+ Build the plugins as quarks
    * (default 'OFF')
    * `cmake -DQUARKS=ON ..`
+ Build supernova-plugins
    * (default 'OFF')
    * `cmake -DSUPERNOVA=ON ..`
+ Print all cmake options
    * `sc3-plugins/build/$ cmake -L ..`

### Troubleshooting

#### Build errors

If you get an error while building that files are missing, it probably means that you didn't clone all the SuperCollider
submodules. Fix this by running `git submodule update --init` in the SuperCollider source directory. If you still have the
issue afterwards, try clearing your build directory and starting your build from scratch.

#### Starting over

If something went wrong and you want to start from scratch, delete everything in the build directory that you made:

```shell
make uninstall # only if you ran `make install` before
cd ..
rm -r build
```

## Adding plugins to the repository

A SuperCollider plugin is a collection of UGens (and their supporting files) with a shared prefix in their name.
If you add a new plugin, please keep to the following pattern:

1. Add a folder in the `source` directory named `<prefix>UGens`
where `prefix` means whichever standard pattern in the file name you have for your UGens. All source files go into this directory and its subdirectories.
2. SuperCollider-specific files (`.sc|.schelp|...`) should be located in a subdirectory named `sc`.
3. If your plugin makes use of external libraries that should be part of the sc-plugins sources (e.g. via ```git-submodule```), add them to `sc3-plugins/external_libraries/`.
As an example, the `GlitchUGens` plugin directory lists as:
    * `source/GlitchUGens/GlitchUGens.cpp`
    * `source/GlitchUGens/sc/GlitchUGens.sc`
    * `source/GlitchUGens/sc/HelpSource/Classes/GlitchBPF.schelp`
    * `source/GlitchUGens/sc/HelpSource/Classes/GlitchBRF.schelp`
    * `source/GlitchUGens/sc/HelpSource/Classes/GlitchHPF.schelp`
    * `source/GlitchUGens/sc/HelpSource/Classes/GlitchRHPF.schelp`
4. Add your folder to the `PLUGIN_DIRS` list in `sc3-plugins/source/CMakeLists.txt`.
5. For the Quark-installable option, there is a SuperCollider script called `Generate_Quark.scd` in `sc3-plugins/quarks/`. Evaluating it indexes the base directory of the extensions for each UGen, and Help-file in each plugin directory.
It then creates a help file for your plugin that lists all classes and help files, as well as a `.quark` file for your plugin in the `build/DIRECTORY` folder.

# Packaging

## How to create a DiskImage (OSX)

To create an OSX DiskImage, follow these steps on an OSX machine:

```shell
mkdir build && cd build
cmake -DSC_PATH=/path/to/sc/ -DOSX_PACKAGE=1 ..
make && make install
```

The DiskImage will be generated in `./sc3-plugins/build/build_osx` containing

+ a `License.txt`,
+ this `README.txt`, and
+ the `SC3plugins` folder.
+ Note: the quarks ```DIRECTORY```-folder is also included by default.

## How to create a tarball/zip-file

```shell
mkdir build && cd build
cmake -DSC_PATH=/path/to/sc/ ..
cpack -G TGZ # `-G ZIP` also works
```

The package will end up in `sc3-plugins/build`.

# List of plugins

An auto-generated list of what plugins are available in this repository, linking to the autogenerated helpfiles:

* [AmplitudeMod](http://doc.sccode.org/Classes/AmplitudeMod.html)
* [AnalyseEvents2](http://doc.sccode.org/Classes/AnalyseEvents2.html)
* [ArrayMax](http://doc.sccode.org/Classes/ArrayMax.html)
* [ArrayMin](http://doc.sccode.org/Classes/ArrayMin.html)
* [Atk](http://doc.sccode.org/Classes/Atk.html)
* [ATKDocsLicensing](http://doc.sccode.org/Classes/ATKDocsLicensing.html)
* [AtkKernelConv](http://doc.sccode.org/Classes/AtkKernelConv.html)
* [ATKLicensing](http://doc.sccode.org/Classes/ATKLicensing.html)
* [AtkMatrixMix](http://doc.sccode.org/Classes/AtkMatrixMix.html)
* [AttackSlope](http://doc.sccode.org/Classes/AttackSlope.html)
* [AutoTrack](http://doc.sccode.org/Classes/AutoTrack.html)
* [AverageOutput](http://doc.sccode.org/Classes/AverageOutput.html)
* [AY](http://doc.sccode.org/Classes/AY.html)
* [BeatStatistics](http://doc.sccode.org/Classes/BeatStatistics.html)
* [BlitB3](http://doc.sccode.org/Classes/BlitB3.html)
* [BlitB3Saw](http://doc.sccode.org/Classes/BlitB3Saw.html)
* [BlitB3Square](http://doc.sccode.org/Classes/BlitB3Square.html)
* [BlitB3Tri](http://doc.sccode.org/Classes/BlitB3Tri.html)
* [BMoog](http://doc.sccode.org/Classes/BMoog.html)
* [Breakcore](http://doc.sccode.org/Classes/Breakcore.html)
* [Brusselator](http://doc.sccode.org/Classes/Brusselator.html)
* [BufMax](http://doc.sccode.org/Classes/BufMax.html)
* [BufMin](http://doc.sccode.org/Classes/BufMin.html)
* [Cepstrum](http://doc.sccode.org/Classes/Cepstrum.html)
* [Chromagram](http://doc.sccode.org/Classes/Chromagram.html)
* [CircleRamp](http://doc.sccode.org/Classes/CircleRamp.html)
* [ComplexRes](http://doc.sccode.org/Classes/ComplexRes.html)
* [Concat](http://doc.sccode.org/Classes/Concat.html)
* [Concat2](http://doc.sccode.org/Classes/Concat2.html)
* [Coyote](http://doc.sccode.org/Classes/Coyote.html)
* [Crest](http://doc.sccode.org/Classes/Crest.html)
* [CrossoverDistortion](http://doc.sccode.org/Classes/CrossoverDistortion.html)
* [Dbrown2](http://doc.sccode.org/Classes/Dbrown2.html)
* [DbufTag](http://doc.sccode.org/Classes/DbufTag.html)
* [Decimator](http://doc.sccode.org/Classes/Decimator.html)
* [DEIND](http://doc.sccode.org/Classes/DEIND.html)
* [DetaBlockerBuf](http://doc.sccode.org/Classes/DetaBlockerBuf.html)
* [DFM1](http://doc.sccode.org/Classes/DFM1.html)
* [Dfsm](http://doc.sccode.org/Classes/Dfsm.html)
* [DiodeRingMod](http://doc.sccode.org/Classes/DiodeRingMod.html)
* [Disintegrator](http://doc.sccode.org/Classes/Disintegrator.html)
* [DNoiseRing](http://doc.sccode.org/Classes/DNoiseRing.html)
* [DoubleNestedAllpassC](http://doc.sccode.org/Classes/DoubleNestedAllpassC.html)
* [DoubleNestedAllpassL](http://doc.sccode.org/Classes/DoubleNestedAllpassL.html)
* [DoubleNestedAllpassN](http://doc.sccode.org/Classes/DoubleNestedAllpassN.html)
* [DoubleWell](http://doc.sccode.org/Classes/DoubleWell.html)
* [DoubleWell2](http://doc.sccode.org/Classes/DoubleWell2.html)
* [DoubleWell3](http://doc.sccode.org/Classes/DoubleWell3.html)
* [DPW3Tri](http://doc.sccode.org/Classes/DPW3Tri.html)
* [DPW4Saw](http://doc.sccode.org/Classes/DPW4Saw.html)
* [Dtag](http://doc.sccode.org/Classes/Dtag.html)
* [DWGBowed](http://doc.sccode.org/Classes/DWGBowed.html)
* [DWGBowedSimple](http://doc.sccode.org/Classes/DWGBowedSimple.html)
* [DWGBowedTor](http://doc.sccode.org/Classes/DWGBowedTor.html)
* [DWGPlucked](http://doc.sccode.org/Classes/DWGPlucked.html)
* [DWGPlucked2](http://doc.sccode.org/Classes/DWGPlucked2.html)
* [EnvDetect](http://doc.sccode.org/Classes/EnvDetect.html)
* [EnvFollow](http://doc.sccode.org/Classes/EnvFollow.html)
* [EQExamples](http://doc.sccode.org/Classes/EQExamples.html)
* [FeatureSave](http://doc.sccode.org/Classes/FeatureSave.html)
* [FFTCrest](http://doc.sccode.org/Classes/FFTCrest.html)
* [FFTPeak](http://doc.sccode.org/Classes/FFTPeak.html)
* [FFTPower](http://doc.sccode.org/Classes/FFTPower.html)
* [FFTSlope](http://doc.sccode.org/Classes/FFTSlope.html)
* [FFTSpread](http://doc.sccode.org/Classes/FFTSpread.html)
* [FFTSubbandFlatness](http://doc.sccode.org/Classes/FFTSubbandFlatness.html)
* [FFTSubbandPower](http://doc.sccode.org/Classes/FFTSubbandPower.html)
* [Fhn2DC](http://doc.sccode.org/Classes/Fhn2DC.html)
* [Fhn2DL](http://doc.sccode.org/Classes/Fhn2DL.html)
* [Fhn2DN](http://doc.sccode.org/Classes/Fhn2DN.html)
* [FilterComparisons](http://doc.sccode.org/Classes/FilterComparisons.html)
* [FitzHughNagumo](http://doc.sccode.org/Classes/FitzHughNagumo.html)
* [FM7](http://doc.sccode.org/Classes/FM7.html)
* [Foa](http://doc.sccode.org/Classes/Foa.html)
* [FoaAsymmetry](http://doc.sccode.org/Classes/FoaAsymmetry.html)
* [FoaBalance](http://doc.sccode.org/Classes/FoaBalance.html)
* [FoaDecode](http://doc.sccode.org/Classes/FoaDecode.html)
* [FoaDecoderKernel](http://doc.sccode.org/Classes/FoaDecoderKernel.html)
* [FoaDecoderMatrix](http://doc.sccode.org/Classes/FoaDecoderMatrix.html)
* [FoaDirect](http://doc.sccode.org/Classes/FoaDirect.html)
* [FoaDirectO](http://doc.sccode.org/Classes/FoaDirectO.html)
* [FoaDirectX](http://doc.sccode.org/Classes/FoaDirectX.html)
* [FoaDirectY](http://doc.sccode.org/Classes/FoaDirectY.html)
* [FoaDirectZ](http://doc.sccode.org/Classes/FoaDirectZ.html)
* [FoaDominate](http://doc.sccode.org/Classes/FoaDominate.html)
* [FoaDominateX](http://doc.sccode.org/Classes/FoaDominateX.html)
* [FoaDominateY](http://doc.sccode.org/Classes/FoaDominateY.html)
* [FoaDominateZ](http://doc.sccode.org/Classes/FoaDominateZ.html)
* [FoaEncode](http://doc.sccode.org/Classes/FoaEncode.html)
* [FoaEncoderKernel](http://doc.sccode.org/Classes/FoaEncoderKernel.html)
* [FoaEncoderMatrix](http://doc.sccode.org/Classes/FoaEncoderMatrix.html)
* [FoaFocus](http://doc.sccode.org/Classes/FoaFocus.html)
* [FoaFocusX](http://doc.sccode.org/Classes/FoaFocusX.html)
* [FoaFocusY](http://doc.sccode.org/Classes/FoaFocusY.html)
* [FoaFocusZ](http://doc.sccode.org/Classes/FoaFocusZ.html)
* [FoaMirror](http://doc.sccode.org/Classes/FoaMirror.html)
* [FoaNFC](http://doc.sccode.org/Classes/FoaNFC.html)
* [FoaPanB](http://doc.sccode.org/Classes/FoaPanB.html)
* [FoaPress](http://doc.sccode.org/Classes/FoaPress.html)
* [FoaPressX](http://doc.sccode.org/Classes/FoaPressX.html)
* [FoaPressY](http://doc.sccode.org/Classes/FoaPressY.html)
* [FoaPressZ](http://doc.sccode.org/Classes/FoaPressZ.html)
* [FoaProximity](http://doc.sccode.org/Classes/FoaProximity.html)
* [FoaPsychoShelf](http://doc.sccode.org/Classes/FoaPsychoShelf.html)
* [FoaPush](http://doc.sccode.org/Classes/FoaPush.html)
* [FoaPushX](http://doc.sccode.org/Classes/FoaPushX.html)
* [FoaPushY](http://doc.sccode.org/Classes/FoaPushY.html)
* [FoaPushZ](http://doc.sccode.org/Classes/FoaPushZ.html)
* [FoaRotate](http://doc.sccode.org/Classes/FoaRotate.html)
* [FoaRTT](http://doc.sccode.org/Classes/FoaRTT.html)
* [FoaSpeakerMatrix](http://doc.sccode.org/Classes/FoaSpeakerMatrix.html)
* [FoaTilt](http://doc.sccode.org/Classes/FoaTilt.html)
* [FoaTransform](http://doc.sccode.org/Classes/FoaTransform.html)
* [FoaTumble](http://doc.sccode.org/Classes/FoaTumble.html)
* [FoaXform](http://doc.sccode.org/Classes/FoaXform.html)
* [FoaXformerMatrix](http://doc.sccode.org/Classes/FoaXformerMatrix.html)
* [FoaZoom](http://doc.sccode.org/Classes/FoaZoom.html)
* [FoaZoomX](http://doc.sccode.org/Classes/FoaZoomX.html)
* [FoaZoomY](http://doc.sccode.org/Classes/FoaZoomY.html)
* [FoaZoomZ](http://doc.sccode.org/Classes/FoaZoomZ.html)
* [FormantTable](http://doc.sccode.org/Classes/FormantTable.html)
* [FrameCompare](http://doc.sccode.org/Classes/FrameCompare.html)
* [Friction](http://doc.sccode.org/Classes/Friction.html)
* [Gammatone](http://doc.sccode.org/Classes/Gammatone.html)
* [GaussClass](http://doc.sccode.org/Classes/GaussClass.html)
* [GaussTrig](http://doc.sccode.org/Classes/GaussTrig.html)
* [Gbman2DC](http://doc.sccode.org/Classes/Gbman2DC.html)
* [Gbman2DL](http://doc.sccode.org/Classes/Gbman2DL.html)
* [Gbman2DN](http://doc.sccode.org/Classes/Gbman2DN.html)
* [Gendy4](http://doc.sccode.org/Classes/Gendy4.html)
* [Gendy5](http://doc.sccode.org/Classes/Gendy5.html)
* [Getenv](http://doc.sccode.org/Classes/Getenv.html)
* [GlitchBPF](http://doc.sccode.org/Classes/GlitchBPF.html)
* [GlitchBRF](http://doc.sccode.org/Classes/GlitchBRF.html)
* [GlitchHPF](http://doc.sccode.org/Classes/GlitchHPF.html)
* [GlitchRHPF](http://doc.sccode.org/Classes/GlitchRHPF.html)
* [Goertzel](http://doc.sccode.org/Classes/Goertzel.html)
* [GravityGrid](http://doc.sccode.org/Classes/GravityGrid.html)
* [GravityGrid2](http://doc.sccode.org/Classes/GravityGrid2.html)
* [Greyhole](http://doc.sccode.org/Classes/Greyhole.html)
* [GreyholeRaw](http://doc.sccode.org/Classes/GreyholeRaw.html)
* [HairCell](http://doc.sccode.org/Classes/HairCell.html)
* [Henon2DC](http://doc.sccode.org/Classes/Henon2DC.html)
* [Henon2DL](http://doc.sccode.org/Classes/Henon2DL.html)
* [Henon2DN](http://doc.sccode.org/Classes/Henon2DN.html)
* [ICepstrum](http://doc.sccode.org/Classes/ICepstrum.html)
* [IIRFilter](http://doc.sccode.org/Classes/IIRFilter.html)
* [InsideOut](http://doc.sccode.org/Classes/InsideOut.html)
* [Instruction](http://doc.sccode.org/Classes/Instruction.html)
* [JPverb](http://doc.sccode.org/Classes/JPverb.html)
* [JPverbRaw](http://doc.sccode.org/Classes/JPverbRaw.html)
* [KeyClarity](http://doc.sccode.org/Classes/KeyClarity.html)
* [KeyMode](http://doc.sccode.org/Classes/KeyMode.html)
* [KMeansRT](http://doc.sccode.org/Classes/KMeansRT.html)
* [KmeansToBPSet1](http://doc.sccode.org/Classes/KmeansToBPSet1.html)
* [LADSPA](http://doc.sccode.org/Classes/LADSPA.html)
* [Latoocarfian2DC](http://doc.sccode.org/Classes/Latoocarfian2DC.html)
* [Latoocarfian2DL](http://doc.sccode.org/Classes/Latoocarfian2DL.html)
* [Latoocarfian2DN](http://doc.sccode.org/Classes/Latoocarfian2DN.html)
* [LFBrownNoise0](http://doc.sccode.org/Classes/LFBrownNoise0.html)
* [LFBrownNoise1](http://doc.sccode.org/Classes/LFBrownNoise1.html)
* [LFBrownNoise2](http://doc.sccode.org/Classes/LFBrownNoise2.html)
* [ListTrig](http://doc.sccode.org/Classes/ListTrig.html)
* [ListTrig2](http://doc.sccode.org/Classes/ListTrig2.html)
* [Logger](http://doc.sccode.org/Classes/Logger.html)
* [LoopBuf](http://doc.sccode.org/Classes/LoopBuf.html)
* [Lorenz2DC](http://doc.sccode.org/Classes/Lorenz2DC.html)
* [Lorenz2DL](http://doc.sccode.org/Classes/Lorenz2DL.html)
* [Lorenz2DN](http://doc.sccode.org/Classes/Lorenz2DN.html)
* [LorenzTrig](http://doc.sccode.org/Classes/LorenzTrig.html)
* [LPCAnalyzer](http://doc.sccode.org/Classes/LPCAnalyzer.html)
* [LPCError](http://doc.sccode.org/Classes/LPCError.html)
* [LTI](http://doc.sccode.org/Classes/LTI.html)
* [MarkovSynth](http://doc.sccode.org/Classes/MarkovSynth.html)
* [MatchingP](http://doc.sccode.org/Classes/MatchingP.html)
* [Max](http://doc.sccode.org/Classes/Max.html)
* [MdaPiano](http://doc.sccode.org/Classes/MdaPiano.html)
* [MeanTriggered](http://doc.sccode.org/Classes/MeanTriggered.html)
* [Meddis](http://doc.sccode.org/Classes/Meddis.html)
* [MedianSeparation](http://doc.sccode.org/Classes/MedianSeparation.html)
* [MedianTriggered](http://doc.sccode.org/Classes/MedianTriggered.html)
* [MembraneCircle](http://doc.sccode.org/Classes/MembraneCircle.html)
* [MembraneHexagon](http://doc.sccode.org/Classes/MembraneHexagon.html)
* [MoogLadder](http://doc.sccode.org/Classes/MoogLadder.html)
* [MoreChaos](http://doc.sccode.org/Classes/MoreChaos.html)
* [NearestN](http://doc.sccode.org/Classes/NearestN.html)
* [NestedAllpassC](http://doc.sccode.org/Classes/NestedAllpassC.html)
* [NestedAllpassL](http://doc.sccode.org/Classes/NestedAllpassL.html)
* [NestedAllpassN](http://doc.sccode.org/Classes/NestedAllpassN.html)
* [NL](http://doc.sccode.org/Classes/NL.html)
* [NL2](http://doc.sccode.org/Classes/NL2.html)
* [NLFiltC](http://doc.sccode.org/Classes/NLFiltC.html)
* [NLFiltL](http://doc.sccode.org/Classes/NLFiltL.html)
* [NLFiltN](http://doc.sccode.org/Classes/NLFiltN.html)
* [NTube](http://doc.sccode.org/Classes/NTube.html)
* [OnsetStatistics](http://doc.sccode.org/Classes/OnsetStatistics.html)
* [Oregonator](http://doc.sccode.org/Classes/Oregonator.html)
* [OteyPiano](http://doc.sccode.org/Classes/OteyPiano.html)
* [Perlin3](http://doc.sccode.org/Classes/Perlin3.html)
* [PlaneTree](http://doc.sccode.org/Classes/PlaneTree.html)
* [Plorenz](http://doc.sccode.org/Classes/Plorenz.html)
* [PrintVal](http://doc.sccode.org/Classes/PrintVal.html)
* [ProbalisticNoiseUGens](http://doc.sccode.org/Classes/ProbalisticNoiseUGens.html)
* [PulseDPW](http://doc.sccode.org/Classes/PulseDPW.html)
* [PV_CommonMag](http://doc.sccode.org/Classes/PV_CommonMag.html)
* [PV_CommonMul](http://doc.sccode.org/Classes/PV_CommonMul.html)
* [PV_Compander](http://doc.sccode.org/Classes/PV_Compander.html)
* [PV_Cutoff](http://doc.sccode.org/Classes/PV_Cutoff.html)
* [PV_ExtractRepeat](http://doc.sccode.org/Classes/PV_ExtractRepeat.html)
* [PV_MagGate](http://doc.sccode.org/Classes/PV_MagGate.html)
* [PV_MagMinus](http://doc.sccode.org/Classes/PV_MagMinus.html)
* [PV_MagSmooth](http://doc.sccode.org/Classes/PV_MagSmooth.html)
* [PV_Morph](http://doc.sccode.org/Classes/PV_Morph.html)
* [PV_SoftWipe](http://doc.sccode.org/Classes/PV_SoftWipe.html)
* [PV_XFade](http://doc.sccode.org/Classes/PV_XFade.html)
* [Qitch](http://doc.sccode.org/Classes/Qitch.html)
* [RLPFD](http://doc.sccode.org/Classes/RLPFD.html)
* [RosslerL](http://doc.sccode.org/Classes/RosslerL.html)
* [SawDPW](http://doc.sccode.org/Classes/SawDPW.html)
* [SensoryDissonance](http://doc.sccode.org/Classes/SensoryDissonance.html)
* [Sieve1](http://doc.sccode.org/Classes/Sieve1.html)
* [SineShaper](http://doc.sccode.org/Classes/SineShaper.html)
* [SLOnset](http://doc.sccode.org/Classes/SLOnset.html)
* [SmoothDecimator](http://doc.sccode.org/Classes/SmoothDecimator.html)
* [SMS](http://doc.sccode.org/Classes/SMS.html)
* [SOMRd](http://doc.sccode.org/Classes/SOMRd.html)
* [SOMTrain](http://doc.sccode.org/Classes/SOMTrain.html)
* [SortBuf](http://doc.sccode.org/Classes/SortBuf.html)
* [SpectralEntropy](http://doc.sccode.org/Classes/SpectralEntropy.html)
* [SpruceBudworm](http://doc.sccode.org/Classes/SpruceBudworm.html)
* [Squiz](http://doc.sccode.org/Classes/Squiz.html)
* [Standard2DC](http://doc.sccode.org/Classes/Standard2DC.html)
* [Standard2DL](http://doc.sccode.org/Classes/Standard2DL.html)
* [Standard2DN](http://doc.sccode.org/Classes/Standard2DN.html)
* [Streson](http://doc.sccode.org/Classes/Streson.html)
* [Summer](http://doc.sccode.org/Classes/Summer.html)
* [SVF](http://doc.sccode.org/Classes/SVF.html)
* [SwitchDelay](http://doc.sccode.org/Classes/SwitchDelay.html)
* [Tartini](http://doc.sccode.org/Classes/Tartini.html)
* [TBetaRand](http://doc.sccode.org/Classes/TBetaRand.html)
* [TBrownRand](http://doc.sccode.org/Classes/TBrownRand.html)
* [TermanWang](http://doc.sccode.org/Classes/TermanWang.html)
* [TextVU](http://doc.sccode.org/Classes/TextVU.html)
* [TGaussRand](http://doc.sccode.org/Classes/TGaussRand.html)
* [TGrains2](http://doc.sccode.org/Classes/TGrains2.html)
* [TGrains3](http://doc.sccode.org/Classes/TGrains3.html)
* [TPV](http://doc.sccode.org/Classes/TPV.html)
* [TrigAvg](http://doc.sccode.org/Classes/TrigAvg.html)
* [TwoTube](http://doc.sccode.org/Classes/TwoTube.html)
* [VBAP](http://doc.sccode.org/Classes/VBAP.html)
* [VBAPSpeaker](http://doc.sccode.org/Classes/VBAPSpeaker.html)
* [VBAPSpeakerArray](http://doc.sccode.org/Classes/VBAPSpeakerArray.html)
* [VMScan2D](http://doc.sccode.org/Classes/VMScan2D.html)
* [VOSIM](http://doc.sccode.org/Classes/VOSIM.html)
* [WalshHadamard](http://doc.sccode.org/Classes/WalshHadamard.html)
* [WAmp](http://doc.sccode.org/Classes/WAmp.html)
* [WaveletDaub](http://doc.sccode.org/Classes/WaveletDaub.html)
* [WaveLoss](http://doc.sccode.org/Classes/WaveLoss.html)
* [WaveTerrain](http://doc.sccode.org/Classes/WaveTerrain.html)
* [WeaklyNonlinear](http://doc.sccode.org/Classes/WeaklyNonlinear.html)
* [WeaklyNonlinear2](http://doc.sccode.org/Classes/WeaklyNonlinear2.html)
* [WrapSummer](http://doc.sccode.org/Classes/WrapSummer.html)
