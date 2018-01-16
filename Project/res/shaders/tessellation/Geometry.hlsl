struct DSOut
{
    float4 hPos : SV_Position;
    float3 wPos : WorldPos;
    float3 wNormal : WorldNormal;
    float2 texCoord : TEXCOORD;
    float3 right : RIGHT;
};

[maxvertexcount(4)]
void GSMain(line DSOut input[2], inout TriangleStream<DSOut> TriangleOutputStream) {

    float hairWidth = 0.02f;

    TriangleOutputStream.Append(input[0]);
    TriangleOutputStream.Append(input[1]);
    input[0].hPos.xyz += hairWidth * input[0].right;
    TriangleOutputStream.Append(input[0]);
    input[1].hPos.xyz += hairWidth * input[1].right;
    TriangleOutputStream.Append(input[1]);
}