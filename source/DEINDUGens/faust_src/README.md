## How to update the faust-generated cpp files
*for GreyHole and JPverb*

Install a recent version of [faust](https://github.com/grame-cncm/faust).

`cd` to the `faust_src` directory and create `cpp` files for the from the `dsp`-files

Generate the code by 

```sh
faust -a SuperCollider.cpp JPverbRaw.dsp -o JPverbRaw.cpp
faust -a SuperCollider.cpp GreyholeRaw.dsp -o GreyholeRaw.cpp
```

*Optionally (and for your personal machine), you may want to optimise the generated code. See [below](#optimised-code-generation) for details.*

Copy `.cpp` files to their intended locations (to be used by the `sc3plugin` `cmake/make` toolchain). you might want to secure the previous versions of the files somewhere.
```
cp JPverbRaw.cpp ..
cp GreyholeRaw.cpp ..
```

Currently, a list of prime numbers is provided by an external header, which we have to provide in the local includes folder

```sh
cp jprev.h ../include/
```

To ensure usage of the right faust headers (for others), we add them to the sc3-plugins repository: copy faust includes (here from their standard install directory) to a local include folder (which gets included by `sc3-plugins`' `cmake` toolchain)

```sh
# remove previous version
rm -rf ../include/faust/
# add new version
cp -r /usr/local/include/faust ../include/
```

Change directory to `sc3-plugins` build directory and run the `cmake/make/make install` toolchain as usual. If unsure, have a look at the README.md at `sc3-plugin` root.

```sh
cd ${SC3_PLUGIN_ROOT}/build
cmake ..
make
make install
```

### Manually compile `.cpp` files

```sh
g++ -O3 -DNO_LIBSNDFILE -DSC_DARWIN -DSC_FAUST_PREFIX="\"\"" -bundle -I./include -I/localvol/sound/src/supercollider/include/{plugin_interface,common,server} -Dmydsp=JPVERB -o JPverbRaw.scx JPverbRaw.cpp
```


### Manually compile `.dsp` files with `faust2supercollider`

```sh
export SUPERCOLLIDER_HEADERS=/localvol/sound/src/supercollider/include/ 
faust2supercollider JPverbRaw.dsp
faust2supercollider GreyholeRaw.dsp
```

This results in plugins and corresponding class definitions `FaustJPverbRaw` and `FaustGreyholeRaw` which, due to difference in names, can reside alongside the `sc3-plugins` plugins.

### Optimised code generation 

*Intended for a specific hardware/operating system/compiler combination. FAUST code optimisation parameters were used succesfully to increase performance by factor 2 on an OSX machine.*

Benchmark using `faustbench` as [explained on the FAUST blog](http://faust.grame.fr/news/2017/04/26/optimizing-compilation-parameters.html)

```sh
faustbench GreyholeRaw.dsp
faustbench JPVerbRaw.dsp
```

These are the flags for an optimised version on an 2.4 GHz OSX 10.12.4 (16E195) running `Apple LLVM version 8.1.0 (clang-802.0.42)`

```
faust -vec -lv 0 -vs 8 -a SuperCollider.cpp JPverbRaw.dsp -o JPverbRaw.cpp
faust -vec -lv 0 -vs 8 -a SuperCollider.cpp GreyholeRaw.dsp -o GreyholeRaw.cpp
```
