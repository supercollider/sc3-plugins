#!/bin/sh
echo "Building sc3-plugins..."
xcodebuild -project xSC3ExtPlugins-Universal.xcodeproj -target "All" -configuration "Deployment" build || exit
echo "Done."

