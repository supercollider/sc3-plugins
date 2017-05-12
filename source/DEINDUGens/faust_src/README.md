## How to update the faust-generated cpp files
*for GreyHole and JPverb*

Install a recent version of [faust](https://github.com/grame-cncm/faust).

`cd` to the `faust_src` directory and create `cpp` files for the from the `dsp`-files

#### generic non-optimised code generation

```
faust -a SuperCollider.cpp JPverbRaw.dsp -o JPverbRaw.cpp
faust -a SuperCollider.cpp GreyholeRaw.dsp -o GreyholeRaw.cpp
```


#### optimised code generation

benchmarked by faustbench on 

```sh
$ gcc -v 
Configured with: --prefix=/Applications/Xcode.app/Contents/Developer/usr --with-gxx-include-dir=/usr/include/c++/4.2.1
Apple LLVM version 8.1.0 (clang-802.0.42)
Target: x86_64-apple-darwin16.5.0
Thread model: posix
```

with

```
faustbench GreyholeRaw.dsp
faustbench JPVerbRaw.dsp
```

resulted in the following optimisation flags:

```
faust -vec -lv 0 -vs 8 -a SuperCollider.cpp JPverbRaw.dsp -o JPverbRaw.cpp
faust -vec -lv 0 -vs 8 -a SuperCollider.cpp GreyholeRaw.dsp -o GreyholeRaw.cpp
```

Copy `cpp` files to their intended locations (to be used by the `sc3plugin` `cmake/make` toolchain). you might want to secure the previous versions of the files somewhere.
```
cp JPverbRaw.cpp ..
cp GreyholeRaw.cpp ..
```

```
# for development purposes, you might
# want to symbolically link
# cp JPverb.sc ../sc/
# cp Greyhole.sc ../sc/
```

Currently, a list of prime numbers is provided by an external header, which we have to provide in the local includes folder

```
cp jprev.h ../include/
```

To ensure usage of the right faust headers (for others), we add them to the sc3-plugins repository: copy faust includes (here from their standard install directory) to local include folder (which is included from the `sc3-plugins` `cmake` toolchain)
```
# remove previous
rm -rf ../include/faust/
# add new version
cp -r /usr/local/include/faust ../include/
```

Change directory to sc3-plugin build directory and run `cmake/make/make install` toolchain as usual. If unsure, have a look at the README.md at `sc3-plugin` root.
```
cd ${SC3-PLUGIN_ROOT}/build
cmake ..
make
make install
```

-----

```
# compile by hand:
# g++ -O3 -DNO_LIBSNDFILE -DSC_DARWIN -DSC_FAUST_PREFIX="\"\"" -bundle -I./include -I/localvol/sound/src/supercollider/include/{plugin_interface,common,server} -Dmydsp=JPVERB -o JPverbRaw.scx JPverbRaw.cpp
```
