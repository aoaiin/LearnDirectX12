//***************************************************************************************
// BlurFilter.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Performs a blur operation on the topmost mip level of an input texture.
//***************************************************************************************

#pragma once

#include "d3dUtil.h"

class BlurFilter
{
public:
	///<summary>
	/// The width and height should match the dimensions of the input texture to blur.
	/// Recreate when the screen is resized. 
	///</summary>
	BlurFilter(ID3D12Device* device, 
		UINT width, UINT height,
		DXGI_FORMAT format);
		
	BlurFilter(const BlurFilter& rhs)=delete;
	BlurFilter& operator=(const BlurFilter& rhs)=delete;
	~BlurFilter()=default;

	ID3D12Resource* Output();	// 输出模糊后的 资源

	// 重载2：创建描述符：在描述符堆中位置，然后调用重载1
	void BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDescriptor, 
		CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuDescriptor,
		UINT descriptorSize);

	void OnResize(UINT newWidth, UINT newHeight);

	///<summary>
	/// Blurs the input texture blurCount times.
	///</summary>
	void Execute(
		ID3D12GraphicsCommandList* cmdList, 
		ID3D12RootSignature* rootSig,
		ID3D12PipelineState* horzBlurPSO,
		ID3D12PipelineState* vertBlurPSO,
		ID3D12Resource* input, 		
		int blurCount);

private:
	std::vector<float> CalcGaussWeights(float sigma);

	void BuildDescriptors();// 重载1：创建 描述符
	void BuildResources();	// 创建纹理资源 CPU->GPU显存

private:

	const int MaxBlurRadius = 5;

	ID3D12Device* md3dDevice = nullptr;

	UINT mWidth = 0;
	UINT mHeight = 0;
	DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 两张纹理 分别有GPU/CPU，SRV/UAV的描述符
	CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur0CpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur0CpuUav;

	CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur0GpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur0GpuUav;


	CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur1CpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mBlur1CpuUav;

	CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur1GpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mBlur1GpuUav;

	// Two for ping-ponging the textures.
	// 模糊操作需要两张纹理
	Microsoft::WRL::ComPtr<ID3D12Resource> mBlurMap0 = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> mBlurMap1 = nullptr;
};
