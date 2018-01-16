struct DSOut {
    float4 hPos : SV_Position;
    float3 wPos : WorldPos;
    float3 wNormal : WorldNormal;
    float2 texCoord : TEXCOORD;
};

Texture2D tex;
SamplerState ss;

float4 PSMain(DSOut input) : SV_Target {
    float3 color = tex.Sample(ss, input.texCoord).rgb;
    return float4(color, 1.f);
}