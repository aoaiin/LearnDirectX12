//***************************************************************************************
// Ssao.h by Frank Luna (C) 2015 All Rights Reserved.
//***************************************************************************************

#ifndef SSAO_H
#define SSAO_H

#pragma once

#include "d3dUtil.h"
#include "FrameResource.h"
 
 
class Ssao
{
public:

	Ssao(ID3D12Device* device, 
        ID3D12GraphicsCommandList* cmdList, 
        UINT width, UINT height);
    Ssao(const Ssao& rhs) = delete;
    Ssao& operator=(const Ssao& rhs) = delete;
    ~Ssao() = default; 

    static const DXGI_FORMAT AmbientMapFormat = DXGI_FORMAT_R16_UNORM;
    static const DXGI_FORMAT NormalMapFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;

    static const int MaxBlurRadius = 5;

	UINT SsaoMapWidth()const;
    UINT SsaoMapHeight()const;

    void GetOffsetVectors(DirectX::XMFLOAT4 offsets[14]);
    std::vector<float> CalcGaussWeights(float sigma);


	ID3D12Resource* NormalMap();
	ID3D12Resource* AmbientMap();
	
    CD3DX12_CPU_DESCRIPTOR_HANDLE NormalMapRtv()const;
	CD3DX12_GPU_DESCRIPTOR_HANDLE NormalMapSrv()const;
    CD3DX12_GPU_DESCRIPTOR_HANDLE AmbientMapSrv()const;

	void BuildDescriptors(
        ID3D12Resource* depthStencilBuffer,
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv,
		CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtv,
        UINT cbvSrvUavDescriptorSize,
        UINT rtvDescriptorSize);

    void RebuildDescriptors(ID3D12Resource* depthStencilBuffer);

    void SetPSOs(ID3D12PipelineState* ssaoPso, ID3D12PipelineState* ssaoBlurPso);

	///<summary>
	/// Call when the backbuffer is resized.  
	///</summary>
	void OnResize(UINT newWidth, UINT newHeight);
  
    ///<summary>
    /// Changes the render target to the Ambient render target and draws a fullscreen
    /// quad to kick off the pixel shader to compute the AmbientMap.  We still keep the
    /// main depth buffer binded to the pipeline, but depth buffer read/writes
    /// are disabled, as we do not need the depth buffer computing the Ambient map.//
    /// 1. 设置RT为 SSAO资源的AmbientMap，并绘制一个 全屏quad
    /// 2. 仍然使用 主深度缓冲区，但不进行读取/写入
    ///</summary>
	void ComputeSsao(
        ID3D12GraphicsCommandList* cmdList, 
        FrameResource* currFrame, 
        int blurCount);
 

private:
 
    ///<summary>
    /// Blurs the ambient map to smooth out the noise caused by only taking a
    /// few random samples per pixel.  We use an edge preserving blur so that 
    /// we do not blur across discontinuities--we want edges to remain edges.
    ///</summary>
    void BlurAmbientMap(ID3D12GraphicsCommandList* cmdList, FrameResource* currFrame, int blurCount);
	void BlurAmbientMap(ID3D12GraphicsCommandList* cmdList, bool horzBlur);

    void BuildResources();
    void BuildRandomVectorTexture(ID3D12GraphicsCommandList* cmdList);
 
	void BuildOffsetVectors();


private:
	ID3D12Device* md3dDevice;

    Microsoft::WRL::ComPtr<ID3D12RootSignature> mSsaoRootSig;
    
    ID3D12PipelineState* mSsaoPso = nullptr;
    ID3D12PipelineState* mBlurPso = nullptr;

    // 随机 向量/颜色 图： 多利用一个上传堆  传数据
    Microsoft::WRL::ComPtr<ID3D12Resource> mRandomVectorMap;        
	Microsoft::WRL::ComPtr<ID3D12Resource> mRandomVectorMapUploadBuffer;

    Microsoft::WRL::ComPtr<ID3D12Resource> mNormalMap;      // PS return  相机空间下的 场景法线（顶点法线插值）
   
    // 1. SSAO的 可及率 存放在 mAmbientMap0
    // 2. 模糊处理（二维高斯分为 水平+垂直）  需要两个map
    Microsoft::WRL::ComPtr<ID3D12Resource> mAmbientMap0;    
    Microsoft::WRL::ComPtr<ID3D12Resource> mAmbientMap1;

    CD3DX12_CPU_DESCRIPTOR_HANDLE mhNormalMapCpuSrv;    // 法线图 需要绘制（RT），读取（SRV）
    CD3DX12_GPU_DESCRIPTOR_HANDLE mhNormalMapGpuSrv;
    CD3DX12_CPU_DESCRIPTOR_HANDLE mhNormalMapCpuRtv;


    //  这里好像没有保存的深度图？
    //  而且绘制深度的时候，也只是绘制到 通用的 深度缓冲区（不像shadowmap有个 资源用于 depth_write）
    // 
    //  看了main中的build描述符，SSAO是获取了通用的深度缓冲区，
    //  然后 调用RebuildDescriptors 创建深度map的描述符
    CD3DX12_CPU_DESCRIPTOR_HANDLE mhDepthMapCpuSrv;     // 深度图 也要进行读取（SRV） 
    CD3DX12_GPU_DESCRIPTOR_HANDLE mhDepthMapGpuSrv;     


    CD3DX12_CPU_DESCRIPTOR_HANDLE mhRandomVectorMapCpuSrv;
    CD3DX12_GPU_DESCRIPTOR_HANDLE mhRandomVectorMapGpuSrv;

    // Need two for ping-ponging during blur.
    CD3DX12_CPU_DESCRIPTOR_HANDLE mhAmbientMap0CpuSrv;
    CD3DX12_GPU_DESCRIPTOR_HANDLE mhAmbientMap0GpuSrv;
    CD3DX12_CPU_DESCRIPTOR_HANDLE mhAmbientMap0CpuRtv;

    CD3DX12_CPU_DESCRIPTOR_HANDLE mhAmbientMap1CpuSrv;
    CD3DX12_GPU_DESCRIPTOR_HANDLE mhAmbientMap1GpuSrv;
    CD3DX12_CPU_DESCRIPTOR_HANDLE mhAmbientMap1CpuRtv;

	UINT mRenderTargetWidth;
	UINT mRenderTargetHeight;

    DirectX::XMFLOAT4 mOffsets[14];     // 随机向量

	D3D12_VIEWPORT mViewport;
	D3D12_RECT mScissorRect;
};

#endif // SSAO_H