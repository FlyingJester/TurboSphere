#include "iff.h"
#include "../../t5.h"
#include <cstdio>

using namespace iff;
    /*
    typedef std::pair< size_t, char *> IffContent

    struct IffData {

        IffData(IffContent, std::string, uint32_t, uint64_t);
        ~IffData();

        IffContent content;
        std::string name;
        uint32_t type;
        uint16_t ID;
    };
    */
IffData::IffData(const IffContent &_iff, const std::string &_name, uint32_t _type, uint64_t _ID){
    content     = _iff;
    name        = _name;
    type        = _type;
    ID          = _ID;
}

IffData::IffData(const IffData &that){
    content = that.content;
    name    = that.name;
    type    = that.type;
    ID      = that.ID;
}

IffData::IffData(){
    content = IffContent(0, NULL);
    name = std::string("");
    type = 0;
    ID = 0;
}

IffData::~IffData(){

}

void IffFile::foreach(void(*callback)(void * userdata, const IffData &value), void *userdata) const {
    if(!callback){
        fprintf(stderr, "[IFF] Internal Error: Callback is NULL.\n");
        exit(EXIT_FAILURE);
    }
    if(values.empty())
        return;

    for(auto i = values.unsafe_begin();
        i!=values.unsafe_end();
        ++i){
        callback(userdata, *i);
    }
}

IffFile::IffFile(){

        std::string name        = std::string("");
        std::string signature   = std::string("");

        std::string filepath    = std::string("");

}

Error IffFile::Load(const char *path){
    if(!T5_IsFile(path))
        return ieDoesntExist;

    if(T5_GetFileSize(path)<70ull)
        return ieBadFile;

    filepath = std::string(path);

    return UnsafeLoad(filepath);
}

Error IffFile::UnsafeLoad(std::string &path){

    FILE *file = fopen(path.c_str(), "rb");

    fseek(file, 0u, SEEK_SET);
    //Get sig size.

    size_t fs = 0;

    while(fgetc(file))
        fs++;

    rewind(file);

    //Make holders for the sig and name.

    char *fname = (char *)calloc(fs+1, 1);
    char *fsignature = (char *)calloc((60-fs)+1, 1);

    //Read the header

    fread(fname, 1, fs, file);
    fgetc(file);
    fread(fsignature, 1, 60-fs, file);

    fseek(file, 0, SEEK_END);
    size_t totalsize = ftell(file);

    fseek(file, 64, SEEK_SET);

    size_t current = 64;

    //Store the sig and name.

    name = std::string(fname);
    signature = std::string(fsignature);

    /*

    typedef std::pair< size_t, char *> IffContent

    struct IffData {
        iffcontent content;
        std::string name;
        uint32_t type;
        uint16_t ID;
    };

    */

    uint32_t l = 0;
    uint32_t ftype = 0;
    uint16_t fID = 0;

    uint64_t trash;

    char *frname = (char *)calloc(64, 1);
    char *fdata = NULL;
    while(!feof(file)){
        current+=fread(&ftype, 4, 1, file);
        current+=fread(&l, 4, 1, file);

        //if(l<76)
        //    fprintf(stderr, "[Sapphire] Warning: data length is too small.\n");

        //l-=76;

        fdata = (char *)realloc(fdata, l);

        current+=fread(&fID, 2, 1, file);

        current+=fread(&trash, 2, 1, file);

        current+=fread(frname, 64, 1, file);

        current+=fread(fdata, l, 1, file);

        void *r = malloc(l);

        memcpy(r, (void *)fdata, l);

        values.push(IffData(IffContent(l, (char *)r), std::string(frname), ftype, fID));
    }



    fclose(file);

    return ieNone;
}

Error IffFile::Save() const{
    return ieNone;
}

Error IffFile::SaveAs(const char *path) const{
    return ieNone;

}
