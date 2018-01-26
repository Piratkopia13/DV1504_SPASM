Texture2D<float4> input : register(t0);
RWTexture2D<float4> output : register(u0);

static const float filter[13] = {
  0.076838, 0.076881, 0.076915, 0.076942, 0.076962, 0.076973, 0.076977, 0.076973, 0.076962, 0.076942, 0.076915, 0.076881, 0.076838

};

#define NUM_THREADS 1024
[numthreads(NUM_THREADS, 1, 1)]
void CSHorizontal(uint3 DispatchThreadID : SV_DispatchThreadID) {

	float4 color;

	uint width;
	uint height;
	input.GetDimensions(width, height);

    uint2 pixCoords = uint2(DispatchThreadID.x + NUM_THREADS * DispatchThreadID.z, DispatchThreadID.y);

    if (pixCoords.x > width)
		return;

    for (int x = 0; x < 13; x++) {
        uint2 index = uint2(clamp(pixCoords.x + x - 6, 0, width), pixCoords.y);
        color += input[index] * filter[x];
    }

    output[pixCoords] = color;

}

[numthreads(1, NUM_THREADS, 1)]
void CSVertical(uint3 DispatchThreadID : SV_DispatchThreadID)
{

	float4 color;

	uint width;
	uint height;
	input.GetDimensions(width, height);

    uint2 pixCoords = uint2(DispatchThreadID.x, DispatchThreadID.y + NUM_THREADS * DispatchThreadID.z);

	if (pixCoords.y > height)
		return;

	for (int y = 0; y < 13; y++) {
		color += input[pixCoords + uint2(0, y - 6)] * filter[y];
	}

    output[pixCoords] = color;

}
