GNU GPL license enclosed in MLfftwUGens.license

various further GNU GPL license are involved: SuperCollider 3, fftw and Tartini

This pack is now released as part of the sc3-plugins project and therefore should be more easily compiled. However, if compiling you will need fftw3f, as detailed below. 


The pack should be compiled to single SC3 plug-in (instructions below) and will contain various UGens:

AnalyseEvents2: on-the-fly event extraction 
AutoTrack- autocorrelation beat tracker
Concat- realtime concatenative synthesiser for live streams
Qitch- constant Q transform pitch detector
Tartini- modified autocorrelation pitch detector

CAUTION: if using this pack as well as BBCut2 and/or the SLUGens (and even the original standalone versions of Qitch and Tartini) please be careful about duplicate class and help files. You will need to download bbcut2 if you want to use some further capabilities inherent in AnalyseEvents2 and AutoTrack, but this plugin takes care of the cross-platform part, i.e. the plugins themselves, which have previously been Mac only.  


How to use:

put the .sc, .html and any platform-specific .scx/.so file in your Extensions folder for SuperCollider:

on my Mac this is :

/Library/Application Support/SuperCollider/Extensions/

Startup SuperCollider or recompile SuperCollider if it is already open (cmd+K on a Mac). Now open the help file in SC, boot the server and try the examples. 




How to build:

The intention is this plug-in works cross-platform and should hopefully compile on Mac (PPC and Intel), Windows and Linux. You will need fftwf, which is a special version of FFTW 

To compile this plug-in yourself, you'll need fftw

http://www.fftw.org/

You need to compile a float versions of the library:

./configure --enable-float

(for compiling a dynamic version of the library rather than a static also use the flag --enable-shared)

To compile, compile all the files as a single plug-in. In Xcode you would duplicate an existing SC3 plugin target, and modify it to contain all the source code files for MachineListening plus some version of the fftwf library. I can't give specific instructions for other operating systems but it should work in the usual way for your system, just make sure fftwf is included at link time.  


Mac OS X hints:

If you are using Xcode and want to do static linking, where the fftw3f.a library will go inside the Mach-O bundle for the plug-in (making a much fatter plug-in but also letting users use it without needing to install fftw3f themselves) you need to- 

1) Make sure you use the same version of gcc you used to build fftw

2) for static linking, make sure no dylib versions are in /usr/local/lib 
I moved them out the way temporarily via

sudo mv libfftw*.dylib ~/Desktop/tmpfftw


You can check what the dependencies of a built Mach-O bundle are using the following commandline developer tool:

otool -L Tartini.scx

If libfftw3f.dylib is listed here, you haven't made a standalone and it will only work on other people's  computers if they have fftw3f installed.  



