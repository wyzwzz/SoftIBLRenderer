# Soft PBR Renderer

##
* Early Z-Buffer Test
    片段先进行Z-Buffer测试 只对通过的片段进行着色计算
* Back Face Culling 测试三角形面的方向量是否为背对
* Clip 测试三角形是否位于视锥体内 可以在顶点着色器计算之后进行
