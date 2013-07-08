#ifndef SCRIPTFS_HEAD
#define SCRIPTFS_HEAD

#ifdef _WIN32
#include <windows.h>
#include <WinBase.h>
#define STRDUP _strdup
#else
#include <dirent.h>
#include <cstring>
#define STRDUP strdup
#endif

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

typedef TS_ByteArray RawData;

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
