#define T5_INTERNAL
#include "t5.h"

#ifdef PREFER_STAT

    #ifdef HAS_STAT
        #include <stat.h>
    #elif defined HAS_STAT_SYS
        #include <sys/stat.h>
    #elif defined HAS_UNISTD
        #include <unistd.h>
    #elif defined HAS_UNISTD_SYS
        #include <sys/unistd.h>
    #endif
#else //PREFER_STAT

    #ifdef HAS_UNISTD
        #include <unistd.h>
    #elif defined HAS_UNISTD_SYS
        #include <sys/unistd.h>
    #elif defined HAS_STAT
        #include <stat.h>
    #elif defined HAS_STAT_SYS
        #include <sys/stat.h>
    #endif
#endif //else to PREFER_STAT


#include <utility>
#include <streambuf>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <cctype>

#ifdef _WIN32
    #define STRDUP _strdup
#else
    #define STRDUP strdup
    #include <errno.h> //
#endif

#ifdef _WIN32
    #include <windows.h>

typedef HANDLE filehandle;
	typedef WIN32_FIND_DATA filedata;
	#define FILENAME(NAME) NAME.cFileName
	#define ISDIRECTORY(NAME) (NAME.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)

#else

    #include <dirent.h>

	typedef DIR * filehandle;
	typedef struct dirent * filedata;
	#define FILENAME(NAME) NAME->d_name
	#define ISDIRECTORY(NAME) (NAME->d_type==DT_DIR)

    #include <cstring>
#endif

static std::vector<const char*> T5_Directories(0);

#ifdef _WIN32

//I would use PathFileExists from Shlwapi, but that introduces another dependency.

#define DOESNTEXIST(attrs) attrs==INVALID_FILE_ATTRIBUTES

bool T5CALL T5_IsFile(const char *path){
	DWORD attrs =  GetFileAttributesA(path);
	if(DOESNTEXIST(attrs)){
		return false;
	}
	return (attrs&FILE_ATTRIBUTE_DIRECTORY)?false:true;
}

bool T5CALL T5_IsDir(const char *path){
	DWORD attrs =  GetFileAttributesA(path);
	if(DOESNTEXIST(attrs)){
		return false;
	}
	return (attrs&FILE_ATTRIBUTE_DIRECTORY)?true:false;
}

#elif defined USE_UNISTD && ((!defined PREFER_STAT)||((!defined HAS_STAT)&&!defined HAS_STAT_SYS))

//TODO: Fix these shambles.
bool T5CALL T5_IsFile(const char *path){

    int info = access(path, F_OK);
    if (info==0)
        return true;
    else
        return false;
}

#elif defined USE_STAT

bool T5CALL T5_IsFile(const char *path){
    struct stat buf;
    stat(path, &buf);
    return S_ISREG(buf.st_mode);
}

bool T5CALL T5_IsDir(const char *path){
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}

#endif

int T5CALL T5_GetNumDirectoriesInDirectory(const char *directory){
    if(!T5_IsDir(directory))
        return 0;

	filehandle dir;
    filedata data;

	int i = 0;
#ifdef _WIN32
	const char *fulldirlist = STRDUP((std::string(directory)+"*.*").c_str());
	DWORD attribs = ::GetFileAttributesA(directory);
	if ((attribs != INVALID_FILE_ATTRIBUTES) && (attribs & FILE_ATTRIBUTE_DIRECTORY)) {
		dir = FindFirstFile(directory, &data);

		if (dir!=INVALID_HANDLE_VALUE){
			do{
#else
	    if ((dir=opendir(directory))!=NULL){
	        while((data=readdir(dir))!=NULL){
#endif
				if((ISDIRECTORY(data))&&(FILENAME(data)[0]!='.')){
					i++;
				}
#ifdef _WIN32
		    } while(FindNextFile(dir, &data));
		    FindClose(dir);
		}//dir!=INVALID_HANDLE_VALUE
	}//attribs!=INVALID_FILE_ATTRIBUTES
#else
        }
        closedir(dir);
	}
#endif

    return i;
}

int T5CALL T5_GetNumFilesInDirectory(const char *directory){
    if(!T5_IsDir(directory))
        return 0;

	filehandle dir;
    filedata data;

	int i = 0;
#ifdef _WIN32
	const char *fulldirlist = STRDUP((std::string(directory)+"*.*").c_str());
	DWORD attribs = ::GetFileAttributesA(directory);
	if ((attribs != INVALID_FILE_ATTRIBUTES) && (attribs & FILE_ATTRIBUTE_DIRECTORY)) {
		dir = FindFirstFile(directory, &data);

		if (dir!=INVALID_HANDLE_VALUE){
			do{
#else
	    if ((dir=opendir(directory))!=NULL){
	        while((data=readdir(dir))!=NULL){
#endif
				if((!ISDIRECTORY(data))&&(FILENAME(data)[0]!='.')){
					i++;
				}
#ifdef _WIN32
		    } while(FindNextFile(dir, &data));
		    FindClose(dir);
		}//dir!=INVALID_HANDLE_VALUE
	}//attribs!=INVALID_FILE_ATTRIBUTES
#else
        }
        closedir(dir);
	}
#endif

    return i;
}

const char ** T5CALL T5_GetFileList(const char *directory, const char **filenames){

    if(!T5_IsDir(directory))
        return NULL;

    if(filenames==NULL){
        filenames = (const char **)calloc(T5_GetNumFilesInDirectory(directory), sizeof(const char *));
    }

	filehandle dir;
    filedata data;

	int i = 0;
	#ifdef _WIN32

	const char *fulldirlist = STRDUP((std::string(directory)+"*.*").c_str());
	DWORD attribs = ::GetFileAttributesA(directory);
	if ((attribs != INVALID_FILE_ATTRIBUTES) && (attribs & FILE_ATTRIBUTE_DIRECTORY)) {
		dir = FindFirstFile(directory, &data);

		if (dir!=INVALID_HANDLE_VALUE){
			do{
#else
	    if ((dir=opendir(directory))!=NULL){
	        while((data=readdir(dir))!=NULL){
#endif
				if((!ISDIRECTORY(data))&&(FILENAME(data)[0]!='.')){
					filenames[i] = STRDUP(FILENAME(data));
					i++;
				}
#ifdef _WIN32
		    } while(FindNextFile(dir, &data));
		    FindClose(dir);
		}//dir!=INVALID_HANDLE_VALUE
	}//attribs!=INVALID_FILE_ATTRIBUTES
#else
        }
        closedir(dir);
	}
#endif
    return filenames;
}

const char ** T5CALL T5_GetDirectoryList(const char *directory, const char **filenames){

    if(!T5_IsDir(directory))
        return NULL;

    if(filenames==NULL){
        filenames = (const char **)calloc(T5_GetNumDirectoriesInDirectory(directory), sizeof(const char *));
    }

	filehandle dir;
    filedata data;

	int i = 0;
	#ifdef _WIN32

	const char *fulldirlist = STRDUP((std::string(directory)+"*.*").c_str());
	DWORD attribs = ::GetFileAttributesA(directory);
	if ((attribs != INVALID_FILE_ATTRIBUTES) && (attribs & FILE_ATTRIBUTE_DIRECTORY)) {
		dir = FindFirstFile(directory, &data);

		if (dir!=INVALID_HANDLE_VALUE){
			do{
#else
	    if ((dir=opendir(directory))!=NULL){
	        while((data=readdir(dir))!=NULL){
#endif
				if((ISDIRECTORY(data))&&(FILENAME(data)[0]!='.')){
					filenames[i] = STRDUP(FILENAME(data));
					i++;
				}
#ifdef _WIN32
		    } while(FindNextFile(dir, &data));
		    FindClose(dir);
		}//dir!=INVALID_HANDLE_VALUE
	}//attribs!=INVALID_FILE_ATTRIBUTES
#else
        }
        closedir(dir);
	}
#endif
    return filenames;
}

void T5CALL T5_FreeFileName(const char *name){
    free((void *)name);
}

void T5CALL T5_FreeFileNames(const char **names, size_t num){
    for(size_t i = 0; i<num; i++)
        free((void *)names[i]);

    free(names);
}

#include <cstdio>

T5_FileText T5CALL T5_LoadFileAsText(const char *path){ //Open a text file and return the contents as a c string.

    FILE *file = fopen(path, "rb");
    fseek(file, 0, SEEK_END);
    size_t fsize = ftell(file);

    fseek(file, 0, SEEK_SET);

    char *text = (char *)calloc(1, fsize+1);

    fread(text, fsize, 1, file);

    return text;
/*
    std::fstream stream;
    stream.open(file, std::fstream::in);

    std::string buffer;

    while(stream.good()){
        getline(stream, buffer);
		buffer+="\n";
    }

    stream.close();

    T5_FileText text = STRDUP(buffer.c_str());

    return text;
*/
}

void T5CALL T5_FreeFileText(T5_FileText text){ //free the filetext.
    free((void *)text);
}

void T5CALL T5_init(int ndirs, ...){   //load T5_Directories, and init T5.
    T5_Directories.resize(0);
    va_list Dirs;
    va_start(Dirs, ndirs);
    for(int i = 0; i<ndirs; i++){
        T5_Directories.push_back(va_arg(Dirs, const char*));
    }
    va_end(Dirs);
}

void T5_AddDir(const char *dir){
    T5_Directories.push_back(dir);
}

T5_file *T5_OpenFile(const char* file){ //Make T5_ifile from filename
    for(unsigned int i = 0; i<T5_Directories.size(); i++){
    std::fstream testfile(std::string(T5_Directories[i]).append(file).c_str());
        if (testfile){
            testfile.close();
            return new T5_file(std::string(T5_Directories[i]).append(file).c_str());
        }
    }
    return NULL;
}

T5_file::T5_file(const char*file){
    values.resize(0);
    //stream.open(file, std::fstream::in|std::fstream::out|std::fstream::app);
    stream.open(file, std::fstream::in|std::fstream::out);
    std::string buffer;
    std::string val = "";
    std::string key = "";
    std::string sec = "";
    bool isSec  = false;
    bool isKey  = true;
    bool secDone= false;
    while(stream.good()){
        getline(stream, buffer);
		if(buffer.empty()){
            continue;
        }

        key     = "";
        val     = "";

        isSec   = false;
        isKey   = true;
        secDone = false;

        for (size_t i=0; i < buffer.length(); i++){
            if(buffer.at(i)<30){
            continue;
            }
            if(buffer.at(i)=='['){
                isSec = true;
                isKey = false;
                key = "";
                continue;
            }
            if(isSec){
                if(buffer.at(i)==']'){
                    secDone = true;
                    break;
                }
                val+=buffer.at(i);
            }
            else if(isKey){
                if(buffer.at(i)=='='){
                isKey=false;
                continue;
                }
                key+=buffer.at(i);
            }
            else{
                val+=buffer.at(i);
            }

        }


        if(isSec&&secDone){
            sec = val.c_str();
                #ifdef VERBOSE
                printf("Section Found: %s", sec.c_str());
                #endif
        }
        else{
        values.push_back(new INIvalue(STRDUP(key.c_str()), STRDUP(val.c_str()), STRDUP(sec.c_str())));
        }
    }
    stream.clear();
}

T5_file::T5_file(){

}

T5_file::~T5_file(){
    stream.clear();
    stream.flush();
    stream.close();
//    while(!values.empty()) delete values.back(), values.pop_back();

//    for(int i=0; i<values.size(); i++){
//        delete values[i];
//    }
}

void T5_close(T5_file *file){
    delete file;
}

void T5_file::flush(void){
    stream.flush();
}

const char* T5_file::getValue(const char* key){
    for(size_t i = 0; i<values.size(); i++){
            #ifdef VERBOSE
            printf("Value %s, Section %s\n", values[i]->val, values[i]->sec);
            #endif
        if(strcmp(values[i]->key, key)==0){
            return values[i]->val;
        }
    }
    return NULL;
}

const char* T5_file::getValueFromSection(const char* key, const char *sec){
	for(unsigned int i = 0; i<values.size(); i++){
		if(strcmp(values[i]->sec, sec)!=0){
            continue;
        }
        if(strcmp(values[i]->key, key)==0){
            return values[i]->val;
        }
    }
    return NULL;
}

void T5_file::writeValue(const char *_value, const char *_key){
   /*
    for(size_t i = 0; i<values.size(); i++){
        if(strcmp(values[i]->key, _key)==0){
            if(strcmp(values[i]->val, _value)==0){
                return;
            }
            values[i]->val = STRDUP(_value);
        }
    }*/
    std::stringstream ss;
    std::string buffer;
    std::string val = "";
    std::string key = "";
    std::string sec = "";
    bool isSec   = false;
    bool isKey   = true;
    bool foundKey= false;
    if(stream.bad())
        exit(77);
    stream.clear();
    stream.seekg(0, std::ios::beg);



    while(stream.good()){
        getline(stream, buffer);
		if(buffer.empty()){
            continue;
        }

        key     = "";
        val     = "";
        bool isKey = true;
        bool isVal = false;

        for (size_t i=0; i < buffer.length(); i++){
            if(isKey){
                if(buffer.at(i)=='['){
                    ss << buffer;
                    ss << "\n";
                    break;
                }
                if(buffer.at(i)=='='){
                    isKey=false;
                    isVal=true;
                    continue;
                }
                key+=buffer.at(i);

            }
            if(isVal){
                if(strcmp(key.c_str(), _key)==0){
                    val = _value;
                    foundKey = true;
                    break;
                }
                //if(isspace(buffer.at(i)))
                //    break;

                val+=buffer.at(i);

            }
        }
        if(isKey)
            continue;
        ss<<key;
        ss<<"=";
        ss<<val;
        ss<<"\n";
    }

    if(!foundKey){
        ss<<_key;
        ss<<"=";
        ss<<_value;
        ss<<"\n";
        //values.push_back(new INIvalue(STRDUP(_key), STRDUP(_value), ""));
    }
    stream.clear();
    stream.seekg(0, std::ios::beg);
    std::streambuf *s1 = stream.rdbuf();
    std::streambuf *s2 = ss.rdbuf();

    //stream.seekp(0);
    //ss.seekp(0);
    stream << ss.rdbuf();
    //swap(s1, s2);
//    swap(stream.rdbuf(), ss.rdbuf());
  //  stream.clear();
 //  stream << ss;
//    stream<<_key;
//    stream<<"=";
//    stream<<_value;
//    stream<<"\n";

    stream.flush();
}

void T5_file::writeValueToSection(const char *value, const char *_key, const char *_sec){
    stream.seekg(std::ios::beg);

    std::string buffer;
    std::string val = "";
    std::string key = "";
    std::string sec = "";
    bool isSec  = false;
    bool isKey  = true;
    bool secDone= false;
    stream.clear();

    while(stream.good()){
        getline(stream, buffer);
		if(buffer.empty()){
            continue;
        }

        key     = "";
        val     = "";

        isSec   = false;
        isKey   = true;
        secDone = false;

        for (size_t i=0; i < buffer.length(); i++){
            if(buffer.at(i)<30){
            continue;
            }
            if(buffer.at(i)=='['){
                isSec = true;
                isKey = false;
                key = "";
                continue;
            }
            if(isSec){
                if(buffer.at(i)==']'){
                    secDone = true;
                    break;
                }
                val+=buffer.at(i);
            }
            else if(isKey){
                if(buffer.at(i)=='='){
                isKey=false;
                continue;
                }
                key+=buffer.at(i);
            }
            else{
                val+=buffer.at(i);
            }

        }


        if(isSec&&secDone){
            if(strcmp(sec.c_str(), _sec)==0){
                stream<<"\n";
                stream<<_key;
                stream<<"=";
                stream<<value;
                return;
            }
        }
    }
    stream.seekg(std::ios::beg);
    stream<<"[";
    stream<<sec;
	stream<<"]";
	stream<<key;
    stream<<"=";
    stream<<value;
    stream<<"\n";

    stream.flush();
}

INIvalue::INIvalue(const char *keyname, const char *value, const char *section){
    key = keyname;
    val = value;
    sec = section;
}

INIvalue::~INIvalue(){
    free((void *)key);
    free((void *)val);
    free((void *)sec);
}
