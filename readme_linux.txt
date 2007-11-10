
linux build instructions
------------------------
The sc3-plugins svn checkout is expect live inside the supercollider source tree.

The directory tree will look something like this:

    supercollider\     -   supercollider source tree
       build\
       headers\
       source\
       etc...
       sc3-plugins\    -   sc3-plugins source tree
	   build\
	   compat\
	   lib\
	   etc...
    stk-4.2.1\         -   the stklib
       stk lib stuff


building sc3-plugins
--------------------

$ cd supercollider/sc3-plugins
$ scons
$ ./distro_linux

where supercollider is the name of the supercollider source tree.

The compiled UGens, Classes and Helpfiles, will be placed into a new 'Extensions' directory in sc3-plugins. This can be moved or better yet, create a symlink to it from your extensions folder.

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
