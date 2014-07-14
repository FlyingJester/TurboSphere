#pragma once
#ifdef _MSC_VER

    #include <concurrent_queue.h>
    using concurrency::concurrent_queue;

#else
    #ifdef USE_INTEL_TBB
        #include <tbb/concurrent_queue.h>
        using tbb::concurrent_queue;
    #else

        /////
        // If USE_PIPE_CONCURRENT_QUEUE is defined, or we are on the Mach kernel, use Unix pipes for concurrent queues.
        // OS X has bad mutex performance. Use a pipe instead, unless specifically told otherwise (w/ USE_MUTEX_QUEUE).
        #if (defined(USE_PIPE_CONCURRENT_QUEUE)) || ((defined(__MACH__)) && (!defined(USE_MUTEX_QUEUE)))
            #include <unistd.h>
            #include <fcntl.h>
            #include <cstdlib>

            template<typename T>
            class concurrent_queue {

                int FD[2];

                public:
                concurrent_queue(){
                    pipe(FD);

                    // Ensure that we can perform non-blocking reads.
                    fcntl(FD[0], F_SETFL, O_NONBLOCK);

                }

                ~concurrent_queue(){
                    close(FD[0]);
                    close(FD[1]);
                }

                inline void push(const T &aP){
                    write(FD[1], &aP, sizeof(T));
                }

                inline bool try_pop(T &aTo){

                    return (read(FD[0], &aTo, sizeof(T))>0);

                }

                inline bool pop_if_present(T &aTo){
                    return try_pop(aTo);
                }

                inline size_t unsafe_size(){
                    uint64_t lAt = lseek(FD[0], 0, SEEK_CUR);
                    uint64_t rAt = lseek(FD[0], 0, SEEK_END);
                    lseek(FD[0], lAt, SEEK_SET);

                    return rAt;
                }

            };
    #else

        #include <queue>

        #if (defined(USE_MUTEX)) && (defined(USE_PTHREAD))
            #warning Both USE_MUTEX and USE_PTHREAD were defined. We will try to use pthreads.
            #warning Fix the sconscript for this plugin on this platform!
        #endif

        #if (!defined(USE_MUTEX)) &&(!defined(USE_PTHREAD))
            #warning Not compiling with MSVC, and USE_INTEL_TBB, USE_PTHREAD, and USE_MUTEX are all undefined.
            #warning If HAS_MUTEX is defined, we will define USE_MUTEX. Otherwise, we will use USE_PTHREAD.
            #warning Fix the SConscript for this plugin on this platform!

            #ifdef HAS_MUTEX
                #define USE_MUTEX
            #else
                #define USE_PTHREAD
            #endif

        #endif

        #ifdef USE_PTHREAD
            #include <mutex>
        #elif defined(USE_MUTEX)
            #include <pthread.h>
        #endif

        template<typename T>
        class concurrent_queue {

            std::queue<T> aQueue;


            #ifdef USE_PTHREAD
                typedef std::mutex mutex;
            #elif defined(USE_MUTEX)
                typedef pthread_mutex_t mutex;
            #endif

            mutable mutex aMutex;

            inline void lock(void) const{
                #ifdef USE_PTHREAD
                    aMutex.lock();
                #elif defined(USE_MUTEX)
                    pthread_mutex_lock(&aMutex);
                #endif
            }

            inline bool trylock(void) const{
                #ifdef USE_PTHREAD
                    return aMutex.try_lock();
                #elif defined(USE_MUTEX)
                    return (pthread_mutex_trylock(&aMutex)==0);
                #endif
            }

            inline void unlock(void) const{
                #ifdef USE_PTHREAD
                    aMutex.unlock();
                #elif defined(USE_MUTEX)
                    pthread_mutex_unlock(&aMutex);
                #endif
            }


            public:
            concurrent_queue(){

                aQueue = std::queue<T> ();
                #ifdef USE_PTHREAD
                pthread_mutex_init(&aMutex, NULL);
                #endif

            }

            ~concurrent_queue(){

                #ifdef USE_PTHREAD
                pthread_mutex_destroy(&aMutex);
                #endif

            }

            inline void push(const T &aP){
                lock();
                aQueue.push(aP);
                unlock();
            }

            inline void push_1000(const T aIn[1000]){
                lock();
                for(int i = 0; i<10; i++)
                    aQueue.push(aIn[i]);
                unlock();
            }

            inline bool try_pop(T &aTo){
                bool lHasALock = trylock();

                if(!lHasALock)
                    return false;

                if(aQueue.empty()){
                    unlock();
                    return false;
                }

                aTo = aQueue.front();
                aQueue.pop();

                unlock();
                return true;
            }

            inline bool pop_if_present(T &aTo){
                return try_pop(aTo);
            }

            inline size_t unsafe_size(){
                lock();
                size_t size = aQueue.size();
                unlock();
                return size;

            }

        };


        #endif
    #endif

#endif
