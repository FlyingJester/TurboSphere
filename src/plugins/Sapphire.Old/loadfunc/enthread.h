#ifndef SAPPHIRE_ENTHREAD_HEAD
#define SAPPHIRE_ENTHREAD_HEAD

/////
// Only to be used in direct conjunction with teh surface and GL threads.
//
// These will not automagically operate the threads.

namespace Select {

    enum Threading {Single = 1, Multi};

    /////
    // Set OpenGL or Surface functions, respectively, to be multi-threaded or
    //   single-threaded.
    //
    // Returns the current mode.
    //
    // If `t' is zero, mode will not be changed. This is useful for checking
    //   which mode is currently in use.
    //

    Threading OpenGLFunctions(Threading t);
    Threading SurfaceFunctions(Threading t);

}

#endif
