
This folder makes it easy to build simple SuperCollider plugins:

**EVERY** .cpp file you place in this folder will be assumed to define a
SuperCollider plugin, and will be compiled as part of the sc3-plugins build
process. (You can softlink .cpp files in here if you like.)

Note that this simple approach isn't suitable for all plugins - if you have
plugins which need auxiliary .h or .cpp files, or which use external lilbraries,
they probably need their own custom build commands.

