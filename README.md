## IDE
	visual studio 2019
## 引用的第三方库
	1.opencv：用于最终窗口的显示和一些贴图的加载
	2.assimp：用于模型的加载
	3.Eigen：线性代数的运算
## 目前已完成功能
	1.基本的图形显示管线，包括顶点变化和光栅化的操作。
	2.相机系统
	3.裁剪功能（以上完成于2022.09）
	4.模型文件的加载(完成于2022.09.15)
	5.光源的设置以及基本光照模型的加入(完成于2022.09.19)
	6.贴图的引入
	7.天空盒	(6、7完成于2022.09.29)	

## 下一步的工作
	1.功能多了以后两个shader类体量有点冗余了。考虑换成继承和工具类来替代
	2.实现阴影算法
	3.渲染速度有点慢了，考虑用cuda引入gpu来运算
