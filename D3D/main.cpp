#include "D3D12App.h"
#include "Timer.h"
#include "ToolFunc.h"
#include<iostream>

using namespace std;

using namespace DirectX;

//HWND mhMainWnd = 0;	//ĳ�����ڵľ����ShowWindow��UpdateWindow������Ҫ���ô˾��

class D3D12InitApp : public D3D12App
{
public:
	D3D12InitApp();
	~D3D12InitApp();

	// �������ݴ��䣺�ϴ��� -> Ĭ�϶�
	// �ο� d3dUtil.h �е� CreateDefaultBuffer ����
	//	ҪCPU�ϵĶ������ݸ��Ƶ�GPU�ϣ����Ǳ���
	//  1. �ȴ���һ���ϴ��ѣ���CPUд�����ݣ�
	//	2. Ȼ�󴴽�һ��Ĭ�϶ѣ����ϴ����еĶ������ݴ���Ĭ�϶ѣ���Ĭ�϶��е�������ֻ��GPU���ʵģ����ھ�̬������˵�����������Ż���һ���ֶΡ�
	//	������������Ҫ�õ����������Ĭ�϶Ѵ�Ŷ������ݡ�
	ComPtr<ID3D12Resource> CreateDefaultBuffer(UINT64 byteSize, const void* initData, ComPtr<ID3D12Resource>& uploadBuffer);

private:
	virtual void Draw()override;

	ComPtr<ID3D12Resource> defaultBuffer;
};


//���嶥��ṹ��
struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};
//ʵ��������ṹ�岢���
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
	// ԭ��һֱ ���Բ���
	// ԭ�� C++û��super��ֻ�� ֱ���� ����::
	D3D12App::Draw();
}

ComPtr<ID3D12Resource> D3D12InitApp::CreateDefaultBuffer(UINT64 byteSize, const void* initData, ComPtr<ID3D12Resource>& uploadBuffer)
{
	//1. �����ϴ��ѣ������ǣ�д��CPU�ڴ����ݣ��������Ĭ�϶�
	ThrowIfFailed(d3dDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), //�����ϴ������͵Ķ�
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),//����Ĺ��캯��������byteSize��������ΪĬ��ֵ������д
		D3D12_RESOURCE_STATE_GENERIC_READ,	//�ϴ��������Դ��Ҫ���Ƹ�Ĭ�϶ѣ������ǿɶ�״̬
		nullptr,	//�������ģ����Դ������ָ���Ż�ֵ
		IID_PPV_ARGS(&uploadBuffer)));

	//2. ����Ĭ�϶ѣ���Ϊ�ϴ��ѵ����ݴ������
	//ComPtr<ID3D12Resource> defaultBuffer;
	ThrowIfFailed(d3dDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),//����Ĭ�϶����͵Ķ�
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(byteSize),
		D3D12_RESOURCE_STATE_COMMON,//Ĭ�϶�Ϊ���մ洢���ݵĵط���������ʱ��ʼ��Ϊ��ͨ״̬
		nullptr,
		IID_PPV_ARGS(&defaultBuffer)));

	//3. ת��״̬������Դ��COMMON״̬ת����COPY_DEST״̬��Ĭ�϶Ѵ�ʱ��Ϊ�������ݵ�Ŀ�꣩
	cmdList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON,
			D3D12_RESOURCE_STATE_COPY_DEST));

	//4. �����ݴ�CPU�ڴ濽����GPU����
	D3D12_SUBRESOURCE_DATA subResourceData;
	subResourceData.pData = initData;
	subResourceData.RowPitch = byteSize;
	subResourceData.SlicePitch = subResourceData.RowPitch;
	//���ĺ���UpdateSubresources�������ݴ�CPU�ڴ濽�����ϴ��ѣ��ٴ��ϴ��ѿ�����Ĭ�϶ѡ�1����������Դ���±꣨ģ���ж��壬��Ϊ��2������Դ��
	UpdateSubresources<1>(cmdList.Get(), defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);

	//5. ת��״̬���ٴν���Դ��COPY_DEST״̬ת����GENERIC_READ״̬(����ֻ�ṩ����ɫ������)
	cmdList->ResourceBarrier(1,
		&CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_GENERIC_READ));

	return defaultBuffer;
}
