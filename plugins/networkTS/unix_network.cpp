#include "networkTS.h"
#include "unix_network.h"

const char *TS_GetHostName(void){

    char *name = (char *)calloc(MAX_HOSTNAME_SIZE, sizeof(char));
    int err = gethostname(name, MAX_HOSTNAME_SIZE);
    if(err!=0){
        //report error.
        TS_FreeHostName(name);
    }
    name[MAX_HOSTNAME_SIZE-1]='\0';
    return name;
}

