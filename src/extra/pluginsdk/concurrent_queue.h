#ifdef _MSC_VER

    #include <concurrent_queue.h>
    using concurrency::concurrent_queue;

#else
    #ifdef USE_INTEL_TBB
        #include <tbb/concurrent_queue.h>
        using tbb::concurrent_queue;
    #else
        #error no concurrent queue implementation found.
    #endif

#endif
