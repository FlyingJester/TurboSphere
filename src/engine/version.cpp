#include "version.h"
#include "v8.h"
#include <cassert>
#include <cstring>
#include <string>

/////
// TurboSphere version information
//
// Formatted as no more than 8 lines of information.
//
// The information is as follows (line numbers precede information)
//
// 1. [program name] - [short description]
// 2. [program name] [full version number]
// 3. V8 Version [V8 version number], JavaScript Version [JS version]
// 4. API Versions: [comma+space delimited list of supported API versions]
// 5. Built on [compilation date]. Compiled using [compiler].
//
//

using std::string;

static const char * short_version_string = "0.4.1b";
static const char * compiler_string =
#ifdef _MSC_VER
    "Microsoft Visual C++"
#elif defined __GNUC__
    "GCC"
#else
    "Clang/LLVM"
#endif
;

const char *TS_GetVersion(void){
    return short_version_string;
}

const char *TS_GetLongVersion(void){
    static bool first = false;
    static string ver = "TurboSphere - A Plugin-Based, JavaScript Game Engine.\n";
    static const char *version_string = NULL;
    if(!first){
        ver += "TurboSphere Version ";
        ver += short_version_string;
        ver += "\n";

        ver += "V8 Version ";
        ver += v8::V8::GetVersion();
        ver += ", JavaScript Version ES6/Harmony\n";

        ver += "Sphere API Versions: 2.0tr1 (TS)\n";

        ver += "Built on ";
        ver += __DATE__;
        ver += ". Compiled using ";
        ver += compiler_string;
        ver += " on " __DATE__ " \n";

        version_string = ver.c_str();

        first = true;
    }

    assert(version_string);

    return version_string;
}
