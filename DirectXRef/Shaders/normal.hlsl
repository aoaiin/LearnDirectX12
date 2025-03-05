//***************************************************************************************
// Default.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Default shader, currently supports lighting.
//***************************************************************************************

// Defaults for number of lights.
#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif

// Include structures and functions for lighting.
#include "LightingUtil.hlsl"


// 漫反射贴图
Texture2D    gDiffuseMap : register(t0);

// 6个静态采样器
SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

// 三个 CB常量缓冲区 的数据结构
// Constant data that varies per frame.
cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
	float4x4 gTexTransform;
};

// Constant data that varies per material.
cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
    float3 gEyePosW;
    float cbPerObjectPad1;
    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    float gNearZ;
    float gFarZ;
    float gTotalTime;
    float gDeltaTime;
    float4 gAmbientLight;

	float4 gFogColor;
	float gFogStart;
	float gFogRange;
	float2 cbPerObjectPad2;

    // Indices [0, NUM_DIR_LIGHTS) are directional lights;
    // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
    // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
    // are spot lights for a maximum of MaxLights per object.
    Light gLights[MaxLights];
};

cbuffer cbMaterial : register(b2)
{
	float4   gDiffuseAlbedo;
    float3   gFresnelR0;
    float    gRoughness;
	float4x4 gMatTransform;
};

struct VertexIn
{
	float3 PosL    : POSITION;
    float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
	float2 TexC    : TEXCOORD;
};

struct GeoOut	// GS的输出/PS的输入
{
    float4 PosH : SV_POSITION; // 输入PS之前，一定要有齐次下的顶点坐标
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 TexC : TEXCOORD;
    uint PrimID : SV_PrimitiveID;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;
	
    // Transform to world space.
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = posW.xyz;

    // Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
    vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);

    // Transform to homogeneous clip space.
    vout.PosH = mul(posW, gViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
	vout.TexC = mul(texC, gMatTransform).xy;

    return vout;
}

// 面的法线：仍然输出两个顶点的 线条
[maxvertexcount(2)]
void GSFNormal(triangle VertexOut gin[3], uint primID : SV_PrimitiveID, inout LineStream<GeoOut> lineStream)
{
    float3 Fnormal = (float3) (gin[0].NormalW + gin[1].NormalW + gin[2].NormalW) / 3.0f;
    normalize(Fnormal);
    
    const float r = 1.0f;
    VertexOut vout[2];
    const float L = 1.0f;
    
    //vout[0].PosW = gin[0].PosW + gin[0].NormalW * r;
    vout[0].PosW = (gin[0].PosW + gin[1].PosW + gin[2].PosW) * 0.33333f ; // 三角形重心
    //vout[1].PosW = vout[1].PosW + gin[0].NormalW * L;
    vout[1].PosW = vout[0].PosW + Fnormal * L;
    
    vout[0].NormalW = gin[0].NormalW;
    vout[1].NormalW = gin[0].NormalW;
    
    GeoOut geoOut[2];
    [unroll]
    for (uint i = 0; i < 2; i++)
    {
        float4 PosW = mul(float4(vout[i].PosW, 1.0f), gWorld);
        geoOut[i].PosH = mul(PosW, gViewProj);
        geoOut[i].NormalW = vout[i].NormalW;
       
        lineStream.Append(geoOut[i]);
    }
    lineStream.RestartStrip();
}


[maxvertexcount(2)]
void GSVNormal(point VertexOut gin[1], uint primID : SV_PrimitiveID, inout LineStream<GeoOut> lineStream)
{
    const float r = 1.0f;
    VertexOut vout[2];
    const float L = 1.0f;
    
    //vout[0].PosW = gin[0].PosW + gin[0].NormalW * r;
    vout[0].PosW = gin[0].PosW ;
    //vout[1].PosW = vout[1].PosW + gin[0].NormalW * L;
    vout[1].PosW = vout[0].PosW + gin[0].NormalW * L;
    
    vout[0].NormalW = gin[0].NormalW;
    vout[1].NormalW = gin[0].NormalW;
    
    GeoOut geoOut[2];
    [unroll]
    for (uint i = 0; i < 2; i++)
    {
        float4 PosW = mul(float4(vout[i].PosW, 1.0f), gWorld);
        geoOut[i].PosH = mul(PosW, gViewProj);
        geoOut[i].NormalW = vout[i].NormalW;
       
        lineStream.Append(geoOut[i]);
    }
}

float4 PS(GeoOut pin) : SV_Target
{
    float4 litColor = float4(pin.NormalW.xyz * 0.5 + 0.5, 1.0f);
    return litColor;
}


