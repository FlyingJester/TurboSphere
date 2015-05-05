#include <Winsock2.h>
#include <string.h>

char *TS_GetHostAddressIP4(){
    
    char name[2048];
    
    gethostname(name, 2048);
    name[2047] = '\0';
    struct hostent *hostentity = gethostbyname(name);
    
    if(!hostentity)
        return NULL;
    
    for(int i = 0; i<hostentity->h_length; i++){
        if(strcmp(hostentity->h_addr_list[i], "127.0.0.1")!=0)
            return hostentity->h_addr_list[i];
    }
    
    return NULL;
}
char *TS_GetHostAddressIP6(){
    return NULL;
}
