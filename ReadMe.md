# 记录一下 学习 DirectX12 的代码

> 笔记和习题 记录在 语雀/TA/D3D
> https://www.yuque.com/yuqueyonghuc94hae/vvitgx/uwyaoliwes9ou2wd

参考以及遇到的问题

1. 初始化篇中，重构代码时：
   一直调试不好，总是在 PeekMessage；进不去 Draw；
   然后发现 C++ 没有 super，只能用 `父类::Draw()` 调用父类的 Draw

2. 感觉直接 看书本 源代码示例比较好： 重新创建了一个 工程 DirectXRef ；
   记得每个章节 引用对应的现有项 文件

3. 然后 从第 6 章开始 写一下习题：
   mainX.cpp:第 X 章的源码示例
   mainXL.cpp:第 X 章的习题
