## How to update the faust-generated cpp files
*for GreyHole and JPverb*

Install a recent version of [faust](https://github.com/grame-cncm/faust).

`cd` to the `faust_src` directory and create `cpp` files for the from the `dsp`-files
```
faust -a SuperCollider.cpp JPverbRaw.dsp -o JPverbRaw.cpp
faust -a SuperCollider.cpp GreyholeRaw.dsp -o GreyholeRaw.cpp
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