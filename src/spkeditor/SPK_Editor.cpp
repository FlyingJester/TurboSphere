#include "../package_spk.h"
#include "../t5.h"
#include <cstdlib>
#include <cstring>
#include <cstdio>

#define SPK_ERROR(x)\
    fprintf(stderr, "[SPK] Error: " x " Exiting.\n");\
    exit(1)
#ifdef _WIN32

#else
//I REQUIRE GETOPT!
#include <getopt.h>
#endif


#ifdef HAS_UNISTD_SYS
    #include <sys/unistd.h>
#elif HAS_UNISTD
    #include <unistd.h>
#else

#endif



int main(int argc, char *argv[]){

    int pack = 0;
    int unpack = 0;

    char *in  = NULL;
    char *out = NULL;

    for(int i = 1; i<argc; i++){
        if(strlen(argv[i])==0)
            continue;
        if(strlen(argv[i])>=2){
            if(*(argv[i])=='-')
                switch((argv[i])[1]){
                case 'u':
                case 'x':
                    unpack=1;
                    break;
                case 'p':
                case 'c':
                    pack=1;
                    break;
                case '-':
                    {//Long option.
                        if((argv[i][2]=='i')&&(argv[i][3]=='f')&&(argv[i][4]=='=')){
                            in = (char *)realloc(in, strlen(argv[i]+5)+1);
                            memcpy(in, argv[i]+5, strlen(argv[i]+5)+1);
                            continue;
                        }
                        if((argv[i][2]=='o')&&(argv[i][3]=='f')&&(argv[i][4]=='=')){
                            out = (char *)realloc(out, strlen(argv[i]+5)+1);
                            memcpy(out, argv[i]+5, strlen(argv[i]+5)+1);
                            continue;
                        }
                    }
                }
            else{
            }
        }

    }

    //Sanity Checks.
    if(unpack&&pack){
        SPK_ERROR("Both pack and unpack options specified.");
    }
    if((!unpack)&&(!pack)){
        SPK_ERROR("Neither pack nor unpack options specified.");
    }
    if((strlen(in)<1)||(strlen(out)<1)){
        if((strlen(in)<1))
            fprintf(stderr, "[SPK] Error: No path specified for input. Exiting.\n");
        if((strlen(out)<1))
            fprintf(stderr, "[SPK] Error: No path specified for output. Exiting.\n");
        exit(1);
    }

    if(unpack){
        if(!T5_IsFile(in)){
            fprintf(stderr, "[SPK] Error: File %s does not exist. Exiting.\n", in);
            exit(1);
        }
        if(!TS_IsSPK(in)){
            fprintf(stderr, "[SPK] Error: File %s is not a valid SPK. Exiting.\n", in);
            exit(1);
        }
    }

    if(pack){
        SPK_ERROR("Packing not implemented yet. Complain at http://forums.spheredev.org/index.php/topic,13 about it");
    }
    //End Sanity Checks.

    if(unpack==1){
        printf("[SPK] Info: Unpacking %s to %s...\n", in, out);
        int Error = TS_UnpackSPK1(in, out);
        if(Error!=0){
            SPK_ERROR("An unspecified error has occured.");
        }

        printf("[spk] Info: Done!\n");
    }
}
