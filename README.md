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

####Prerequisites

* C++ Compiler that at least mostly supports C++11. See list of [Verified Compilers].
* Bass, V8, and SDL2. More on how to get these later.
* Windows or a Unix-based OS.
* Windows or X11.
* OpenGL 2.1 or greater
* Windows DirectSound, ALSA, or Mac OS X.

####Verified Compilers

TurboSphere is known and tested to compile using the following compilers:

* GCC 4.6-4.8 on Linux
* MSVC 2010
* Windows SDK 7.1 64-bit

It is suspected, but unverified or not tested to work on the following compilers:

* GCC 4.3-4.5 on Linux
* GCC 4.6-4.8 on OS X
* Android NDK 64-bit
* MSVC 2008
* MSVC 2013

In addition, it is theoretically possible, but completely untested to work on the following compilers (here be dragons!):

* LLVM+Clang 3.1 on OS X and Linux
* MingW

Notes:
MingW is known to compile Google V8, but seems unable to compile it as a shared library. It also has some issues the plugin loading code.

Clang is verified to compile T5. Nothing else has been tested with it.

TurboSphere has been compiled using GCC 4.6 on OS X in the past (ca. TS 0.2.1), but was extremely buggy.

Android-specific and OpenGL ES-specific code exists in TurboSphere, although actually testing an Android build would require the Android Emulator and more knowledge of Java than I have. I have compiled a majority of the engine and plugin code, but was stopped when I needed to compile against OpenGL ES, and needed an Android build of Bass. Bass, V8, and SDL2 are all known to work on Android ARM and Android x86. There's theoretically nothing stopping this from working.

####How To Compile TurboSphere

To compile TurboSphere, you will need to get three third-party libraries, along with several addon libraries for them.

* Bass
- BassMidi
* V8
* SDL2
- SDL2_ttf
- SDL2_image

You can download Bass and SDL2 in binary form for Windows, and Bass in binary form for Linux. Most Linux distros have SDL2 in their package managers (SDL 1.3 is SOMETIMES acceptable).

Get Bass from [Un4Seen](http://www.un4seen.com/). You will also need the BassMidi addon. The downloads for Bass contain both x86 and x86_64 libraries, as well as development libraries in the case of Windows.

Get SDL2 from [LibSDL.org](http://libsdl.org/). You can get SDL2_ttf [here](http://www.libsdl.org/projects/SDL_ttf/) and SDL2_image [here](http://www.libsdl.org/projects/SDL_image/).

V8 is the most troublesome component used by TurboSphere to get working. I would recommend tackling this first!

To begin, you need to get a copy of V8. Specifically, starting with TurboSphere 0.3.1, a copy of V8 3.19. You can get it using subversion with the command `svn checkout http://v8.googlecode.com/svn/branches/3.19 v8`. You can also use the git repository for V8 to get a zipped copy of any release you want.

If you are on OS X, GCC is the recommended compiler. TurboSphere is most thoroughly tested to compile using GCC.

On OS X or Linux (or anywhere `make` is supported), you can build V8 with the following commands:

`make dependencies`
`make x86.release -Dlibrary=shared`

If you are using an unsupported version of GCC, you try adding `-werror=no` and `-strictaliasing=no`.

On Windows, building V8 is more difficult.

You will need Python 2.6 or 2.7. But you will need this to build TurboSphere anyway. Get that and Scons for Windows.

To build on Windows, use the following commands:

`svn co http://gyp.googlecode.com/svn/trunk build/gyp`
`svn co http://src.chromium.org/svn/trunk/deps/third_party/cygwin@66844 third_party/cygwin`
`svn co https://src.chromium.org/chrome/trunk/deps/third_party/icu46 third_party/icu`

You ABSOLUTELY need this specific copy of Cygwin. If you don't have it, the build will fail in a rather silent way.

Now, use this command to generate the project file:

`python build\gyp_v8 -Dlibrary=shared`

This will generate the MSVC project file, all.sln. Build the solution to generate v8.dll and v8.lib. If you are using the Windows SDK 7.1, you will need to change the toolchain in the project file (I know of no way to override this from the command line if you just want to use MSBUILD).

Installing Bass on Linux is simple. Use `scons --install_libs=y confwrite` as root user from the TurboSphere root. This will generate a folder and install the TurboSphere libraries to it. It is /lib64/turbosphere or /lib/turbosphere on 64-bit and 32-bit systems, respectively. Copy libbass.so to this folder. You do NOT need to put libbassmidi.so here, as TurboSphere will NOT check this folder when loading Bass plugins.

Install V8 (libv8.so) to this directory as well.

On Windows, place the *.lib files in the TurboSphere source root, and the .dll files in the bin/Release folder.

Finally, to compile TurboSphere:

On Windows, run:

`scons --build_plugins=all buildplugins --install_libs=y libinstall --useamdlibm=n`

On Linux, you will need to compile the engine and install the core TurboSphere libraries first.

As root, run:

`scons --install_libs=y confwrite`

Then, you probably will want to compile the plugins (TurboSphere does little without them).

As a normal user, run:

`scons --build_plugins=all buildplugins`

This will install the plugins and engine to bin/Release. You will probably need to run `ldconfig` after this, since the TurboSphere core libraries are installed to sub-directory of the standard library location (to make it easier to wipe out TurboSphere if you don't want it around anymore).

To use TurboSphere, just navigate to bin/Release and try `./turbosphere`. It ''should'' run the test script, which demonstrates some of its abilities. If it crashes be sure to let us know!

####Things to Note

* On Windows, always append `--useamdlibm=n` to `scons`.
* Remember that bassmidi goes in the same directory as TurboSphere's executable on Linux, unlike the other libraries.

####Using AMD LibM

Firstly, you don't need to recompile TurboSphere to use AMD's libm with it!

Secondly, this only works on Linux and Windows, and only in 64-bit builds. That's just how AMD's libM is.

Profiling TurboSphere has shown performance improvements of up to 6 percent on Linux 64-bit when using AMD's libm instead of the GNU libm. TurboSphere is designed to make it very simple to switch, even ''after it has been compiled''.

Just download AMDlibM from AMD (I can't link dirctly to the page for it...legal reasons), copy the shared library into the bin/Release folder, and start TurboSphere. You should get a message in the terminal saying that it is using amd libm. There you are, 1-5 percent performance improvement just by copying a file!

Note that it seems to be legal to redistribute the shared library with TurboSphere, but it isn't because I am not a laywer. I can't say for certain if it is legal a copy of amdlibm to be distributed with TurboSphere. The license seems to allow all this, but I will not risk it.

You probably will want to review the license for AMD's libM before distributing it with any game made for TurboSphere.

Getting Help
------------

If you get a crash, have trouble compiling TurboSphere, or find a bug, please leave a ticket with at least a somewhat detailed description on the [GitHub Page](https://github.com/flyingjester/turbosphere), or--if you are feeling more conversational--report it on the [TurboSphere thread](http://forums.spheredev.org/index.php/topic,13.msg53.html#msg53) in the Spherical Forums. Every ticket and comment on these pages is read by real human beings. Every crash and bug is taken seriously.

If you want help learning how to use TurboSphere or Sphere, or have any questions or comments about TurboSphere, feel free to comment on the [TurboSphere thread](http://forums.spheredev.org/index.php/topic,13.msg53.html#msg53) in the Spherical Forums.

TurboSphere is actively developed. Improving it and making it more stable are very important, and feedback is vital for this. You really can help make the engine better just by giving feedback.

License
-------

TurboSphere is dually licensed under the BSD 3-Clause License (aka Modified BSD License) and the Zlib license.

License Text:

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

Related Projects
----------------

TurboSphere is a reimplementation of the Sphere RPG Engine. There is another reimplementation of Sphere under development, called Sphere-sfml. It can be found on the Spherical Forums.

Sphere-sfml is written in C#. It is not feature-complete yet, but in general has more functionality than TurboSphere implements. There are cases in which each has features the other lacks. It lacks TurboSphere's plugin system, advanced audio support, and dynamic recompiling JavaScript engine. Sphere-sfml has a precompiler for JS which compiles it directly to CIL and therefore benefitting from improvements made to Microsoft .NET technology. Sphere-sfml has much better MapEngine support than TurboSphere, which is a vital component of the original Sphere engine.

Several projects have been compared to Sphere in the past. These include:

-   Love2D
-   Verge
-   IKA