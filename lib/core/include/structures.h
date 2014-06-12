#ifndef STRUCURES_HEAD
#define STRUCURES_HEAD
#include <stdint.h>
#include <cstdio>
#ifdef _WIN32
	#ifdef GRAPHICALG_INTERNAL
		#define GRAPHICALG_EXPORT __declspec(dllexport)
	#else
		#define GRAPHICALG_EXPORT __declspec(dllimport)
	#endif
#define GRAPHICALGCALL _cdecl
#else
	#define GRAPHICALG_EXPORT extern "C"
#endif

#ifdef _WIN32
typedef struct GRAPHICALG_EXPORT TS_Point {
#else
GRAPHICALG_EXPORT typedef struct TS_Point {
#endif

	int x;
	int y;
    TS_Point(int, int);
    ~TS_Point();
} TS_Point;

#ifdef _WIN32
typedef struct GRAPHICALG_EXPORT TS_Segment {
#else
GRAPHICALG_EXPORT typedef struct TS_Segment {
#endif

	int x1;
	int y1;
	int x2;
	int y2;
    TS_Segment(int, int, int, int);
    TS_Segment();
    ~TS_Segment();
} TS_Segment;

#endif
