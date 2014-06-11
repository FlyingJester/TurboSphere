#ifndef SAPPHIRE_IFF_HEAD
#define SAPPHIRE_IFF_HEAD

#include "../common/concurrent_queue.h"
#include <utility>
#include <string>

namespace iff{

    // [IffFile [IffData[ IffContent[size, data], ID, type, name]]...]

    enum Error {ieNone = 0, ieAccess = 1, ieDoesntExist, ieBadFile};

    typedef std::pair< int32_t, char *> IffContent;


    struct IffData {

        IffData(const IffContent &, const std::string &, uint32_t, uint64_t);
        IffData(const IffData &that);
        IffData();
        ~IffData();

        IffContent content;
        std::string name;
        uint32_t type;
        uint16_t ID;
    };

    typedef void(*IffCallback)(void * userdata, const IffData &value);

    class IffFile{
    public:
        IffFile();
        ~IffFile(){
        }

        Error Load(const char *path);
        Error Save() const;
        Error SaveAs(const char *path) const;


        std::string name;
        std::string signature;

        std::string filepath;

        /////
        // Data is NULL terminated.
        //
        // Length does NOT include a NULL terminator.
        //
        // Length is equivalent to strlen(data).
        //
        //                          length  data
        concurrent_queue<IffData > values;

        virtual void foreach(IffCallback callback, void *userdata) const;
    private:
        virtual Error UnsafeLoad(std::string &path);
    };
}


#endif
