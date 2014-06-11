#include "math.h"
#include <math.h>
#include <cstdio>

#define PLUGINNAME "SapphireMath"

///////////////////////////////////////////////////////////////////////////////
/*

/////
// What is in this file:

 - Replacements for Math.h functions for use with AMD's libm
 - Code to automatically select AMD's libm or the system libm on startup

*/
///////////////////////////////////////////////////////////////////////////////

#include "../../../common/dlopenwrap.h"

#ifdef _MSC_VER //Good job, Microsoft.
#include <cmath>
double round(double in){
	return floor(in+0.5);
}
#endif

    double    (*Turbo::sin)(double) = NULL;
    double    (*Turbo::cos)(double) = NULL;
    double    (*Turbo::floor)(double) = NULL;
    double    (*Turbo::ceil)(double) = NULL;
    double    (*Turbo::max)(double, double) = NULL;
    double    (*Turbo::min)(double, double) = NULL;
    double    (*Turbo::sqrt)(double) = NULL;

    template <class T>
    T tempmin(T a, T b){if(a<b) return a; else return b;}
    template <class T>
    T tempmax(T a, T b){if(a>b) return a; else return b;}

    #include <cstdint>

    template <class T>
    T tempceil(T a){

        int overflow = 0;
        int underflow = 0;

        while(a>INT64_MAX){
            a-= INT64_MAX;
            overflow++;
        }
        while(a<INT64_MIN){
            a-= INT64_MIN;
            underflow++;
        }

        int64_t foo = a;

        if(a<foo)
            foo++;
        a = foo;

        while(overflow){
            a+=INT32_MAX;
            overflow--;
        }
        while(underflow){
            a+=INT32_MIN;
            underflow--;
        }

        return a;
    }

void MathInit(void){

    #ifdef USE_AMDLIBM
    fhandle amdlibmhandle = DLOPENLIBRARY("amdlibm", "./pluginlib", DL_LAZY|DL_LOCAL);

    //void * handle = dlopen("./libamdlibm.so", RTLD_GLOBAL|RTLD_NOW);

    if(amdlibmhandle==NULL) {
    #endif

        Turbo::sin = sin;
        Turbo::cos = cos;
        Turbo::floor= floor;
        Turbo::ceil = ceil;
        Turbo::sqrt = sqrt;
        Turbo::min = tempmin<double>;
        Turbo::max = tempmax<double>;


    #ifdef USE_AMDLIBM

        printf("[" PLUGINNAME "] Info: Could not open AMD libm. Using system libm.\n");
    }
    else{
        printf("[" PLUGINNAME "] Info: Using AMD libm.\n");

        Turbo::sin = (double(*)(double))DLOPENFUNCTION(amdlibmhandle, "amd_sin");
        if (Turbo::sin == NULL)  {
            fprintf (stderr, "[" PLUGINNAME "] PrimitivesInit error: Could not load amd_sin from AMD libm.\n\tReverting to normal libm sin function.\n");
            Turbo::sin = sin;
        }
        Turbo::cos = (double(*)(double))DLOPENFUNCTION(amdlibmhandle, "amd_cos");
        if (Turbo::cos == NULL)  {
            fprintf (stderr, "[" PLUGINNAME "] PrimitivesInit error: Could not load amd_cos from AMD libm.\n\tReverting to normal libm cos function.\n");
            Turbo::cos = cos;
        }
        Turbo::floor = (double(*)(double))DLOPENFUNCTION(amdlibmhandle, "amd_floor");
        if (Turbo::floor == NULL)  {
            fprintf (stderr, "[" PLUGINNAME "] PrimitivesInit error: Could not load amd_floor from AMD libm.\n\tReverting to normal libm floor function.\n");
            Turbo::floor = floor;
        }
        Turbo::ceil = (double(*)(double))DLOPENFUNCTION(amdlibmhandle, "amd_ceil");
        if (Turbo::ceil == NULL)  {
            fprintf (stderr, "[" PLUGINNAME "] PrimitivesInit error: Could not load amd_ceil from AMD libm.\n\tReverting to normal libm ceil function.\n");
            Turbo::ceil = ceil;
        }
       Turbo:: min = (double(*)(double, double))DLOPENFUNCTION(amdlibmhandle, "amd_fmin");
        if (Turbo::min == NULL)  {
            fprintf (stderr, "[" PLUGINNAME "] PrimitivesInit error: Could not load amd_fmin from AMD libm.\n\tReverting to normal libm min function.\n");
            Turbo::min = tempmin;
        }
        Turbo::max = (double(*)(double, double))DLOPENFUNCTION(amdlibmhandle, "amd_fmax");
        if (Turbo::max == NULL)  {
            fprintf (stderr, "[" PLUGINNAME "] PrimitivesInit error: Could not load amd_fmax from AMD libm.\n\tReverting to normal libm max function.\n");
            Turbo::max = tempmax;
        }
        Turbo::sqrt = (double(*)(double))DLOPENFUNCTION(amdlibmhandle, "amd_sqrt");
        if (Turbo::sqrt == NULL)  {
            fprintf (stderr, "[" PLUGINNAME "] PrimitivesInit error: Could not load amd_sqrt from AMD libm.\n\tReverting to normal libm sqrt function.\n");
            Turbo::sqrt = sqrt;
        }
    }

    #endif
}
