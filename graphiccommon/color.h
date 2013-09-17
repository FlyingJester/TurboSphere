#ifndef COLOR_HEAD
#define COLOR_HEAD
#ifdef MSC_VER
#include <stdint.h>
#else
#include <inttypes.h>
#endif

#ifdef MSC_VER

    #define MEMALIGN(X) _declspec(align(X))
	#ifdef GRAPHICC_INTERNAL
		#define GRAPHICC_EXPORT __declspec(dllexport)
	#else
		#define GRAPHICC_EXPORT __declspec(dllimport)
	#endif
    #define GRAPHICCCALL _cdecl
    #define GRAPHICC_CLASS class GRAPHICC_EXPORT
#else

    #define MEMALIGN(X) __attribute__((aligned(X)))
	#define GRAPHICC_EXPORT extern "C"
	#define GRAPHICC_PRECALL extern "C"

    #define GRAPHICC_CLASS GRAPHICC_EXPORT class
#endif

GRAPHICC_CLASS MEMALIGN(4) TS_Color{
public:
	TS_Color(uint8_t, uint8_t, uint8_t, uint8_t);
	TS_Color(void);
	~TS_Color(void);
	uint32_t toInt(void) const;
		uint8_t red, green, blue, alpha;
    bool operator==(const TS_Color &c2);
    uint64_t reserved;
};

GRAPHICC_EXPORT inline uint32_t concatRGBA(int, int, int, int);

#endif
