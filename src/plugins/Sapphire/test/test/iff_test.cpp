#include "../../iff.h"
#include "../../../../t5.h"
#include <cstdio>
#include <cstring>
#include <cassert>

void WriteIffScripts(void * unused, const iff::IffData &value){

    assert(unused == (void *)0xFEFEFE);

    if(value.type!=23){
        fprintf(stderr, "[IffTest] Warning: Encountered a file that is not a script.\n");
        //return;
    }

    if(value.content.second==NULL){
        fprintf(stderr, "[IffTest] Warning: Encountered a file that is empty. Skipping\n");
        return;
    }

    printf("[IffTest] Writing Script %s of size %i.\n", value.name.c_str(), value.content.first);

    FILE *file = fopen(value.name.c_str(), "wb");



    fwrite(value.content.second, 1, value.content.first, file);

    fflush(file);
    fclose(file);
}

int main(int argc, char *argv[]){

    fprintf(stdout, "[IffTest] Info: IffTest tests the TurboSphere IFF reader built into Sapphire.\n\tThis test relies on a clean embedded script IFF file existing.\n\tIf you have run TurboSphere from this directory before, clean and then rebuild TurboSphere before running this test.\n");

    assert(argc>1);
    assert(strlen(argv[1])>2);

    for(int iter = 1; iter<argc; iter++){

        assert(T5_IsFile(argv[iter]));

        printf("[IffTest] Reading from file %s.\n", argv[iter]);

        iff::IffFile file = iff::IffFile();
        file.Load(argv[iter]);

        file.foreach(WriteIffScripts, (void *)0xFEFEFE);

    }
}
