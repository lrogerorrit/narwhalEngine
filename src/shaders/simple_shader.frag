#version 460

layout(location=0) in vec3 fragColor;
layout(location=0) out vec4 outColor;

layout(push_constant) uniform Push{
	mat4 transform;
	vec3 color;
}push;


void main(){
    outColor= vec4(fragColor,1.0);
}