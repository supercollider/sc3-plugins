# Apple dmg packaging:
# cpack does not package *.dmg without a proper *.app so use OSX_PACKAGE option
# and a cmake script bundles the dmg
install(CODE "set(osx_readme \"${CMAKE_SOURCE_DIR}/osx_package/README-for-mac-distro.txt\")")
install(CODE "set(license \"${CMAKE_SOURCE_DIR}/license.txt\")")
install(SCRIPT "${CMAKE_SOURCE_DIR}/osx_package/CMakeOSXBundle.cmake")
