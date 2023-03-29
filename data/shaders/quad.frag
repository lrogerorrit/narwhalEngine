#version 460

layout(location=0) in vec2 f_uv;
layout(binding=0,rgba8) uniform readonly image2D text;

layout(location=0) out vec4 outColor;

ivec2 toTexturePos(vec2 uv, vec2 textureSz){
	return ivec2(uv*textureSz);
}

void main(){
	vec2 imgSize= imageSize(text);

	vec4 color = imageLoad(text,toTexturePos(f_uv,imgSize));
	outColor= vec4(color.rgb,1);
	//outColor=vec4(f_uv.x,f_uv.y,0,1);

}