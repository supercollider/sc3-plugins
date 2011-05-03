If you add a new plugin, please keep to this pattern:

Add a folder in the 'source' directory, that ends with "UGens", and starting
with whichever standard pattern in the filename you have for the UGens.
Your source files should go into this directory. Beware, all your sc-specific
files (sc|html|txt|...) should go into a 'sc' subdirectory.

e.g. the GLitchUGens comprise:
source/GlitchUGens/GlitchUGens.cpp
source/GlitchUGens/sc/GlitchUGens.html
source/GlitchUGens/sc/GlitchUGens.sc

then edit sc3-plugins/source/CMakeLists.txt and add your folder to PLUGIN_DIRS.


For the Quark-installable option, I have created a document Generate_Quark.scd in utils/, which
also creates a help file, listing all the classes and helpfiles.
It indexes the base directory of the Extensions for each UGen, and help, and Help that are in each directory.

It also creates a Quark file that goes in the build/DIRECTORY folder.


NOTE: I also added a VERSION file, which should be updated on a regular basis, in line with the releases
 of SuperCollider.


apr. 2011, yvan volochine
jan. 2009, nescivi
