struct VSIn {
  float4 position : POSITION0;
  float3 positionOffset : TEXCOORD0;
  float4 color : TEXCOORD1;
  float4 textureOffsets : TEXCOORD2;
  float blendFactor : TEXCOORD3;
};

struct GSIn {
  float4 worldPos : POSITION;
  float4 color : COLOR0;
};

struct PSIn {
  float4 pos : SV_POSITION;
  float4 color : TEXCOORD0;
};

// =====================
// === VERTEX SHADER === 
// =====================
GSIn VSMain(VSIn input) {
  GSIn output;
    
  output.color = input.color;
  output.worldPos = float4(input.position.xyz + input.positionOffset, 1.f);

  return output;
}

cbuffer cameraData : register(b0) {
  matrix mVP;
  float3 camPos;
};

float4 format(float3 vec) {
  return mul(float4(vec, 1.f), mVP);
}

// =======================
// === GEOMETRY SHADER === 
// =======================
[maxvertexcount(6)]
void GSMain (point GSIn input[1], inout TriangleStream<PSIn> output) {

    //float halfSize = 1.f;
    //float3 p_pos = input[0].worldPos.xyz;
    //float3 p_camPos = camPos;
	
    //float3 planeNormal = normalize(p_pos - p_camPos);
    //float3 right = cross(planeNormal, float3(0.f, 1.f, 0.f));
    //float3 up = cross(right, planeNormal);

    //PSIn psin;
    //psin.color = input[0].color;
  
    //psin.pos = format(p_pos - (right + up) * halfSize);
    //output.Append(psin);
    //psin.pos = format(p_pos - (right - up) * halfSize);
    //output.Append(psin);
    //psin.pos = format(p_pos + (right - up) * halfSize);
    //output.Append(psin);
    //psin.pos = format(p_pos + (right + up) * halfSize);
    //output.Append(psin);
  //output.RestartStrip();
  //psin.pos = pos + (right + up) * halfSize;
  //output.Append(psin);
  //psin.pos = pos - (right + up) * halfSize;
  //output.Append(psin);
  //psin.pos = pos + (right - up) * halfSize;
  //output.Append(psin);

  PSIn psin;
  psin.color = input[0].color;
  psin.pos = input[0].worldPos;
  output.Append(psin);

  }


// ====================
// === PIXEL SHADER === 
// ====================
float4 PSMain(PSIn input) : SV_Target0 {
  return input.color;
}