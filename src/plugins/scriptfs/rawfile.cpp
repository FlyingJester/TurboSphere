#include "rawfile.h"
#include "../common/plugin.h"

TS_RawFile::TS_RawFile(void){
    size = 0;
    position = 0;
    data = NULL;
}

TS_RawFile::~TS_RawFile(){
    if((size!=0)||(data!=NULL)){
        rawfiledataFree(data, size);
    }
}

int TS_RawFile::Close(void){
    if(data==NULL)
        return SCRIPTFS_ERROR_NOFILE;
    rawfiledataFree(data, size);
    data = NULL;
    return SCRIPTFS_ERROR_NONE;
}

size_t TS_RawFile::getSize(){
    return size;
}

size_t TS_RawFile::getPosition(){
    return position;
}

int TS_RawFile::setPosition(size_t pos){
    if(pos>size)
        return SCRIPTFS_ERROR_RNG;
    position = pos;
    #ifdef USE_SDLRWOPS
        data->seek(data, pos, RW_SEEK_SET);
    #endif
    return SCRIPTFS_ERROR_NONE;
}

#ifdef HAS_MMAP
#warning TS_RawFile::Open is not implemented for MMAP.

#elif defined USE_SDLRWOPS

int TS_RawFile::Open(const char *file, bool writeable){
    if(file==NULL)
        return SCRIPTFS_ERROR_NOFILE;

    data = SDL_RWFromFile(file, writeable?"rb":"a+b");
    size = data->size(data);
    return SCRIPTFS_ERROR_NONE;
}

TS_ByteArray TS_RawFile::read(size_t num){
    TS_ByteArray array;
    array.length = num;
    array.data = (char *)malloc(num);
    position += SDL_RWread(data, array.data, 1,num);
    return array;
}

octawordP TS_RawFile::md5(void){
    size_t oldpos = position;
    setPosition(0);
    //char *buffer = (char *)malloc(size);
    octawordP digest = (octawordP)malloc(16);

    if(size<SCRIPTFS_MAX_TEMPORARY_BUFFER_SIZE){

        TS_ByteArray array = read(size);
        md5_buffer(array.data, size, digest);

        free(array.data);
    }
    else{
        md5_t md5_p;
        md5_init(&md5_p);

        char buffer[SCRIPTFS_DYN_TEMPORARY_BUFFER_SIZE];

        unsigned int left = 0;
        for(left=size; left<SCRIPTFS_DYN_TEMPORARY_BUFFER_SIZE; left-=SCRIPTFS_DYN_TEMPORARY_BUFFER_SIZE){
            SDL_RWread(data, buffer, 1, SCRIPTFS_DYN_TEMPORARY_BUFFER_SIZE);
            md5_process(&md5_p, buffer, SCRIPTFS_DYN_TEMPORARY_BUFFER_SIZE);
        }
        if(left>0){
            SDL_RWread(data, buffer, 1, left);
            md5_process(&md5_p, buffer, left);
        }
        md5_finish(&md5_p, digest);
    }
    setPosition(oldpos);
    return digest;
}

md5valueP StringFromMD5(octawordP hash){
    char *value = (char *)malloc(33);
    value[32]=0;
    char *hashChar = (char *)hash;
    for (int i=0;i<16;++i){
        sprintf(hashChar+(2*i), "%02x" ,value[i]);
    }
    return value;
}

int TS_RawFile::write(TS_ByteArray array){
    size_t n = SDL_RWwrite(data, array.data, 1, array.length);
    size+=n;
    return n;
}

#endif
