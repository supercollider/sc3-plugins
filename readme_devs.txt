For the installation with scons I have cleaned up the folder and plugin structure a bit,
 so it more or less follows a standard pattern.

If you add a new plugin, please keep to this pattern!


The folder with the class extensions should be in a directory, ending with "UGens", and starting
 with whichever standard pattern in the filename you have for the UGens.

e.g. the JoshUGens comprise:
source/JoshAmbiUGens.cpp
source/JoshGrainUGens.cpp
source/JoshPVUGens.cpp
source/JoshUGens.cpp

and have their Extensions in the directory:
build/JoshUGens


For the Quark-installable option, I have created a document Generate_Quark.scd in utils/, which
also creates a help file, listing all the classes and helpfiles.
It indexes the base directory of the Extensions for each UGen, and help, and Help that are in each directory.

It also creates a Quark file that goes in the build/DIRECTORY folder.


NOTE: I also added a VERSION file, which should be updated on a regular basis, in line with the releases
 of SuperCollider.


jan. 2009, nescivi