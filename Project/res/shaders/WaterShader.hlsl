#include "Phong.hlsl"

struct VSIn {
	float4 position : POSITION;
	float2 texCoord : TEXCOORD0;
};

struct PSIn {
	float4 position : SV_Position;
	float2 texCoord : TEXCOORD0;
	float4 clipSpace : CLIPSPACE;
	float3 fragToCam : TOCAM;
};

cbuffer ModelData : register(b0) {
	matrix mWorld;
	matrix mVP;
}
cbuffer WorldData : register(b1) {
	float3 cameraPos;
}

PSIn VSMain(VSIn input) {
	
	// Tile factor for the dudv and normal textures
	float dudvTileFactor = 60.f;

	PSIn output;

	input.position.w = 1;
	output.position = mul(input.position, mWorld);
	float3 worldPos = output.position.xyz;
	output.position = mul(output.position, mVP);
	output.texCoord = input.texCoord * dudvTileFactor;
	output.clipSpace = output.position;
	output.fragToCam = cameraPos - worldPos;

	return output;

}

cbuffer WaterStuff : register(b0) {
  float dudvMoveAmount;
}
cbuffer Lights : register(b1) {
	DirectionalLight dirLight;
}

Texture2D tex[4];
SamplerState ss;


float4 PSMain(PSIn input) : SV_TARGET {
	
	// Distortion strenght
	float dudvStrenght = 0.01f;

	// Calculate projective texture coordinates
	float2 reflectionTexCoords;
	reflectionTexCoords.x = -input.clipSpace.x / input.clipSpace.w / 2.f + 0.5f;
	reflectionTexCoords.y = -input.clipSpace.y / input.clipSpace.w / 2.f + 0.5f;

	// near and far variables should be set in a cbuffer.. but im lazy
	float near = 0.1f;
	float far = 1000.0f;
	// Get the depth information from the depth buffer texture
	float depth = tex[1].Sample(ss, float2(-reflectionTexCoords.x, reflectionTexCoords.y)).r;
	// Convert texture depth to a distance
	// This is the distance to the bottom of the water from the camera
	float floorDistance = 2.f * near * far / (far + near - (2.f * depth - 1.f) * (far - near));
	// Clipspace z coordinate is the distance to the water surface from the camera
	float waterDistance = input.clipSpace.z;
	// Subtract to get the water depth
	float waterDepth = floorDistance - waterDistance;

	// Do some wierd stuff while sampling the dudv texture to get nice looking distorted texture coordinates
	float2 distortedTexCoords = tex[2].Sample(ss, float2(input.texCoord.x + dudvMoveAmount, input.texCoord.y - dudvMoveAmount)).rg * .1f;
	distortedTexCoords = input.texCoord + float2(distortedTexCoords.x, distortedTexCoords.y + dudvMoveAmount);
	float2 totalDistortion = (tex[2].Sample(ss, distortedTexCoords).rg * 2.f - 1.f) * dudvStrenght * saturate(waterDepth / 5.f);
	
	// Apply distortion
	reflectionTexCoords += totalDistortion;
	// Clamp values to remove artifacts at screen edges
	reflectionTexCoords = clamp(reflectionTexCoords, 0.001f, 0.999f);

	// Invert
	reflectionTexCoords = -reflectionTexCoords;

	// Create refraction coordinates
	float2 refractionTexCoords;
	refractionTexCoords.x = reflectionTexCoords.x;
	refractionTexCoords.y = -reflectionTexCoords.y;


	// Get normal from the normal map
	float3 normalMap = tex[3].Sample(ss, distortedTexCoords).rgb;
	float3 normal = float3(normalMap.r * 2.f - 1.f, normalMap.b, normalMap.g * 2.f - 1.f) * 0.5f;
	normal = normalize(normal);

	// Specular lighting
	input.fragToCam = normalize(input.fragToCam);
	float shininess = 100.f;
	float ks = 0.8f;
	float3 r = reflect(dirLight.direction, normal);
	r = normalize(r);
	float specularCoefficient = pow(saturate(dot(input.fragToCam, r)), shininess) * ks * saturate(waterDepth);
	float3 specularColor = float3(specularCoefficient * dirLight.color);

	// Calculate fresnel effect factor
	// This is a number from 0 - 1 that increases with a smaller angle
	float fresnel = dot(input.fragToCam, normal * .4f);
	fresnel = saturate(pow(fresnel, 1.0f));
	
	// Get the mixed color of both reflection and refraction textures
	//float4 mixedColor = lerp(tex[0].Sample(ss, reflectionTexCoords), tex[1].Sample(ss, refractionTexCoords), fresnel);
	float4 mixedColor = lerp(tex[0].Sample(ss, reflectionTexCoords), float4(0.f, 0.f, 0.f, 0.f), fresnel);

	// Apply a small amount of blue-ish color
	float3 resultingColor = lerp(mixedColor.rgb, float3(0.12f, 0.41f, 0.69f), 0.2f) + specularColor;
	
	// Set the alpha according to the water depth
	// This will fade out the water where it hits the shore
	return float4(resultingColor, saturate(waterDepth / 1.f * mixedColor.a));

	//return float4(waterDepth / 10.f, waterDepth / 10.f, waterDepth / 10.f, 1.0f);
	//return float4(1.f, 0.f, 0.f, 1.f);
	//return float4(fresnel, 0.f, 0.f, 1.0f);

}