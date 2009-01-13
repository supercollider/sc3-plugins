
linux build instructions
------------------------
building sc3-plugins
--------------------

$ scons
$ scons install

The SuperCollider source is expected to be at ../ but you can set the path by passing an argument to scons:

$ scons SC3PATH=/my/path/to/supercollider_source
$ scons install

You can determine the installation directory with the option PREFIX (the default is /usr/local, and will put everything in /usr/local/share/SuperCollider).
You can either install the plugins as Quark-installable units (option QUARKS=1), or just install all of them (QUARKS=0).
E.g. if you want them in your home directory, and Quark-able

$ scons SC3PATH=/my/path/to/supercollider_source QUARKS=1 PREFIX=/home/nescivi
$ scons install

The plugins will end up in:
/home/nescivi/share/SuperCollider/SC3plugins
(Look for the helpfile SC3pluginQuarks to install the quarks.)

if the QUARKS=0 it will be:
/home/nescivi/share/SuperCollider/Extensions/SC3plugins




------------
The StkUGens
------------
If you are planning to build the StkUGens you will need the stklib. This is provided in the file release area of the sourceforge project site at:

http://sourceforge.net/project/showfiles.php?group_id=182627

- Build stk:

$ tar zxvf libstk.tar.gz
$ cd scdev/stk-4.2.1
$ ./configure
$ cd src
$ make

Or, if your distribution features the stklib, install the development version.

To build, execute scons as follows:

$ scons STK=yes STKPATH=/path/to_stklib/

- export the compiled UGens to a new 'Extensions' folder:
$ ./distro_linux

- For the StkUGens to work and boot scsynth succesfully if this ugen pack was installed, the rawwaves folder needs to be installed at the same level of scsynth as noted on the UGens Pack directory. a sylink works fine for this.

------------
The AY Ugens
------------

Execute:
$ scons AY=yes
$ ./distro_linux

questions - blackrain at realizedsound dot net

have fun,

x
