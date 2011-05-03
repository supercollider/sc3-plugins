To package SC3-plugins:

----------
dmg (osx):
----------

    sc3-plugins/$ mkdir build && cd build

    sc3-plugins/build/$ cmake -DSC_PATH=/PATH/TO/SC -DOSX_PACKAGE=1 ..

    sc3-plugins/build/$ make && make install

The dmg image will be generated in sc3-plugins/build_osx
note: the quarks DIRECTORY is included in the dmg by default

-------------
tar.gz / zip:
-------------

    sc3-plugins/$ mkdir build && cd build

    sc3-plugins/build/$ cmake -DSC_PATH=/PATH/TO/SC ..

and then, with your chosen PKG_FORMAT (TGZ|ZIP):

    sc3-plugins/build/$ cpack -G PKG_FORMAT

The package will end up in sc3-plugins/build
