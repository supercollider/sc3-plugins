
linux instructions

requirements
------------
sc3-plugins svn checkout is expect live inside the supercollider source tree.
The stk library is needed as well if you are planning to build the StkUGens.
this is provided in the file release area of the sourceforge site.

http://sourceforge.net/project/showfiles.php?group_id=182627

the library has to be prebuilt prior attempting to build sc3-plugins.

to build stk:

$ cd scdev/stk-4.2.1
$ ./configure
$ cd src
$ make

building sc3-plugins
--------------------
the directory tree will look something like this:


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
    stk-4.2.1\
       stk lib stuff


building sc3-plugins:

$ cd supercollider/sc3-plugins
$ scons
$ ./distro_linux

where supercollider is the name you gave to your supercollider source tree.

the resulting Extensions folder can now be moved out of the tree to your sc extensions folder (or even better create a symlink).
you can add a sub dir named 'test' inside this Extensions directory and drop folders in there to disable a particular ugens pack.

for the StkUGens to work and boot scsynth succesfully if this ugen pack was installed, the rawwaves folder needs to be installed at the same level of scsynth as noted on the UGens Pack directory. a sylink works fine for this.


questions - blackrain at realizedsound dot net

have fun,

x
