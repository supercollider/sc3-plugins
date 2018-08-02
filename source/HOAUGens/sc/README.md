# HOAUGens

[HOA](https://en.wikipedia.org/wiki/Ambisonics#Higher-order_Ambisonics) for [SuperCollider](http://supercollider.github.io/).


## Overview

The HOAUGens is a collections of SuperCollider Unit Generators designed for [Higher-order Ambinsonics](https://en.wikipedia.org/wiki/Ambisonics#Higher-order_Ambisonics). These UGens are distributed as part of the [sc3-plugins](https://github.com/supercollider/sc3-plugins) project.

The HOAUGens are intended to be used with their accompagnying SuperCollider Quark called [SC-HOA](https://github.com/florian-grond/SC-HOA). Please consult the SC-HOA repository for additional information.

As a whole, the HOA library provides classes to encode, transform and decode Higher Order Ambisonics sound signals up to order 5. The basis for the library are UGens compiled from Faust code written by Pierre Lecomte, see [ambitools](https://github.com/sekisushai/ambitools). Documentation and tutorials are partly based on the [ambitools documentation](https://github.com/sekisushai/ambitools/blob/master/Documentation/documentation.pdf) (pdf).

Additional plane wave encoders and optimizers have been added, adpated from [CICM](http://cicm.mshparisnord.org/)'s [HoaLibrary](http://hoalibrary.mshparisnord.fr/) and [Aaron Heller](https://www.sri.com/about/people/aaron-j-heller)'s [ambidecodertoolbox](https://bitbucket.org/ambidecodertoolbox/adt.git).  

The SuperCollider library was written by [Florian Grond](http://www.grond.at). It includes soundfield recordings made together with Romain Dumoulin using the Eigenmike from [CIRMMT](http://www.cirmmt.org).


## Installation

The HOAUGens can be compiled and installed as part of the sc3-plugins collection. Compilation of the HOAUGens can be turned `ON` by setting the corresponding cmake variable:
```shell
cmake -DHOA_UGENS=ON ..
```
This variable is set to `OFF` by default.

The HOAUGens can be compiled successfully on **macOS** and **Linux**.

**note for Windows** The HOAUGens are unable to build using MSVC due a limitation in the depth of bracket nesting that is permitted by this compiler. These UGens have not been tested on Windows, and any build for this platform should be considered experimental.

**note for Embedded Linux systems** Although these UGens can be built on Linux, there have been issues compiling them successfully on small Embedded Linux systems (Raspberry Pi, etc.).


## Acknowledgements

The implementation of SC-HOA was supported by a postdoctoral fellowship of Fonds de Recherche du Québec - Société et Culture (FRQSC) http://www.frqsc.gouv.qc.ca/ conducted at CIRMMT https://www.cirmmt.org/ and through the Metalab at the Société des Arts Technologiques http://sat.qc.ca/ in Montreal.
Special thanks goes to Nicolas Bouillot, Michal Seta, Till Bovermann and Carlo Capocasa.
