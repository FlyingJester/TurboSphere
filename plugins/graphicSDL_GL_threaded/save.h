#ifndef SDL_GL_THREADED_HEAD
#define SDL_GL_THREADED_HEAD

//WARNING: Little endian only, as with the rest of TurboSphere.
int save_PNG(const char * path, void *pixels, unsigned int width, unsigned int height, char flags);

#define SDL_GL_SAVEPNG_NOERROR 0
#define SDL_GL_SAVEPNG_OOM 1
#define SDL_GL_SAVEPNG_NOFILE 2
#define SDL_GL_SAVEPNG_PNGERROR 3
#define SDL_GL_SAVEPNG_UNKNOWN 4


#endif
