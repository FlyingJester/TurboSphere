#include "package_spk.h"
#include "t5.h"
#include <cstdio>
#include <cstring>
#include <cassert>


#ifdef _WIN32

    #include "zlib.h"

#else
    #define USEMKDIR
    #ifdef HAS_STAT
        #include <stat.h>
    #elif defined HAS_STAT_SYS
        #include <sys/stat.h>
    #else
    #endif

    #ifdef HAS_TYPES
        #include <types.h>
    #elif defined HAS_STAT_TYPES
        #include <sys/types.h>
    #else
    #endif

    #include <zlib.h>

#endif

#if ZLIB_VER_MINOR < 2 || ZLIB_VER_MAJOR != 1

    #error Compiling against wrong version of Zlib. Requires Zlib 1.2 or greater.

#endif

#ifdef USEMKDIR
    #include <errno.h>
#else
    #warning you lost USEMKDIR somewhere.
#endif

////////////////////////////////////////////////////////////////////////////////
////License
////////////////////////////////////////////////////////////////////////////////

/*
TurboSphere is dually licensed under the BSD 3-Clause License (aka Modified BSD License) and the Zlib license.
The TurboSphere SPK editor and all associated first-party Libraries (T5 and TSPK) are dually licensed under the BSD 3-Clause License (aka Modified BSD License) and the Zlib license.

Copyright (c) 2013 Martin McDonough.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

   3. The name of the author may not be used to endorse or promote products derived from this software without specific prior written permission.

Permission is granted to anyone to use this software for any purpose, including commercial applications, and to alter it and redistribute it freely, subject to the following restrictions:

   1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software. This notice may not be removed or altered from any source distribution.
*/

////////////////////////////////////////////////////////////////////////////////
////Official File Format
////////////////////////////////////////////////////////////////////////////////

/*
Sphere Package Format
Chad Austin
2000.11.13


I originally meant to use .tar.gz for packages, but seeking in zlib streams is
slow. Then I tried .zip, but there are no good reference libraries for zip
files. Finally, I resorted to making my own format. ;_; It's basically like
a zip file, but it has some simpler implementation details to make the code
easier to write (and avoid unnecessary things like encryption).


--- 16-byte header ---

byte signature[4]; // ".spk"
word version; // 1
dword num_files;
dword index_offset;
byte reserved[2];


--- directory format ---

[0..num_files]
  word entry_version; // 1
  word file_name_length; // including null-termination
  dword file_offset;
  dword file_size;
  dword compressed_size;
  byte file_name[file_name_length]; // null-terminated


--- file format ---

Each file is a zlib-compressed stream of size 'compressed_size' which will
uncompress to size 'file_size'.
*/

TS_DirectoryErrors TS_CreateDirectory(const char *path){
    if(T5_IsFile(path))
        return TS_DirectoryErrors::ISFILE;
    if(T5_IsDir(path))
        return TS_DirectoryErrors::EXISTS;

#ifdef USEMKDIR
    const int err = mkdir(path, 0777);

    if(err==-1)
        switch(errno){
        case EACCES:
        case EPERM:
        case EFAULT:
            return TS_DirectoryErrors::BADACCESS;
        case ELOOP:
            return TS_DirectoryErrors::LOOPS;
        case EMLINK:
            return TS_DirectoryErrors::TOOMANYLINKS;
        case ENAMETOOLONG:
            return TS_DirectoryErrors::NAMETOOLONG;
        case ENOENT:
            return TS_DirectoryErrors::BADINTERMEDIATE;
        case ENOMEM:
            return TS_DirectoryErrors::OOM;
        case ENOSPC:
            return TS_DirectoryErrors::OOS;
        default:
            return TS_DirectoryErrors::UNKNOWN;
        }

    return TS_DirectoryErrors::NOERROR;

#endif



}

int TS_IsSPK(const char *path){
    uint16_t version = 0;

    if(!T5_IsFile(path))
        return version;
    FILE *file = fopen(path, "rb");

    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    if(filesize<=16)
        return version;

    char signature[4];
    char sigspk[4] = {'.', 's','p', 'k'};

    fread(signature, 1, 4, file);
    if(memcmp(signature, sigspk, 4)){
        fclose(file);
        return version;
    }

    fread(&version, 1, 2, file);

    fclose(file);
    return version;

}

inline int TS_ReadSPKHeader(FILE *file, TS_SPKHeader *header){

    fread(header->signature, 1, 4, file);
    fread(&(header->version), 1, 2, file);
    fread(&(header->num_files), 1, 4, file);
    fread(&(header->index_offset), 1, 4, file);

    fseek(file, 16, SEEK_SET);

    return 16;
}

int TS_PackSPK(const char *destination, const char *source){

    return 0;
}

int TS_UnpackSPK(const char *source, const char *destination){

    //Basic sanity checks on the caller.
    assert(source);
    assert(destination);

    //We'll need this later. NOT JUST FOR SANITY CHECKS.
    size_t dest_len = strnlen(destination, 0xFFFF); //Supposedly an impossibly long length on any FS.

    //Basic sanity checks on the strings passed.
    assert(dest_len);
    assert(strnlen(source, 0xFFFF));

    //Verify that Source is sane.
    if(!T5_IsFile(source))
        return 0;
    uint16_t ver = TS_IsSPK(source);
    if(!ver)
        return 0;

    TS_SPK SPKFile;
    SPKFile.filehandle = fopen(source, "rb");
    SPKFile.header = new TS_SPKHeader();

    //Verify that we can, in fact, use destination.
    auto err = 0;

    if(!T5_IsDir(destination))
        err = TS_CreateDirectory(destination);

    if(err){
        return 1;
    }

    //Open the SPK's header.
    TS_ReadSPKHeader(SPKFile.filehandle, SPKFile.header);

    //Basic sanity check.
    assert(SPKFile.header->version==ver);

/*
[0..num_files]
2  word entry_version; // 1
2  word file_name_length; // including null-termination
4  dword file_offset;
4  dword file_size;
4  dword compressed_size;
   byte file_name[file_name_length]; // null-terminated
*/
    void *uncompressed_buffer = malloc(1024);
    void *compressed_buffer = malloc(1024);

    fseek(SPKFile.filehandle, SPKFile.header->index_offset, SEEK_SET);

    printf("[SPK] Info: Unpacking %i files.\n", SPKFile.header->num_files);

    for(unsigned int i = 0; i<SPKFile.header->num_files; i++){
        printf("[SPK] Info: Unpacking file %i.\n", i+1);
        TS_SPKFile *file = new TS_SPKFile();
        //Read the basic attributes to the file header struct.
        fread(&(file->header.version),          2, 1, SPKFile.filehandle);
        fread(&(file->header.filename_length),  2, 1, SPKFile.filehandle);
        fread(&(file->header.fileoffset),       4, 1, SPKFile.filehandle);
        fread(&(file->header.filesize),         4, 1, SPKFile.filehandle);
        fread(&(file->header.compressedsize),   4, 1, SPKFile.filehandle);

        //fseek(SPKFile.filehandle, file->header.fileoffset, SEEK_SET);

        uncompressed_buffer = realloc(uncompressed_buffer, file->header.filesize);
        compressed_buffer = realloc(compressed_buffer, file->header.compressedsize);

        file->header.filename = (char *)malloc(file->header.filename_length+strlen(destination)+2);
        fread(file->header.filename+strlen(destination)+1,   1, file->header.filename_length, SPKFile.filehandle);

        file->header.filename[strlen(destination)]='/';
        memcpy(file->header.filename, destination, strlen(destination));
        file->header.filename[file->header.filename_length+strlen(destination)]='\0';


        if(file->header.compressedsize>0xFFFF)
            printf("[SPK] Warning: File '%s' is larger than 64KB.\n", file->header.filename);
        //Determine the containing directory for the destination file..
        size_t pathlength = 0;
        if(strrchr(file->header.filename, '/')!=NULL)
            pathlength = strrchr(file->header.filename, '/')-file->header.filename;


        file->directory=NULL;

        if(pathlength!=0){
            file->directory = (char *)malloc(pathlength+1);
            file->directory[pathlength] = '\0';
            memcpy(file->directory, file->header.filename, pathlength);
        }


        printf("[SPK] Info: Path is %s.\n",file->directory );

        size_t lastPoint = ftell(SPKFile.filehandle);
        fseek(SPKFile.filehandle, file->header.fileoffset, SEEK_SET);
        //Read the compressed file;
        fread(compressed_buffer, file->header.compressedsize, 1, SPKFile.filehandle);

        //uncompress the data.
        /*
            z_const Bytef *next_in;      next input byte
            uInt     avail_in;   number of bytes available at next_in
            uLong    total_in;   total number of input bytes read so far

            Bytef    *next_out;  next output byte should be put there
            uInt     avail_out;  remaining free space at next_out
            uLong    total_out;  total number of bytes output so far

            z_const char *msg;   last error message, NULL if no error
            struct internal_state FAR *state;  not visible by applications

            alloc_func zalloc;   used to allocate the internal state
            free_func  zfree;    used to free the internal state
            voidpf     opaque;   private data object passed to zalloc and zfree

            int     data_type;   best guess about the data type: binary or text
            uLong   adler;       adler32 value of the uncompressed data
    uLong   reserved;    reserved for future use
        */
        z_stream_s stream;
        stream.zalloc = Z_NULL;
        stream.zfree  = Z_NULL;
        stream.opaque = Z_NULL;
        stream.avail_in = 0;
        stream.next_in = Z_NULL;

        int ret = inflateInit(&stream);
        size_t used = 0;

        stream.avail_in = file->header.compressedsize;
        stream.next_in = (Bytef*)compressed_buffer;

        stream.avail_out = file->header.filesize;
        stream.next_out = (Bytef*)uncompressed_buffer;

        int err = 0xFF;

        do{
            err = inflate(&stream, Z_NO_FLUSH);
            used = file->header.filesize - stream.avail_out;
        }while(err==Z_OK);

        assert(used==file->header.filesize);
        inflateEnd(&stream);

        //Write the uncompressed output.

        if((file->directory!=NULL)&&(!T5_IsDir(file->directory))){
            err = TS_CreateDirectory(file->directory);
            if((err!=TS_DirectoryErrors::EXISTS)||(err!=TS_DirectoryErrors::NOERROR))
                if(err==19)
                    err = TS_CreateDirectory(file->directory);
            if((err!=TS_DirectoryErrors::EXISTS)||(err!=TS_DirectoryErrors::NOERROR))
                printf("[SPK] Error: %i. Don't know what it means.\n", err);
        }
        printf("[SPK] Info: Creating file %s, of size %i\n", file->header.filename, file->header.compressedsize);
        FILE *outfile = fopen(file->header.filename, "wb+");
        assert(uncompressed_buffer);
        assert(file->header.filename);
        assert(outfile);
        fwrite(uncompressed_buffer, 1, file->header.filesize, outfile);
        fflush(outfile);
        fclose(outfile);
        fseek(SPKFile.filehandle, lastPoint, SEEK_SET);
    }

    free(uncompressed_buffer);
    free(compressed_buffer);

    return 0;
}
