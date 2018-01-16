struct Vertex {
    float4 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

struct HSConstantDataOutput {
    float edges[2] : SV_TessFactor;
    //float inside : SV_InsideTessFactor;
};

HSConstantDataOutput ConstantHS(InputPatch<Vertex, 3> ip, uint patchID : SV_PrimitiveID) {
    
    //float tessFactor = 1.f;
    
    HSConstantDataOutput output;

    //for (int i = 0; i < 2; i++)
    //output.edges[i] = tessFactor;
    output.edges[0] = 64;
    output.edges[1] = 8;
    //output.inside = tessFactor;

    return output;

}


[domain("isoline")]
[partitioning("integer")]
[outputtopology("line")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantHS")]
Vertex HSMain(InputPatch<Vertex, 3> p, uint i : SV_OutputControlPointID, uint patchID : SV_PrimitiveID) {
    return p[i];
}