struct Vertex {
    float4 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

struct HSConstantDataOutput {
    float edges[2] : SV_TessFactor;
    //float inside : SV_InsideTessFactor;
};

struct DSOut {
    float4 hPos : SV_Position;
    float3 wPos : WorldPos;
    float3 wNormal : WorldNormal;
    float2 texCoord : TEXCOORD;
    float3 right : RIGHT;
};

cbuffer ModelData : register(b0) {
    matrix mWorld;
    matrix mVP;
};

static float3 g_randBarycentric[64] = {
    float3(0.0593334, 0.45772, 0.482947),
    float3(0.874102, 0.874102, -0.748204),
    float3(0.158522, 0.158522, 0.682956),
    float3(0.23344, 0.733487, 0.0330724),
    float3(0.0653961, 0.179742, 0.754862),
    float3(0.450473, 0.450473, 0.0990541),
    float3(0.242648, 0.242648, 0.514703),
    float3(0.417756, 0.417756, 0.164488),
    float3(0.339871, 0.471351, 0.188778),
    float3(0.336239, 0.336239, 0.327521),
    float3(0.139865, 0.398581, 0.461554),
    float3(0.380082, 0.380082, 0.239835),
    float3(0.14361, 0.743037, 0.113352),
    float3(0.577779, 0.577779, -0.155557),
    float3(0.314846, 0.314846, 0.370307),
    float3(0.276904, 0.484282, 0.238814),
    float3(0.523987, 0.472654, 0.00335923),
    float3(0.0547813, 0.450953, 0.494266),
    float3(0.202258, 0.202258, 0.595483),
    float3(0.202599, 0.478664, 0.318737),
    float3(0.0399764, 0.0399764, 0.920047),
    float3(0.790119, 0.137656, 0.0722256),
    float3(0.276302, 0.276302, 0.447396),
    float3(0.526252, 0.526252, -0.0525031),
    float3(0.733519, 0.733519, -0.467038),
    float3(0.364841, 0.364841, 0.270318),
    float3(0.443188, 0.443188, 0.113623),
    float3(0.29764, 0.29764, 0.40472),
    float3(0.626719, 0.21682, 0.156461),
    float3(0.345587, 0.0772792, 0.577134),
    float3(0.262026, 0.609977, 0.127998),
    float3(0.786116, 0.786116, -0.572231),
    float3(0.80276, 0.80276, -0.605521),
    float3(0.259267, 0.0910541, 0.649679),
    float3(0.179894, 0.177771, 0.642335),
    float3(0.511789, 0.511789, -0.0235775),
    float3(0.752261, 0.0466677, 0.201071),
    float3(0.571017, 0.0938613, 0.335121),
    float3(0.407574, 0.158548, 0.433877),
    float3(0.224439, 0.565059, 0.210502),
    float3(0.100444, 0.548846, 0.350711),
    float3(0.0060304, 0.471945, 0.522025),
    float3(0.583104, 0.583104, -0.166208),
    float3(0.196029, 0.570189, 0.233782),
    float3(0.224824, 0.224824, 0.550352),
    float3(0.823186, 0.135071, 0.0417436),
    float3(0.298274, 0.298274, 0.403451),
    float3(0.0655742, 0.0655742, 0.868852),
    float3(0.81041, 0.81041, -0.620821),
    float3(0.169664, 0.357667, 0.472669),
    float3(0.144662, 0.0872601, 0.768078),
    float3(0.0236268, 0.0236268, 0.952746),
    float3(0.377897, 0.0798649, 0.542238),
    float3(0.0692421, 0.0692421, 0.861516),
    float3(0.592692, 0.592692, -0.185384),
    float3(0.346261, 0.346261, 0.307477),
    float3(0.366349, 0.485452, 0.1482),
    float3(0.445053, 0.445053, 0.109894),
    float3(0.30379, 0.122267, 0.573943),
    float3(0.409194, 0.219381, 0.371424),
    float3(0.515793, 0.515793, -0.0315857),
    float3(0.044551, 0.821518, 0.133931),
    float3(0.0797703, 0.0797703, 0.840459),
    float3(0.661771, 0.661771, -0.323542)
};

Texture2D tHairLength;
SamplerState ss;

[domain("isoline")]
DSOut DSMain(HSConstantDataOutput input, float2 UV : SV_DomainLocation, const OutputPatch<Vertex, 3> tri) {
    DSOut output = (DSOut) 0;

    int index = UV.y * input.edges[0] + .5; // Convert UV.y from [0,1] to [0, density]

    float3 finalNormal = g_randBarycentric[index].x * tri[0].normal + g_randBarycentric[index].y * tri[1].normal + g_randBarycentric[index].z * tri[2].normal;
    output.wNormal = mul(float4(finalNormal, 0.f), mWorld).xyz;

    float2 finalTexCoord = g_randBarycentric[index].x * tri[0].texCoord + g_randBarycentric[index].y * tri[1].texCoord + g_randBarycentric[index].z * tri[2].texCoord;
    output.texCoord = finalTexCoord;

    //float hairLength = 0.1f * tHairLength.SampleLevel(ss, output.texCoord, 0).r;
    float hairLength = 0.2f;
    float3 hairSag = float3(0.f, 0.4f, 0.f);

    // random point on the triangle
    float3 base = g_randBarycentric[index].x * tri[0].position.xyz + g_randBarycentric[index].y * tri[1].position.xyz + g_randBarycentric[index].z * tri[2].position.xyz;
    float3 finalPos = base + hairLength * UV.x * normalize(finalNormal - hairSag * pow(UV.x, 2));

    //finalPos += pow(hairSag, UV.x) * UV.x;

    output.wPos = mul(float4(finalPos, 1.f), mWorld).xyz;
    output.hPos = mul(float4(output.wPos, 1.f), mVP);
    output.right = normalize(tri[0].position.xyz - base);

    return output;
}