#ifndef SCRIPTFS_RAWFILE_HEAD
#define SCRIPTFS_RAWFILE_HEAD
#include <cstdlib>
#include "scriptfs.h"
#include "bytearray.h"
#include <cassert>
#include "../common/plugin.h"

EXTERN_OBJECT_TEMPLATES(RawFile);

void RawfileInit(void);

#ifdef HAS_MMAP
    #include <sys/mman.h>

    typedef void * rawfiledata;

    inline void rawfiledataFree(rawfiledata dat, size_t len){
        int error = munmap(dat, len);
        assert(error==0);
    }

#else
    #ifdef USE_SDLRWOPS
        #include "../../SDL2/SDL_rwops.h"
        #include "../common/plugin.h"
        typedef SDL_RWops * rawfiledata;


        inline void rawfiledataFree(rawfiledata dat, size_t len){
            dat->close(dat);
        }

    #endif
#endif

class MINMEMALIGN TS_RawFile{
public:
    TS_RawFile();
    ~TS_RawFile();
    int Open(const char *file, bool writeable);
    int Close(void);
    size_t getSize(void);
    size_t getPosition(void);
    int setPosition(size_t pos);
    TS_ByteArray read(size_t num);
    int write(TS_ByteArray array);
    octawordP md5(void);
private:
    size_t position;
    size_t size;
    rawfiledata data;
};

md5valueP StringFromMD5(octawordP);

/*
OpenRawFile(filename [, writeable]): Open or create a file, and grab a RawFile handle to it.
RawFile.read(num_bytes): Read num_bytes from the file, and return that data as a ByteArray.
RawFile.write(byte_array): Write the data in byte_array into the file.
RawFile.close(): Close (and flush) the RawFile object.
Size
RawFile.getSize(): Get the size of the file in bytes.
File pointer position
RawFile.getPosition(): Get the current position which the data is read from.
RawFile.setPosition(position): Set the position that the file will be read from.
*/

v8Function OpenRawfile(V8ARGS);
v8Function RawfileRead(V8ARGS);
v8Function RawfileGetPosition(V8ARGS);
v8Function RawfileSetPosition(V8ARGS);
v8Function RawfileWrite(V8ARGS);
v8Function CloseRawfile(V8ARGS);
v8Function RawfileGetLength(V8ARGS);

#endif
