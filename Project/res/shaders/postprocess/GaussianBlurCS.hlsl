Texture2D<float4> input : register(t0);
RWTexture2D<float4> output : register(u0);

static const float filter[13] = {
  0.076838, 0.076881, 0.076915, 0.076942, 0.076962, 0.076973, 0.076977, 0.076973, 0.076962, 0.076942, 0.076915, 0.076881, 0.076838

};

[numthreads(1, 1, 1)]
void CSHorizontal(uint3 DispatchThreadID : SV_DispatchThreadID) {

  float4 color;

  uint width;
  uint height;
  input.GetDimensions(width, height);

  for (int x = 0; x < 13; x++) {
    uint2 index = uint2(clamp(DispatchThreadID.x + x - 6, 0, width), DispatchThreadID.y);
      color += input[index] * filter[x];
  }

  output[DispatchThreadID.xy] = color;

}

[numthreads(1, 1, 1)]
void CSVertical(uint3 DispatchThreadID : SV_DispatchThreadID)
{

  float4 color;

  for (int y = 0; y < 13; y++) {
    color += input[DispatchThreadID.xy + uint2(0, y - 6)] * filter[y];
  }

  output[DispatchThreadID.xy] = color;

}