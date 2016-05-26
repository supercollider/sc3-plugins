ATK for Supercollider3 : Read Me
========================

This is the Supercollider3 version of the Ambisonic Toolkit (ATK).
It can be used with [Supercollider3](http://supercollider.github.io/) on OSX,
Linux and Windows, and is distributed via the
[sc3-plugins](https://github.com/supercollider/sc3-plugins) project.

The Ambisonic Toolkit (ATK) is intended to bring together a number of
tools and methods for working with Ambisonic surround sound. The intention
is for the toolset to be both ergonomic and comprehensive, providing both
classic and novel algorithms to creatively manipulate and synthesise
complex Ambisonic soundfields.

The tools are framed for the user to think in terms of the soundfield
kernel. By this, it is meant the ATK addresses the holistic problem of
creatively controlling a complete soundfield, allowing and encouraging
the composer to think beyond the placement of sounds in a sound-space
and instead attend to the impression and image of a soundfield. This
approach takes advantage of the model the Ambisonic technology presents,
and is viewed to be the idiomatic mode for working with the Ambisonic
technique.

We hope you enjoy the ATK!

For more information please visit the [Ambisonic Toolkit
website](http:www.ambisonictoolkit.net/) or send us an
[e-mail](mailto:info[at]ambisonictoolkit.net).



&nbsp;

&nbsp;

Installing
==========

&nbsp;

Requirements
------------

* ATK for [SuperCollider3](http://supercollider.github.io) requires version 3.5
or later. Download the latest version
[here](http://supercollider.github.io/download), or fork the source code at
[GitHub](http://supercollider.github.io/).

&nbsp;

sc3-plugins
-----------

Compiled releases are available from the
[sc3-plugins](https://github.com/supercollider/sc3-plugins/releases)
releases page.

Place the downloaded `SC3plugins` folder in your `Extensions` folder. On Mac OS X, this resolves to:

    ~/Library/Application Support/SuperCollider/Extensions

You may need to create the `Extensions` folder if it does not already exist.  

On other platforms, you can find where this is by running the following line of code in SuperCollider:

		(  
    // post the directory in which to move the SC3Plugins folder  
    Platform.userExtensionDir.postln;  
    )  
    (  
    // alternatively, SC can open it for you  
    // (assuming it already exists! - you may need to create /Extensions)  
    Platform.userExtensionDir.openOS;  
    )  

Additionally, the SuperCollider3 version of the ATK has a number of
dependencies. Please install the following:

* Install the [MathLib Quark](https://github.com/supercollider-quarks/MathLib)
most simply by running `Quarks.gui` in the SuperCollider IDE. Further install
options and information on Quarks can be found in SuperCollider's Help system.
See [Using Quarks](http://doc.sccode.org/Guides/UsingQuarks.html) and also the
[supercollider-quarks](https://github.com/supercollider-quarks/quarks) GitHub page.

* Download and install [ATK Kernels](http://www.ambisonictoolkit.net/download/kernels/).
* Download and install [ATK Sound File Example Recordings](http://www.ambisonictoolkit.net/download/recordings/).

<p><strong>NOTE:</strong> The ATK requires SuperCollider3 version 3.5 or later. Download the latest version <a href="http://supercollider.github.io/download" target="_blank">here</a>, or fork the source code at <a href="http://supercollider.github.io/" target="_blank">GitHub</a>.</p>

</div>

&nbsp;

### Source code

You can build ATK for SuperCollider from the [sc3-plugins](https://github.com/supercollider/sc3-plugins) source-code.


&nbsp;

If you are using Ambisonic Toolkit with Reaper as well,
the convolution kernels are installed in the same place and have
the exact same content. We do not expect this to cause any conflicts.

If you want to take a look at the installed files and do not see the
Library folder in Finder, please press the ALT button while clicking
the "Go" menu in Finder. The Library folder will show up as an
additional option.

&nbsp;

Need Some Sound Files to Play Around With?
------------------------------------------

You can find a collection of sound files here. (Download as part of installation):

* [http://www.ambisonictoolkit.net/download/recordings/](http://www.ambisonictoolkit.net/download/recordings/)

&nbsp;

Additional sound files can be grabbed from these fine sources:

* [http://ambisonia.com/](http://ambisonia.com/).
* [http://www.freesound.org/browse/tags/B-format/](http://www.freesound.org/browse/tags/B-format/).
* [http://www.surround-library.com/](http://www.surround-library.com/) (commercial library ambisonic sound effects).
* [http://www.spheric-collection.com/](http://www.spheric-collection.com/) (commercial library ambisonic sound effects).

&nbsp;

And most of the catalogue of Nimbus Records are UHJ recordings:

* [http://www.wyastone.co.uk/](http://www.wyastone.co.uk/).

&nbsp;

&nbsp;

Feedback and Bug Reports
========================

Known issues are logged at [GitHub](https://github.com/ambisonictoolkit/atk-sc3/issues).

If you experience problems or have questions pertaining to the ATK for Reaper plugins, please create an issue in the [ATK-Reaper issue tracker](https://github.com/ambisonictoolkit/atk-sc3/issues).

If you use the plugins for some project, please [let us know](mailto:info[at]ambisonictoolkit.net). We [plan on](https://github.com/ambisonictoolkit/ambisonictoolkit.github.io/issues/9) adding a gallery of example artistic and creative projects that make use of the Ambisonic Toolkit.

&nbsp;


List of Changes
---------------



&nbsp;

&nbsp;

Credits
=======

&nbsp;

Copyright the ATK Community, Joseph Anderson, and Joshua Parmenter, 2011, 2016.

* J Anderson : [[e-mail]](mailto:j.anderson[at]ambisonictoolkit.net)
* J Parmenter : [[e-mail]](mailto:j.parmenter[at]ambisonictoolkit.net)

&nbsp;

The development of the ATK for SuperCollider3 is
supported by [DXARTS, Center for Digital Arts and Experimental Media](https://dxarts.washington.edu/).

The filter kernels distributed with the Ambisonic Toolkit are licensed
under a Creative Commons Attribution-Share Alike 3.0 Unported [(CC BY-SA 3.0)](http://creativecommons.org/licenses/by-sa/3.0/) License and
are copyright the Ambisonic Toolkit Community and Joseph Anderson,
2011.

&nbsp;

&nbsp;

Third Party Notices
===================

&nbsp;

Diametric Decoder Theorem (DDT) decoding
----------------------------------------

Support for Gerzon's Diametric Decoder Theorem (DDT) decoding algorithm
is derived from Aaron Heller's Octave code available at:
http://www.ai.sri.com/ajh/ambisonics/

Benjamin, et al., "Localization in Horizontal-Only Ambisonic Systems"
Preprint from AES-121, 10/2006, San Francisco

Implementation in the SuperCollider3 version of the ATK is by [Joseph
Anderson](mailto:j.anderson[at]ambisonictoolkit.net).

&nbsp;

Irregular array decoding
------------------------

Irregular array decoding coefficients (5.0) are kindly provided by
Bruce Wiggins: http://www.brucewiggins.co.uk/

B. Wiggins, "An Investigation into the Real-time Manipulation and
Control of Three-dimensional Sound Fields," PhD Thesis, University of
Derby, Derby, 2004.

&nbsp;

CIPIC HRTF Database (University of California)
----------------------------------------------

V. R. Algazi, R. O. Duda, D. M. Thompson, and C. Avendano, "The CIPIC
HRTF Database," in Proceedings of the 2001 IEEE ASSP Workshop on
Applications of Signal Processing to Audio and Acoustics, New Paltz, NY,
2001.

"The CIPIC HRTF Database - CIPIC International Laboratory." [Online].
Available: <http://interface.cipic.ucdavis.edu/sound/hrtf.html>.
[Accessed: 07-Jul-2011].

**CIPIC Notices:**

Copyright (c) 2001 The Regents of the University of California. All
Rights Reserved

Disclaimer

THE REGENTS OF THE UNIVERSITY OF CALIFORNIA MAKE NO REPRESENTATION OR
WARRANTIES WITH RESPECT TO THE CONTENTS HEREOF AND SPECIFICALLY DISCLAIM
ANY IMPLIED WARRANTIES OR MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR
PURPOSE.

Further, the Regents of the University of California reserve the right
to revise this software and/or documentation and to make changes from
time to time in the content hereof without obligation of the Regents of
the University of California to notify any person of such revision or
change.

Use of Materials

The Regents of the University of California hereby grant users
permission to reproduce and/or use materials available therein for any
purpose- educational, research or commercial. However, each reproduction
of any part of the materials must include the copyright notice, if it is
present. In addition, as a courtesy, if these materials are used in
published research, this use should be acknowledged in the publication.
If these materials are used in the development of commercial products,
the Regents of the University of California request that written
acknowledgment of such use be sent to:

CIPIC- Center for Image Processing and Integrated Computing University
of California 1 Shields Avenue Davis, CA 95616-8553

&nbsp;

Listen HRTF Database (IRCAM)
----------------------------

"LISTEN HRTF DATABASE." [Online]. Available:
<http://recherche.ircam.fr/equipes/salles/listen/>. [Accessed:
07-Jul-2011].

**IRCAM Notices:**

Copyright (c) 2002 IRCAM (Institut de Recherche et Coordination
Acoustique/Musique). All Rights Reserved

Use of Materials

The Listen database is public and available for any use. We would
however appreciate an acknowledgment of the database somewhere in the
description of your work (e.g. paper) or in your development.

Contacts:

Olivier Warusfel, Room Acoustics Team, IRCAM 1, place Igor Stravinsky
75004 PARIS, France
