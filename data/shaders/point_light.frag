#version 460

layout (location=0) in vec2 fragOffset;

layout (location=0) out vec4 outColor;


layout(constant_id=0) const int NUM_POINT_LIGHTS = 10;

struct PointLight{
	vec3 position;
	vec4 color;
};

layout(set=0,binding=0) uniform GlobalUbo{
	mat4 projection;
	mat4 view;
	mat4 invView;
	vec4 ambientLightColor; // w is intensity
	PointLight PointLights[NUM_POINT_LIGHTS]; //TODO: Replace 10 with specialization constant
	int numLights;
}ubo;

layout(push_constant) uniform Push{
	vec3 position;
	vec4 color;
	float radius;
} push;


void main() {
	float dist= length(fragOffset);
	if (dist>1.0) {
		discard;
	}
	outColor= vec4(push.color.xyz,1.0);

}