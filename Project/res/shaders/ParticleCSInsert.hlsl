// ------------------
// CS Shader Inserter
// ------------------

struct Particle
{
    float3 position : POSITION;
    float3 velocity : VELOCITY;
};

cbuffer ParticleParameters
{
    float3 EmitterLocation;
    float3 RandomVector;
}

static const float3 direction[8] =
{
    normalize(float3(1.0f, 1.0f, 1.0f)),
    normalize(float3(-1.0f, 1.0f, 1.0f)),
    normalize(float3(-1.0f, -1.0f, 1.0f)),
    normalize(float3(1.0f, -1.0f, 1.0f)),
    normalize(float3(1.0f, 1.0f, -1.0f)),
    normalize(float3(-1.0f, 1.0f, -1.0f)),
    normalize(float3(-1.0f, -1.0f, -1.0f)),
    normalize(float3(1.0f, -1.0f, -1.0f))
};

AppendStructuredBuffer<Particle> NewStateInsert;

[numthreads(1, 1, 1)]
void CSMain(uint3 GroupThreadID : SV_GroupThreadID, uint3 GroupID : SV_GroupID)
{
    Particle p;    

    p.position = EmitterLocation;
	//p.velocity = reflect(direction[GroupThreadID.x], RandomVector) / 5000.0f;
    p.velocity = RandomVector / 5000.0f;

	AllMemoryBarrier();

    NewStateInsert.Append(p);
}
// -----------------
//        End
// -----------------