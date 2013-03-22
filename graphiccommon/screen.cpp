#define GRAPHICC_INTERNAL
#include "screen.h"

unsigned int SCREENWIDTH = 300;
unsigned int SCREENHEIGHT = 240;

unsigned int MIN_SCREENWIDTH = 300;
unsigned int MIN_SCREENHEIGHT = 240;

unsigned int GetScreenWidth(){
	return SCREENWIDTH;
}

unsigned int GetScreenHeight(){
	return SCREENHEIGHT;
}

int SetScreenWidth(unsigned int w){
	if(w>MIN_SCREENWIDTH){
		SCREENWIDTH = w;
		return 0;
	}
	return 1;
}

int SetScreenHeight(unsigned int h){
	if(h>MIN_SCREENHEIGHT){
		SCREENHEIGHT = h;
		return 0;
	}
	return 1;
}

unsigned int GetMinScreenWidth(){
	return MIN_SCREENWIDTH;
}

unsigned int GetMinScreenHeight(){
	return MIN_SCREENHEIGHT;
}
