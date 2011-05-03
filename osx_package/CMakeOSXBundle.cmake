execute_process(COMMAND cp "${osx_readme}" "${CMAKE_INSTALL_PREFIX}/README.txt"
                COMMAND cp "${license}" "${CMAKE_INSTALL_PREFIX}"
                COMMAND hdiutil create
                            -srcfolder "${CMAKE_INSTALL_PREFIX}"
                            -format "UDRW" -volname "SC3-plugins"
                            "${CMAKE_INSTALL_PREFIX}/SC3ExtPlugins.dmg")
