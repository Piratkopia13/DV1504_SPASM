Texture2D<float4> input : register(t0);
RWTexture2D<float4> output : register(u0);
SamplerState ss : register(s0);

cbuffer cb : register(b0)
{
    float pixelSize;
}

static const float filter[13] =
{
    0.076838, 0.076881, 0.076915, 0.076942, 0.076962, 0.076973, 0.076977, 0.076973, 0.076962, 0.076942, 0.076915, 0.076881, 0.076838
};

float getBrightness(float3 color)
{
    return dot(color, float3(0.2126, 0.7152, 0.0722));
}
float4 sampleTex(uint2 pixCoords)
{
    //return input.SampleLevel(ss, pixCoords * pixelSize, 0);
    return input[pixCoords * pixelSize];
}
float4 sampleFiltered(uint2 pixCoords) {
  return sampleTex(pixCoords);

}

#define NUM_THREADS 1024
#define BLOOM_THRESHOLD 0.5
[numthreads(NUM_THREADS, 1, 1)]

  void CSHorizontal
  (
  uint3 DispatchThreadID : SV_DispatchThreadID)
{

    uint width;
    uint height;
    output.GetDimensions(width, height);

    uint2 pixCoords = uint2(DispatchThreadID.x + NUM_THREADS * DispatchThreadID.z, DispatchThreadID.y);

    //input.Sample();

    if (pixCoords.x > width || getBrightness(sampleTex(pixCoords).rgb) < BLOOM_THRESHOLD)
      return;

    float4 color;

    for (int x = 0; x < 13; x++)
    {
      uint2 index = uint2(clamp(pixCoords.x + x - 6, 0, width), pixCoords.y);
      color += sampleTex(index) * filter[x];
    }


    output[pixCoords] = color;
    //output[pixCoords] = float4(1,0,0,1);

  }

[numthreads(1, NUM_THREADS, 1)]

  void CSVertical
  (
  uint3 DispatchThreadID : SV_DispatchThreadID)
{

    uint width;
    uint height;
    output.GetDimensions(width, height);

    uint2 pixCoords = uint2(DispatchThreadID.x, DispatchThreadID.y + NUM_THREADS * DispatchThreadID.z);

    if (pixCoords.y > height)
      return;

    float4 color;

    for (int y = 0; y < 13; y++)
    {
      color += input[pixCoords + uint2(0, y - 6)] * filter[y];
    }

    output[pixCoords] = color;

  }
