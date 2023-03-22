#version 460

layout(location=0) in vec2 f_uv;
layout(binding=0,rgba8) uniform readonly image2D text;

layout(location=0) out vec4 outColor;

void main(){
	vec4 color = imageLoad(text,ivec2(f_uv*vec2(1024,1024)));
	outColor= vec4(color.rgb,1);
	//outColor=vec4(f_uv.x,f_uv.y,0,1);

}