#define GRAPHICC_INTERNAL
#include "color.h"
#include <cmath>
#include <stdint.h>
//integrals are used because of bit-wise ops.

TS_Color::TS_Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a){
    red		= r;
    green	= g;
    blue	= b;
    alpha	= a;
    reserved= 0;
}

TS_Color::TS_Color(void){
    red     = 0;
    green   = 0;
    blue    = 0;
    alpha   = 0;
    reserved= 0;
}

TS_Color::~TS_Color(void){

}

bool TS_Color::operator==(const TS_Color &c2) {
    return (c2.toInt()==this->toInt());
}

uint32_t TS_Color::toInt(void) const{
uint32_t c = concatRGBA(red, green, blue, alpha);
return c;
}


uint32_t concatRGBA(int r, int g, int b, int a){
	//printf("concatenating RGBA\n");
	/*
	uint32_t rc=((r>255)?255:r);
	uint32_t gc=((r>255)?255:g);
	uint32_t bc=((r>255)?255:b);
	uint32_t ac=((r>255)?255:a);
    */
//	return ac | (bc<<8) | (gc<<16) | (rc<<24);
	return r | (g<<8) | (b<<16) | (a<<24);

}
