sc3-plugins: Information for Developers
=======================================

## Building on Windows

This is doable (with VS 2013), but you will have to manually set the CMake variables
`FFTW3_INCLUDE_DIR` and `FFTW3F_LIBRARY` to the directory of FFTW3 and the lib file created while
building SuperCollider. See the SuperCollider repo's `README_WINDOWS.md` file for instructions
related to that.

## Packaging / Releasing

### Release

When a release is made, a binary is automatically uploaded for macOS, but will need to be renamed.

### Windows packaging

Build the `package` target to generate a zip file.

### macOS packaging

Build the `package` target to generate a tarball.

Another (legacy) option is to set `-DOSX_PACKAGE=ON` when configuring cmake, and then building the
`install` target. This will generate a `dmg`. There is no real benefit to using this, and it may be
removed in future versions of this software.
