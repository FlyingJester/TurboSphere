#include <cstdlib>
#include <t5.h>
#include "libapi.hpp"

int LoadCallback(void *into, size_t len, void *arg){
    t5::DataSource *source = static_cast<t5::DataSource *>(arg);
    int a = source->Tell();
    source->Read(into, len);
    return source->Tell()-a;
}

int main(int argc, char *argv[]){
    if(argc<2)
        return EXIT_FAILURE;

    if(!t5::IsFile(argv[1]))
        return EXIT_FAILURE;

    t5::DataSource *source = t5::DataSource::FromPath(t5::DataSource::eRead, argv[1]);

    source->Read(nullptr, 0);

}
