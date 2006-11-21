
linux instructions

requirements
------------
the plugins expect a supercollider source tree at the same level as a sc3-plugins svn check out.
at the time of this writting, the stk library is needed as well.
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

scdev/ 
    supercollider\
       supercollider source is here
    stk-4.2.1\
       stk lib stuff
    sc3-plugins\

(scdev can be any name)

build sc3-plugins:

$ cd scdev/sc3-plugins
$ scons
$ ./distro_linux

the resulting Extensions folder can now be moved out of the tree to your sc extensions folder.
you can add a test sub dir and drop folders in there to disable a particular ugens pack.

questions - blackrain at realizedsound dot net

have fun,

x
