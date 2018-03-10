static const float filter[13] = {
    0.076838, 0.076881, 0.076915, 0.076942, 0.076962, 0.076973, 0.076977, 0.076973, 0.076962, 0.076942, 0.076915, 0.076881, 0.076838
};

float getBrightness(float3 color) {
    return dot(color, float3(0.2126, 0.7152, 0.0722));
}

struct VSIn {
    float4 position : POSITION0;
};

struct PSIn {
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD0;
};

cbuffer CBuffer : register(b0) {
    float invWindowWidth;
    float invWindowHeight;
}

PSIn VSMain(VSIn input) {
    PSIn output;

    input.position.w = 1.f;
	// input position is already in clip space coordinates
    output.position = input.position;
    output.texCoord.x = input.position.x / 2.f + 0.5f;
    output.texCoord.y = -input.position.y / 2.f + 0.5f;

    return output;

}

Texture2D tex : register(t0);
Texture2D depthTex : register(t1);
SamplerState ss : register(s0);

#define DEPTH_THRESHOLD 0.001f

float4 PSHorizontal(PSIn input) : SV_Target0 {

    float4 color;

    float centerDepth = depthTex.Sample(ss, input.texCoord).r;
    float4 centerColor = tex.Sample(ss, input.texCoord);
    for (int x = 0; x < 13; x++) {
        float2 coord = input.texCoord + float2((x - 6.f) * invWindowWidth, 0.f);
        if (abs(depthTex.Sample(ss, coord).r - centerDepth) < DEPTH_THRESHOLD) {
            color += tex.Sample(ss, coord) * filter[x];
        } else {
            color += centerColor * filter[x];
        }
    }

    return color;

}

float4 PSVertical(PSIn input) : SV_Target0 {

    float4 color;

    float centerDepth = depthTex.Sample(ss, input.texCoord).r;
    float4 centerColor = tex.Sample(ss, input.texCoord);
    for (int y = 0; y < 13; y++) {
        float2 coord = input.texCoord + float2(0.f, (y - 6.f) * invWindowHeight);
        if (abs(depthTex.Sample(ss, coord).r - centerDepth) < DEPTH_THRESHOLD) {
            color += tex.Sample(ss, coord) * filter[y];
        } else {
            color += centerColor * filter[y];
        }
    }

    return color;

}

