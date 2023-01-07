#version 460

layout(location=0) in vec3 fragColor;
layout(location=1) in vec3 fragPosWorld;
layout(location=2) in vec3 fragNormalWorld;

layout(location=0) out vec4 outColor;

layout(push_constant) uniform Push{
	mat4 modelMatrix;
	mat4 normalMatrix;
}push;

layout(constant_id=0) const int NUM_POINT_LIGHTS = 10;

struct PointLight{
	vec3 position;
	vec4 color;
};

layout(set=0,binding=0) uniform GlobalUbo{
	mat4 projection;
	mat4 view;
	vec4 ambientLightColor; // w is intensity
	PointLight PointLights[NUM_POINT_LIGHTS]; //TODO: Replace 10 with specialization constant
	int numLights;
}ubo;



void main(){
	
	vec3 diffuseLight= ubo.ambientLightColor.xyz* ubo.ambientLightColor.w;
	vec3 surfaceNormal= normalize(fragNormalWorld);
	
	for(int i=0;i<ubo.numLights;i++){
		PointLight light= ubo.PointLights[i];
		vec3 lightDir= (light.position-fragPosWorld);
		float attenuation= 1.0/dot(lightDir,lightDir); // 1/distance^2 (dot of dir,dir) == distance^2
		float cosAngIncidence= max(dot(normalize(fragNormalWorld),normalize(lightDir)),0.0);
		vec3 intensity= light.color.xyz* light.color.w*attenuation;
		
		diffuseLight+= intensity*cosAngIncidence;
	} 

	//float lightIntensity= AMBIENT+ max(dot(normalWorldSpace,ubo.directionToLight),0);
	outColor= vec4(diffuseLight*fragColor,1.0);
}