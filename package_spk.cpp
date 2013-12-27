#include "package_spk.h"
#include "t5.h"
#include <cstdio>
#include <cstring>
#include <cassert>
#include <string>

#define DEFAULT_CHUNK_SIZE 0xFF

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


int SPK_UncompressFileFromHandle(void *output, FILE *spk, TS_SPKFileHeader *header){
    assert(output);
    auto offset = ftell(spk);
    fseek(spk, header->fileoffset, SEEK_SET);
    void *in = malloc(header->compressedsize);
    fread(in, 1, header->filesize, spk);

    int err = SPK_UncompressFileFromBuffer(output, in, header);

    fseek(spk, offset, SEEK_SET);
    return err;
}

int SPK_UncompressFileFromBuffer(void *output, void *input, TS_SPKFileHeader *header){

    assert(input);
    assert(output);

    z_stream_s stream;
    stream.zalloc = Z_NULL;
    stream.zfree  = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = 0;
    stream.next_in = Z_NULL;

    inflateInit(&stream);
    size_t used = 0;

    stream.avail_in = header->compressedsize;
    stream.next_in = (Bytef*)input;

    stream.avail_out = header->filesize;
    stream.next_out = (Bytef*)output;

    int err = 0xFF;

    do{
        err = inflate(&stream, Z_NO_FLUSH);
        used = header->filesize - stream.avail_out;
    }while(err==Z_OK);

    if(err!=Z_STREAM_END)
        return err;

    assert(used==header->filesize);
    err = inflateEnd(&stream);
    return err;
}

int SPK_UncompressFileFromHandleToFile(const char *destination, FILE *spk, TS_SPKFileHeader *header){
    assert(destination);
    void *output = malloc(header->filesize);
    int err = SPK_UncompressFileFromHandle(output, spk, header);

    FILE *outfile = fopen(destination, "wb+");
    assert(outfile);
    fwrite(output, 1, header->filesize, outfile);
    return err;
}

int SPK_UncompressFileFromBufferToFile(const char *destination, void *input, TS_SPKFileHeader *header){
    assert(destination);
    void *output = malloc(header->filesize);
    int err = SPK_UncompressFileFromBuffer(output, input, header);

    FILE *outfile = fopen(destination, "wb+");
    assert(outfile);
    fwrite(output, 1, header->filesize, outfile);
    return err;
}

TS_DirectoryErrors TS_CreateDirectory(const char *path){
    if(path==NULL)
        return TS_DirectoryErrors::UNKNOWN;
    if(strnlen(path, 4)==0)
        return TS_DirectoryErrors::UNKNOWN;

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

inline int SPK_ReadSPKFileHeader(FILE *file, TS_SPKFileHeader *header){

    fread(&(header->version),          2, 1, file);
    fread(&(header->filename_length),  2, 1, file);
    fread(&(header->fileoffset),       4, 1, file);
    fread(&(header->filesize),         4, 1, file);
    fread(&(header->compressedsize),   4, 1, file);

    header->filename = (char *)malloc(header->filename_length);
    fread(header->filename, 1, header->filename_length, file);

    return 16+header->filename_length;

}

const char **SPK_GetFileList(const char* source, int *num_files){
    char ** ret = NULL;

    assert(source);
    assert(strnlen(source, 0xFFFF));

    if(!T5_IsFile(source))
        return 0;
    uint16_t ver = TS_IsSPK(source);
    if(!ver)
        return 0;

    TS_SPK SPKFile;
    SPKFile.filehandle = fopen(source, "rb");
    SPKFile.header = new TS_SPKHeader();

    //Open the SPK's header.
    TS_ReadSPKHeader(SPKFile.filehandle, SPKFile.header);

    //Basic sanity check.
    assert(SPKFile.header->version==ver);

    fseek(SPKFile.filehandle, SPKFile.header->index_offset, SEEK_SET);

    ret = (char **)malloc(sizeof(char *)*SPKFile.header->num_files);
    *num_files = SPKFile.header->num_files;
    for(unsigned int i = 0; i<SPKFile.header->num_files; i++){
        TS_SPKFile *file = new TS_SPKFile();
        //Read the basic attributes to the file header struct.

        fseek(SPKFile.filehandle, 2, SEEK_CUR);
        fread(&(file->header.filename_length),  2, 1, SPKFile.filehandle);
        fseek(SPKFile.filehandle, 12, SEEK_CUR);

        file->header.filename = (char *)malloc(file->header.filename_length);
        fread(file->header.filename,   1, file->header.filename_length, SPKFile.filehandle);

        ret[i] = file->header.filename;
        printf("[SPK] Info: Path is %s.\n",file->directory );
        delete file;
    }
    delete SPKFile.header;
    return (const char **)ret;
}

struct Directory{
    char *name;
    size_t num_up;
    Directory **up;
    Directory *down;
};

size_t SPK_WriteHeader(FILE *file, size_t num_files){
    uint8_t     signature[4] = {'.', 's', 'p', 'k'};
    uint16_t    version[1] = {1};        //Must equal 1 for SPK
    uint32_t    numfiles[1] = {(uint32_t)num_files};
    uint32_t    index_offset[1] = {32};
    uint8_t     reserved[2]     = {0, 0};

    size_t offset = 0;

    offset += fwrite(signature,   1, 4, file);
    offset += fwrite(version,     1, 2, file);
    offset += fwrite(numfiles,   2, 2, file);
    offset += fwrite(index_offset,2, 2, file);
    offset += fwrite(reserved,    1, 2, file);

    return offset;

}

void SPK_WriteFileHeader(FILE *file, TS_SPKFileHeader &header){
    fwrite(&(header.version), 1, 2, file);
    fwrite(&(header.filename_length), 1, 2, file);
    fwrite(&(header.fileoffset), 2, 2, file);
    fwrite(&(header.filesize), 2, 2, file);
    fwrite(&(header.compressedsize), 2, 2, file);
    fwrite(&(header.filename), 1, header.filename_length, file);
}

void SPK_In_AddDirs(Directory *directory, std::vector<Directory *> &dirs, std::vector<TS_SPKFile *> &files){
    const char **dirlist = (const char **)alloca(sizeof(char *)*directory->num_up);
    dirlist = T5_GetDirectoryList(directory->name, dirlist);
    for(size_t i = 0; i<directory->num_up; i++){
        //Add files.
        size_t num_files = T5_GetNumFilesInDirectory(directory->name);
        const char **filelist = (const char **)alloca(sizeof(char *)*num_files);
        filelist = T5_GetFileList(directory->name, filelist);
        for(size_t i = 0; i<num_files; i++){
            TS_SPKFile *file = new TS_SPKFile();
            file->header = TS_SPKFileHeader();
                file->header.version = 1u;
                file->header.filename_length = strnlen(filelist[i], 0xFFFF)+1;
                file->header.filesize = T5_GetFileSize(filelist[i]);
                file->header.filename = (char *)filelist[i];
            file->directory = directory->name;
            file->filehandle = NULL;
            file->uncompressed_size = file->header.filesize;
        }

        //Add and trace child directories.
        size_t num = dirs.size();
        dirs.push_back(new Directory());
        dirs[num]->down = directory;
        dirs[num]->name = (char *)dirlist[i];
        directory->up[i] = dirs[num];
        dirs[num]->num_up = T5_GetNumDirectoriesInDirectory(dirs[num]->name);
        dirs[num]->up = (Directory **)malloc(sizeof(Directory *)*dirs[num]->num_up);
        SPK_In_AddDirs(dirs[num], dirs, files);
    }
    //free((void *)dirlist);
}

int TS_PackSPK(const char *destination, const char *source){

    if(!T5_IsDir(source)){
        return 5;
    }

    TS_SPK SPK;
    SPK.SPKfiles = std::vector<TS_SPKFile *>();

    std::vector<Directory *> dirs = std::vector<Directory *>();
    /*
    TS_SPKFileHeader header;
    FILE *filehandle;
    char *directory;
    size_t compressed_size;
    size_t uncompressed_size;
    void *compressed_contents;
    void *uncompressed_contents;
    */

    dirs.push_back(new Directory());

    dirs[0]->name = (char *)malloc(1);
    dirs[0]->name[0] = '\0';
    dirs[0]->down = NULL;
    dirs[0]->num_up = T5_GetNumDirectoriesInDirectory(source);
    dirs[0]->up = (Directory **)malloc(sizeof(Directory *)*dirs[0]->num_up);

    SPK_In_AddDirs(dirs[0], dirs, SPK.SPKfiles);

    SPK.header = new TS_SPKHeader();
    SPK.filehandle = fopen(destination, "wb+");
    SPK.header->num_files = SPK.SPKfiles.size();
    SPK_WriteHeader(SPK.filehandle, SPK.header->num_files);

    for(size_t i = 0; i<SPK.header->num_files; i++)
    {
        SPK.SPKfiles[i]->uncompressed_contents = (void *)T5_LoadFileAsText((std::string(source).append("/")+std::string(SPK.SPKfiles[i]->header.filename)).c_str());
        SPK.SPKfiles[i]->compressed_contents = malloc(SPK.SPKfiles[i]->header.filesize+2);


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

        int err = deflateInit(&stream, Z_BEST_COMPRESSION);

        size_t used = 0;

        stream.avail_in = SPK.SPKfiles[i]->header.filesize;
        stream.next_in = (Bytef*)(SPK.SPKfiles[i]->uncompressed_contents);

        stream.avail_out = SPK.SPKfiles[i]->header.filesize+2;
        stream.next_out = (Bytef*)(SPK.SPKfiles[i]->compressed_contents);

        do{
            err = deflate(&stream, Z_NO_FLUSH);
            used = SPK.SPKfiles[i]->header.filesize - stream.avail_out;
        }while(err==Z_OK);

        SPK.SPKfiles[i]->header.compressedsize = used;
        deflateEnd(&stream);

    }

    fseek(SPK.filehandle, 32, SEEK_SET);

    uint32_t zero = 0x00000000;

    void *data = NULL;
    size_t offset = 0;
    size_t length = 32;
    //Concatenate all headers.
    for(size_t i = 0; i<SPK.header->num_files; i++){

        size_t headersize = 16+SPK.SPKfiles[i]->header.filename_length;
    }

    return 0;

}

//`ordered` should be set to 1 on I/O bound systems. It should be set to 2 on CPU-bound systems.
//A value of 0 will let the function decide, based on the number of files involved.
int SPK_OpenSPK(TS_SPK &SPK, const char *source, char ordered){

    //Basic sanity checks on the caller.
    assert(source);
    assert(strnlen(source, 0xFFFF));

    if(!T5_IsFile(source))
        return 0;
    uint16_t ver = TS_IsSPK(source);
    if(!ver)
        return 0;

    SPK.filehandle = fopen(source, "rb");
    SPK.header = new TS_SPKHeader();
    //std::vector<TS_SPKFile *> SPKfiles;
    SPK.SPKfiles = std::vector<TS_SPKFile *>();

    TS_ReadSPKHeader(SPK.filehandle, SPK.header);
    fseek(SPK.filehandle, SPK.header->index_offset, SEEK_SET);

    //To determine the first compressed file in the archive.
    //Try to do as much file I/O sequentially as possible.
    size_t first[2] = {(size_t)ftell(SPK.filehandle), 0u};

    //Read the compressed file headers in the archive.
    for(size_t i = 0; i<SPK.header->num_files; i++){
        SPK.SPKfiles.push_back(new TS_SPKFile());
        SPK_ReadSPKFileHeader(SPK.filehandle, &(SPK.SPKfiles[i]->header));
        if(SPK.SPKfiles[i]->header.fileoffset<first[0]){
            first[0] = SPK.SPKfiles[i]->header.fileoffset;
            first[1] = i;
        }
    }

    //Dispatch
    if(ordered==1)
        goto orderedStart;

    if(ordered==2)
        goto unorderedStart;

    if(SPK.header->num_files>1000)
        goto unorderedStart;

    if(SPK.header->num_files==0)
        goto ending;


orderedStart:
        {
        size_t loc = first[0];
        fseek(SPK.filehandle, first[0], SEEK_SET);
        //Record which files we have already read.
        char *found = (char *)alloca(SPK.header->num_files);
        memset(found, 0, SPK.header->num_files);
        size_t i = 0; //The number of files read.

        do{
            if(!found[first[1]]){
                SPK.SPKfiles[first[1]]->compressed_contents = malloc(SPK.SPKfiles[first[1]]->header.compressedsize);
                loc+=fread(SPK.SPKfiles[first[1]]->compressed_contents, 1, SPK.SPKfiles[first[1]]->header.compressedsize, SPK.filehandle);

                found[first[1]] = 1;
            }

            for(size_t e = 0; e<SPK.header->num_files; e++){
                if(e==i)
                    continue;
                if(SPK.SPKfiles[e]->header.fileoffset==loc){
                    first[0] = loc;
                    first[1] = e;
                    break;
                }
            }

            if(first[0]==loc){
                if(!found[first[1]])
                    continue;

                for(size_t e = 0; e<SPK.header->num_files; e++){
                    if(found[e])
                        continue;

                    first[0] = SPK.SPKfiles[e]->header.fileoffset;
                    fseek(SPK.filehandle, first[0], SEEK_SET);
                    loc = first[0];
                    first[1] = e;

                }

            }
        }while(i<SPK.header->num_files);
    }
    goto ending;

unorderedStart:

    for(size_t i = 0; i<SPK.header->num_files; i++){
        fseek(SPK.filehandle, SPK.SPKfiles[i]->header.fileoffset, SEEK_SET);
        SPK.SPKfiles[i]->compressed_contents = malloc(SPK.SPKfiles[i]->header.compressedsize);
        fread(SPK.SPKfiles[i]->compressed_contents, 1, SPK.SPKfiles[i]->header.compressedsize, SPK.filehandle);
    }


ending:
    fclose(SPK.filehandle);
    SPK.filehandle = NULL;


    return 0;

}

int TS_UnpackSPK(TS_SPK &SPK, const char *destination){

    auto err = 0;

    assert(destination);

    //We'll need this later. NOT JUST FOR SANITY CHECKS.
    size_t dest_len = strnlen(destination, 0xFFFF); //Supposedly an impossibly long length on any FS.
    assert(dest_len);


    char ** dirs = (char **)alloca(sizeof(char *)*SPK.header->num_files);
    memset(dirs, 0, sizeof(char *)*SPK.header->num_files);


    //Create the directories.
    for(size_t i = 0; i<SPK.header->num_files; i++){
        size_t len = 0;
        if(strrchr(SPK.SPKfiles[i]->header.filename, (int)'/')!=0)
            len = strrchr(SPK.SPKfiles[i]->header.filename, (int)'/')-SPK.SPKfiles[i]->header.filename;

        if(len){

            dirs[i] = (char *)malloc(len+2+dest_len);
            assert(dirs[i]);
            memcpy(dirs[i], destination, dest_len);
            dirs[i][dest_len]='/';
            memcpy(dirs[i]+dest_len+1, SPK.SPKfiles[i]->header.filename, len);
            dirs[i][len+dest_len+1] = '\0';

            bool found = false;
            for(size_t e = 0; e<SPK.header->num_files; e++){
                if(dirs[e] == NULL)
                    continue;

                if(i==e)
                    continue;

                if(!strcmp(dirs[i], dirs[e])){
                    found = true;
                    break;
                }

            }
            if(!found){
                err = TS_CreateDirectory(dirs[i]);
                if((err!=TS_DirectoryErrors::EXISTS)||(err!=TS_DirectoryErrors::NOERROR))
                    if(err==19)
                        err = TS_CreateDirectory(dirs[i]);
                if((err!=TS_DirectoryErrors::EXISTS)||(err!=TS_DirectoryErrors::NOERROR))
                    printf("[SPK] Error: %i. Don't know what it means.\n", err);
            }
        }

    }

    //Parallel this:
    for(size_t i = 0; i<SPK.header->num_files; i++){
        free(dirs[i]);
        char *dest_file_name = (char *)alloca(dest_len+1+SPK.SPKfiles[i]->header.filename_length);
        memcpy(dest_file_name, destination, dest_len);
        dest_file_name[dest_len] = '/';
        memcpy(dest_file_name+dest_len+1, SPK.SPKfiles[i]->header.filename, SPK.SPKfiles[i]->header.filename_length);
        FILE *dest = fopen(dest_file_name, "wb+");

        SPK.SPKfiles[i]->uncompressed_contents = malloc(SPK.SPKfiles[i]->header.filesize);

        z_stream_s stream;
        stream.zalloc = Z_NULL;
        stream.zfree  = Z_NULL;
        stream.opaque = Z_NULL;
        stream.avail_in = 0;
        stream.next_in = Z_NULL;

        inflateInit(&stream);
        size_t used = 0;

        stream.avail_in = SPK.SPKfiles[i]->header.compressedsize;
        stream.next_in = (Bytef*)(SPK.SPKfiles[i]->compressed_contents);

        stream.avail_out = SPK.SPKfiles[i]->header.filesize;
        stream.next_out = (Bytef*)(SPK.SPKfiles[i]->uncompressed_contents);

        int err = 0xFF;

        do{
            err = inflate(&stream, Z_NO_FLUSH);
            used = SPK.SPKfiles[i]->header.filesize - stream.avail_out;
        }while(err==Z_OK);

        assert(used==SPK.SPKfiles[i]->header.filesize);
        inflateEnd(&stream);

        fwrite(SPK.SPKfiles[i]->uncompressed_contents, 1, SPK.SPKfiles[i]->header.filesize, dest);
        fflush(dest);
        free(SPK.SPKfiles[i]->uncompressed_contents);
        fclose(dest);

    }

    return 0;

}

int SPK_FreeSPK(TS_SPK &SPK){

    for(size_t i = 0; i<SPK.header->num_files; i++){
        free(SPK.SPKfiles[i]->compressed_contents);
        free(SPK.SPKfiles[i]->header.filename);
        delete SPK.SPKfiles[i];
    }
    delete SPK.header;
    return 0;
}

int TS_UnpackSPK1(const char *source, const char *destination){

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

        inflateInit(&stream);
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
        delete file;
    }

    free(uncompressed_buffer);
    free(compressed_buffer);
    return 0;
}
