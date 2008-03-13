
Windows build instructions
------------------------

What you need
-------------

* Python (unicode version http://python.org/download/)
* scone (http://www.scons.org/)
* MingW (http://www.mingw.org/download.shtml)
* pthreads-win32 (http://sourceware.org/pthreads-win32/)
* SuperCollider3 source tree (http://sourceforge.net/projects/supercollider/)

and of course
* sc3-plugins source code (http://sourceforge.net/projects/sc3-plugins/)

Building 
--------

Note: make sure you got the Python directory in your path

Open a command shell (cmd.exe), change into the directory in which the 
sc3-plugins code is and execute

> scons SC3PATH=C:/SuperCollider3/ PTHREADSPATH=C:/pthreads-win32

adapt the paths according to your system.


Installation
------------

Copy the whole build folder (including the scx-files and the class/help files)
into your Extension folder (e.g. C:/Documents and Settings/User/SuperCollider/Extensions).

The StkUGens & The AY Ugens
---------------------------
are not yet supported, sorry.


have fun
Chris [frauenberger@dcs.qmul.ac.uk]
