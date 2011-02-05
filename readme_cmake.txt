---------------------------------
How to use the cmake build system
---------------------------------


Make a directory for the cmake build files to go into:

sc3-plugins/$ mkdir cmake_build
sc3-plugins/$ cd cmake_build

sc3-plugins/cmake_build/$ cmake ..

      It assumes you have the supercollider header include files located in:
      /usr/include/SuperCollider/

      If you have them in another location:

      sc3-plugins/cmake_build/$ cmake -DSC_PATH=/usr/local/include/SuperCollider/ ..

      or

      sc3-plugins/cmake_build/$ cmake -DSC_PATH=/path/to/sc3source/ ..


sc3-plugins/cmake_build/$ make

sc3-plugins/cmake_build/$ make install

--------------
Starting over:
--------------

  if something went wrong and you want to start from scratch, delete everything in the cmake_build directory that you made:
  sc3-plugins/cmake_build/$ rm -r *
  

--------------
open questions
--------------

  How to set install target?
