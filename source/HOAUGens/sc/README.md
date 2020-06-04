# HOAUGens

[HOA](https://en.wikipedia.org/wiki/Ambisonics#Higher-order_Ambisonics) for [SuperCollider](http://supercollider.github.io/).


## Overview

The HOAUGens is a collections of SuperCollider Unit Generators designed for [Higher-order Ambinsonics](https://en.wikipedia.org/wiki/Ambisonics#Higher-order_Ambisonics). These UGens are distributed as part of the [sc3-plugins](https://github.com/supercollider/sc3-plugins) project.

The HOAUGens are intended to be used with their accompagnying SuperCollider Quark called [SC-HOA](https://github.com/florian-grond/SC-HOA). Please consult the SC-HOA repository for additional information.

As a whole, the HOA library provides classes to encode, transform and decode Higher Order Ambisonics sound signals up to order 5. The basis for the library are UGens compiled from Faust code written by Pierre Lecomte, see [ambitools](https://github.com/sekisushai/ambitools). Documentation and tutorials are partly based on the [ambitools documentation](https://github.com/sekisushai/ambitools/blob/master/Documentation/documentation.pdf) (pdf).

The SuperCollider library was written by [Florian Grond](http://www.grond.at). It includes soundfield recordings made together with Romain Dumoulin using the Eigenmike from [CIRMMT](http://www.cirmmt.org).

## Additional lightweight Ugens

AmbiPanner is addapted from Aaron Heler's & Florian Grond's [high order ambisonic encoder](https://bitbucket.org/ambidecodertoolbox/adt/src/b6c8c11dc421e7b6a1261b03872788bbd7a8fee7/faust/ambi_panner_ambix_o5.dsp?at=master&fileviewer=file-view-default) from the [ambidecodertoolkit](https://bitbucket.org/ambidecodertoolbox/adt/overview) project.

Plane wave High order ambisonic encoder and optimisations Enc3D, InPhase, MaxRE are addapted from CICM's [HoaLib](http://hoalibrary.mshparisnord.fr/en) [faust implementation](https://github.com/CICM/HoaLibrary-Faust).

## Acknowledgements

The implementation of HOAUGens was supported by a postdoctoral fellowship of Fonds de Recherche du Québec - Société et Culture (FRQSC) http://www.frqsc.gouv.qc.ca/ conducted at CIRMMT https://www.cirmmt.org/ and through the Metalab at the Société des Arts Technologiques http://sat.qc.ca/ in Montreal.
Special thanks goes to Nicolas Bouillot, Michal Seta, Till Bovermann, Carlo Capocasa and Pierre Guillot.
