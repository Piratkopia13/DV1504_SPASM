// -----------------
// CS Shader Updater
// -----------------

struct Particle
{
    float3 position : POSITION;
    float3 velocity : VELOCITY;
};

cbuffer ParticleCount
{
    uint4 NumParticles;
};

AppendStructuredBuffer<Particle> NewStateUpdate : register(u0);
ConsumeStructuredBuffer<Particle> CurrentStateUpdate : register(u1);

static int groupDim_x = 512;
[numthreads(groupDim_x, 1, 1)]
void CSMain(uint3 DispatchThreadID : SV_DispatchThreadID)
{
    // The current thread's ID (each thread updates one particle.
    uint currID = DispatchThreadID.x;
    if (currID >= NumParticles.x)
        return;

    Particle p = CurrentStateUpdate.Consume();
    p.position += p.velocity;
    NewStateUpdate.Append(p);
    
}
// -----------------
//        End
// -----------------