# CMAKE_INSTALL_PACKAGE should be 'sc3-plugins/<cmake_build_folder>/build_osx'
# this is the folder layout for the generated dmg ('build_osx' is not included):
# build_osx/README.txt
# build_osx/License.txt
# build_osx/SC3plugins
execute_process(COMMAND cp "${osx_readme}" "${CMAKE_INSTALL_PREFIX}/README.txt"
                COMMAND cp "${license}" "${CMAKE_INSTALL_PREFIX}"
                COMMAND hdiutil create
                            -srcfolder "${CMAKE_INSTALL_PREFIX}"
                            -format "UDZO" -volname "SC3-plugins"
                            "${CMAKE_INSTALL_PREFIX}/SC3ExtPlugins.dmg")
