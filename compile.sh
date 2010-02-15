#!/bin/sh

if [ $# == 0 ]; then
    echo 
"--------------------------------------------------------------------------------
Building SuperCollider as a 32 bit UNIVERSAL BINARY (intel + ppc).
For faster build, or if you don't have UB versions of the libs 
(e.g. libsndfile),you may specify your computer's architecture 
by adding ARCHS=i386 or ARCHS=ppc as a parameter to this script.
Or, if you are using Mac OS 10.6, you may also specify ARCHS=32_64
to build a 32/64-bit Universal Binary version of the server and 
plugins along with a 32-bit version of the application
--------------------------------------------------------------------------------
"
fi

if [ $# != 0  ]; then
    if [ $* == "ARCHS=32_64" ]; then
        echo "Building 32/64 bit sc3-plugins"
        xcodebuild -project xSC3ExtPlugins-Universal.xcodeproj -target "All32-64" -configuration "Deployment32-64" build || exit
        echo "Done."
    else
        echo "Building sc3-plugins"
        xcodebuild -project xSC3ExtPlugins-Universal.xcodeproj -target "All" -configuration "Deployment" $* build || exit
        echo "Done."
    fi
else
echo "Building sc3-plugins"
xcodebuild -project xSC3ExtPlugins-Universal.xcodeproj -target "All" -configuration "Deployment" build || exit
echo "Done."
fi

