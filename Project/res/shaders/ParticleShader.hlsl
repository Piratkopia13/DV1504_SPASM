//
// Structs
//
struct Particle
{
    float3 p_position : POSITION;
    float3 p_velocity : VELOCITY;
};

struct VSIn
{
    uint vertexid : SV_VertexID;
};


struct PSIn
{
    float4 ps_position : SV_POSITION;
    float2 ps_uv : TEXCOORD0;
};

struct GSIn
{
    float3 g_position : POSITION;
};
//
// End of Structs
//



//
// Vertex Shader
//
StructuredBuffer<Particle> ParticleState;

GSIn VSMain(VSIn input)
{
    GSIn output;

    output.g_position.xyz = ParticleState[input.vertexid].p_position;
    //output.g_position = float4(1.f, 1.f, 0.f, 1.f);
    //output.g_color = float4(1.f, 1.f, 1.f, 1.f);

    return output;
}
//
// End of Vertex Shader
//



//
// Geometry Shader
//
cbuffer CameraData : register(b0)
{
    float3 up;
    float3 cWPos;
};

cbuffer ModelData : register(b1)
{
    matrix mVP;
};

[maxvertexcount(6)]
void GSMain(point GSIn input[1], inout TriangleStream<PSIn> output)
{
    float3 nor = cWPos - input[0].g_position.xyz;
    nor = normalize(nor);
    float size = 1.f / 1.f;
    float3 camera_up = up * size;
    float3 right = normalize(cross(nor, camera_up)) * size;

    PSIn elements[6];

    for (int i = 0; i < 6; i++)
        elements[i].ps_position = float4(input[0].g_position, 1.f);
    
    
    
    //Bottom left
	elements[0].ps_position.xyz += -right - camera_up;
	elements[0].ps_uv = float2(0.f, 1.f);
    // Top left
	elements[1].ps_position.xyz += -right + camera_up;
	elements[1].ps_uv = float2(0.f, 0.f);
    //Top right
	elements[2].ps_position.xyz += right + camera_up;
	elements[2].ps_uv = float2(1.f, 0.f);

    //Top right
	elements[3].ps_position = elements[2].ps_position;
	elements[3].ps_uv = float2(1.f, 0.f);
    //Bottom right
	elements[4].ps_position.xyz += right - camera_up;
	elements[4].ps_uv = float2(1.f, 1.f);
    //Bottom left
	elements[5].ps_position = elements[0].ps_position;
	elements[5].ps_uv = float2(0.f, 1.0f);
        
    for (int i = 0; i < 6; i++)
        elements[i].ps_position = mul(elements[i].ps_position, mVP);


    for (int i = 0; i < 3; i++)
    {
        output.Append(elements[i]);
    }

    output.RestartStrip();

    for (int i = 3; i < 6; i++)
    {
        output.Append(elements[i]);
    }

}
//
// End of Geometry Shader
//



//
// Pixel Shader
//

Texture2D tex;
SamplerState ss;

float4 PSMain(PSIn input) : SV_TARGET
{
	return tex.Sample(ss, input.ps_uv);
}
//
// End of Pixel Shader
//



