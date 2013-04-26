#ifndef T5_HEAD
#define T5_HEAD

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

#ifdef _WIN32
	#ifdef T5_INTERNAL
		#define T5_EXPORT __declspec(dllexport)
	#else
		#define T5_EXPORT __declspec(dllimport)
	#endif
    #define T5_CLASS_EXPORT class T5_EXPORT

#define T5CALL _cdecl
#else
	#define T5_EXPORT extern "C"

    #define T5CALL //

	#define T5_CLASS_EXPORT extern "C" class

#endif


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
#ifdef T5_INTERNAL
private:
	std::fstream stream;
	std::vector<INIvalue*> values;
#endif
};

T5_EXPORT void T5CALL T5_AddDir(const char *dir);
T5_EXPORT T5_file* T5CALL T5_OpenFile(const char*);
T5_EXPORT void T5CALL T5_init(int ndirs, ...);
T5_EXPORT void T5CALL T5_close(T5_file*);

#endif
