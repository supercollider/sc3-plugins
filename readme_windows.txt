
Windows build instructions
------------------------

What you need
-------------

* Python (unicode version http://python.org/download/)
* Scons (http://www.scons.org/)
* MinGW/MSYS (http://www.mingw.org/download.shtml)
* pthreads-win32 (http://sourceware.org/pthreads-win32/)
* FFTW (http://www.fftw.org/)
* SuperCollider3 source tree (http://sourceforge.net/projects/supercollider/)

and of course
* sc3-plugins source code (http://sourceforge.net/projects/sc3-plugins/)

Building 
--------

Note: make sure you have the Python directory in your path

Open a command shell (cmd.exe or MSYS), 'cd' into the directory where the sc3-plugins source is located, and execute:

$ scons

The SuperCollider source is expected to be at ../ but you can set the path by passing an argument to scons:

$ scons SC3PATH=C:/SuperCollider3/ 

The Pthread source is expected to be found at ../../pthreads-win32, but again, you can set the path by passing an argument to scons:

$ scons PTHREADSPATH=C:/pthreads-win32

Installation
------------

Copy the contents of the build folder (including the scx-files and the class/help files) into your Extensions folder (e.g. C:/Documents and Settings/User/SuperCollider/Extensions).

The StkUGens & The AY Ugens
---------------------------
are not yet supported, sorry.


have fun
Chris [frauenberger@dcs.qmul.ac.uk]
Martin [martin.victory@gmail.com]
