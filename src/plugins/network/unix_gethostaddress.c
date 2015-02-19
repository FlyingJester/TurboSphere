#include <netdb.h>
#include <unistd.h>
#include <string.h>

char *TS_GetHostAddressIP4(){
    char name[0xFF];
    
    gethostname(name, 0xFF);
    name[0xFF-1] = '\0';
    struct hostent *hostentity = gethostbyname2(name, AF_INET);
         
    if(!hostentity)
        return NULL;
        
    for(int i = 0; i<hostentity->h_length; i++){
        if(strcmp(hostentity->h_addr_list[i], "127.0.0.1")!=0)
            return hostentity->h_addr_list[i];
    }
    
    return NULL;
    
}

char *TS_GetHostAddressIP6(){
    char name[0xFF];
    
    gethostname(name, 0xFF);
    name[0xFF-1] = '\0';
    
    struct hostent *hostentity = gethostbyname2(name, AF_INET6);
     
    if(!hostentity)
        return NULL;
        
    for(int i = 0; i<hostentity->h_length; i++){
        if(strcmp(hostentity->h_addr_list[i], "::1")!=0)
            return hostentity->h_addr_list[i];
    }
    return NULL;
}
