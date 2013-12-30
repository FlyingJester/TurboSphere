#version 140

uniform sampler2D textureSampler;

in vec4 VColor;

void main(void){
    vec4 texcolor = texture2D(textureSampler, gl_TexCoord[0].st);
	gl_FragColor = texcolor*gl_Color;
}
