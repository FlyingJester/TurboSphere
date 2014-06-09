#ifndef SAPPHIRE_PRIMITIVE_SCRIPT_HEAD
#define SAPPHIRE_PRIMITIVE_SCRIPT_HEAD

#ifndef __cplusplus
    #error This file is the bridge between V8 and the concurrent draw queue. Using it in C is a categorical mistake.
#endif

#include "sapphire.h"
#include "fjgl.h"
#include "drawqueue.hpp"
#include "../common/plugin.h"
#include "glextra/gl_extra.h"
#include "image.hpp"
#include "cmath"
#include <SDL2/SDL_atomic.h>

namespace Sapphire {
    namespace Script {
        void FlipScreen(Turbo::JSArguments);
        void Point(Turbo::JSArguments);
        void GradientLine(Turbo::JSArguments);
        void GradientTriangle(Turbo::JSArguments);
    }
/////
// DrawQueue wrapper types

    class PolygonOp : public Sapphire::DrawingOp {
        public:

        PolygonOp();
        //PolygonOp(const PolygonOp&);
        PolygonOp(int n, const int *x, const int *y, const RGBA *color, Sapphire::Image *im){
            glGenBuffers(3, buffers);
            glGenVertexArrays(1, &Array);

            image = im;

            num_vertices = n;

            float coords[n<<1];
            float texcoords[n<<1];
            GLfloat colors[n<<2];

            for(int i = 0; i<n; i++){
                coords[(i<<1)  ] = x[i];
                coords[(i<<1)+1] = y[i];

                colors[(i<<2)  ] = ( color[i] >> 24 ) & 0xFF;
                colors[(i<<2)+1] = ( color[i] >> 16 ) & 0xFF;
                colors[(i<<2)+2] = ( color[i] >> 8  ) & 0xFF;
                colors[(i<<2)+3] = ( color[i]       ) & 0xFF;

                colors[(i<<2)  ] /= 255.0f;
                colors[(i<<2)+1] /= 255.0f;
                colors[(i<<2)+2] /= 255.0f;
                colors[(i<<2)+3] /= 255.0f;

            }

            float fi = 0.0f;
            float fn = n;
            for(int i = 0; i<n; i++, fi++){
                texcoords[(i<<1)  ] = TexCoordU(fi/fn);
                texcoords[(i<<1)+1] = TexCoordV(fi/fn);
            }

            glBindBuffer(GL_ARRAY_BUFFER, GetVertex());

            glBufferData(GL_ARRAY_BUFFER, n<<3, coords, GL_STATIC_DRAW);
            glVertexPointer(2, GL_FLOAT, 0, NULL);

            glBindVertexArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, GetTexcoord());
            glBufferData(GL_ARRAY_BUFFER, n<<3, texcoords, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, GetColor());
            glBufferData(GL_ARRAY_BUFFER, n<<4, colors, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

        }
        ~PolygonOp(){
            glDeleteBuffers(3, buffers);
            glDeleteVertexArrays(1, &Array);
        }

        Sapphire::Image *image;
        SDL_atomic_t IsDirty; //1 for dirty, 0 for clean.


        size_t num_vertices;

        virtual int call(void){
            glBindVertexArray(Array);
            glBindTexture(GL_TEXTURE_2D, image->GetTexture());
            glDrawArrays(GL_TRIANGLE_FAN, 0, num_vertices);
            glBindVertexArray(0);
            return num_vertices;
        }

        virtual void SetVertexAttribBinding(uint32_t vertex, uint32_t texcoord, uint32_t color);

        protected:

        inline float TexCoordU(float at){
            if(at<=0.25f)
                return at*4.0f;
            if((at>0.25f)&&(at<=0.5f))
                return 1.0f;
            if((at>0.5f)&&(at<=0.75f))
                return (0.25f-(at-0.5f))*4.0f;
            if(at>0.75f)
                return 0.0f;

            return 2.0f;
        }

        inline float TexCoordV(float at){
            if(at<=0.25f)
                return 0.0f;
            if((at>0.25f)&&(at<=0.5f))
                return (at-0.25f)*4.0f;
            if((at>0.5f)&&(at<=0.75f))
                return 1.0f;
            if(at>0.75f)
                return (0.25f-(at-0.75f))*4.0f;

            return 2.0f;
        }

        virtual void PrepareTexcoord(float *out){
                float fi = 0.0f;
                float fn = num_vertices;
                for(int i = 0; i<(int)num_vertices; i++, fi++){
                    out[(i<<1)  ] = TexCoordU(fi/fn);
                    out[(i<<1)+1] = TexCoordV(fi/fn);
                }
        }

        inline GLuint &GetVertex   (void){return buffers[0];}
        inline GLuint &GetTexcoord (void){return buffers[1];}
        inline GLuint &GetColor    (void){return buffers[2];}
        GLuint buffers[3];
        GLuint Array;
    };

    template<size_t n, GLenum mode=GL_POLYGON>
    class PrimitiveOp : public PolygonOp {
    public:
        //PrimitiveOp(const PrimitiveOp&);
        PrimitiveOp(int x[n], int y[n], RGBA color[n], GLuint tex){
            glGenBuffers(3, buffers);
            glGenVertexArrays(1, &Array);

            float coords[n<<1];
            float texcoords[n<<1];
            float colors[n<<2];

            for(int i = 0; i<n; i++){
                coords[(i<<1)  ] = x[i];
                coords[(i<<1)+1] = y[i];

                colors[(i<<2)  ] = ( color[i] >> 24 ) & 0xFF;
                colors[(i<<2)+1] = ( color[i] >> 16 ) & 0xFF;
                colors[(i<<2)+2] = ( color[i] >> 8  ) & 0xFF;
                colors[(i<<2)+3] = ( color[i]       ) & 0xFF;

                colors[(i<<2)  ] /= 255.0f;
                colors[(i<<2)+1] /= 255.0f;
                colors[(i<<2)+2] /= 255.0f;
                colors[(i<<2)+3] /= 255.0f;

            }

            PrepareTexcoord(texcoords);

            glBindVertexArray(Array);

            glBindBuffer(GL_ARRAY_BUFFER, GetVertex());
            glBufferData(GL_ARRAY_BUFFER, n<<3, coords, GL_STATIC_DRAW);
            glVertexPointer(2, GL_FLOAT, 0, NULL);
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, GetTexcoord());
            glBufferData(GL_ARRAY_BUFFER, n<<3, texcoords, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, GetColor());
            glBufferData(GL_ARRAY_BUFFER, n<<4, colors, GL_STATIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

        }
        ~PrimitiveOp(){
            glDeleteBuffers(3, buffers);
            glDeleteVertexArrays(1, &Array);
        }


        //The vertex attributes for shaders must have been enabled by the owner group.
        virtual int call(void){
            glBindVertexArray(Array);
            glBindTexture(GL_TEXTURE_2D, image->GetTexture());
            glDrawArrays(mode, 0, n);
            glBindVertexArray(0);
            return n;
        }

    protected:
    };

    class PointOp : public PrimitiveOp<1, GL_POINTS> {
    public:
        PointOp(int x, int y, RGBA color, GLuint tex) : PrimitiveOp(&x, &y, &color, tex){}
        PointOp(int *x, int *y, RGBA *color, GLuint tex) : PrimitiveOp(x, y, color, tex){}
        virtual void PrepareTexcoord(float out[2]){
            out[0] = 0.0f;
            out[1] = 0.0f;
        }
        ~PointOp();
        virtual int call(void);
    };

    class LineOp : public PrimitiveOp<2, GL_LINE_LOOP> {
    public:
        LineOp(int x[2], int y[2], RGBA color[2], GLuint tex) : PrimitiveOp(x, y, color, tex){}
        virtual void PrepareTexcoord(float out[4]){
            out[0] = 0.0f;
            out[1] = 0.0f;
            out[2] = 1.0f;
            out[3] = 0.0f;
        }
        ~LineOp();
        virtual int call(void);
    };

    class TriangleOp : public PrimitiveOp<3, GL_TRIANGLE_FAN> {
    public:
        TriangleOp(int x[3], int y[3], RGBA color[3], GLuint tex) : PrimitiveOp(x, y, color, tex){}
        virtual void PrepareTexcoord(float out[6]){
            out[0] = 0.0f;
            out[1] = 0.0f;
            out[2] = 1.0f;
            out[3] = 0.0f;
            out[4] = 1.0f;
            out[5] = 1.0f;
        }
        ~TriangleOp();
        virtual int call(void);
    };

}



#endif
