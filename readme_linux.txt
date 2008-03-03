
linux build instructions
------------------------
building sc3-plugins
--------------------

$ scons
$ ./distro_linux

The SuperCollider source is expected to be at ../ but you can set the path by passing an argument to scons:

$ scons SC3PATH=/my/path/to/supercollider_source
$ ./distro_linux

The compiled UGens, Classes and Helpfiles, will be placed into a new 'Extensions' directory in sc3-plugins. This can be moved or better yet, create a symlink to it from your extensions folder.

$ ln -s /my/path/to_sc3-plugins/Extensions /myhome/share/SuperCollider/Extensions/sc3-plugins

You can add a sub dir named 'test' inside this Extensions directory and move folders in there to disable a particular ugen pack.

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
