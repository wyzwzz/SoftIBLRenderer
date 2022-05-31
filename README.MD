# Soft PBR Renderer

Soft Renderer with IBL

## Features
### Accelerate
* Frustum Cull 只渲染位于相机视锥体内的模型
* Back Face Culling 三角形背面剔除
* Geometry Clip 剔除整个位于标准化设备坐标外的三角形
* Hierarchy Z-Buffer Test (optional) 只栅格化通过层次ZBuffer测试的三角形 
* Early Z-Buffer Test 只对通过ZBuffer Test的片段进行着色计算
* OpenMP
### Render
* Physical Base Render
* IBL
* ACES Tone Mapping
* Support Multiple Models And Lights
* Support Model Transform And Model Loading Dynamically
## ScreeShots
### IBL
dusk
![statue](screenshots/ibl_statue.png)
![cerberus](screenshots/ibl_cerberus1.png)
day
![statue](screenshots/ibl_statue1.png)
![cerberus](screenshots/ibl_cerberus.png)
### Direct Lighting
![cerberus](screenshots/cerberus.png)

![chest](screenshots/chest.png)

![statue](screenshots/statue.png)

![muti-models](screenshots/multi-models.png)

![statues](screenshots/statues.png)

![chests](screenshots/chests.png)
