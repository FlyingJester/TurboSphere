#ifndef TS_VERSION
#define TS_VERSION

#ifdef __cplusplus
extern "C"{
#endif

//A multi-line version description, intended to be printed to terminal.
const char *TS_GetLongVersion(void);
//Just the version number, intended for the script GetVersionString funciton.
const char *TS_GetVersion(void);


#ifdef __cplusplus
}
#endif
#endif
