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
    size_t compressed_size;
    size_t uncompressed_size;
    void *compressed_contents;
    void *uncompressed_contents;
};

struct TS_SPK{
    TS_SPKHeader *header;
    FILE *filehandle;
    std::vector<TS_SPKFile *> SPKfiles;
};

int SPK_UncompressFileFromHandle(void *output, FILE *spk, TS_SPKFileHeader *header);
int SPK_UncompressFileFromBuffer(void *output, void *input, TS_SPKFileHeader *header);
int SPK_UncompressFileFromHandleToFile(const char *destination, FILE *spk, TS_SPKFileHeader *header);
int SPK_UncompressFileFromBufferToFile(const char *destination, void *input, TS_SPKFileHeader *header);

const char **SPK_GetFileList(const char* source, int *num_files);
int TS_IsSPK(const char *file);

int TS_PackSPK(const char *destination, const char *source);

//Multithreaded. We must be cautious.
int SPK_OpenSPK(TS_SPK &SPK, const char *source, char ordered = 2);
int TS_UnpackSPK(TS_SPK &SPK, const char *destination);
int SPK_FreeSPK(TS_SPK &SPK);

//Monolithic, single-threaded version.
int TS_UnpackSPK1(const char *source, const char *destination);

#endif
