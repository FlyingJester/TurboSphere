TurboSphere
===========

Description
-----------

TurboSphere is a reimplementation and modernization of the [Sphere RPG Engine](spheredev.org/). It is a simple, fast, and powerful game engine. It is a clean reimplementation of Sphere, no source code and very little code structure is inherited from Sphere.

TurboSphere uses plugins for all its functionality.

TurboSphere uses [SDL2](http://www.libsdl.org/) and [Google's V8](http://code.google.com/p/v8/). This combination allows for simple or complex games to be created easily using JavaScript.

TurboSphere also uses the T5 key/value file reading library, and can optionally use AMD's libm instead of the system libm.

Want to ask a question? TurboSphere is supported on the [Sphere forums](http://forums.spheredev.org/). Be sure to mention you are using TurboSphere, as the original Sphere is also supported there.

For building instructions, see the docs/INSTALL files. The Linux file includes instructions for getting and building the 
right version of V8. Scons is the only current build system used for TurboSphere.

Installation
------------

If you have a binary distribution for Windows, just decompress the archive and run the turbosphere executable. You can get binary releases for Windows from TurboSphere's [SourceForge Page](http://sourceforge.net/projects/turbosphere/) or from the [FlyingJesterEntertainment Site](http://flyingjesterentertainment.webs.com/turbosphere.htm).

If you want to use TurboSphere on another platform, skip down to the Compiling section. TurboSphere is tested to compile and run on Linux, and has in the past been compiled and run on OS X. If you have any issues with installation, compilation, or anything else, see the Getting Help section.

Compiling
---------

*TODO

For basic info, see the install files in the docs directory. the Linux file explains how to get and build V8, and the Scons file gives basic info on using scons. 

On Windows, always append `--useamdlibm=n` to `scons`.

Getting Help
------------

If you get a crash, have trouble compiling TurboSphere, or find a bug, please leave a ticket with at least a somewhat detailed description on the [GitHub Page](https://github.com/flyingjester/turbosphere), or--if you are feeling more conversational--report it on the [TurboSphere thread](http://forums.spheredev.org/index.php/topic,13.msg53.html#msg53) in the Spherical Forums. Every ticket and comment on these pages is read by real human beings. Every crash and bug is taken seriously.

If you want help learning how to use TurboSphere or Sphere, or have any questions or comments about TurboSphere, feel free to comment on the [TurboSphere thread](http://forums.spheredev.org/index.php/topic,13.msg53.html#msg53) in the Spherical Forums.

TurboSphere is actively developed. Improving it and making it more stable are very important, and feedback is vital for this. You really can help make the engine better just by giving feedback.

License
-------

TurboSphere is dually licensed under the BSD 3-Clause License (aka Modified BSD License) and the Zlib license.

License Text:
`
Copyright (c) 2012-2013 Martin McDonough.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.  

3. The name of the author may not be used to endorse or promote products derived from this software without specific prior written permission.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software. This notice may not be removed or altered from any source distribution.


THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
`
Related Projects
----------------

TurboSphere is a reimplementation of the Sphere RPG Engine. There is another reimplementation of Sphere under development, called Sphere-sfml. It can be found on the Spherical Forums.

Sphere-sfml is written in C#. It is not feature-complete yet, but in general has more functionality than TurboSphere implements. There are cases in which each has features the other lacks. It lacks TurboSphere's plugin system, advanced audio support, and dynamic recompiling JavaScript engine. Sphere-sfml has a precompiler for JS which compiles it directly to CIL and therefore benefitting from improvements made to Microsoft .NET technology. Sphere-sfml has much better MapEngine support than TurboSphere, which is a vital component of the original Sphere engine.

Several projects have been compared to Sphere in the past. These include:

-   Love2D
-   Verge
-   IKA