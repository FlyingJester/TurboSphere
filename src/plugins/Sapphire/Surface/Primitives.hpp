#pragma once
#include <memory>
#include <SDL2/SDL_Surface.h>
#include <color.h>

namespace Sapphire{
    namespace Surface{
        
        struct point{int x, y;};
        struct vertex{struct point p; TS_Color color;};
        struct circle{struct point p; unsigned radius; TS_Color colors[2];};
        
        // Currently is a simple, in-order, threaded surface operator
        class SurfaceOperator{
        
            void Operate();
        
            struct SurfaceOperatorImpl;
            std::unique_ptr<SurfaceOperatorImpl> impl;
            
        public:
                        
            // Internal immediate calls
            // All calls lock the surface
            static void Point(SDL_Surface *s, const struct vertex p[1]);
            static void Line(SDL_Surface *s, const struct vertex vertices[2]);
            static void Triangle(SDL_Surface *s, const struct vertex vertices[3]);
            static void Quad(SDL_Surface *s, const struct vertex vertices[4]);
            static void Circle(SDL_Surface *s, const struct circle[1]);
            static void OutlinedCircle(SDL_Surface *s, const struct circle[1]); // Only the first color is used.     
            
            SurfaceOperator();
            ~SurfaceOperator();
            
            void QueuePoint(SDL_Surface *s, int x, int y, TS_Color *c);
            void QueueLine(SDL_Surface *s, int x1, int y1, int x2, int y2, TS_Color *c1, TS_Color *c2);
            void QueueTriangle(SDL_Surface *s, int x1, int y1, int x2, int y2, int x3, int y3, TS_Color *c1, TS_Color *c2, TS_Color *c3);
            void QueueQuad(SDL_Surface *s, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, TS_Color *c1, TS_Color *c2, TS_Color *c3, TS_Color *c4);
            void QueueCircle(SDL_Surface *s, int x, int y, unsigned radius, TS_Color *inner_color, TS_Color *outer_color);
            void QueueOutlinedCircle(SDL_Surface *s, int x, int y, unsigned radius, TS_Color *color);
            
            template<typename T, void (*Function)(SDL_Surface *, const T[])>
            static void FunctionWrapper(SDL_Surface *s, void *arg){
                T *value = static_cast<T *>(arg);
                Function(s, value);
                delete[] value;
            }
            
            void LockSurface(SDL_Surface *s);
            void UnlockSurface(SDL_Surface *s);
            
        };
    }
}
