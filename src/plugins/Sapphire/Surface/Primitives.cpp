#include <deque>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cmath>

#include <lines.h>
#include <circles.h>
#include <color.h>

#include "../Thread/Thread.hpp"
#include "../libyyymonitor/monitor.hpp"
#include "Primitives.hpp"

namespace Sapphire {
namespace Surface {

/////    
// Immediate Operations:    
void SurfaceOperator::Point(SDL_Surface *s, const struct vertex p[1]){
    assert(s);
    assert(p);
    
    SDL_LockSurface(s);
    
    unsigned pixel = p->p.x + (p->p.y*s->w);
    
    static_cast<uint32_t *>(s->pixels)[pixel] = p->color.toInt();
    
    SDL_UnlockSurface(s);
    
}

void SurfaceOperator::Line(SDL_Surface *s, const struct vertex vertices[2]){
    
    assert(s);
    assert(vertices);
    
    int n_points = 0;
    
    struct TS_Point line[2] = {{vertices[0].p.x, vertices[0].p.y}, {vertices[1].p.x, vertices[1].p.y}};
    
    struct TS_Point *points = CalcLine(line, &n_points);
    
    SDL_LockSurface(s);
    
    for(int i = 0; i<n_points; i++){
        unsigned pixel = points[i].x + (points[i].y*s->w);
        
        #define C_COMP(T)\
            ((vertices[0].color.T*i) + (vertices[1].color.T*(n_points-i)))
                
        static_cast<uint32_t *>(s->pixels)[pixel] = concatRGBA(C_COMP(red), C_COMP(green), C_COMP(blue), C_COMP(alpha));
        
        #undef C_COMP
        
    }
    
    free(points);
    
    SDL_UnlockSurface(s);
    
}

void SurfaceOperator::Triangle(SDL_Surface *s, const struct vertex vertices[3]){
    struct TS_Point *lines[3];
    int n_points[3];
    
    unsigned first_x = s->w, first_y = s->h, last_x = 0, last_y = 0;
    
    for(int i = 0; i<3; i++){
        struct TS_Point line[2] = {{vertices[i].p.x, vertices[i].p.y}, {vertices[(i+1)%3].p.x, vertices[(i+1)%3].p.y}};
        lines[i] = CalcLine(line, n_points+i);
        
        if(vertices[i].p.x<first_x)
            first_x = vertices[i].p.x;
        if(vertices[i].p.y<first_y)
            first_y = vertices[i].p.y;
            
        if(vertices[i].p.x>last_x)
            last_x = vertices[i].p.x;
        if(vertices[i].p.y>last_y)
            last_y = vertices[i].p.y;
    }
    
    float distances[3];
    float total_distance;
    
    SDL_LockSurface(s);
    
    for(int y = first_y; y<last_y; y++){
        bool on = false;
        for(int x = first_x; x<last_x; x++){
            
            // Calculate if we are inside the triangle;
            for(int l = 0; l<3; l++){
                for(int i = 0; i<n_points[l]; i++){
                    if((lines[l][i].x==x) && (lines[l][i].y==y)) on=!on;
                }
            }
            
            
            if(on){
                
                bool matches = false;
                uint32_t color;
                
                total_distance = 0.0f;
                
                for(int i = 0; i<3; i++){
                    
                    // Early break, also guards against std::sqrt(0).
                    if((x==vertices[i].p.x) && (y==vertices[i].p.y)){
                        color = vertices[i].color.toInt();
                        matches = true;
                        break;
                    }
                    
                    distances[i] = std::sqrt(std::pow(float(x-vertices[i].p.x), 2)+std::pow(float(y-vertices[i].p.y), 2)); 
                    total_distance+=distances[i];
                }
                
                if(!matches){
                    unsigned r, g, b, a;
                    
                    #define COMP_I(I, T)\
                      (distances[I]*float(vertices[I].color.T))
                    
                    #define ACCUM_COMP(T)\
                        (COMP_I(0, T) + COMP_I(1, T) + COMP_I(2, T))
                    
                    #define CALC_COMP(T)\
                        ACCUM_COMP(T) / total_distance
                    
                    r = CALC_COMP(red);
                    g = CALC_COMP(green);
                    b = CALC_COMP(blue);
                    a = CALC_COMP(alpha);
                    
                    #undef COMP_I
                    #undef ACCUM_COMP
                    #undef CALC_COMP
                    
                    color = concatRGBA(r, g, b, a); 
                }
                
                
                static_cast<uint32_t *>(s->pixels)[x+(y*s->w)] = color;
            }
        }
    }
    
    
    SDL_UnlockSurface(s);

}

void SurfaceOperator::Quad(SDL_Surface *s, const struct vertex vertices[4]){
    
    const struct vertex triangles[2][3] = {{vertices[0], vertices[1], vertices[2]}, {vertices[0], vertices[2], vertices[3]}};
    
    Triangle(s, triangles[0]);
    Triangle(s, triangles[1]);
    
}

void SurfaceOperator::Circle(SDL_Surface *s, const struct circle[1]){
    
}

void SurfaceOperator::OutlinedCircle(SDL_Surface *s, const struct circle[1]){ // Only the first color is used.

}

/////
// Guts implementation

struct SurfaceOperator::SurfaceOperatorImpl{
    std::unique_ptr<TS_Thread> thread;
    std::unique_ptr<Turbo::Monitor> monitor;
    
    struct Operation{
        void (*wrapper)(SDL_Surface *s, void *arg);
        SDL_Surface *surf;
        void *arg;
    };
    
    std::deque<struct Operation> operations;    
    
    struct reference_type {
        SDL_Surface *surf; 
        unsigned references;
    };
    
    std::vector<struct reference_type> references;
    
    struct reference_finder{
        const SDL_Surface * const that;
        
        reference_finder(const SDL_Surface *s)
          : that(s){}
        
        template<class T>
        bool operator() (const T other){
            return other.surf==that;
        }
    };
    
};


template<typename OP, typename T, void(*func)(SDL_Surface *, const T[]), typename Q>
void push_back_operation(Q &queue, SDL_Surface *s, T *t){
    OP op = {func, s, t};
    queue.push_back(op);
}

#define VERTEX_FROM_ARG(THAT, N)\
THAT[N-1].p.x = x##N;\
THAT[N-1].p.y = y##N;\
THAT[N-1].color = *c##N


/////
// Queuing operations
//static void Point(SDL_Surface *s, const struct vertex p[1]);
void SurfaceOperator::QueuePoint(SDL_Surface *s, int x1, int y1, TS_Color *c1){
    struct vertex *v = new struct vertex[1];
    VERTEX_FROM_ARG(v, 1);
    
    struct SurfaceOperatorImpl::Operation op = {FunctionWrapper<struct vertex, Point>, s, v};
    impl->operations.push_back(op);
}

//static void Line(SDL_Surface *s, const struct vertex vertices[2]);
void SurfaceOperator::QueueLine(SDL_Surface *s, int x1, int y1, int x2, int y2, TS_Color *c1, TS_Color *c2){
    
    struct vertex *v = new struct vertex[2];
    VERTEX_FROM_ARG(v, 1);
    VERTEX_FROM_ARG(v, 2);
    
    struct SurfaceOperatorImpl::Operation op = {FunctionWrapper<struct vertex, Line>, s, v};
    impl->operations.push_back(op);
    
}

//static void Triangle(SDL_Surface *s, const struct vertex vertices[3]);
void SurfaceOperator::QueueTriangle(SDL_Surface *s, int x1, int y1, int x2, int y2, int x3, int y3, TS_Color *c1, TS_Color *c2, TS_Color *c3){
    
    struct vertex *v = new struct vertex[3];
    VERTEX_FROM_ARG(v, 1);
    VERTEX_FROM_ARG(v, 2);
    VERTEX_FROM_ARG(v, 3);
    
    struct SurfaceOperatorImpl::Operation op = {FunctionWrapper<struct vertex, Triangle>, s, v};
    impl->operations.push_back(op);
    
}
//static void Quad(SDL_Surface *s, const struct vertex vertices[4]);
void SurfaceOperator::QueueQuad(SDL_Surface *s, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, TS_Color *c1, TS_Color *c2, TS_Color *c3, TS_Color *c4){
    
    struct vertex *v = new struct vertex[4];
    VERTEX_FROM_ARG(v, 1);
    VERTEX_FROM_ARG(v, 2);
    VERTEX_FROM_ARG(v, 3);
    VERTEX_FROM_ARG(v, 4);
    
    struct SurfaceOperatorImpl::Operation op = {FunctionWrapper<struct vertex, Quad>, s, v};
    impl->operations.push_back(op);
    
}
//static void Circle(SDL_Surface *s, const struct circle[1]);
void SurfaceOperator::QueueCircle(SDL_Surface *s, int x, int y, unsigned radius, TS_Color *inner_color, TS_Color *outer_color){
    
    struct circle *c = new struct circle[1];
    c[0].p.x = x;
    c[0].p.y = y;
    c[0].radius = radius;
    c[0].colors[0] = *inner_color;
    c[0].colors[1] = *outer_color;
    
    
    struct SurfaceOperatorImpl::Operation op = {FunctionWrapper<struct circle, Circle>, s, c};
    impl->operations.push_back(op);
    
    
}
//static void OutlinedCircle(SDL_Surface *s, const struct circle[1]); // Only the first color is used.
void SurfaceOperator::QueueOutlinedCircle(SDL_Surface *s, int x, int y, unsigned radius, TS_Color *color){
    
    struct circle *c = new struct circle[1];
    c[0].p.x = x;
    c[0].p.y = y;
    c[0].radius = radius;
    c[0].colors[0] = *color;
    
    struct SurfaceOperatorImpl::Operation op = {FunctionWrapper<struct circle, OutlinedCircle>, s, c};
    impl->operations.push_back(op);
    
}

#undef VERTEX_FROM_ARG

/////
// Drawing thread guts

void SurfaceOperator::LockSurface(SDL_Surface *surface){
    struct SurfaceOperatorImpl::reference_finder f(surface);
    struct Turbo::Monitor::Locker locker = impl->monitor->CreateLocker();
    
    std::vector<struct SurfaceOperatorImpl::reference_type>::const_reverse_iterator i =
      std::find_if(impl->references.crbegin(), impl->references.crend(), f);
    
    // If we don't even know about the surface, then we have never operated on it.
    if(i==impl->references.crend())
        return;
    
    while(i->references!=0){
        impl->monitor->Wait();
        i = std::find_if(impl->references.crbegin(), impl->references.crend(), f);
        
        assert(i!=impl->references.crend());
    }
    
}

void SurfaceOperator::Operate(){
    struct Turbo::Monitor::Locker locker = impl->monitor->CreateLocker();
    
    if(impl->operations.empty())
        impl->monitor->Wait();
    
    if(impl->operations.empty())
        return;

    const struct SurfaceOperatorImpl::Operation op = impl->operations.front();
    impl->operations.pop_front();

    op.wrapper(op.surf, op.arg);
    
    struct SurfaceOperatorImpl::reference_finder f(op.surf);
    
    std::vector<struct SurfaceOperatorImpl::reference_type>::reverse_iterator i =
      std::find_if(impl->references.rbegin(), impl->references.rend(), f);
    
    // Record the changes.
    if(i==impl->references.rbegin()){
        struct SurfaceOperatorImpl::reference_type ref = {op.surf, 0u};
        impl->references.push_back(ref);
    }
    else{
        assert(i->references!=0);
        i->references--;
    }
    
}

}
}
