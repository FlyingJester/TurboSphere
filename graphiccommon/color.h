#ifndef COLOR_HEAD
#define COLOR_HEAD
#ifdef _WIN32
#include <stdint.h>
#else
#include <inttypes.h>
#endif

#ifdef _WIN32
	#ifdef GRAPHICC_INTERNAL
		#define GRAPHICC_EXPORT __declspec(dllexport)
	#else
		#define GRAPHICC_EXPORT __declspec(dllimport)
	#endif
#define GRAPHICCCALL _cdecl
#define GRAPHICC_CLASS class GRAPHICC_EXPORT
#else
	#define GRAPHICC_EXPORT extern "C"
	#define GRAPHICC_PRECALL extern "C"

#define GRAPHICC_CLASS GRAPHICC_EXPORT class
#endif

GRAPHICC_CLASS TS_Color{
public:
	TS_Color(uint8_t, uint8_t, uint8_t, uint8_t);
	TS_Color(void);
	~TS_Color(void);
	uint32_t toInt(void);
		uint8_t red, green, blue, alpha;
};

GRAPHICC_EXPORT uint32_t concatRGBA(int, int, int, int);

#endif
