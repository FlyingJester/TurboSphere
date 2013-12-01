#ifndef T5_HEAD
#define T5_HEAD

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

#ifdef _MSC_VER
	#ifdef T5_INTERNAL
		#define T5_EXPORT __declspec(dllexport)
	#else
		#define T5_EXPORT __declspec(dllimport)
	#endif
    #define T5_CLASS_EXPORT class T5_EXPORT

#define T5CALL _cdecl
#else
	#define T5_EXPORT extern "C"

    #define T5CALL

	#define T5_CLASS_EXPORT extern "C" class

#endif



typedef const char * T5_FileText;

class INIvalue{
public:
    INIvalue(const char*,const char*,const char*);
    ~INIvalue();
    const char *key;
    const char *val;
    const char *sec;
};

T5_CLASS_EXPORT T5_file{
public:
	T5_file(const char *);
	T5_file();
	~T5_file();
    const char* getValueFromSection(const char*, const char*);
    const char* getValue(const char*);
    void writeValue(const char *, const char *);
    void writeValueToSection(const char *, const char*, const char*);
    void flush(void);
private:
	std::fstream stream;
	std::vector<INIvalue*> values;
};


T5_EXPORT void T5CALL T5_FreeFileName(const char *name);
T5_EXPORT void T5CALL T5_FreeFileNames(const char **names, size_t num);

T5_EXPORT const char ** T5CALL T5_GetFileList(const char *directory, const char **filenames);
T5_EXPORT const char ** T5CALL T5_GetDirectoryList(const char *directory, const char **filenames);
T5_EXPORT int  T5CALL T5_GetNumFilesInDirectory(const char *directory);
T5_EXPORT int  T5CALL T5_GetNumDirectoriesInDirectory(const char *directory);
T5_EXPORT bool T5CALL T5_IsFile(const char *path);
T5_EXPORT bool T5CALL T5_IsDir(const char *path);

T5_EXPORT T5_FileText T5CALL T5_LoadFileAsText(const char *file);
T5_EXPORT void T5CALL T5_FreeFileText(T5_FileText text);

T5_EXPORT void T5CALL T5_AddDir(const char *dir);
T5_EXPORT T5_file* T5CALL T5_OpenFile(const char*);
T5_EXPORT void T5CALL T5_init(int ndirs, ...);
T5_EXPORT void T5CALL T5_close(T5_file*);

#endif
