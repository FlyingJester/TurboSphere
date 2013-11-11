#ifndef TS_LITERAL_TYPE_HEAD
#define TS_LITERAL_TYPE_HEAD

//Why can't we all even agree on THIS?

	#ifdef _WIN32
		#define DOUBLE_LITERAL(x) x
		#define FLOAT_LITERAL(x) x##f
	#else
		#define DOUBLE_LITERAL(x) x##d
		#define FLOAT_LITERAL(x) x##f
	#endif

#endif