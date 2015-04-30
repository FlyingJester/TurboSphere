TurboSphere
===========

[![Build Status](https://semaphoreci.com/api/v1/projects/865f14b2-7958-4f6b-993f-cf99f4ade2c6/412075/badge.svg)](https://semaphoreci.com/flyingjester/turbosphere)

Description
-----------

TurboSphere is a reimplementation and modernization of the [Sphere RPG Engine](spheredev.org/). It is a simple, fast, and powerful game engine. It is a clean reimplementation of Sphere, no source code and very little code structure is inherited from Sphere.

TurboSphere uses plugins for all its functionality.

Why Use TurboSphere to Make a Game
----------------------------------

TurboSphere has a fast and reliable JavaScript engine, SpiderMonkey, which gives all the power of scripting as a web app has. The Sphere family of game engines differ in several key ways from web-based development, however:
* The Event Loop is exposed to script
* Direct access to audio, graphics, input, and networking systems is exposed

Many well-written HTML5 games are easily ported to TurboSphere, and can run even faster in the less abstracted and lighter-weight environment TurboSphere provides over a web browser.
The use of plugins allows more advanced developers who want to use C or C++ for certain tasks to easily integrate native C/C++ code with JavaScript components.

This combination of the well-known, performant JavaScript language and more direct access to a computer or devices capabilities are a chief strength of TurboSphere.

TurboSphere is free to use, modify, and distribute, and its permissive license is intended to allow for ease of use in both open source and proprietary projects.

Technical Description
---------------------

TurboSphere uses [SDL2](http://www.libsdl.org/), OpenGL 3.3 or 4.1, and [Mozilla's SpiderMonkey](https://developer.mozilla.org/en-US/docs/Mozilla/Projects/SpiderMonkey). This combination allows for simple or complex games to be created easily using JavaScript.

TurboSphere also uses the T5 file I/O library.

Want to ask a question? TurboSphere is supported on the [Sphere forums](http://forums.spheredev.org/). Be sure to mention you are using TurboSphere, as the original Sphere is also supported there.

For building instructions, see the docs/INSTALL files. Scons is the only current build system used for TurboSphere.

Installation
------------

If you have a binary distribution for Windows, just decompress the archive and run the turbosphere executable. You can get binary releases for Windows from TurboSphere's [SourceForge Page](http://sourceforge.net/projects/turbosphere/) or from the [FlyingJesterEntertainment Site](http://flyingjesterentertainment.webs.com/turbosphere.htm).

If you want to use TurboSphere on another platform, skip down to the Compiling section. TurboSphere is tested to compile and run on Linux, and has in the past been compiled and run on OS X. If you have any issues with installation, compilation, or anything else, see the Getting Help section.

Compiling
---------

####Prerequisites

* C++ Compiler that at least mostly supports C++11. See list of [Verified Compilers].
* SpiderMonkey 40 and SDL2. More on how to get these later.
* Windows or a Unix-based OS.
* Windows or X11.
* OpenGL 3.3 or greater (OpenGL 2.1 possible)
* OpenAL (usually OpenAL-soft), or Mac OS X.
* libsndfile

####Verified Compilers

TurboSphere is known and tested to compile using the following compilers:

* GCC 4.8
* Clang 3.2

It is suspected, but unverified or not tested to work on the following compilers:

* GCC 4.3-4.7 on Linux
* GCC 4.6-4.7 on OS X
* MSVC 2013
* Solaris Studio with GNU libstdc++

In addition, it is theoretically possible, but completely untested to work on the following compilers (here be dragons!):

* MingW
* Android NDK 64-bit

####How To Compile TurboSphere

To compile TurboSphere, you will need to get three third-party libraries, along with several addon libraries for them.

* SpiderMonkey
* SDL2
- SDL2_image
* libsndfile

You can download SDL2 and SDL2_image in binary form for Windows, and Bass in binary form for Linux. Most Linux distros have SDL2 in their package managers (SDL 1.3 is SOMETIMES acceptable).

Get SDL2 from [LibSDL.org](http://libsdl.org/). You can SDL2_image [here](http://www.libsdl.org/projects/SDL_image/).

To build SpiderMonkey, it is easiest to build Gecko in its entirety. This can be done by cloning [Mozilla's Github repo for Firefox](https://github.com/Mozilla/Gecko-dev) and copying the files from js/src/dist. Note that you do NOT need to actually build all of Gecko.

From your Gecko checkout:
`./mach configure`
`cd js/src`
`./configure`
`make`

Place the js/src/dist/include folder in TurboSphere's src/thirdparty/ directory (so that it is src/thirdparty/include), and place libmozjs.so/libmozjs.dylib/mozjs.lib into /src/thirdparty/lib.

On Linux and OS X, you will need to rename the library from libmozjs-.so/libmozjs-.dylib to libmozjs.so/libmozjs.dylib, and on OS X you will need to change the install name using `install_name_tool` to be `@executable_path/libmozjs.dylib`. Be sure to do this before building TurboSphere.

On Linux, you will need to put libmozjs.so in /usr/local/lib. On OS X, you will need a copy in the binary directory (being sure it is a copy with the doctored install name). On Windows, place libmozjs-.dll into the binary directory. Note that you may or may not need to rename the dll to libmozjs.dll.

Download the TurboSphere submodules with `git init submodule` and `git update submodule`.

Finally, to compile TurboSphere:

`scons`

This will install the plugins and engine to the TurboSphere root's bin directory. On Linux, you will need to copy libconfigmanager, libgraphiccommon, and libgraphicalg to /usr/local/lib.

On OS X, you sometimes need to run scons twice to ensure that the install names are correctly set on the core libraries and that turbosphere (the binary) is properly copied. This is only an issue on clean builds, subsequent builds will always have the newest binaries in the bin directory.

To use TurboSphere, just navigate to bin/Release and try `./turbosphere`. It _should_ run the test script, which demonstrates some of its abilities. If it crashes be sure to let us know!

####Things to Note

Getting Help
------------

If you get a crash, have trouble compiling TurboSphere, or find a bug, please leave a ticket with at least a somewhat detailed description on the [GitHub Page](https://github.com/flyingjester/turbosphere), or--if you are feeling more conversational--report it on the [TurboSphere thread](http://forums.spheredev.org/index.php/topic,13.msg53.html#msg53) in the Spherical Forums. Every ticket and comment on these pages is read by real human beings. Every crash and bug is taken seriously.

If you want help learning how to use TurboSphere or Sphere, or have any questions or comments about TurboSphere, feel free to comment on the [TurboSphere thread](http://forums.spheredev.org/index.php/topic,13.msg53.html#msg53) in the Spherical Forums.

TurboSphere is actively developed. Improving it and making it more stable are very important, and feedback is vital for this. You really can help make the engine better just by giving feedback.

License
-------

TurboSphere is dually licensed under the BSD 3-Clause License (aka Modified BSD License) and the Zlib license.

License Text:

   Copyright (c) 2012-2014 Martin McDonough.

   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

   3. The name of the author may not be used to endorse or promote products derived from this software without specific prior written permission.

   Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software. This notice may not be removed or altered from any source distribution.


   THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Additonally, certain parts of TurboSphere are released under other licenses. See lib/thirdparty for these licenses.

libyyymonitor is under the TurboSphere license, rather than the GPL, with permission of the Kashyyyk project.

Related Projects
----------------

TurboSphere is a reimplementation of the Sphere RPG Engine. There are other reimplementations of Sphere under development. The other main Sphere projects are Sphere-sfml and MiniSphere. Both these, as well as TurboSphere, can be found on the Spherical Forums.

Sphere-sfml is written in C#. It is more feature-complete than TurboSphere, but is only intended to run on Windows. It has a somewhat slower JavaScript environment than TurboSphere's, but is also more stable.

MiniSphere is aimed at being a simple, if slower implementation of Sphere's API in portable C. It is in the early stages of development.

Several projects have been compared to Sphere in the past. These include:

-   Love2D
-   Verge
-   IKA
