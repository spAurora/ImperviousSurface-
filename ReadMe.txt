基于面向对象思想的不透水面提取软件

首先采用超像素分割算法对影像进行过分割，然后采用原创层次树融合算法对超像素进行融合构造
融合层次树，确定融合层次后，获取各面块对象的光谱特征以及形态特征等，依据特征进行分类

核心dll源码GitHub地址：https://github.com/spAurora/ImperviousSurface-
说明：请在win10系统下执行Test.exe程序

地物类型：0未分类，1水体，2植被，3裸土，4建筑，5道路
特征编码：0：Area 1：Density 2：brightnessBGR 3.BrightnessBGRNIR
	  4：NDVI 5：NDWI    6：SBI  7.BAI  8.ShapeIndex


