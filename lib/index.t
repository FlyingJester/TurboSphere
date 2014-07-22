#!/bin/cat
Directory:lib
Purpose:Code distributed as binary-only for TurboSphere

+Listing
+SConscript = Scons file to configure the library path for compilation.
+-thirdparty = Directory holding third party (not TurboSphere project or Sphere-Group)
|  binaries.
+-core = The binary files and header files for the three core libraries for TurboSphere
|  go here.
+-T5 = T5 binary files and header files go here.