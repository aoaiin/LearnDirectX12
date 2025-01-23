#include "D3D12App.h"
#include "Timer.h"
#include "ToolFunc.h"
#include<iostream>

using namespace std;

using namespace DirectX;

//HWND mhMainWnd = 0;	//某个窗口的句柄，ShowWindow和UpdateWindow函数均要调用此句柄

class D3D12InitApp : public D3D12App
{
public:
	D3D12InitApp();
	~D3D12InitApp();

	// 顶点数据传输：上传堆 -> 默认堆
	// 参考 d3dUtil.h 中的 CreateDefaultBuffer 函数
	//	要CPU上的顶点数据复制到GPU上，我们必须
	//  1. 先创建一个上传堆，让CPU写入数据，
	//	2. 然后创建一个默认堆，将上传堆中的顶点数据传至默认堆，而默认堆中的数据是只能GPU访问的，对于静态物体来说，这是性能优化的一种手段。
	//	而最终我们需要拿到这个创建的默认堆存放顶点数据。
	ComPtr<ID3D12Resource> CreateDefaultBuffer(UINT64 byteSize, const void* initData, ComPtr<ID3D12Resource>& uploadBuffer);

private:
	virtual void Draw()override;

	ComPtr<ID3D12Resource> defaultBuffer;
};


//定义顶点结构体
struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};
//实例化顶点结构体并填充
std::array<Vertex, 8> vertices =
{
	Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
	Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) }),
	Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) }),
	Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) }),
	Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) }),
	Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }),
	Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) }),
	Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) })
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nShowCmd)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	try
	{
		D3D12InitApp theApp;
		if (!theApp.Init(hInstance, nShowCmd))
			return 0;

		return theApp.Run();
	}
	catch (DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
}



D3D12InitApp::D3D12InitApp()
{
}

D3D12InitApp::~D3D12InitApp()
{
}

void D3D12InitApp::Draw()
{
	// 原来一直 调试不好
	// 原来 C++没有super，只能 直接用 父类::
	D3D12App::Draw();
}

ComPtr<ID3D12Resource> D3D12InitApp::CreateDefaultBuffer(UINT64 byteSize, const void* initData, ComPtr<ID3D12Resource>& uploadBuffer)
{
	//1. 创建上传堆，作用是：写入CPU内存数据，并传输给默认堆
	ThrowIfFailed(d3dDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //创建上传堆类型的堆
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),//变体的构造函数，传入byteSize，其他均为默认值，简化书写
		D3D12_RESOURCE_STATE_GENERIC_READ,	//上传堆里的资源需要复制给默认堆，所以是可读状态
		nullptr,	//不是深度模板资源，不用指定优化值
		IID_PPV_ARGS(&uploadBuffer)));

	//2. 创建默认堆，作为上传堆的数据传输对象
	//ComPtr<ID3D12Resource> defaultBuffer;
	ThrowIfFailed(d3dDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),//创建默认堆类型的堆
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_COMMON,//默认堆为最终存储数据的地方，所以暂时初始化为普通状态
		nullptr,
		IID_PPV_ARGS(&defaultBuffer)));

	//3. 转换状态：将资源从COMMON状态转换到COPY_DEST状态（默认堆此时作为接收数据的目标）
	cmdList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_COPY_DEST));

	//4. 将数据从CPU内存拷贝到GPU缓存
	D3D12_SUBRESOURCE_DATA subResourceData;
	subResourceData.pData = initData;
	subResourceData.RowPitch = byteSize;
	subResourceData.SlicePitch = subResourceData.RowPitch;
	//核心函数UpdateSubresources，将数据从CPU内存拷贝至上传堆，再从上传堆拷贝至默认堆。1是最大的子资源的下标（模板中定义，意为有2个子资源）
	UpdateSubresources<1>(cmdList.Get(), defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);

	//5. 转换状态：再次将资源从COPY_DEST状态转换到GENERIC_READ状态(现在只提供给着色器访问)
	cmdList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_GENERIC_READ));

	return defaultBuffer;
}
