//***************************************************************************************
// Default.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//
// Default shader, currently supports lighting.
//***************************************************************************************

// Defaults for number of lights.
#ifndef NUM_DIR_LIGHTS
    #define NUM_DIR_LIGHTS 1
#endif

#ifndef NUM_POINT_LIGHTS
    #define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
    #define NUM_SPOT_LIGHTS 0
#endif

// Include structures and functions for lighting.
#include "LightingUtil.hlsl"

// Constant data that varies per frame.

cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
};

cbuffer cbMaterial : register(b1)
{
	float4 gDiffuseAlbedo;
    float3 gFresnelR0;
    float  gRoughness;
	float4x4 gMatTransform;
};

// Constant data that varies per material.
cbuffer cbPass : register(b2)
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

    // Indices [0, NUM_DIR_LIGHTS) are directional lights;
    // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
    // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
    // are spot lights for a maximum of MaxLights per object.
    Light gLights[MaxLights];
};
 
struct VertexIn
{
	float3 PosL    : POSITION;
    float3 NormalL : NORMAL;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
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

    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    float3 worldNormal = normalize(pin.NormalW);
    float3 worldView = normalize(gEyePosW - pin.PosW);
    
    Material mat = { gDiffuseAlbedo, gFresnelR0, gRoughness };  // 材质属性：这个gRoughness应该是 光滑度？因为看下面要 1-Shininess
    float3 shadowFactor = 1.0f;//暂时使用1.0，不对计算产生影响    
    
    //卡通着色
    float3 lightVec = -gLights[0].Direction;
    float m = (1.0f - mat.Shininess) * 256.0f; //粗糙度因子里的m值 ，这里之前 .属性名写错了
    float3 halfVec = normalize(lightVec + worldView); //半角向量
    
    float ks = pow(max(dot(worldNormal, halfVec), 0.0f), m);
    if (ks >= 0.0f && ks <= 0.1f)
        ks = 0.0f;
    if (ks > 0.1f && ks <= 0.8f)
        ks = 0.5f;
    if (ks > 0.8f && ks <= 1.0f)
        ks = 0.8f;
    
    float roughnessFactor = (m + 8.0f) / 8.0f * ks; //粗糙度因子
    float3 fresnelFactor = SchlickFresnel(mat.FresnelR0, halfVec, lightVec); //菲尼尔因子
    
    float3 specAlbedo = fresnelFactor * roughnessFactor; //镜面反射反照率=菲尼尔因子*粗糙度因子
    specAlbedo = specAlbedo / (specAlbedo + 1.0f); //将镜面反射反照率缩放到[0，1]
    
    float kd = max(dot(worldNormal, lightVec), 0.0f);
    if (kd <= 0.1f)   
        kd = 0.1f;    
    if (kd > 0.1f && kd <= 0.5f)
        kd = 0.15f;
    if (kd > 0.5f && kd <= 1.0f)
        kd = 0.25f;
    //最终光强
    float3 lightStrength = kd*5 * gLights[0].Strength; //方向光单位面积上的辐照度 
    
    //漫反射+高光反射=入射光量*总的反照率
    float3 directLight = lightStrength * (mat.DiffuseAlbedo.rgb + specAlbedo); 
    
    //环境光照
    float4 ambient = gAmbientLight * gDiffuseAlbedo;
    //最终光照
    float3 finalCol = ambient.xyz + directLight;
    

    return float4(finalCol, gDiffuseAlbedo.a);
}


