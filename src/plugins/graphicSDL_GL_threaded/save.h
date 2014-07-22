#ifndef SDL_GL_THREADED_HEAD
#define SDL_GL_THREADED_HEAD

enum TGA_format{
    R8G8B8A8, R8G8B8, R5G5B5A1
};

//WARNING: Little endian only, as with the rest of TurboSphere.
int save_PNG(const char * path, const void *pixels, unsigned int width, unsigned int height, char flags);
int save_TGA(const char * path, const void *pixels, unsigned int width, unsigned int height, TGA_format format, char flags);

int save_AUTO(const char * path, const void * pixels, unsigned int width, unsigned int height);

#define SDL_GL_SAVE_NOERROR 0
#define SDL_GL_SAVE_OOM 1
#define SDL_GL_SAVE_NOFILE 2
#define SDL_GL_SAVE_PNGERROR 3
#define SDL_GL_SAVE_UNKNOWN 4

#define SDL_GL_SAVETGA_COMPRESS     0x01
#define SDL_GL_SAVETGA_GLNATIVE     0x02
#define SDL_GL_SAVETGA_CORONACOMPAT 0x08
#define SDL_GL_SAVETGA_BGRA         0x10
#define SDL_GL_SAVETGA_RGBA         0x40

#endif
