//***************************************************************************************
// BoxApp.cpp by Frank Luna (C) 2015 All Rights Reserved.
//
// Shows how to draw a box in Direct3D 12.
//
// Controls:
//   Hold the left mouse button down and move the mouse to rotate.
//   Hold the right mouse button down and move the mouse to zoom in and out.
//***************************************************************************************

#include "d3dApp.h"
#include "MathHelper.h"
#include "UploadBuffer.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

//struct Vertex1 
//{
//    XMFLOAT3 Pos;
//    XMFLOAT3 Tangent;
//    XMFLOAT3 Normal;
//    XMFLOAT2 Tex0;
//    XMFLOAT2 Tex1;
//    XMCOLOR Color;
//
//};
//
//std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout =
//{
//    //{语义，语义索引 ， 格式， 输入槽，元素相对于结构体首地址的偏移， 实例化，实例化step}
//    // 偏移 ：float是4字节
//    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//    { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//    { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 52, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
//};



struct Vertex   // 顶点结构体: 还需要提供 输入布局描述
{
    XMFLOAT3 Pos;
    XMFLOAT4 Color;
};

struct VPosData
{
    XMFLOAT3 Pos;
};
struct VColorData
{
    //XMFLOAT4 Color;
    XMCOLOR Color;  //将颜色进度 减少到32位（用一个 int32 = 4* uint8）
};

struct ObjectConstants  //常量缓冲区 的 单个物体的常量数据
{
    XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
    float gTime = 0.0f;
    XMFLOAT4 gPulseColor;
};

class BoxApp : public D3DApp
{
public:
    BoxApp(HINSTANCE hInstance);
    BoxApp(const BoxApp& rhs) = delete;
    BoxApp& operator=(const BoxApp& rhs) = delete;
    ~BoxApp();

    virtual bool Initialize()override;

private:
    virtual void OnResize()override;    // 窗口宽高比改变时，后台缓冲区等等资源 需要重新设置
    virtual void Update(const GameTimer& gt)override;
    virtual void Draw(const GameTimer& gt)override;

    // d3d 中 窗口消息处理 中对应的 鼠标事件
    // wParam为输入的虚拟键代码，lParam为系统反馈的光标信息（被分为xy）
    virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
    virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
    virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

    void BuildDescriptorHeaps();	// 创建 描述符堆（给常量cbv） ： 创建描述符堆的描述，创建堆
    void BuildConstantBuffers();    // 创建 1. 常量缓冲区/上传堆  ；2. 常量缓冲区 视图（将描述符/视图 放入描述符堆）   （两个都创建了）
    void BuildRootSignature();      // 创建 根签名（ 将序列化的根签名描述（根参数） 与根签名绑定）
    void BuildShadersAndInputLayout();  // 1.编译着色器 ；2.对着色器 的 输入布局描述/输入参数说明
    void BuildBoxGeometry();        // 创建几何体：顶点、索引，
    void BuildPSO();    // 构建 PSO

private:

    ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
    ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;

    std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;

    std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;
    // 习题 7 ：多物体
    //std::unique_ptr<MeshGeometry> mBlendGeo = nullptr;

    ComPtr<ID3DBlob> mvsByteCode = nullptr;
    ComPtr<ID3DBlob> mpsByteCode = nullptr;

    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

    ComPtr<ID3D12PipelineState> mPSO = nullptr;

    XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
    XMFLOAT4X4 mView = MathHelper::Identity4x4();
    XMFLOAT4X4 mProj = MathHelper::Identity4x4();

    float mTheta = 1.5f * XM_PI;
    float mPhi = XM_PIDIV4;
    float mRadius = 5.0f;   // 球坐标 半径

    POINT mLastMousePos;

    //下面代码两秒换一次拓扑方式。
    int mFrameCount = 0;
    int mPrimativeIndex = 0;

    // mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
    PSTR cmdLine, int showCmd)
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    try
    {
        BoxApp theApp(hInstance);
        if (!theApp.Initialize())
            return 0;

        return theApp.Run();
    }
    catch (DxException& e)
    {
        MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
        return 0;
    }
}

BoxApp::BoxApp(HINSTANCE hInstance)
    : D3DApp(hInstance)
{
}

BoxApp::~BoxApp()
{
}

bool BoxApp::Initialize()
{
    if (!D3DApp::Initialize())
        return false;

    // Reset the command list to prep for initialization commands.
    // reset命令列表 为后面初始化做准备
    ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

    // 习题 12、13
    // 视口设置
    //mScreenViewport.TopLeftX = -mClientWidth / 4;
    //mScreenViewport.TopLeftY = 0;
    //mScreenViewport.Width = static_cast<float>(mClientWidth);
    //mScreenViewport.Height = static_cast<float>(mClientHeight);
    //mScreenViewport.MinDepth = 0.0f;
    //mScreenViewport.MaxDepth = 1.0f;

    // 裁剪矩形设置（矩形外的像素都将被剔除）
    // 前两个为左上点坐标，后两个为右下点坐标
    //mScissorRect = { 0, 0, mClientWidth / 2, mClientHeight / 2 };

    //cbv_srv_uavDescriptorSize = sizeof(D3D12_DESCRIPTOR_HEAP_DESC);

    BuildDescriptorHeaps(); // 先创建  描述符堆
    BuildConstantBuffers(); // 创建 常量缓冲区，描述符；将 描述符/view 放入描述符堆中

    BuildRootSignature();   // 创建 根签名

    BuildShadersAndInputLayout();

    BuildBoxGeometry();

    BuildPSO();

    // Execute the initialization commands.
    ThrowIfFailed(mCommandList->Close());   // 先关闭命令列表
    ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists); // 将 cmdlist数组 提交到 命令队列

    // Wait until initialization is complete. 
    // 刷新命令队列 ：（CPU和GPU同步）等待直到初始化命令完成
    FlushCommandQueue();

    return true;
}

void BoxApp::OnResize()
{
    D3DApp::OnResize();

    // The window resized, so update the aspect ratio and recompute the projection matrix.
    // 窗口大小改变，更新宽高比，重新计算投影矩阵
    XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
    XMStoreFloat4x4(&mProj, P);
}

void BoxApp::Update(const GameTimer& gt)
{
    // Convert Spherical to Cartesian coordinates.
    // 转换球面坐标
    float x = mRadius * sinf(mPhi) * cosf(mTheta);
    float z = mRadius * sinf(mPhi) * sinf(mTheta);
    float y = mRadius * cosf(mPhi);

    // Build the view matrix. 
    XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX view = XMMatrixLookAtLH(pos, target, up);  // 构建观察矩阵
    XMStoreFloat4x4(&mView, view);

    XMMATRIX world = XMLoadFloat4x4(&mWorld);
    XMMATRIX proj = XMLoadFloat4x4(&mProj);
    XMMATRIX worldViewProj = world * view * proj;   // 世界矩阵 * 观察矩阵 * 投影矩阵

    // Update the constant buffer with the latest worldViewProj matrix.
    ObjectConstants objConstants;
    XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
    // DX 库 中矩阵是行主序存储的，而 HLSL 中是列主序存储的（矩阵也是右乘的），所以需要转置

    // 习题6 ：时间变化
    objConstants.gTime = gt.TotalTime();
    // 习题 16 ：颜色混合
    objConstants.gPulseColor = XMFLOAT4(Colors::Gold);

    // 更新数据：将 数据 拷贝到 上传堆
    // 用mvp矩阵更新常量缓冲
    mObjectCB->CopyData(0, objConstants);


    // 习题7 ：另一个物体的 变换
    auto trans = XMMatrixTranslation(0, 1, 0);
    XMMATRIX worldViewProj2 = trans * world * view * proj;
    ObjectConstants objConstants2;
    XMStoreFloat4x4(&objConstants2.WorldViewProj, XMMatrixTranspose(worldViewProj2));
    mObjectCB->CopyData(1, objConstants2);
}

void BoxApp::Draw(const GameTimer& gt)
{
    // 1. 将 上一次绘制的 cmd 进行重置 ：重复使用记录命令的相关内存
    // Reuse the memory associated with command recording.
    // We can only reset when the associated command lists have finished execution on the GPU.
    ThrowIfFailed(mDirectCmdListAlloc->Reset());

    // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    // Reusing the command list reuses memory.      
    ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), mPSO.Get()));  // 复用命令列表及其内存

    // 2. 设置视口和裁剪矩形
    mCommandList->RSSetViewports(1, &mScreenViewport);
    mCommandList->RSSetScissorRects(1, &mScissorRect);


    // Indicate a state transition on the resource usage.
    // 3. 将后台缓冲资源从呈现状态转换到渲染目标状态（即准备接收图像渲染）
    mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

    // 4. 清除后台缓冲区和深度缓冲区，并赋值
    // Clear the back buffer and depth buffer.
    mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
    mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    // 5. 指定将要渲染的缓冲区，即指定RTV和DSV
    // Specify the buffers we are going to render to.
    mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());


    ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvHeap.Get() };
    mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);   //设置 （常量）描述符堆 到 cmdlist

    mCommandList->SetGraphicsRootSignature(mRootSignature.Get());   // 将 根签名设置到 cmdlist 上

    //mCommandList->IASetVertexBuffers(0, 1, &mBoxGeo->VertexBufferView());       // 设置顶点缓冲区 ，利用MeshGeometry中的VertexBufferView；然后在cmdlist中设置 顶点缓冲区
    mCommandList->IASetVertexBuffers(0, 1, &mBoxGeo->VertexPosBufferView());
    mCommandList->IASetVertexBuffers(1, 1, &mBoxGeo->VertexColorBufferView());

    mCommandList->IASetIndexBuffer(&mBoxGeo->IndexBufferView());				 // 设置索引缓冲区    


    mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);    // 设置图元拓扑
    
    //// 习题3
    //// 设置图元拓扑
    //if (!((mFrameCount++) % 320)) mPrimativeIndex++;// 两秒+1 (可以按照帧数计算 ：每秒160帧，则2秒一变就 /320)
    //int chose = mPrimativeIndex % 5;
    //if (chose == 1) mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);// 点列表
    //else if (chose == 2) mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);// 线条带
    //else if (chose == 3) mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);// 线列表
    //else if (chose == 4) mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);// 三角形带
    //else if (chose == 0)mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);// 三角形列表
    //// 显示图元拓扑选择
    //const std::wstring name[] = { L"TriangleList", L"PointList", L"LineStrip", L"LineList", L"TriangleStrip" };
    //mMainWndCaption = name[chose];


    // 设置 根描述符表 （寄存器槽号、要绑定的第一个描述符）：指定 绑定到渲染流水线的资源
    mCommandList->SetGraphicsRootDescriptorTable(0, mCbvHeap->GetGPUDescriptorHandleForHeapStart()); 

    //mCommandList->DrawIndexedInstanced(         // 绘制顶点（通过索引缓冲区绘制）
    //    mBoxGeo->DrawArgs["box"].IndexCount,	// 每个实例要绘制的索引数、实例化个数、起始索引位置，。。。
    //    1, 0, 0, 0);

    mCommandList->DrawIndexedInstanced(mBoxGeo->DrawArgs["box"].IndexCount,// 每个实例要绘制的索引数
        1,// 实例化个数
        mBoxGeo->DrawArgs["box"].StartIndexLocation,// 起始索引位置
        mBoxGeo->DrawArgs["box"].BaseVertexLocation,// 子物体起始索引在全局索引中的位置
        0);// 实例化的高级技术


    // 绘制 第二个物体 之前，更改 根签名描述表，换成另一个常量缓冲区的矩阵
    CD3DX12_GPU_DESCRIPTOR_HANDLE handle(mCbvHeap->GetGPUDescriptorHandleForHeapStart());
    handle.Offset(1, mCbvSrvUavDescriptorSize);    // 偏移到此次绘制的 CBV处
    mCommandList->SetGraphicsRootDescriptorTable(0, handle);
    mCommandList->DrawIndexedInstanced(mBoxGeo->DrawArgs["pyramid"].IndexCount,
        1, mBoxGeo->DrawArgs["pyramid"].StartIndexLocation, mBoxGeo->DrawArgs["pyramid"].BaseVertexLocation, 0);


    // Indicate a state transition on the resource usage.
    // 等到渲染完成，将后台缓冲区的状态改成呈现状态（状态转换），使其之后推到前台缓冲区显示。
    mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

    // Done recording commands. 完成命令的记录，关闭命令列表，等待传入命令队列。
    ThrowIfFailed(mCommandList->Close());

    // Add the command list to the queue for execution. 
    // 等CPU将命令都准备好后，需要将待执行的命令列表加入GPU的命令队列。
    ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);


    // swap the back and front buffers
    // 然后交换前后台缓冲区索引（这里的算法是1变0，0变1，为了让后台缓冲区索引永远为0）。
    ThrowIfFailed(mSwapChain->Present(0, 0));
    mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

    // Wait until frame commands are complete.  This waiting is inefficient and is
    // done for simplicity.  Later we will show how to organize our rendering code
    // so we do not have to wait per frame.
    FlushCommandQueue();    // 最后设置围栏值，刷新命令队列，使CPU和GPU同步
}

void BoxApp::OnMouseDown(WPARAM btnState, int x, int y)
{
    mLastMousePos.x = x;    // 按下的时候记录坐标x分量
    mLastMousePos.y = y;    // 按下的时候记录坐标y分量

    SetCapture(mhMainWnd);  // 在属于当前线程的指定窗口（窗口句柄）里，设置鼠标捕获
}

void BoxApp::OnMouseUp(WPARAM btnState, int x, int y)
{
    ReleaseCapture();   // 按键抬起后释放鼠标捕获
}

void BoxApp::OnMouseMove(WPARAM btnState, int x, int y)
{
    if ((btnState & MK_LBUTTON) != 0) // 如果在左键按下状态
    {
        // Make each pixel correspond to a quarter of a degree.
        // 将鼠标的移动距离换算成弧度，0.25为调节阈值  （当前位置-上次鼠标点击位置）
        float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
        float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

        // Update angles based on input to orbit camera around box.
        mTheta += dx;
        mPhi += dy;

        // Restrict the angle mPhi.
        // 限制 mPhi 的范围（上下动 是受限的）
        mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
    }
    else if ((btnState & MK_RBUTTON) != 0) // 如果在右键按下状态
    {
        // Make each pixel correspond to 0.005 unit in the scene.
        // 将鼠标的移动距离换算成缩放大小，0.005为调节阈值
        float dx = 0.005f * static_cast<float>(x - mLastMousePos.x);
        float dy = 0.005f * static_cast<float>(y - mLastMousePos.y);

        // Update the camera radius based on input.
        // 根据鼠标输入更新摄像机可视范围半径
        mRadius += dx - dy;

        // Restrict the radius.
        mRadius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
    }

    // 将当前鼠标坐标赋值给“上一次鼠标坐标”，为下一次鼠标操作提供先前值
    mLastMousePos.x = x;
    mLastMousePos.y = y;
}

void BoxApp::BuildDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;  // 描述符堆的 描述
    //cbvHeapDesc.NumDescriptors = 1;
    cbvHeapDesc.NumDescriptors = 2;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;  // 可以 着色器访问
    cbvHeapDesc.NodeMask = 0;   // 单个适配器
    // 创建 描述符堆
    ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&cbvHeapDesc,
        IID_PPV_ARGS(&mCbvHeap)));
    
    //cbv_srv_uavDescriptorSize = sizeof(D3D12_DESCRIPTOR_HEAP_DESC);
}

void BoxApp::BuildConstantBuffers()
{
    // 1. 获取常量缓冲区： 上传堆作为常量缓冲区（封装到UploadBuffer）
    //mObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(md3dDevice.Get(), 1, true);
    mObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(md3dDevice.Get(), 2, true); // 创建两个 上传堆/常量缓冲区

    UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));  // 计算常量缓冲区的大小（256B整数倍）

    //获得常量缓冲区首地址
    D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mObjectCB->Resource()->GetGPUVirtualAddress();

    // 通过常量缓冲区元素偏移值计算最终元素地址
    // Offset to the ith object constant buffer in the buffer.
    int boxCBufIndex = 0;
    cbAddress += boxCBufIndex * objCBByteSize;

    // 常量缓冲区 视图 的 描述符
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
    cbvDesc.BufferLocation = cbAddress;	    // 常量缓冲区的首地址
    cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

    // 2. 创建 常量缓冲区 视图（将描述符/视图 放入描述符堆）
    md3dDevice->CreateConstantBufferView(
        &cbvDesc,
        mCbvHeap->GetCPUDescriptorHandleForHeapStart());



    //boxCBufIndex = 1;
    //cbAddress += boxCBufIndex * objCBByteSize;
    cbAddress += objCBByteSize;
    cbvDesc.BufferLocation = cbAddress;
    CD3DX12_CPU_DESCRIPTOR_HANDLE handle(mCbvHeap->GetCPUDescriptorHandleForHeapStart());
    handle.Offset(1, mCbvSrvUavDescriptorSize);
    md3dDevice->CreateConstantBufferView(&cbvDesc, handle);
    //md3dDevice->CreateConstantBufferView(&cbvDesc, mCbvHeap->GetCPUDescriptorHandleForHeapStart());
}

void BoxApp::BuildRootSignature()
{
    // Shader programs typically require resources as input (constant buffers,
    // textures, samplers).  The root signature defines the resources the shader
    // programs expect.  If we think of the shader programs as a function, and
    // the input resources as function parameters, then the root signature can be
    // thought of as defining the function signature.  

    // Root parameter can be a table, root descriptor or root constants. 根参数可以是描述符表、根描述符、根常量
    CD3DX12_ROOT_PARAMETER slotRootParameter[1];

    // Create a single descriptor table of CBVs.  创建由单个CBV所组成的描述符表
    CD3DX12_DESCRIPTOR_RANGE cbvTable;
    cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);   // 描述符表的类型、描述符数量、绑定到的寄存器槽号
    slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

    // A root signature is an array of root parameters. 根签名由一系列根参数组成
    //  根签名描述（根签名的参数个数、根参数指针、静态采样数量、静态采样描述结构体、一组顶点缓冲区绑定的输入布局）
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
    // 用单个槽创建一个根签名，该槽指向由单个常量缓冲区组成的描述符范围
    ComPtr<ID3DBlob> serializedRootSig = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;

    // 序列化 根签名
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

    if (errorBlob != nullptr)
    {
        ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
    }
    ThrowIfFailed(hr);

    // 创建根签名 与 序列化内存绑定
    ThrowIfFailed(md3dDevice->CreateRootSignature(
        0,                                      // 适配器
        serializedRootSig->GetBufferPointer(),  // 根签名绑定的序列化内存指针
        serializedRootSig->GetBufferSize(), // 根签名绑定的序列化内存byte
        IID_PPV_ARGS(&mRootSignature)));    // // 根签名COM ID
}

void BoxApp::BuildShadersAndInputLayout()
{
    //处理顶点数据的输入布局描述

    HRESULT hr = S_OK;

    // 编译Shader
    mvsByteCode = d3dUtil::CompileShader(L"Shaders\\color6.hlsl", nullptr, "VS", "vs_5_0");
    mpsByteCode = d3dUtil::CompileShader(L"Shaders\\color6.hlsl", nullptr, "PS", "ps_5_0");

    // 对Vertex结构体中的Pos和Color做了具体的描述
    // 向DX提供该顶点结构体的描述，使它了解应该怎样处理结构体中的每个成员
    //mInputLayout =
    //{
    //    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    //    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    //};
    // 分别使用第0、1号寄存器，分别都不作偏移
    mInputLayout =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        //{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        { "COLOR", 0, DXGI_FORMAT_B8G8R8A8_UNORM, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };
}

void BoxApp::BuildBoxGeometry()
{
    //// 习题4 四棱锥
    //std::array<VPosData, 5> verticesPos =
    //{
    //    VPosData({ XMFLOAT3(+0.0f, +1.0f, +0.0f) }),
    //    VPosData({ XMFLOAT3(-1.0f, -1.0f, -1.0f) }),
    //    VPosData({ XMFLOAT3(+1.0f, -1.0f, -1.0f) }),
    //    VPosData({ XMFLOAT3(+1.0f, -1.0f, +1.0f) }),
    //    VPosData({ XMFLOAT3(-1.0f, -1.0f, +1.0f) })
    //};
    //std::array<VColorData, 5> verticesColor =
    //{
    //    VColorData({ XMFLOAT4(Colors::Red) }),
    //    VColorData({ XMFLOAT4(Colors::Green) }),
    //    VColorData({ XMFLOAT4(Colors::Green) }),
    //    VColorData({ XMFLOAT4(Colors::Green) }),
    //    VColorData({ XMFLOAT4(Colors::Green) })
    //};
    //std::array<std::uint16_t, 18> indices =
    //{
    //    // 前
    //    0, 2, 1,
    //    // 后
    //    0, 4, 3,
    //    // 左
    //    0, 1, 4,
    //    // 右
    //    0, 3, 2,
    //    // 下
    //    2, 4, 1,
    //    2, 3, 4
    //};


    //std::array<Vertex, 8> vertices =
    //{
    //    Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
    //    Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) }),
    //    Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) }),
    //    Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) }),
    //    Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) }),
    //    Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }),
    //    Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) }),
    //    Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) })
    //};

    std::array<VPosData, 13> verticesPos =
    {
        VPosData({ XMFLOAT3(-1.0f, -1.0f, -1.0f) }),
        VPosData({ XMFLOAT3(-1.0f, +1.0f, -1.0f) }),
        VPosData({ XMFLOAT3(+1.0f, +1.0f, -1.0f) }),
        VPosData({ XMFLOAT3(+1.0f, -1.0f, -1.0f) }),
        VPosData({ XMFLOAT3(-1.0f, -1.0f, +1.0f) }),
        VPosData({ XMFLOAT3(-1.0f, +1.0f, +1.0f) }),
        VPosData({ XMFLOAT3(+1.0f, +1.0f, +1.0f) }),
        VPosData({ XMFLOAT3(+1.0f, -1.0f, +1.0f) }),

        VPosData({ XMFLOAT3(+0.0f, +1.0f, +0.0f) }),
        VPosData({ XMFLOAT3(-1.0f, -1.0f, -1.0f) }),
        VPosData({ XMFLOAT3(+1.0f, -1.0f, -1.0f) }),
        VPosData({ XMFLOAT3(+1.0f, -1.0f, +1.0f) }),
        VPosData({ XMFLOAT3(-1.0f, -1.0f, +1.0f) })
    };
    //std::array<VColorData, 8> verticesColor =
    //{
    //    VColorData({ XMFLOAT4(Colors::White) }),
    //    VColorData({ XMFLOAT4(Colors::White) }),
    //    //VColorData({ XMFLOAT4(Colors::Black) }),
    //    VColorData({ XMFLOAT4(Colors::Red) }),
    //    VColorData({ XMFLOAT4(Colors::Red) }),
    //    //VColorData({ XMFLOAT4(Colors::Green) }),
    //    VColorData({ XMFLOAT4(Colors::Blue) }),
    //    VColorData({ XMFLOAT4(Colors::Blue) }),
    //    //VColorData({ XMFLOAT4(Colors::Yellow) }),
    //    VColorData({ XMFLOAT4(Colors::Cyan) }),
    //    VColorData({ XMFLOAT4(Colors::Cyan) }),
    //    //VColorData({ XMFLOAT4(Colors::Magenta) })
    //};
    std::array<VColorData, 13> verticesColor =
    {
        VColorData({ XMCOLOR(Colors::White) }),
        VColorData({ XMCOLOR(Colors::White) }),
        //VColorData({ XMFLOAT4(Colors::Black) }),
        VColorData({ XMCOLOR(Colors::Red) }),
        VColorData({ XMCOLOR(Colors::Red) }),
        //VColorData({ XMFLOAT4(Colors::Green) }),
        VColorData({ XMCOLOR(Colors::Blue) }),
        VColorData({ XMCOLOR(Colors::Blue) }),
        //VColorData({ XMFLOAT4(Colors::Yellow) }),
        VColorData({ XMCOLOR(Colors::Cyan) }),
        VColorData({ XMCOLOR(Colors::Cyan) }),
        //VColorData({ XMFLOAT4(Colors::Magenta) })

        VColorData({ XMCOLOR(Colors::Blue) }),
        VColorData({ XMCOLOR(Colors::Yellow) }),
        VColorData({ XMCOLOR(Colors::Cyan) }),
        VColorData({ XMCOLOR(Colors::White) }),
        VColorData({ XMCOLOR(Colors::Magenta) })
    };

    std::array<std::uint16_t, 54> indices =
    {
        // 立方体
        // 前
        0, 1, 2,
        0, 2, 3,
        // 后
        4, 6, 5,
        4, 7, 6,
        // 左
        4, 5, 1,
        4, 1, 0,
        // 右
        3, 2, 6,
        3, 6, 7,
        // 上
        1, 5, 6,
        1, 6, 2,
        // 下
        4, 0, 3,
        4, 3, 7,

        // 三棱锥
        // 前
        0, 2, 1,
        // 后
        0, 4, 3,
        // 左
        0, 1, 4,
        // 右
        0, 3, 2,
        // 下
        2, 4, 1,
        2, 3, 4
    };

    const UINT vPosBufferByteSize = (UINT)verticesPos.size() * sizeof(VPosData);
    const UINT vColorBufferByteSize = (UINT)verticesColor.size() * sizeof(VColorData);

    //const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
    const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

    mBoxGeo = std::make_unique<MeshGeometry>(); // d3dUtil 中定义的几何体
    mBoxGeo->Name = "boxGeo";

    //mBlendGeo = std::make_unique<MeshGeometry>(); // 习题7，但是buffer什么的也要改，还是用 原来的
    //mBlendGeo->Name = "boxGeo";

    // CPU 内存空间，并拷贝数据到里面
    //ThrowIfFailed(D3DCreateBlob(vbByteSize, &mBoxGeo->VertexBufferCPU));    //创建  数据内存空间
    //CopyMemory(mBoxGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);  // 将数据 拷贝至 系统内存中
    ThrowIfFailed(D3DCreateBlob(vPosBufferByteSize, &mBoxGeo->vPosBufferCpu));
    ThrowIfFailed(D3DCreateBlob(vColorBufferByteSize, &mBoxGeo->vColorBufferCpu));
    CopyMemory(mBoxGeo->vPosBufferCpu->GetBufferPointer(), verticesPos.data(), vPosBufferByteSize);
    CopyMemory(mBoxGeo->vColorBufferCpu->GetBufferPointer(), verticesColor.data(), vColorBufferByteSize);

    ThrowIfFailed(D3DCreateBlob(ibByteSize, &mBoxGeo->IndexBufferCPU));
    CopyMemory(mBoxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    // 创建 默认缓冲区 ，将资源从CPU内存 传到 GPU
    //   CreateDefaultBuffer：创建 上传堆；创建默认堆； 转换默认堆状态；将数据（data用D3D12_SUBRESOURCE_DATA接）复制到 默认堆；转换默认堆状态
    //mBoxGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
    //    mCommandList.Get(), vertices.data(), vbByteSize, mBoxGeo->VertexBufferUploader);
    mBoxGeo->vPosBufferGpu = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(), mCommandList.Get(), verticesPos.data(), vPosBufferByteSize, mBoxGeo->vPosBufferUploader);
    mBoxGeo->vColorBufferGpu = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(), mCommandList.Get(), verticesColor.data(), vColorBufferByteSize, mBoxGeo->vColorBufferUploader);

    mBoxGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
        mCommandList.Get(), indices.data(), ibByteSize, mBoxGeo->IndexBufferUploader);

    // 这里设置 vertex buffer view 和 index buffer view （MeshGeometry进行了封装）
    //mBoxGeo->VertexByteStride = sizeof(Vertex);
    //mBoxGeo->VertexBufferByteSize = vbByteSize;
    mBoxGeo->vPosByteStride = sizeof(VPosData);
    mBoxGeo->vPosBufferByteSize = vPosBufferByteSize;
    mBoxGeo->vColorByteStride = sizeof(VColorData);
    mBoxGeo->vColorBufferByteSize = vColorBufferByteSize;

    mBoxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
    mBoxGeo->IndexBufferByteSize = ibByteSize;

    //SubmeshGeometry submesh;
    //submesh.IndexCount = (UINT)indices.size();
    //submesh.StartIndexLocation = 0;
    //submesh.BaseVertexLocation = 0;

    // 创建子物体
    SubmeshGeometry submeshBox;
    submeshBox.IndexCount = 36;
    submeshBox.StartIndexLocation = 0;
    submeshBox.BaseVertexLocation = 0;

    SubmeshGeometry submeshPyramid;
    submeshPyramid.IndexCount = 18;
    submeshPyramid.StartIndexLocation = 36;
    submeshPyramid.BaseVertexLocation = 8;

    //mBoxGeo->DrawArgs["box"] = submesh;     // 将 要绘制的子物体（名称：数据索引） 传入map

    mBoxGeo->DrawArgs["box"] = submeshBox;
    mBoxGeo->DrawArgs["pyramid"] = submeshPyramid;
}

void BoxApp::BuildPSO()
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
    ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };   // 输入布局描述
    psoDesc.pRootSignature = mRootSignature.Get();               // 与此PSO绑定的 根签名 指针
    psoDesc.VS =
    {
        reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()),
        mvsByteCode->GetBufferSize()
    };
    psoDesc.PS =
    {
        reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()),
        mpsByteCode->GetBufferSize()
    };
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
    //psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
     //psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;// 不剔除
     //psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;// 前面剔除
    psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;// 后面剔除

    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = mBackBufferFormat;
    psoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
    psoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
    psoDesc.DSVFormat = mDepthStencilFormat;


    ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)));
}