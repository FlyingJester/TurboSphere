#ifndef TS_OPTIONS_HEAD
#define TS_OPTIONS_HEAD

#ifdef __cplusplus
extern "C" {
#endif

// Checks for stubbing options, such as --version or -v, which result in
// TurboSphere performing some small task (like printing version info) and
// then exiting WITHOUT starting properly or trying to run a game.

void TS_CMD_ProcessStubbingOptions(int argc, char *argv[]);

// Returns the number of arguments on the command line that can be passed to
// V8. Returns 0 if the `standard' TS V8 options should be used.
unsigned TS_CMD_GetNumV8Options(int argc, char *argv[]);

// Parse out V8 args from the command line args.
// Returns a string that can be passed to V8.
// The result must be freed using free().
char *TS_CMD_ProcessV8Options(int argc, char *argv[]);

#ifdef __cplusplus
}
#endif

#endif
