#ifndef PACKAGE_SPK_HEAD
#define PACKAGE_SPK_HEAD
#include <cstdint>
#include <cstdio>
#include <vector>

enum TS_DirectoryErrors {NOERROR = 0, BADACCESS = 0xF, EXISTS, LOOPS, NAMETOOLONG, BADINTERMEDIATE, ISFILE, OOM, OOS, TOOMANYLINKS, UNKNOWN = 0xFF};

TS_DirectoryErrors TS_CreateDirectory(const char *path);

struct TS_SPKFileHeader;

struct TS_SPKHeader {
    char        signature[4];   //Should equal ".spk" for SPK
    uint16_t    version;        //Must equal 1 for SPK
    uint32_t    num_files;
    uint32_t    index_offset;
    char        reserved[2];    //If this isn't zero'd-out, Sphere 1.5 won't load the SPK. TurboSphere doesn't care.
};

struct TS_SPKFileHeader{
    uint16_t version;
    uint16_t filename_length;   //This includes the null terminator
    uint32_t fileoffset;
    uint32_t filesize;
    uint32_t compressedsize;
    char *filename;             //Null-terminated.
};

struct TS_SPKFile{
    TS_SPKFileHeader header;
    FILE *filehandle;
    char *directory;
    void *uncompressed_contents;
};

struct TS_SPK{
    TS_SPKHeader *header;
    FILE *filehandle;
    std::vector<TS_SPKFile *> SPKfiles;
};

int TS_IsSPK(const char *file);

int TS_PackSPK(const char *destination, const char *source);

int TS_UnpackSPK(const char *source, const char *destination);

#endif
