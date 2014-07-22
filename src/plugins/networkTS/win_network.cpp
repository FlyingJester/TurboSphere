#include "networkTS.h"
#include "win_network.h"



const char *TS_GetHostName(void){
	static DWORD buffsize = (MAX_COMPUTERNAME_LENGTH-2)*sizeof(char);
    char *name = (char *)calloc(buffsize+1, sizeof(char));
	auto err = GetComputerName(name, &buffsize);
    if(err=0){
		return NULL;
	}
	
	//name[MAX_COMPUTERNAME_LENGTH]='\0';
    return name;
}

