#ifndef TS_NORETURN_HEAD
#define TS_NORETURN_HEAD

#ifdef _MSC_VER
#define TS_NORETURN __declspec(noreturn)
#elif defined __GNUC__
#define TS_NORETURN __attribute__((noreturn))
#else
#define TS_NORETURN
#endif

#endif
