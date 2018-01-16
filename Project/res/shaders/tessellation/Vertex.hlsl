struct Vertex {
    float4 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

Vertex VSMain(Vertex input) {
    return input;
}