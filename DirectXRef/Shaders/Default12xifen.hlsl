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

//#include "xifen.hlsl"

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


// 根据位置计算后输出到输出带
void AppendVertex(float3 p, inout TriangleStream<GeoOut> triStream)
{  
    VertexOut v;
    
    v.PosW = p;
    v.NormalW = normalize(v.PosW);
    v.TexC = float2(0.0f, 0.0f);
    //v.PosW = v.NormalW * r;
    
    GeoOut geoOut;
    float4 PosW = mul(float4(v.PosW, 1.0f), gWorld);
    geoOut.PosW = PosW;
    geoOut.NormalW = mul(v.NormalW, (float3x3) gWorld);
    geoOut.PosH = mul(PosW, gViewProj);
    geoOut.TexC = v.TexC;
    
    triStream.Append(geoOut);
}
void Subdivide(int cnt, VertexOut gin[3], inout TriangleStream<GeoOut> triStream)
{
    uint depth = pow(2, cnt);
    uint vcnt = depth * 2 + 1; // 最下面一层顶点数
    float len = distance(gin[0].PosW, gin[1].PosW) / (float) depth; // 每个小三角形边长
    float3 rightup = gin[1].PosW - gin[0].PosW; // 右上方
    rightup = normalize(rightup);
    float3 right = gin[2].PosW - gin[0].PosW; // 右方
    right = normalize(right);
    
    // 用于确定每一层三角形的位置
    float3 down = gin[0].PosW;
    float3 up = down + rightup * len;
    
    [unroll]
    for (uint i = 1; i <= depth; i++)
    {
        for (int j = 0; j < vcnt; j++)
        {
            if (j % 2 == 0)
                AppendVertex(down + right * len * (j / 2), triStream);
            else
                AppendVertex(up + right * len * (j / 2), triStream);
        }
        vcnt -= 2;
        down = up;
        up += rightup * len;
        triStream.RestartStrip();
    }
}


// 三角形运动（爆炸效果）
[maxvertexcount(3)]
void GS3(triangle VertexOut gin[3], uint primID : SV_PrimitiveID, inout LineStream<GeoOut> triStream)
{
    // 计算三角形的法线（三个顶点的插值）
    float3 triangleNormal = gin[0].NormalW + gin[1].NormalW + gin[2].NormalW;
    normalize(triangleNormal);
    float a = 0.0f, v = 0.0f;

    if (gTotalTime >= 1.0f)
    {
        a = 16.0f, v = 3.0f;    // 设置 速度/加速度
    }
    
    // 三角形 三个点的运动（dx=v+at）
    gin[0].PosW += v + triangleNormal * a * (gTotalTime - 1.0f);
    gin[1].PosW += v + triangleNormal * a * (gTotalTime - 1.0f);
    gin[2].PosW += v + triangleNormal * a * (gTotalTime - 1.0f);
    
    GeoOut vout[3];
    [unroll]
    for (uint i = 0; i < 3; i++)
    {
        float4 PosW = mul(float4(gin[i].PosW, 1.0f), gWorld);
        vout[i].PosW = PosW;
        vout[i].NormalW = mul(gin[i].NormalW, (float3x3) gWorld);
        vout[i].PosH = mul(PosW, gViewProj);
        vout[i].TexC = gin[i].TexC;

        triStream.Append(vout[i]);
    }
}

[maxvertexcount(100)]
void GS1(triangle VertexOut gin[3], uint primID : SV_PrimitiveID, inout TriangleStream<GeoOut> triStream)
{
    //Subdivide(3, gin, triStream);
    
    const float r = 8.0f;
    uint depth = pow(2, 3);
    //uint vcnt = depth * 2 + 1; // 最下面一层顶点数
    uint vcnt = depth * 2 + 1; // 最下面一层顶点数
    float len = distance(gin[0].PosW, gin[1].PosW) / (float) depth; // 每个小三角形边长
    float3 rightup = gin[1].PosW - gin[0].PosW; // 右上方
    rightup = normalize(rightup);
    float3 right = gin[2].PosW - gin[0].PosW; // 右方
    right = normalize(right);
    
    // 用于确定每一层三角形的位置
    float3 down = gin[0].PosW;
    float3 up = down + rightup * len;
    
    float3 p;
    VertexOut v;
    GeoOut geoOut;
    [unroll]
    for (uint i = 1; i <= depth; i++)
    {
        for (int j = 0; j < vcnt; j++)
        {
            if (j % 2 == 0)
                p = down + right * len * (j / 2);
                //AppendVertex(down + right * len * (j / 2), triStream);
            else
                p = up + right * len * (j / 2);
                //AppendVertex(up + right * len * (j / 2), triStream);          

            v.PosW = p;
            v.NormalW = normalize(v.PosW);
            v.TexC = float2(0.0f, 0.0f);
            v.PosW = v.NormalW * r;
            
            float4 PosW = mul(float4(v.PosW, 1.0f), gWorld);
            geoOut.PosW = PosW;
            geoOut.NormalW = mul(v.NormalW, (float3x3) gWorld);
            geoOut.PosH = mul(PosW, gViewProj);
            geoOut.TexC = v.TexC;
            geoOut.PrimID = primID;
    
            triStream.Append(geoOut);
        }
        vcnt -= 2;
        down = up;
        up += rightup * len;
        triStream.RestartStrip();
    }
}


// 原20面体
[maxvertexcount(8)]
void GS0(triangle VertexOut gin[3], uint primID : SV_PrimitiveID, inout TriangleStream<GeoOut> triStream)
{
    GeoOut geoOut[3];
    [unroll]
    for (uint i = 0; i < 3; i++)
    {
        float4 PosW = mul(float4(gin[i].PosW, 1.0f), gWorld);
        geoOut[i].PosW = PosW;
        geoOut[i].NormalW = mul(gin[i].NormalW, (float3x3) gWorld);
        geoOut[i].PosH = mul(PosW, gViewProj);
        geoOut[i].TexC = gin[i].TexC;
        
        triStream.Append(geoOut[i]);
    }
}

// 细分一次
[maxvertexcount(12)]
void GS2(triangle VertexOut gin[3], uint primID : SV_PrimitiveID, inout TriangleStream<GeoOut> triStream)
{
    const float r = 8.0f;
    
    VertexOut m[3];
    for (int i = 0; i < 3; i++)
    {
        // 计算中点坐标
        m[i].PosW = (gin[i].PosW + gin[(i + 1) % 3].PosW) * 0.5f;
        // 归一化法线
        m[i].NormalW = normalize(m[i].PosW);
        // 定义UV
        m[i].TexC = float2(0.0f, 0.0f);
        // 将顶点投影到半径为r的球面上
        //m[i].PosW = m[i].NormalW * r;
    }
    
    VertexOut geoOutVert[6];
    geoOutVert[0] = gin[0];
    geoOutVert[1] = m[0];
    geoOutVert[2] = m[2];
    geoOutVert[3] = m[1];
    geoOutVert[4] = gin[2];
    geoOutVert[5] = gin[1]; // 顶部的三角形要用 带的形式 绘制出来，但是没有邻近的点，只能strip，重新输入顶点
    
    GeoOut geoOut[6];
    [unroll]
    for (int i = 0; i < 6; i++)
    {
        float4 PosW = mul(float4(geoOutVert[i].PosW, 1.0f), gWorld);
        geoOut[i].PosW = PosW;
        geoOut[i].NormalW = mul(geoOutVert[i].NormalW, (float3x3) gWorld);
        geoOut[i].PosH = mul(PosW, gViewProj);
        geoOut[i].TexC = geoOutVert[i].TexC;
        
        //triStream.Append(geoOut[i]);
    }
    
    for (int i = 0; i < 5; i++)
    {
        triStream.Append(geoOut[i]);
    }
    triStream.RestartStrip();
    triStream.Append(geoOut[1]);
    triStream.Append(geoOut[5]);
    triStream.Append(geoOut[3]);
}


[maxvertexcount(2)]
void GSVNormal(point VertexOut gin[1], uint primID : SV_PrimitiveID, inout LineStream<GeoOut> lineStream)
{
    const float r = 1.0f;
    VertexOut vout[2];
    const float L = 1.0f;
    
    //vout[0].PosW = gin[0].PosW + gin[0].NormalW * r;
    vout[0].PosW = gin[0].PosW ;
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


