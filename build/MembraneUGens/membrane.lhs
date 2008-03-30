    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

> module Main where

> import Sound.OpenSoundControl
> import Sound.SC3
> import Sound.SC3.UGen.Rate (Rate(AR))

> membraneCircle :: UGen -> UGen -> UGen -> UGen
> membraneCircle input tension loss = mkOsc AR "MembraneCircle" [input, tension, loss] 1

> membraneHexagon :: UGen -> UGen -> UGen -> UGen
> membraneHexagon input tension loss = mkOsc AR "MembraneHexagon" [input, tension, loss] 1

Excites the mesh with some pink noise, triggered by the mouse button,
mouseX is tension and mouseY is duration of excitation.

> startSynth m = do let x = mouseX KR 0.01 0.1 Linear 0.1
>                       loss = mouseY KR 0.999999 0.999 Exponential 0.1
>                       sin = sinOsc AR 1000 0
>                       wobble = sinOsc KR 2 0
>                       tension = x + (wobble * 0.0001)
>                       p = envPerc 0.0001 1 1 [EnvNum (-4), EnvNum (-4)]
>                       trigger = mouseButton KR 0 1 0.1
>                       e = envGen KR trigger 1 0 0.1 DoNothing p
>                   n <- pinkNoise AR
>                   withSC3 reset
>                   audition $ out (MCE [0, 1]) $ m (n * e * 0.1) tension loss

> startHexagon = startSynth membraneHexagon

> startCircle = startSynth membraneCircle
