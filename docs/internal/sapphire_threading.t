Overview of the threading model in Sapphire:

LifeTime:
    At Initialization:

    When the Sapphire plugin is initialized, it creates an OpenGL context that
    is used to share all other contexts. This context does nothing but stay
    resident until the plugin exits.

    A second context is created, which will be used exclusively for loading
    resources and generating GL names.

    A threading mode is then chosen, either single- or multi-threaded. In both
    cases, a third context is created. This context performs all drawing, and
    stays resident in the thread it was created in. When the threading mode
    is changed, this context is destroyed and replaced with a new context.

    In the case of multi-threaded mode, the a thread is created prior to the
    drawing context being created. The drawing context will then be created and
    used from this thread.


Function Calls:

    When in single-threaded mode, Sapphire calls the FJ-GL functions to perform
    drawing and GL management.

    In Multi-Threaded mode, function pointers to the thread-safe version of the
    FJ-GL function, along with a pointer to a a struct that contains the args
    to this function (in a funciton-specific manner). All thread-safe functions
    will be defined to have this type:

        void function(void *arg);

Data Formatting and Lifetime:

    All arguments for threaded OpenGL functions will be stored in singular
    structs, with the needed data in a function-specific format in this
    struct. The struct could be anything that can be referred to by address,
    but only this address will be passed to the function.

    All callback information should be passed using atomics. These atomics will
    be referred to in the arg structs by address, but they will NOT be
    freed with the arg struct. Any resource external to the arg struct will be
    freed by function that queued the threaded function. The standard way of
    doing this is using an atomic to signal when the extra resources have been
    prepared by the threaded function.

    For example, with GetClipRect:

Time

 ||
_||_
\  /
 \/

    Thread 1:           |   Thread 2:           |
    ====================+=======================|
    Allocate arg-struct | ...                   |
    for GetClipRect.    |                       |
    ====================+=======================|
    Allocate memory for | ...                   |
    the cliprect struct.|                       |
    ====================+=======================|
    Create stack'd      | ...                   |
    atomic for signal.  |                       |
    ====================+=======================|
    Poll the atomic.    | Process queue until   |
                        | the GetClipRect.      |
    ====================+=======================|
    Poll the atomic.    | Populate the cliprect |
                        | struct with data.     |
    ====================+=======================|
    Poll the atomic-true| Set the atomic.       |
    ====================+=======================|
    Read the data out   | Free the arg-struct   |
    of the cliprect.    | memory.               |
    ====================+=======================|
    Free cliprect, free | ...                   |
    the atomic.         |                       |
    ====================+=======================|

Function Names:

    Set* is an alias to Immediate* or Threaded*, and may have a return value,
    depending on the function;
