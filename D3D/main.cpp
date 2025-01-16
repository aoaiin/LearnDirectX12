#include "D3D12App.h"
#include "Timer.h"
#include "ToolFunc.h"
#include<iostream>
using namespace std;

//HWND mhMainWnd = 0;	//某个窗口的句柄，ShowWindow和UpdateWindow函数均要调用此句柄

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam);
//窗口过程函数
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//消息处理
	switch (msg)
	{
		//当窗口被销毁时，终止消息循环
	case WM_DESTROY:
		PostQuitMessage(0);	//终止消息循环，并发出WM_QUIT消息
		return 0;
	default:
		break;
	}
	//将上面没有处理的消息转发给默认的窗口过程
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//窗口过程函数的声明,HWND是主窗口句柄
//LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lparam);
//
////窗口过程函数
//LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	//消息处理
//	switch (msg)
//	{
//		//当窗口被销毁时，终止消息循环
//	case WM_DESTROY:
//		PostQuitMessage(0);	//终止消息循环，并发出WM_QUIT消息
//		return 0;
//	default:
//		break;
//	}
//	//将上面没有处理的消息转发给默认的窗口过程
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
