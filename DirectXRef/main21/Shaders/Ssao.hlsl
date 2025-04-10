//=============================================================================
// Ssao.hlsl by Frank Luna (C) 2015 All Rights Reserved.
//=============================================================================

cbuffer cbSsao : register(b0)	// SSAO常量
{
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gProjTex;
	float4   gOffsetVectors[14];	// 立方体 偏移向量

    // For SsaoBlur.hlsl
    float4 gBlurWeights[3];

    float2 gInvRenderTargetSize;

    // Coordinates given in view space.
    float    gOcclusionRadius;			// 随机点采样的半径
    float    gOcclusionFadeStart;		// 遮蔽计算（根据深度差 在start~end 计算遮蔽率）
    float    gOcclusionFadeEnd;
    float    gSurfaceEpsilon;
};

cbuffer cbRootConstants : register(b1)	// 根常量1 ：模糊
{
    bool gHorizontalBlur;		// 设置 水平/垂直
};
 
// Nonnumeric values cannot be added to a cbuffer.
Texture2D gNormalMap    : register(t0); // 根参数2的描述符表大小=2 ，而Normal的描述符后面就是 DSV
Texture2D gDepthMap     : register(t1);	
Texture2D gRandomVecMap : register(t2);

SamplerState gsamPointClamp : register(s0);
SamplerState gsamLinearClamp : register(s1);
SamplerState gsamDepthMap : register(s2);
SamplerState gsamLinearWrap : register(s3);

static const int gSampleCount = 14;
 
static const float2 gTexCoords[6] =	// 按顺序的两个三角形 （坐标类似在 屏幕[0-1]间）
{
    float2(0.0f, 1.0f),
    float2(0.0f, 0.0f),
    float2(1.0f, 0.0f),
    float2(0.0f, 1.0f),
    float2(1.0f, 0.0f),
    float2(1.0f, 1.0f)
};
 
struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosV : POSITION;
	float2 TexC : TEXCOORD0;
};

VertexOut VS(uint vid : SV_VertexID)	// SSAO只绘制6个点
{
    VertexOut vout;

    vout.TexC = gTexCoords[vid];	// 取出点

    // Quad covering screen in NDC space.
	// 变换到 ndc
	//	xy 铺满 -1~1屏幕 ； z=0 在中间
    vout.PosH = float4(2.0f*vout.TexC.x - 1.0f, 1.0f - 2.0f*vout.TexC.y, 0.0f, 1.0f);
 
    // Transform quad corners to view space near plane.
    float4 ph = mul(vout.PosH, gInvProj);		
    vout.PosV = ph.xyz / ph.w;							// 从ndc逆变换到 相机空间 近平面  

    return vout;
}

// Determines how much the sample point q occludes the point p as a function
// of distZ.
float OcclusionFunction(float distZ)
{
	//
	// If depth(q) is "behind" depth(p), then q cannot occlude p.  Moreover, if 
	// depth(q) and depth(p) are sufficiently close, then we also assume q cannot
	// occlude p because q needs to be in front of p by Epsilon to occlude p.
	//
	// We use the following function to determine the occlusion.  
	// 
	//
	//       1.0     -------------\
	//               |           |  \
	//               |           |    \
	//               |           |      \ 
	//               |           |        \
	//               |           |          \
	//               |           |            \
	//  ------|------|-----------|-------------|---------|--> zv
	//        0     Eps          z0            z1        
	//
	
	float occlusion = 0.0f;
	if(distZ > gSurfaceEpsilon)
	{
		float fadeLength = gOcclusionFadeEnd - gOcclusionFadeStart;
		
		// Linearly decrease occlusion from 1 to 0 as distZ goes 
		// from gOcclusionFadeStart to gOcclusionFadeEnd.	
		occlusion = saturate( (gOcclusionFadeEnd-distZ)/fadeLength );	// 根据深度，线性减少
	}
	
	return occlusion;	
}

float NdcDepthToViewDepth(float z_ndc)
{
    // z_ndc = A + B/viewZ, where gProj[2,2]=A and gProj[3,2]=B.
    float viewZ = gProj[3][2] / (z_ndc - gProj[2][2]);
    return viewZ;
}
 
float4 PS(VertexOut pin) : SV_Target
{
	// p -- the point we are computing the ambient occlusion for.
	// n -- normal vector at p.
	// q -- a random offset from p.
	// r -- a potential occluder that might occlude p.

	// Get viewspace normal and z-coord of this pixel.  
	// 1. 得到 相机空间下的该点p的   法线、深度
	//		采样 normalmap 就是View下的（前面的draw）
	//		采样 ndc深度 ，变换到View下	---------  深度图下的点，就是离相机 最近的p的深度z
    float3 n = normalize(gNormalMap.SampleLevel(gsamPointClamp, pin.TexC, 0.0f).xyz);
    float pz = gDepthMap.SampleLevel(gsamDepthMap, pin.TexC, 0.0f).r;
    pz = NdcDepthToViewDepth(pz);

	//
	// Reconstruct full view space position (x,y,z).
	// Find t such that p = t*pin.PosV.
	// p.z = t*pin.PosV.z
	// t = p.z / pin.PosV.z
	//
	// 1.1 重建 p 在观察空间中的位置 P（xyz） ：相似->缩放
    float3 p = (pz / pin.PosV.z) * pin.PosV; 
	
	
	
	// Extract random vector and map from [0,1] --> [-1, +1].
	//	SSAO类中的 randomvectorMap 是 256*256 的
	//	采样后的 随机向量/颜色，变化到 -1~1
	float3 randVec = 2.0f*gRandomVecMap.SampleLevel(gsamLinearWrap, 4.0f*pin.TexC, 0.0f).rgb - 1.0f;

	float occlusionSum = 0.0f;
	
	// Sample neighboring points about p in the hemisphere oriented by n.
	for(int i = 0; i < gSampleCount; ++i)
	{
		// Are offset vectors are fixed and uniformly distributed (so that our offset vectors
		// do not clump in the same direction).  If we reflect them about a random vector
		// then we get a random uniform distribution of offset vectors.
		//	对 立方体生成的  固定方向/不同长度的均匀 偏移向量， 通过 随机向量图的向量 进行反射：
		//			-> 得到随机的 均匀分布的 偏移向量
		float3 offset = reflect(gOffsetVectors[i].xyz, randVec);
	
		// Flip offset vector if it is behind the plane defined by (p, n).
		float flip = sign( dot(offset, n) );	// n 同向/正面
		
		// Sample a point near p within the occlusion radius.
		// 2. 点p出发，随机方向 点q
		float3 q = p + flip * gOcclusionRadius * offset;
		
		
		// 3.  构建  出随机采样点q 方向上 可能的遮蔽p的 点r
		
		// Project q and generate projective tex-coords.  
		// 3.1 将q投影到 平面 （r和q 投影到近平面上应该是一致的）
		float4 projQ = mul(float4(q, 1.0f), gProjTex);
		projQ /= projQ.w;

		// Find the nearest depth value along the ray from the eye to q (this is not
		// the depth of q, as q is just an arbitrary point near p and might
		// occupy empty space).  To find the nearest depth we look it up in the depthmap.
		// 3.2 （q投影的uv）采样 深度值 -- 点r的，并转换到 View空间
		float rz = gDepthMap.SampleLevel(gsamDepthMap, projQ.xy, 0.0f).r;
        rz = NdcDepthToViewDepth(rz);

		// Reconstruct full view space position r = (rx,ry,rz).  We know r
		// lies on the ray of q, so there exists a t such that r = t*q.
		// r.z = t*q.z ==> t = r.z / q.z
		// 3.3 同样 根据相似，获取 r的xyz
		float3 r = (rz / q.z) * q;
		
		//
		// Test whether r occludes p.
		//   * The product dot(n, normalize(r - p)) measures how much in front
		//     of the plane(p,n) the occluder point r is.  The more in front it is, the
		//     more occlusion weight we give it.  This also prevents self shadowing where 
		//     a point r on an angled plane (p,n) could give a false occlusion since they
		//     have different depth values with respect to the eye.
		//   * The weight of the occlusion is scaled based on how far the occluder is from
		//     the point we are computing the occlusion of.  If the occluder r is far away
		//     from p, then it does not occlude it.
		// 
		
		// 4. 遮蔽检测：
		//	r和p之间的深度（越远，遮蔽效果越弱； 过小，表示在同一平面，也不遮挡）
		//   防止 自相交r=p ：r和p不能是同一平面的
		float distZ = p.z - r.z;
		float dp = max(dot(n, normalize(r - p)), 0.0f);

        float occlusion = dp*OcclusionFunction(distZ);	// 遮蔽检测

		occlusionSum += occlusion;
	}
	
	// 5.计算
	occlusionSum /= gSampleCount;	// 除以随机采样数 -》 遮蔽率
	
	float access = 1.0f - occlusionSum;	// 可及率

	// Sharpen the contrast of the SSAO map to make the SSAO affect more dramatic.
	//	提高对比度
	return saturate(pow(access, 6.0f));
}
