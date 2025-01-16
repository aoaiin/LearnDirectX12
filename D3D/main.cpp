#include "D3D12App.h"
#include "Timer.h"
#include "ToolFunc.h"
#include<iostream>
using namespace std;

//HWND mhMainWnd = 0;	//ĳ�����ڵľ����ShowWindow��UpdateWindow������Ҫ���ô˾��

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam);
//���ڹ��̺���
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//��Ϣ����
	switch (msg)
	{
		//�����ڱ�����ʱ����ֹ��Ϣѭ��
	case WM_DESTROY:
		PostQuitMessage(0);	//��ֹ��Ϣѭ����������WM_QUIT��Ϣ
		return 0;
	default:
		break;
	}
	//������û�д������Ϣת����Ĭ�ϵĴ��ڹ���
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//���ڹ��̺���������,HWND�������ھ��
//LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam);
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

class D3D12InitApp : public D3D12App
{
public:
	D3D12InitApp();
	~D3D12InitApp();
private:
	virtual void Draw()override;
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
	//__super::Draw();
}
