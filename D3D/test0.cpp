//#include <Windows.h>
//#include <wrl.h>
//#include <dxgi1_4.h>
//#include <d3d12.h>
//#include <d3dcompiler.h>
//#include <DirectXMath.h>
//#include <DirectXPackedVector.h>
//#include <DirectXColors.h>
//#include <DirectXCollision.h>
//#include <string>
//#include <memory>
//#include <algorithm>
//#include <vector>
//#include <array>
//#include <unordered_map>
//#include <cstdint>
//#include <fstream>
//#include <sstream>
//#include <windowsx.h>
////#include "F:\MyDX12WorkSpace\DX12Initialize\common\d3dx12.h"
//#include <comdef.h>
//
//using namespace Microsoft::WRL;
//
//HWND mhMainWnd = 0;	//ĳ�����ڵľ����ShowWindow��UpdateWindow������Ҫ���ô˾��
////���ڹ��̺���������,HWND�������ھ��
//LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam);
//
////int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nCmdShow) {
////
////}
//
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nShowCmd)
//{
//#if defined(DEBUG) | defined(_DEBUG)
//	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//#endif
//	//���ڳ�ʼ�������ṹ��(WNDCLASS)
//	WNDCLASS wc;
//	wc.style = CS_HREDRAW | CS_VREDRAW;	//����������߸ı䣬�����»��ƴ���
//	wc.lpfnWndProc = MainWndProc;	//ָ�����ڹ���
//	wc.cbClsExtra = 0;	//�����������ֶ���Ϊ��ǰӦ�÷��������ڴ�ռ䣨���ﲻ���䣬������0��
//	wc.cbWndExtra = 0;	//�����������ֶ���Ϊ��ǰӦ�÷��������ڴ�ռ䣨���ﲻ���䣬������0��
//	wc.hInstance = hInstance;	//Ӧ�ó���ʵ���������WinMain���룩
//	wc.hIcon = LoadIcon(0, IDC_ARROW);	//ʹ��Ĭ�ϵ�Ӧ�ó���ͼ��
//	wc.hCursor = LoadCursor(0, IDC_ARROW);	//ʹ�ñ�׼�����ָ����ʽ
//	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);	//ָ���˰�ɫ������ˢ���
//	wc.lpszMenuName = 0;	//û�в˵���
//	wc.lpszClassName = L"MainWnd";	//������
//	//������ע��ʧ��
//	if (!RegisterClass(&wc))
//	{
//		//��Ϣ����������1����Ϣ���������ھ������ΪNULL������2����Ϣ����ʾ���ı���Ϣ������3�������ı�������4����Ϣ����ʽ
//		MessageBox(0, L"RegisterClass Failed", 0, 0);
//		return 0;
//	}
//
//	//������ע��ɹ�
//	RECT R;	//�ü�����
//	R.left = 0;
//	R.top = 0;
//	R.right = 1280;
//	R.bottom = 720;
//	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);	//���ݴ��ڵĿͻ�����С���㴰�ڵĴ�С
//	int width = R.right - R.left;
//	int hight = R.bottom - R.top;
//
//	//��������,���ز���ֵ
//	mhMainWnd = CreateWindow(L"MainWnd", L"DX12Initialize", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, hight, 0, 0, hInstance, 0);
//	//���ڴ���ʧ��
//	if (!mhMainWnd)
//	{
//		MessageBox(0, L"CreatWindow Failed", 0, 0);
//		return 0;
//	}
//	//���ڴ����ɹ�,����ʾ�����´���
//	ShowWindow(mhMainWnd, nShowCmd);
//	UpdateWindow(mhMainWnd);
//
//
//
//	//��Ϣѭ��
////������Ϣ�ṹ��
//	MSG msg = { 0 };
//	BOOL bRet = 0;
//	//���GetMessage����������0��˵��û�н��ܵ�WM_QUIT
//	while (bRet = GetMessage(&msg, 0, 0, 0) != 0)
//	{
//		//�������-1��˵��GetMessage���������ˣ����������
//		if (bRet == -1)
//		{
//			MessageBox(0, L"GetMessage Failed", L"Errow", MB_OK);
//		}
//		//�����������ֵ��˵�����յ�����Ϣ
//		else
//		{
//			TranslateMessage(&msg);	//���̰���ת�������������Ϣת��Ϊ�ַ���Ϣ
//			DispatchMessage(&msg);	//����Ϣ���ɸ���Ӧ�Ĵ��ڹ���
//		}
//	}
//	return (int)msg.wParam;
//
//	//return 0;
//}
//
////���ڹ��̺���
//LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	//��Ϣ����
//	switch (msg)
//	{
//		//�����ڱ�����ʱ����ֹ��Ϣѭ��
//	case WM_DESTROY:
//		PostQuitMessage(0);	//��ֹ��Ϣѭ����������WM_QUIT��Ϣ
//		return 0;
//	default:
//		break;
//	}
//	//������û�д������Ϣת����Ĭ�ϵĴ��ڹ���
//	return DefWindowProc(hwnd, msg, wParam, lParam);
//}
//
//
////AnsiToWString������ת���ɿ��ַ����͵��ַ�����wstring��
////��Windowsƽ̨�ϣ�����Ӧ�ö�ʹ��wstring��wchar_t������ʽ�����ַ���ǰ+L
//inline std::wstring AnsiToWString(const std::string& str)
//{
//	WCHAR buffer[512];
//	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
//	return std::wstring(buffer);
//}
////DxException��
//class DxException
//{
//public:
//	DxException() = default;
//	DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);
//
//	std::wstring ToString()const;
//
//	HRESULT ErrorCode = S_OK;
//	std::wstring FunctionName;
//	std::wstring Filename;
//	int LineNumber = -1;
//};
//DxException::DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber) :
//	ErrorCode(hr),
//	FunctionName(functionName),
//	Filename(filename),
//	LineNumber(lineNumber)
//{
//}
//
//std::wstring DxException::ToString()const
//{
//	// Get the string description of the error code.
//	_com_error err(ErrorCode);
//	std::wstring msg = err.ErrorMessage();
//
//	return FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; error: " + msg;
//}
//
////�궨��ThrowIfFailed
//#ifndef ThrowIfFailed
//#define ThrowIfFailed(x)                                              \
//{                                                                     \
//    HRESULT hr__ = (x);                                               \
//    std::wstring wfn = AnsiToWString(__FILE__);                       \
//    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
//}
//#endif
//
//
//void CreateDevice()
//{
//	ComPtr<IDXGIFactory4> dxgiFactory;
//	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)));
//	ComPtr<ID3D12Device> d3dDevice;
//	ThrowIfFailed(D3D12CreateDevice(nullptr, //�˲����������Ϊnullptr����ʹ����������
//		D3D_FEATURE_LEVEL_12_0,		//Ӧ�ó�����ҪӲ����֧�ֵ���͹��ܼ���
//		IID_PPV_ARGS(&d3dDevice)));	//���������豸
//}
//void CreateFence()
//{
//	ComPtr<ID3D12Fence> fence;
//	ThrowIfFailed(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
//}
//
//void GetDescriptorSize()
//{
//	UINT rtvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//	UINT dsvDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
//	UINT cbv_srv_uavDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
//}
//void SetMSAA()
//{
//	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msaaQualityLevels;
//	msaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//UNORM�ǹ�һ��������޷�������
//	msaaQualityLevels.SampleCount = 1;
//	msaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
//	msaaQualityLevels.NumQualityLevels = 0;
//	//��ǰͼ��������MSAA���ز�����֧�֣�ע�⣺�ڶ������������������������
//	ThrowIfFailed(d3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msaaQualityLevels, sizeof(msaaQualityLevels)));
//	//NumQualityLevels��Check��������������
//	//���֧��MSAA����Check�������ص�NumQualityLevels > 0
//	//expressionΪ�٣���Ϊ0��������ֹ�������У�����ӡһ��������Ϣ
//	assert(msaaQualityLevels.NumQualityLevels > 0);
//}
//void CreateCommandObject()
//{
//	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
//	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
//	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
//	ComPtr<ID3D12CommandQueue> cmdQueue;
//	ThrowIfFailed(d3dDevice->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&cmdQueue)));
//	ComPtr<ID3D12CommandAllocator> cmdAllocator;
//	ThrowIfFailed(d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllocator)));//&cmdAllocator�ȼ���cmdAllocator.GetAddressOf
//	ComPtr<ID3D12GraphicsCommandList> cmdList;	//ע��˴��Ľӿ�����ID3D12GraphicsCommandList��������ID3D12CommandList
//	ThrowIfFailed(d3dDevice->CreateCommandList(0, //����ֵΪ0����GPU
//		D3D12_COMMAND_LIST_TYPE_DIRECT, //�����б�����
//		cmdAllocator.Get(),	//����������ӿ�ָ��
//		nullptr,	//��ˮ��״̬����PSO�����ﲻ���ƣ����Կ�ָ��
//		IID_PPV_ARGS(&cmdList)));	//���ش����������б�
//	cmdList->Close();	//���������б�ǰ���뽫��ر�
//}
//
//void CreateSwapChain()
//{
//	ComPtr<IDXGISwapChain> swapChain;
//	swapChain.Reset();
//	DXGI_SWAP_CHAIN_DESC swapChainDesc;	//�����������ṹ��
//	swapChainDesc.BufferDesc.Width = 1280;	//�������ֱ��ʵĿ��
//	swapChainDesc.BufferDesc.Height = 720;	//�������ֱ��ʵĸ߶�
//	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	//����������ʾ��ʽ
//	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;	//ˢ���ʵķ���
//	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;	//ˢ���ʵķ�ĸ
//	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;	//����ɨ��VS����ɨ��(δָ����)
//	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	//ͼ�������Ļ�����죨δָ���ģ�
//	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	//��������Ⱦ����̨������������Ϊ��ȾĿ�꣩
//	swapChainDesc.OutputWindow = mhMainWnd;	//��Ⱦ���ھ��
//	swapChainDesc.SampleDesc.Count = 1;	//���ز�������
//	swapChainDesc.SampleDesc.Quality = 0;	//���ز�������
//	swapChainDesc.Windowed = true;	//�Ƿ񴰿ڻ�
//	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;	//�̶�д��
//	swapChainDesc.BufferCount = 2;	//��̨������������˫���壩
//	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;	//����Ӧ����ģʽ���Զ�ѡ�������ڵ�ǰ���ڳߴ����ʾģʽ��
//	//����DXGI�ӿ��µĹ����ഴ��������
//	ThrowIfFailed(dxgiFactory->CreateSwapChain(cmdQueue.Get(), &swapChainDesc, swapChain.GetAddressOf()));
//}
//
//void CreateDescriptorHeap()
//{
//	//���ȴ���RTV��
//	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc;
//	rtvDescriptorHeapDesc.NumDescriptors = 2;
//	rtvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
//	rtvDescriptorHeapDesc.NodeMask = 0;
//	ComPtr<ID3D12DescriptorHeap> rtvHeap;
//	ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&rtvHeap)));
//	//Ȼ�󴴽�DSV��
//	D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeapDesc;
//	dsvDescriptorHeapDesc.NumDescriptors = 1;
//	dsvDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//	dsvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
//	dsvDescriptorHeapDesc.NodeMask = 0;
//	ComPtr<ID3D12DescriptorHeap> dsvHeap;
//	ThrowIfFailed(d3dDevice->CreateDescriptorHeap(&dsvDescriptorHeapDesc, IID_PPV_ARGS(&dsvHeap)));
//}
//
//
//void CreateRTV()
//{
//	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
//	ComPtr<ID3D12Resource> swapChainBuffer[2];
//	for (int i = 0; i < 2; i++)
//	{
//		//��ô��ڽ������еĺ�̨��������Դ
//		swapChain->GetBuffer(i, IID_PPV_ARGS(swapChainBuffer[i].GetAddressOf()));
//		//����RTV
//		d3dDevice->CreateRenderTargetView(swapChainBuffer[i].Get(),
//			nullptr,	//�ڽ������������Ѿ������˸���Դ�����ݸ�ʽ����������ָ��Ϊ��ָ��
//			rtvHeapHandle);	//����������ṹ�壨�����Ǳ��壬�̳���CD3DX12_CPU_DESCRIPTOR_HANDLE��
//		//ƫ�Ƶ����������е���һ��������
//		rtvHeapHandle.Offset(1, rtvDescriptorSize);
//	}
//}
//
//void CreateDSV()
//{
//	//��CPU�д��������ģ��������Դ
//	D3D12_RESOURCE_DESC dsvResourceDesc;
//	dsvResourceDesc.Alignment = 0;	//ָ������
//	dsvResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;	//ָ����Դά�ȣ����ͣ�ΪTEXTURE2D
//	dsvResourceDesc.DepthOrArraySize = 1;	//�������Ϊ1
//	dsvResourceDesc.Width = 1280;	//��Դ��
//	dsvResourceDesc.Height = 720;	//��Դ��
//	dsvResourceDesc.MipLevels = 1;	//MIPMAP�㼶����
//	dsvResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;	//ָ�������֣����ﲻָ����
//	dsvResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;	//���ģ����Դ��Flag
//	dsvResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	//24λ��ȣ�8λģ��,���и������͵ĸ�ʽDXGI_FORMAT_R24G8_TYPELESSҲ����ʹ��
//	dsvResourceDesc.SampleDesc.Count = 4;	//���ز�������
//	dsvResourceDesc.SampleDesc.Quality = msaaQualityLevels.NumQualityLevels - 1;	//���ز�������
//	CD3DX12_CLEAR_VALUE optClear;	//�����Դ���Ż�ֵ��������������ִ���ٶȣ�CreateCommittedResource�����д��룩
//	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//24λ��ȣ�8λģ��,���и������͵ĸ�ʽDXGI_FORMAT_R24G8_TYPELESSҲ����ʹ��
//	optClear.DepthStencil.Depth = 1;	//��ʼ���ֵΪ1
//	optClear.DepthStencil.Stencil = 0;	//��ʼģ��ֵΪ0
//	//����һ����Դ��һ���ѣ�������Դ�ύ�����У������ģ�������ύ��GPU�Դ��У�
//	ComPtr<ID3D12Resource> depthStencilBuffer;
//	ThrowIfFailed(d3dDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),	//������ΪĬ�϶ѣ�����д�룩
//		D3D12_HEAP_FLAG_NONE,	//Flag
//		&dsvResourceDesc,	//���涨���DSV��Դָ��
//		D3D12_RESOURCE_STATE_COMMON,	//��Դ��״̬Ϊ��ʼ״̬
//		&optClear,	//���涨����Ż�ֵָ��
//		IID_PPV_ARGS(&depthStencilBuffer)));	//�������ģ����Դ
//	//����DSV(�������DSV���Խṹ�壬�ʹ���RTV��ͬ��RTV��ͨ�����)
//	//D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
//	//dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
//	//dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
//	//dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
//	//dsvDesc.Texture2D.MipSlice = 0;
//	d3dDevice->CreateDepthStencilView(depthStencilBuffer.Get(),
//		nullptr,	//D3D12_DEPTH_STENCIL_VIEW_DESC����ָ�룬����&dsvDesc������ע�ʹ��룩��
//		//�����ڴ������ģ����Դʱ�Ѿ��������ģ���������ԣ������������ָ��Ϊ��ָ��
//		dsvHeap->GetCPUDescriptorHandleForHeapStart());	//DSV���
//}