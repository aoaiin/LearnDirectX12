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
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int nShowCmd)
//{
//#if defined(DEBUG) | defined(_DEBUG)
//    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//#endif
//
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
//
//	//��Ϣѭ��
//	//������Ϣ�ṹ��
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
//    //return 0;
//}
//
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
//bool InitWindow(HINSTANCE hInstance, int nShowCmd)
//{
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
//	//CreateWindowW(lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)
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
//	return true;
//}
//
//
//int Run()
//{
//	//��Ϣѭ��
//	//������Ϣ�ṹ��
//	MSG msg = { 0 };
//	//���GetMessage����������0��˵��û�н��ܵ�WM_QUIT
//	while (msg.message != WM_QUIT)
//	{
//		//����д�����Ϣ�ͽ��д���
//		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) //PeekMessage�������Զ����msg�ṹ��Ԫ��
//		{
//			TranslateMessage(&msg);	//���̰���ת�������������Ϣת��Ϊ�ַ���Ϣ
//			DispatchMessage(&msg);	//����Ϣ���ɸ���Ӧ�Ĵ��ڹ���
//		}
//		//�����ִ�ж�������Ϸ�߼�
//		else
//		{
//			Draw();
//		}
//	}
//	return (int)msg.wParam;
//}
//
////����ָ��ӿںͱ���
//ComPtr<ID3D12Device> d3dDevice;
//ComPtr<IDXGIFactory4> dxgiFactory;
//ComPtr<ID3D12Fence> fence;
//ComPtr<ID3D12CommandAllocator> cmdAllocator;
//ComPtr<ID3D12CommandQueue> cmdQueue;
//ComPtr<ID3D12GraphicsCommandList> cmdList;
//ComPtr<ID3D12Resource> depthStencilBuffer;
//ComPtr<ID3D12Resource> swapChainBuffer[2];
//ComPtr<IDXGISwapChain> swapChain;
//ComPtr<ID3D12DescriptorHeap> rtvHeap;
//ComPtr<ID3D12DescriptorHeap> dsvHeap;
//
//D3D12_VIEWPORT viewPort;
//D3D12_RECT scissorRect;
//UINT rtvDescriptorSize = 0;
//UINT dsvDescriptorSize = 0;
//UINT cbv_srv_uavDescriptorSize = 0;
//UINT mCurrentBackBuffer = 0;
//
//bool InitDirect3D()
//{
//	/*����D3D12���Բ�*/
//#if defined(DEBUG) || defined(_DEBUG)
//	{
//		ComPtr<ID3D12Debug> debugController;
//		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
//		debugController->EnableDebugLayer();
//	}
//#endif
//
//		CreateDevice();
//		CreateFence();
//		GetDescriptorSize();
//		SetMSAA();
//		CreateCommandObject();
//		CreateSwapChain();
//		CreateDescriptorHeap();
//		CreateRTV();
//		CreateDSV();
//		CreateViewPortAndScissorRect();
//
//		return true;
//}