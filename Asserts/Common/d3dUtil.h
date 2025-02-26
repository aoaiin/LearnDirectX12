//***************************************************************************************
// d3dUtil.h by Frank Luna (C) 2015 All Rights Reserved.
//
// General helper code.
//***************************************************************************************

#pragma once

#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>
#include "d3dx12.h"
#include "DDSTextureLoader.h"
#include "MathHelper.h"

extern const int gNumFrameResources;

inline void d3dSetDebugName(IDXGIObject* obj, const char* name)
{
    if(obj)
    {
        obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
    }
}
inline void d3dSetDebugName(ID3D12Device* obj, const char* name)
{
    if(obj)
    {
        obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
    }
}
inline void d3dSetDebugName(ID3D12DeviceChild* obj, const char* name)
{
    if(obj)
    {
        obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
    }
}

inline std::wstring AnsiToWString(const std::string& str)
{
    WCHAR buffer[512];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
    return std::wstring(buffer);
}

/*
#if defined(_DEBUG)
    #ifndef Assert
    #define Assert(x, description)                                  \
    {                                                               \
        static bool ignoreAssert = false;                           \
        if(!ignoreAssert && !(x))                                   \
        {                                                           \
            Debug::AssertResult result = Debug::ShowAssertDialog(   \
            (L#x), description, AnsiToWString(__FILE__), __LINE__); \
        if(result == Debug::AssertIgnore)                           \
        {                                                           \
            ignoreAssert = true;                                    \
        }                                                           \
                    else if(result == Debug::AssertBreak)           \
        {                                                           \
            __debugbreak();                                         \
        }                                                           \
        }                                                           \
    }
    #endif
#else
    #ifndef Assert
    #define Assert(x, description) 
    #endif
#endif 		
    */

class d3dUtil
{
public:

    static bool IsKeyDown(int vkeyCode);

    static std::string ToString(HRESULT hr);

    static UINT CalcConstantBufferByteSize(UINT byteSize)
    {
        // Constant buffers must be a multiple of the minimum hardware
        // allocation size (usually 256 bytes).  So round up to nearest
        // multiple of 256.  We do this by adding 255 and then masking off
        // the lower 2 bytes which store all bits < 256.
        // Example: Suppose byteSize = 300.
        // (300 + 255) & ~255
        // 555 & ~255
        // 0x022B & ~0x00ff
        // 0x022B & 0xff00
        // 0x0200
        // 512
        return (byteSize + 255) & ~255;
    }

    static Microsoft::WRL::ComPtr<ID3DBlob> LoadBinary(const std::wstring& filename);

    static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
        ID3D12Device* device,
        ID3D12GraphicsCommandList* cmdList,
        const void* initData,
        UINT64 byteSize,
        Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

    // 对 D3DCompileFromFile 的封装
	static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(  
		const std::wstring& filename,
		const D3D_SHADER_MACRO* defines,
		const std::string& entrypoint,
		const std::string& target);
};

class DxException
{
public:
    DxException() = default;
    DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

    std::wstring ToString()const;

    HRESULT ErrorCode = S_OK;
    std::wstring FunctionName;
    std::wstring Filename;
    int LineNumber = -1;
};

// Defines a subrange of geometry in a MeshGeometry.  This is for when multiple
// geometries are stored in one vertex and index buffer.  It provides the offsets
// and data needed to draw a subset of geometry stores in the vertex and index 
// buffers so that we can implement the technique described by Figure 6.3.
// 在MeshGeometry中定义了一个几何体的子物体。这用于混合物体（多个物体的局部缓冲区 组合为一个全局）的情况。
struct SubmeshGeometry
{
	UINT IndexCount = 0;// 索引数量
	UINT StartIndexLocation = 0;// 基准索引位置
	INT BaseVertexLocation = 0;// 基准顶点位置

    // Bounding box of the geometry defined by this submesh. 
    // This is used in later chapters of the book.
	DirectX::BoundingBox Bounds;
};

struct MeshGeometry
{
	// Give it a name so we can look it up by name.
	std::string Name;

	// System memory copies.  Use Blobs because the vertex/index format can be generic.
	// It is up to the client to cast appropriately.  
    // 系统变量内存的 (复制样本) 。我们使用Blob因为vertex/index可以用自定义结构体声明
    // **CPU系统内存上的 顶点/索引 数据
    // ** 一般作为 副本，可以看到下面 View取地址，取的都是GPU端的缓冲区（因为要具体执行）
	Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;     
	Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU  = nullptr;

    // GPU 默认堆中 的顶点/索引 缓冲区资源
	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;   //GPU默认堆中的 顶点缓冲区（最终的GPU顶点缓冲区）
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;    //GPU默认堆中的 索引缓冲区（最终的GPU索引缓冲区）

    // GPU 上传堆中 的顶点/索引 缓冲区资源
	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

    // Data about the buffers.// 缓冲区相关数据
	UINT VertexByteStride = 0;
	UINT VertexBufferByteSize = 0;
	DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
	UINT IndexBufferByteSize = 0;


    // 习题2：两个顶点缓冲区 传递数据
    Microsoft::WRL::ComPtr<ID3DBlob> vPosBufferCpu = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> vColorBufferCpu = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> vPosBufferUploader = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> vColorBufferUploader = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> vPosBufferGpu = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> vColorBufferGpu = nullptr;

    UINT vPosByteStride = 0;
    UINT vColorByteStride = 0;
    UINT vPosBufferByteSize = 0;
    UINT vColorBufferByteSize = 0;



	// A MeshGeometry may store multiple geometries in one vertex/index buffer.
	// Use this container to define the Submesh geometries so we can draw
	// the Submeshes individually.
    // 一个MeshGeomotry可以用全局顶点/索引缓冲区中存储多个几何体；
    // 使用这个容器来定义Submesh的几何图形，我们就可以利用submesh的name来查找它进行单独绘制
	std::unordered_map<std::string, SubmeshGeometry> DrawArgs;

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
	{
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();// 顶点缓冲区资源虚拟地址
		vbv.StrideInBytes = VertexByteStride;   // 顶点缓冲区大小（所有顶点数据大小）
		vbv.SizeInBytes = VertexBufferByteSize; // 每个顶点元素占用的字节数

		return vbv;
	}
    D3D12_VERTEX_BUFFER_VIEW VertexPosBufferView()const
    {
        D3D12_VERTEX_BUFFER_VIEW vbv;
        vbv.BufferLocation = vPosBufferGpu->GetGPUVirtualAddress();
        vbv.StrideInBytes = vPosByteStride;
        vbv.SizeInBytes = vPosBufferByteSize;
        return vbv;
    }
    D3D12_VERTEX_BUFFER_VIEW VertexColorBufferView()const
    {
        D3D12_VERTEX_BUFFER_VIEW vbv;
        vbv.BufferLocation = vColorBufferGpu->GetGPUVirtualAddress();
        vbv.StrideInBytes = vColorByteStride;
        vbv.SizeInBytes = vColorBufferByteSize;
        return vbv;
    }


	D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
	{
		D3D12_INDEX_BUFFER_VIEW ibv;
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = IndexBufferByteSize;

		return ibv;
	}

	// We can free this memory after we finish upload to the GPU.  在将数据上传到GPU后释放掉上传堆的内存
    // 等上传堆资源传至默认堆后,释放上传堆里的内存
	void DisposeUploaders()
	{
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}
};

struct Light    // 各种光源的属性
{   
    DirectX::XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };
    float FalloffStart = 1.0f;                          // point/spot light only
    DirectX::XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };// directional/spot light only
    float FalloffEnd = 10.0f;                           // point/spot light only
    DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };  // point/spot light only
    float SpotPower = 64.0f;                            // spot light only
};

#define MaxLights 16

struct MaterialConstants    // 材质属性（从 材质中提取出来 :传入constantBuffer的数据 ）
{
	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };   // 漫反射/材质 的 颜色/反射率
	DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };    // 菲涅尔
	float Roughness = 0.25f;    // 粗糙度

	// Used in texture mapping.
    // 用于 物体的材质变化（实际上就是uv贴图变换）
    // ObjectConstants 中的 TexTrans ：是将贴图 贴到物体上的变换
    // 这个 Mat 是 **对贴图本身的变化/不同的采样** （更新材质的纹理 本身）
	DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
};

// Simple struct to represent a material for our demos.  A production 3D engine
// would likely create a class hierarchy of Materials.
struct Material
{
	// Unique material name for lookup.
	std::string Name;

	// Index into constant buffer corresponding to this material.
	int MatCBIndex = -1;

	// Index into SRV heap for diffuse **texture**.  之后加载了 Texture，就是纹理的索引（纹理描述符SRV在堆中的索引）
	int DiffuseSrvHeapIndex = -1;

	// Index into SRV heap for normal texture.
	int NormalSrvHeapIndex = -1;

	// Dirty flag indicating the material has changed and we need to update the constant buffer.
	// Because we have a material constant buffer for each FrameResource, we have to apply the
	// update to each FrameResource.  Thus, when we modify a material we should set 
	// NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
	int NumFramesDirty = gNumFrameResources;


	// Material constant buffer data used for shading.  
    // 将 这些属性提取出来，放到MaterialConstants中，用于着色
	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = .25f;
	DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();   // 材质 的变换矩阵（物体的）
};

struct Texture
{
	// Unique material name for lookup.
	std::string Name;   // 纹理的名称

	std::wstring Filename;  // 纹理所在路径的目录名

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;  // 纹理最后能让GPU访问的资源（资源=存储空间）
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;// 上传堆
};

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif