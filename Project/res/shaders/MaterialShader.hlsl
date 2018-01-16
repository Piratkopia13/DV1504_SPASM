#include "Phong.hlsl"

struct VSIn {
	float4 position : POSITION0;
	float2 texCoords : TEXCOORD0;
	float3 normal : NORMAL0;
	float3 tangent : TANGENT0;
	float3 bitangent : BINORMAL0;
};

struct PSIn {
	float4 position : SV_Position;
	float3 normal : NORMAL0;
	float2 texCoords : TEXCOORD0;
	float clip : SV_ClipDistance0;
	float3 toCam : TOCAM;
	//Material material : MAT;
	LightList lights : LIGHTS;
};


cbuffer ModelData : register(b0) {
	matrix mWorld;
	matrix mVP;
	Material material;
}
cbuffer WorldData : register(b1) {
	float4 clippingPlane;
	float3 cameraPos;
}

struct PointLightInput {
	float3 color;
	float padding;
	float3 position;
	float attenuation;
};
cbuffer Lights : register(b2) {
	DirectionalLight dirLight;
	PointLightInput pointLights[NUM_POINT_LIGHTS];
}

PSIn VSMain(VSIn input) {
	PSIn output;

	// Copy over the directional light
	output.lights.dirLight = dirLight;
	// Copy over point lights
	// Different struct layouts are used for PointLightInput and PointLight
	// So make sure the memory layout match
	output.lights.pointLights = pointLights;

	input.position.w = 1.f;
	output.position = mul(input.position, mWorld);

	// Calculate the distance from the vertex to the clipping plane
	// This needs to be done with world coordinates
	output.clip = dot(output.position, clippingPlane);

	// World space vector pointing from the vertex position to the camera
	output.toCam = cameraPos - output.position.xyz;

	for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
		// World space vector poiting from the vertex position to the point light
		output.lights.pointLights[i].fragToLight = pointLights[i].position - output.position.xyz;
		// The world space distance from the vertex to the light
		output.lights.pointLights[i].distanceToLight = length(output.lights.pointLights[i].fragToLight);
	}


	output.position = mul(output.position, mVP);

	if (material.hasNormalTexture) {
	    // Convert to tangent space
		float3x3 TBN = {
			mul(input.tangent, (float3x3) mWorld),
			mul(input.bitangent, (float3x3) mWorld),
			mul(input.normal, (float3x3) mWorld)
		};
		TBN = transpose(TBN);

		output.toCam = mul(output.toCam, TBN);
		output.lights.dirLight.direction = mul(output.lights.dirLight.direction, TBN);
		for (int i = 0; i < NUM_POINT_LIGHTS; i++)
			output.lights.pointLights[i].fragToLight = mul(output.lights.pointLights[i].fragToLight, TBN);
	}

	output.normal = mul(input.normal, (float3x3) mWorld);
	output.normal = normalize(output.normal);

	output.texCoords = input.texCoords;

	return output;

}


Texture2D tex[3];
SamplerState ss;

float4 PSMain(PSIn input) : SV_Target0 {

	PhongInput phongInput;
	phongInput.mat = material;
	phongInput.fragToCam = input.toCam;
	phongInput.lights = input.lights;

	phongInput.diffuseColor = material.modelColor;
	if (material.hasDiffuseTexture)
		phongInput.diffuseColor *= tex[0].Sample(ss, input.texCoords);

	phongInput.normal = input.normal;
	if (material.hasNormalTexture)
		phongInput.normal = tex[1].Sample(ss, input.texCoords).rgb * 2.f - 1.f;

	phongInput.specMap = float3(1.f, 1.f, 1.f);
	if (material.hasSpecularTexture)
		phongInput.specMap = tex[2].Sample(ss, input.texCoords).rgb;


	return phongShade(phongInput);
    //return float4(0.f, 1.f, 0.f, 1.f);

}

