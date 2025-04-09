//***************************************************************************************
// Default.hlsl by Frank Luna (C) 2015 All Rights Reserved.
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

// Include common HLSL code.
#include "Common.hlsl"

struct VertexIn
{
	float3 PosL    : POSITION;
    float3 NormalL : NORMAL;
	float2 TexC    : TEXCOORD;
	float3 TangentU : TANGENT;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
    float4 ShadowPosH : POSITION0;  // 光源相机下的 阴影点的坐标
    float3 PosW    : POSITION1;
    float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
	float2 TexC    : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	// Fetch the material data.
	MaterialData matData = gMaterialData[gMaterialIndex];
	
    // Transform to world space.
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = posW.xyz;

    // Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
    vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);
	
	vout.TangentW = mul(vin.TangentU, (float3x3)gWorld);

    // Transform to homogeneous clip space.
    vout.PosH = mul(posW, gViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
	vout.TexC = mul(texC, matData.MatTransform).xy;

    // Generate projective tex-coords to project shadow map onto scene.
    // 生成 该阴影点 坐标（光源相机下：进行 View-Proj-T ）
    vout.ShadowPosH = mul(posW, gShadowTransform);
	
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	// Fetch the material data.
	MaterialData matData = gMaterialData[gMaterialIndex];
	float4 diffuseAlbedo = matData.DiffuseAlbedo;
	float3 fresnelR0 = matData.FresnelR0;
	float  roughness = matData.Roughness;
	uint diffuseMapIndex = matData.DiffuseMapIndex;
	uint normalMapIndex = matData.NormalMapIndex;
	
    // Dynamically look up the texture in the array.
    //  采用 diffusemap
    diffuseAlbedo *= gTextureMaps[diffuseMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);

#ifdef ALPHA_TEST
    // Discard pixel if texture alpha < 0.1.  We do this test as soon 
    // as possible in the shader so that we can potentially exit the
    // shader early, thereby skipping the rest of the shader code.
    clip(diffuseAlbedo.a - 0.1f);
#endif

	// PS中得到的 法向 经过插值，需要normalize
    pin.NormalW = normalize(pin.NormalW);
    // 采样 法线图（0-1），利用 像素法线、切线计算TBN，变换到 世界空间下的 法线
	float4 normalMapSample = gTextureMaps[normalMapIndex].Sample(gsamAnisotropicWrap, pin.TexC);
	float3 bumpedNormalW = NormalSampleToWorldSpace(normalMapSample.rgb, pin.NormalW, pin.TangentW);

	// Uncomment to turn off normal mapping.
    //bumpedNormalW = pin.NormalW;

    // Vector from point being lit to eye. 
    float3 toEyeW = normalize(gEyePosW - pin.PosW);

    // Light terms.
    float4 ambient = gAmbientLight*diffuseAlbedo;

    // Only the first light casts a shadow.
    float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);     // 阴影因子（只用第一个光，第一个分量）
    shadowFactor[0] = CalcShadowFactor(pin.ShadowPosH);

    const float shininess = (1.0f - roughness) * normalMapSample.a; // 法线图的alpha中存放的时 光泽度
    Material mat = { diffuseAlbedo, fresnelR0, shininess };
    float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
        bumpedNormalW, toEyeW, shadowFactor);   // 用 直接光照计算diffuse项

    float4 litColor = ambient + directLight;

	// Add in specular reflections. 镜面反射（来自环境/间接）
    float3 r = reflect(-toEyeW, bumpedNormalW);
    float4 reflectionColor = gCubeMap.Sample(gsamLinearWrap, r);          // 采样cubemap得到环境光  
    float3 fresnelFactor = SchlickFresnel(fresnelR0, bumpedNormalW, r);   // 用 r（环境的入射）、法线，计算菲尼尔反射因子
    litColor.rgb += shininess * fresnelFactor * reflectionColor.rgb;
	
    // Common convention to take alpha from diffuse albedo.
    litColor.a = diffuseAlbedo.a;

    return litColor;
}


