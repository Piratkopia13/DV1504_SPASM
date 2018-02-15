float EDGE_THRESHOLD_MIN = 0.0312;
float EDGE_THRESHOLD_MAX = 0.125;
float SUBPIXEL_QUALITY = 0.75;
int ITERATIONS = 12;
float QUALITY[12] = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.5, 2.0, 2.0, 2.0, 2.0, 4.0, 8.0 };

bool isHorizontal = false;
bool reached1 = false;
bool reached2 = false;
bool reachedBoth = false;
bool is1Steepest = false;
bool isDirection1 = false;
bool isLumaCenterSmaller = false;
bool correctVariation = false;
float luma1, luma2, stepLength, lumaLocalAverage, offset, distance1, distance2, lumaEnd;

//
float3 colorCenter = tex.Sample(ss, input.texCoord).rgb;

float lumaCenter = rgbToLuma(colorCenter);

float lumaDown = rgbToLuma(tex.Sample(ss, input.texCoord, int2(0, 1)).rgb);
float lumaUp = rgbToLuma(tex.Sample(ss, input.texCoord, int2(0, -1)).rgb);
float lumaLeft = rgbToLuma(tex.Sample(ss, input.texCoord, int2(-1, 0)).rgb);
float lumaRight = rgbToLuma(tex.Sample(ss, input.texCoord, int2(1, 1)).rgb);

float lumaMin = min(lumaCenter, min(min(lumaDown, LumaUp), min(lumaLeft, lumaRight)));
float lumaMax = max(lumaCenter, max(max(lumaDown, LumaUp), max(lumaLeft, lumaRight)));

float lumaRange = lumaMax - lumaMin;

if (lumaRange < max(EDGE_THRESHOLD_MIN, lumaMax * EDGE_THRESHOLD_MAX) {
	output.color = colorCenter;
	return output;	//eller hur man nu gör en early exit

}

float lumaDownLeft = rgbToLuma(tex.Sample(ss, input.texCoord, int2(-1, 1)).rgb);
float lumaUpLeft = rgbToLuma(tex.Sample(ss, input.texCoord, int2(-1, -1)).rgb);
float lumaDownRight = rgbToLuma(tex.Sample(ss, input.texCoord, int2(1, 1)).rgb);
float lumaUpRight = rgbToLuma(tex.Sample(ss, input.texCoord, int2(1, -1)).rgb);

float lumaDownUp = lumaDown + lumaUp;
float lumaLeftRight = lumaLeft + lumaRight;

float lumaLeftCorners = lumaDownLeft + lumaUpLeft;
float lumaDownCorners = lumaDownLeft + lumaDownRight;
float lumaRightCorners = lumaDownRight + lumaUpRight;
float lumaUpCorners = lumaUpRight + lumaUpLeft;

float edgeHorizontal = abs(-2.0 * lumaLeft + lumaLeftCorners) + abs(-2.0 * lumaCenter + lumaDownUp) * 2.0 + abs(-2.0 * lumaRight + lumaRightCorners);
float edgeVertical = abs(-2.0 * lumaUp + lumaUpCorners) + abs(-2.0 *lumaCenter + lumaLeftRight) * 2.0 + abs(-2.0 * lumaDown + lumaDownCorners);

if (edgeHorizontal >= edgeVertical) {
	isHorizontal = true;
}

if (isHorizontal) {
	luma1 = lumaDown;
	luma2 = lumaUp;
}
else {
	luma1 = lumaleft;
	luma2 = lumaRight;
}

float gradient1 = luma1 - lumaCenter;
float gradient2 = luma2 - lumaCenter;

if (abs(gradient1) >= abs(gradient2)) {
	is1Steepest = true;
}

float gradientScaled = 0.25*max(abs(gradient1), abs(gradient2));


if (isHorizontal) {
	//stepLength = inverseScreenSize.y
}
else {
	//stepLength = inverseScreenSize.x
}

if (is1Steepest) {
	stepLength = -stepLength;
	lumaLocalAverage = 0.5*(luma1 + lumaCenter);
}
else {
	lumaLocalAverage = 0.5*(luma2 + lumaCenter);
}

float2 currentTexCoord = input.TexCoord;

if (isHorizontal) {
	offset = float2(inverseScreenSize.x, 0.0);//Byt ut inverseScreenSize
	currentTexCoord.y += stepLength * 0.5;
}
else {
	offset = float2(0.0, inverseScreenSize.y);//Byt ut inverseScreenSize
	currentTexCoord.x += stepLength * 0.5;
}

float2 texCoord1 = currentTexCoord - offset;
float2 texCoord2 = currentTexCoord + offset;

for (int i = 0; i < ITERATIONS; i++) {
	if (!reached1) {
		lumaEnd1 = rgbToLuma(tex.Sample(ss, texCoord1).rgb);
		lumaEnd1 -= lumaLocalAverage;
	}
	if (!reached2) {
		lumaEnd2 = rgbToLuma(tex.Sample(ss, texCoord2).rgb);
		lumaEnd2 -= lumaLocalAverage;
	}
	if (abs(lumaEnd1) >= gradientScaled) {
		reached1 = true;
	}
	if (abs(lumaEnd2) >= gradientScaled) {
		reached2 = true;
	}
	if (reached1 && reached2) {
		reachedBoth = true;
	}
	if (reachedBoth) {
		i = ITERATIONS;
	}
	else {
		if (!reached1) {
			texCoords1 -= offset * QUALITY[i]; //Kan vara syntaxfel här
		}
		if (!reached2) {
			texCoords2 -= offset * QUALITY[i]; //samma
		}
	}
}

if (isHorizontal) {
	distance1 = input.TexCoord.x - texCoord1.x;
	distance2 = texCoord2.x - input.TexCoord.x;
}
else {
	distance1 = input.TexCoord.y - texCoord1.y;
	distance2 = texCoord2.y - input.TexCoord.y;
}

if (distance1 < distance2) {
	isDirection1 = true;
}

float distanceFinal = min(distance1, distance2);
float edgeThickness = (distance1 + distance2);
float pixelOffset = -distanceFinal / edgeThickness + 0.5;

if (lumaCenter < lumaLocalAverage) {
	isLumaCenterSmaller = true;
}

if (isDirection1) {
	lumaEnd = lumaEnd1;
}
else {
	lumaEnd = lumaEnd2;
}

if (lumaEnd < 0.0 && !isLumaCenterSmaller) {//Denna kan vara fel
	correctVariation = true;
}

float finalOffset = 0.0;

if (correctVariation) {
	finalOffset = pixelOffset;
}

float lumaAverage = (1.0 / 12.0) * (2.0 * (lumaDownUp + lumaLeftRight) + lumaLeftCorners + lumaRightCorners);

float subPixelOffset1 = clamp(abs(lumaAverage - lumaCenter) / lumaRange, 0.0, 1.0);
float subPixelOffset2 = (-2.0 * subPixelOffset1 + 3.0) * subPixelOffset1 * subPixelOffset1;
float subPixelOffsetFinal = subPixelOffset2 * subPixelOffset2 * SUBPIXEL_QUALITY;

finalOffset = max(finalOffset, subPixelOffsetFinal);

float2 finalTexCoord = input.TexCoord;
if (isHorizontal) {
	finalTexCoord.y += finalOffset * stepLength;
}
else {
	finalTexCoord.x += finalOffset * stepLength;
}

float3 finalColor = tex.Sample(ss, finalTexCoord).rgb;

output.color = finalColor;

float rgbToLuma(float3 rgb) {
	return sqrt(dot(rgb, float3(0.299, 0.587, 0.114)));
}