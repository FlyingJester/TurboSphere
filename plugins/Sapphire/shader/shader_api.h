#ifndef SAPPHIRE_SHADER_API_HEAD
#define SAPPHIRE_SHADER_API_HEAD

/////
// Offsets

void TS_ShaderSetOffsetX(int x);
void TS_ShaderSetOffsetY(int y);
void TS_ShaderModifyOffsets(int x, int y);

int TS_ShaderGetOffsetX(void);
int TS_ShaderGetOffsetY(void);

inline void TS_ShaderGetOffsets(int *x, int *y){
    *x = TS_ShaderGetOffsetX();
    *y = TS_ShaderGetOffsetY();
}

inline void TS_ShaderSetOffsets(int x, int y){
    TS_ShaderSetOffsetX(x);
    TS_ShaderSetOffsetY(y);
}

void TS_ShaderSetColorFloatA(float color[4]);

inline void TS_ShaderSetColorFloat(float red, float green, float blue, float alpha){
    float color[4] = {red, green, blue, alpha};
    TS_ShaderSetColorFloatA(color);
}

inline void TS_ShaderSetColorInt(uint32_t color){
    float color[4] = {
        ((float)((color>>24)&0xFF))/255.0f,
        ((float)((color>>16)&0xFF))/255.0f,
        ((float)((color>>8 )&0xFF))/255.0f,
        ((float)((color    )&0xFF))/255.0f,
    };
    TS_ShaderSetColorFloatA(color);
}


#endif
