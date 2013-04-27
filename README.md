# sc3-plugins
## Community collection of UGen plugins for SuperCollider

* [Description](#description)
* [Installation](#installation)
  * [Cmake build system](#how-to-use-the-cmake-build-system)
  * [Options](#options)
  * [Starting over](#starting-over)
* [Adding plugins](#adding-plugins)
* [Creating a package](#creating-a-package)
  * [dmg](#dmg)
  * [tgz/zip](#tgz/zip)

# Description

SuperCollider "UGen" plugins are extensions for the [SuperCollider](http://supercollider.github.io) audio synthesis server (see also [quarks](http://quarks.sourceforge.net/) to extend the programming language). For community discussion and support see the [SuperCollider mailing lists](http://www.birmingham.ac.uk/facilities/BEAST/research/supercollider/mailinglist.aspx).

Downloads are available from [Sourceforge project page](https://sourceforge.net/projects/sc3-plugins/files/).

Here is an [automatically-generated list](http://github.com/supercollider/sc3-plugins/blob/master/plugins_list.md) of what's available in the download (it's a listing of the helpfiles).

# Installation

### How to use the cmake build system

Make a directory for the cmake build files to go into:

* `sc3-plugins/$ mkdir build && cd build`

* `sc3-plugins/build/$ cmake ..`

It assumes you have the supercollider header include files located in `/usr/include/SuperCollider/`

If you have them in another location:

* `sc3-plugins/build/$ cmake -DSC_PATH=/path/to/sc3source/ ..`

and then:

* `sc3-plugins/build/$ make`

* `sc3-plugins/build/$ make install`

On OSX, the plugins will end up in `sc3-plugins/build/SC3plugins`.
Copy *SC3plugins* folder into `/Library/Application Support/SuperCollider/Extensions` (or into a user specific `~/Library` folder).

WARNING: on OSX, if you want to install into `CMAKE_INSTALL_PREFIX`, you have to
specify it by disabling `IN_PLACE_BUILD` cmake option which defaults to ON (see below).

### Options

* To set the install target: (default on linux `/usr/local`)

`sc3-plugins/build/$ cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..`

* (OSX ONLY) Install in cmake build folder instead of `CMAKE_INSTALL_PREFIX` (default=ON)

`sc3-plugins/build/$ cmake -DIN_PLACE_BUILD=ON`

* Build the plugins as quarks: (default 'OFF')

`sc3-plugins/build/$ cmake -DQUARKS=ON ..`

* Build supernova-plugins: (default 'OFF')

`sc3-plugins/build/$ cmake -DSUPERNOVA=ON ..`

* Print all cmake options:

`sc3-plugins/build/$ cmake -L ..`

### Starting over

If something went wrong and you want to start from scratch, delete everything in the build directory that you made:

`sc3-plugins/build/$ make uninstall` (only if you did `make install` before)

`sc3-plugins/build/$ rm -r *`

# Adding plugins

If you add a new plugin, please keep to this pattern:

Add a folder in the `source` directory, that ends with "UGens", and starting with whichever standard pattern in the filename you have for the UGens.
Your source files should go into this directory. Beware, all your sc-specific files (`sc|schelp|...`) should go into a `sc` subdirectory.

e.g. the GLitchUGens comprise:

* `source/GlitchUGens/GlitchUGens.cpp`
* `source/GlitchUGens/sc/GlitchUGens.sc`
* `source/GlitchUGens/sc/HelpSource/Classes/GlitchBPF.schelp`
* `source/GlitchUGens/sc/HelpSource/Classes/GlitchBRF.schelp`
* `source/GlitchUGens/sc/HelpSource/Classes/GlitchHPF.schelp`
* `source/GlitchUGens/sc/HelpSource/Classes/GlitchRHPF.schelp`

then edit `sc3-plugins/source/CMakeLists.txt` and add your folder to `PLUGIN_DIRS`.


For the Quark-installable option, there is a document Generate_Quark.scd in `quarks` folder, which also creates a help file, listing all the classes and helpfiles.
It indexes the base directory of the Extensions for each UGen, and help, and Help that are in each directory.

It also creates a Quark file that goes in the `build/DIRECTORY` folder.

# Creating a package

To package SC3-plugins:

### dmg (osx)

```shell
sc3-plugins/$ mkdir build && cd build
sc3-plugins/build/$ cmake -DSC_PATH=/PATH/TO/SC -DOSX_PACKAGE=1 ..
sc3-plugins/build/$ make && make install
```

The dmg image will be generated in `sc3-plugins/build/build_osx`.
The dmg image will contain `License.txt`, `README.txt` and `SC3plugins`.
note: the quarks DIRECTORY is included in the dmg by default.

### tgz/zip

```shell
sc3-plugins/$ mkdir build && cd build
sc3-plugins/build/$ cmake -DSC_PATH=/PATH/TO/SC ..
```

and then, with your chosen PKG_FORMAT (TGZ|ZIP):

`sc3-plugins/build/$ cpack -G PKG_FORMAT`

The package will end up in `sc3-plugins/build`.