#define BLOOM_THRESHOLD 0.5

static const float filter[13] =
{
  0.076838, 0.076881, 0.076915, 0.076942, 0.076962, 0.076973, 0.076977, 0.076973, 0.076962, 0.076942, 0.076915, 0.076881, 0.076838
};

float getBrightness(float3 color)
{
  return dot(color, float3(0.2126, 0.7152, 0.0722));
}

struct VSIn
{
    float4 position : POSITION0;
};

struct PSIn
{
    float4 position : SV_Position;
    float2 texCoord : TEXCOORD0;
};

PSIn VSMain(VSIn input)
{
    PSIn output;

    input.position.w = 1.f;
	// input position is already in clip space coordinates
    output.position = input.position;
    output.texCoord.x = input.position.x / 2.f + 0.5f;
    output.texCoord.y = -input.position.y / 2.f + 0.5f;

    return output;

}

Texture2D tex : register(t0);
SamplerState ss : register(s0);

float4 PSHorizontal(PSIn input) : SV_Target0
{

  //if (getBrightness(tex.Sample(ss, input.texCoord).rgb) < BLOOM_THRESHOLD)
  //  discard;

  float4 color;

  float width, height;
  tex.GetDimensions(width, height);

  for (int x = 0; x < 13; x++) {
    //float2 index = float2(input.texCoord.x + (x - 6.f) / width, input.texCoord.y);
    color += tex.Sample(ss, input.texCoord + float2((x - 6.f) / 160.f, 0.f)) * filter[x];
  }

  return color/* * getBrightness(tex.Sample(ss, input.texCoord).rgb)*/;
  //return float4(1,0,0,1);

  //return tex.Sample(ss, input.texCoord);

}

float4 PSVertical(PSIn input) : SV_Target0
{

  float4 color;

  float width, height;
  tex.GetDimensions(width, height);

  for (int y = 0; y < 13; y++) {
    color += tex.Sample(ss, input.texCoord + float2(0.f, (y - 6.f) / height)) * filter[y];
  }

  return color;
  //return tex.Sample(ss, input.texCoord);
  //return float4(1, 0, 0, 1);


}

