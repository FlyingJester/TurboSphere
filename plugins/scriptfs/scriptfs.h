#ifndef SCRIPTFS_HEAD
#define SCRIPTFS_HEAD
#define PLUGINNAME "scriptfsT5"
#include "md5.h"
#ifdef _WIN32
#include <windows.h>
#include <WinBase.h>
#define STRDUP _strdup
#else
#include <dirent.h>
#include <cstring>
#define STRDUP strdup
#endif

#define SCRIPTFS_MAX_TEMPORARY_BUFFER_SIZE 0xFFFFF
#define SCRIPTFS_DYN_TEMPORARY_BUFFER_SIZE 0xFFFF

#define SCRIPTFS_ERROR_NONE    0x0
#define SCRIPTFS_ERROR_RNG    -0x1
#define SCRIPTFS_ERROR_FMT    -0x2
#define SCRIPTFS_ERROR_NOFILE -0x3

const char *ScriptFSDescribeError(int err);

typedef unsigned long long * octawordP;
typedef const char * md5valueP;

#include "../../graphicalg/structures.h"

#ifdef _WIN32
	#define SCRIPTFS_EXPORT __declspec(dllexport)
    #define CCALL __cdecl

typedef HANDLE filehandle;
	typedef WIN32_FIND_DATA filedata;
	#define FILENAME(NAME) NAME.cFileName
	#define ISDIRECTORY(NAME) (NAME.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)

#define STRDUP _strdup
#endif
#ifndef _WIN32
	#define CCALL
	#define SCRIPTFS_EXPORT extern "C"

	typedef DIR * filehandle;
	typedef struct dirent * filedata;
	#define FILENAME(NAME) NAME->d_name
	#define ISDIRECTORY(NAME) (NAME->d_type==DT_DIR)

    #include <sys/stat.h>
    #include <cstring>
	#define STRDUP strdup
#endif

#ifdef _WIN32
	extern "C" {
#endif

SCRIPTFS_EXPORT void    CCALL Close(void);
SCRIPTFS_EXPORT const char *  CCALL Init(void);
SCRIPTFS_EXPORT int     CCALL GetNumFunctions(void);
SCRIPTFS_EXPORT void ** CCALL GetFunctions(void);
SCRIPTFS_EXPORT const char ** CCALL GetFunctionNames(void);
SCRIPTFS_EXPORT int     CCALL GetNumVariables(void);
SCRIPTFS_EXPORT void ** CCALL GetVariables(void);
SCRIPTFS_EXPORT const char ** CCALL GetVariableNames(void);

#ifdef _WIN32
	}
#endif

#endif
