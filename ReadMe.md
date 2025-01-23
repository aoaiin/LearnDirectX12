# 记录一下 学习 DirectX12 的代码

参考以及遇到的问题

1. 初始化篇中，重构代码时：
   一直调试不好，总是在 PeekMessage；进不去 Draw；
   然后发现 C++ 没有 super，只能用 `父类::Draw()` 调用父类的 Draw
