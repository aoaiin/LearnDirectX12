//=============================================================================
// Performs a separable Guassian blur with a blur radius up to 5 pixels.
//=============================================================================

cbuffer cbSettings : register(b0)
{
	// We cannot have an array entry in a constant buffer that gets mapped onto
	// root constants, so list each element.  
	
	int gBlurRadius;

	// Support up to 11 blur weights.
	float w0;
	float w1;
	float w2;
	float w3;
	float w4;
	float w5;
	float w6;
	float w7;
	float w8;
	float w9;
	float w10;
};

static const int gMaxBlurRadius = 5;


Texture2D gInput            : register(t0);// 输入的SRV纹理
RWTexture2D<float4> gOutput : register(u0);// 输出的UAV纹理

#define N 256								//		1个线程组有256个线程
#define CacheSize (N + 2*gMaxBlurRadius)	//		共享内存大小（因为模糊操作，左右两边需要多出模糊半径）
groupshared float4 gCache[CacheSize];		// 声明共享内存（一个线程组内）

// 按线程组处理
// 一个线程组 的线程数 ： 下面是每个线程的操作
[numthreads(N, 1, 1)]
void HorzBlurCS(int3 groupThreadID : SV_GroupThreadID,		// 组内线程ID
				int3 dispatchThreadID : SV_DispatchThreadID)// 分派ID（整个dispatch/纹理 中该线程的id）
{
	// 权重数组
	float weights[11] = { w0, w1, w2, w3, w4, w5, w6, w7, w8, w9, w10 };

	
	// 将输入的纹理 先复制/处理到 共享内存
	// 
	// 对左右两边 超出纹理的位置 进行clamp 
	if(groupThreadID.x < gBlurRadius)
	{
		int x = max(dispatchThreadID.x - gBlurRadius, 0);
		// 使用图像边缘颜色 覆盖
		gCache[groupThreadID.x] = gInput[int2(x, dispatchThreadID.y)];
	}
	if(groupThreadID.x >= N-gBlurRadius)
	{
		int x = min(dispatchThreadID.x + gBlurRadius, gInput.Length.x-1);
		gCache[groupThreadID.x+2*gBlurRadius] = gInput[int2(x, dispatchThreadID.y)];
	}
	// 设置 cache 当前像素（x + radius）
	gCache[groupThreadID.x+gBlurRadius] = gInput[min(dispatchThreadID.xy, gInput.Length.xy-1)];
	// 注意，最后一组 右侧的线程可能有剩余，所以要确保小于length
	
	
	// 等待所有线程完成任务：将 共享内存 填满
	GroupMemoryBarrierWithGroupSync();
	
	
	
	//
	// Now blur each pixel.
	//
	float4 blurColor = float4(0, 0, 0, 0);
	
	for(int i = -gBlurRadius; i <= gBlurRadius; ++i)
	{
		int k = groupThreadID.x + gBlurRadius + i;
		
		blurColor += weights[i+gBlurRadius]*gCache[k];	// 收集 该线程组 范围内的像素颜色
	}
	
	gOutput[dispatchThreadID.xy] = blurColor;	// 设置该位置的像素 
}

[numthreads(1, N, 1)]
void VertBlurCS(int3 groupThreadID : SV_GroupThreadID,
				int3 dispatchThreadID : SV_DispatchThreadID)
{
	// Put in an array for each indexing.
	float weights[11] = { w0, w1, w2, w3, w4, w5, w6, w7, w8, w9, w10 };

	//
	// Fill local thread storage to reduce bandwidth.  To blur 
	// N pixels, we will need to load N + 2*BlurRadius pixels
	// due to the blur radius.
	//
	
	// This thread group runs N threads.  To get the extra 2*BlurRadius pixels, 
	// have 2*BlurRadius threads sample an extra pixel.
	if(groupThreadID.y < gBlurRadius)
	{
		// Clamp out of bound samples that occur at image borders.
		int y = max(dispatchThreadID.y - gBlurRadius, 0);
		gCache[groupThreadID.y] = gInput[int2(dispatchThreadID.x, y)];
	}
	if(groupThreadID.y >= N-gBlurRadius)
	{
		// Clamp out of bound samples that occur at image borders.
		int y = min(dispatchThreadID.y + gBlurRadius, gInput.Length.y-1);
		gCache[groupThreadID.y+2*gBlurRadius] = gInput[int2(dispatchThreadID.x, y)];
	}
	
	// Clamp out of bound samples that occur at image borders.
	gCache[groupThreadID.y+gBlurRadius] = gInput[min(dispatchThreadID.xy, gInput.Length.xy-1)];


	// Wait for all threads to finish.
	GroupMemoryBarrierWithGroupSync();
	
	//
	// Now blur each pixel.
	//

	float4 blurColor = float4(0, 0, 0, 0);
	
	for(int i = -gBlurRadius; i <= gBlurRadius; ++i)
	{
		int k = groupThreadID.y + gBlurRadius + i;
		
		blurColor += weights[i+gBlurRadius]*gCache[k];
	}
	
	gOutput[dispatchThreadID.xy] = blurColor;
}