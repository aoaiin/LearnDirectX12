//***************************************************************************************
// color.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Transforms and colors geometry.
//***************************************************************************************

// 此处的cbuffer cbPerObject : register(b0)意为：
//		从寄存器槽号为0的地址去访问常量缓冲区中的常量数据。	这就是我们根签名所作的工作。
//	而结构体数据里的语义，就是输入布局所作的工作。
cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj; 
	float gTime;
	float4 gPulseColor;
};

struct VertexIn
{
	float3 PosL  : POSITION;
    float4 Color : COLOR;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	//// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
		
	// Transform to homogeneous clip space.
	//vin.PosL.xy += 0.5f * sin(vin.PosL.x) * sin(3.0f * gTime);
	//vin.PosL.z *= 0.6f + 0.4f * sin(2.0f * gTime);
	//vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	// Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;
    
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    return pin.Color;

	//float4 finalColor = pin.Color * ((sin(gTime) + 2) / 2 );
	//return finalColor;

	//clip(pin.Color.r - 0.5f);
	//return pin.Color;

	//const float pi = 3.1415926;
	//float s = 0.5f * sin(2 * gTime - 0.25 * pi) + 0.5f;
	//float4 c = lerp(pin.Color, gPulseColor, s);
	//return c;
}