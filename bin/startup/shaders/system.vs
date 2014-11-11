#version 140

in vec4 Color;
out vec4 VColor;

void main(void){
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_FrontColor = vec4(gl_Color);
	gl_Position = ((gl_Vertex+vec4(16.0, 16.0, 0.0, 0.0))/vec4(300.0, -375.0/2.0, 1.0, 1.0))-vec4(1.0, -1.0, 0.0, 0.0)+(gl_Color/1024.0);
}
