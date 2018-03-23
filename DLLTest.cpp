// DLLTest.cpp : 定义 DLL 应用程序的导出函数。
//
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <list>
#include <vector>
#include <cstdlib>
#include <forward_list>
#include <cmath>
#include <time.h>

#include "stdafx.h"
#include "DLLTest.h"

#include "cv.h"
#include "highgui.h"
#include "Slic.h"
#include "SlicMerge.h"
#include "HierarchicalTree.h"
#include "classify.h"

using namespace cv;
using namespace std;

#pragma comment(linker, "/STACK:102400000,102400000")    //防止栈溢出

int width;
int height;
int finalNumberOfLabels;
Mat srimg;
int* clabels;
CSuperPixelSet* csps;
BTreeNode* hierarchicalTree;
ArrayHeadGraphNode *mAhgn;
int *newLabels;
ObjectNode* oNode;
int setValue;
int objectNum;
Mat imgClassify;


DLLTEST_API void ShowPic()
{
	Mat img;
	img = imread("D:\\liuzhou.tif", 1);
	namedWindow("srimg");
	imshow("srimg",img);
	waitKey(0);
}

DLLTEST_API void StartSegment()
{
	int sz;
	int i, ii;
	int x, y;
	int step;
	int numseeds;
	int k;
	int dims[2] = {0};
	int* outputNumSuperpixels;
	int* outlabels;
	
	unsigned char* imgbytes; //等同于src.data
	int numelements;
	int numSuperpixels = 200;//default value
	double compactness = 10;//default value

	Mat zy1, zy2, zy3, zy4;
	zy1 = imread("D:\\b.bmp",0);
	zy2 = imread("D:\\g.bmp",0);
	zy3 = imread("D:\\r.bmp",0);
	zy4 = imread("D:\\ir.bmp",0);
	if (zy1.empty() || zy2.empty() || zy3.empty() || zy4.empty())
	{
		printf("Can not open Image\n");
		system("pause");
		exit(0);
	}

	CvSize size;
	size.width = zy1.cols;
	size.height = zy1.rows;
	srimg.create(size.height, size.width, CV_8UC4);
	for (int i = 0;i<size.height;i++)
		for (int j = 0; j<size.width; j++)
		{
			srimg.data[(i*size.width+j)*4]=zy1.data[i*size.width+j];
			srimg.data[(i*size.width+j)*4+1]=zy2.data[i*size.width+j];
			srimg.data[(i*size.width+j)*4+2]=zy3.data[i*size.width+j];
			srimg.data[(i*size.width+j)*4+3]=zy4.data[i*size.width+j];
		}

	width = srimg.cols;
    height = srimg.rows;
    sz = width*height;
    int* rin = new int[height*width];       //@可以用uchar节省空间
    int* gin = new int[height*width];
    int* bin = new int[height*width];
	int* nirIn = new int[height*width];		//近红外波段
    double* lvec = new double[height*width]; //@同时为LAB开辟空间，可以优化
    double* avec = new double[height*width];
    double* bvec = new double[height*width];
    int* klabels = new int[height*width];
    clabels = new int[height*width];
    int* seedIndices = new int[height*width];
	newLabels = new int[height*width];

	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
		{
			bin[i*width + j] = srimg.data[(i*width + j)*4];
			gin[i*width + j] = srimg.data[(i*width + j)*4 + 1];
			rin[i*width + j] = srimg.data[(i*width + j)*4 + 2];
			nirIn[i*width+j] = srimg.data[(i*width + j)*4 + 3];
		}
	
	//部分参数定义  转换可能有所不同
    numelements = 3;   //默认只对彩色图像做处理
    //numdims明显的二维
    dims[0] = height;
    dims[1] = width;
    imgbytes = srimg.data; //指向原图像数据域
    numSuperpixels = 1500; //**超像素个数,适用于demo
    compactness = 15; //**紧凑度

	//convert from rgb to lab
	rgbtolab(rin,gin,bin,sz,lvec,avec,bvec);

	//find seeds
	step = sqrt((double)(sz)/(double)(numSuperpixels))+0.5;
	getLABXYSeeds(step,width,height,seedIndices,&numseeds);

	double* kseedsx = new double[numseeds];
	double* kseedsy = new double[numseeds];
	double* kseedsl = new double[numseeds];
	double* kseedsa = new double[numseeds];
	double* kseedsb = new double[numseeds];
	for(k = 0; k < numseeds; k++)
	{
		kseedsx[k] = seedIndices[k]%width;
		kseedsy[k] = seedIndices[k]/width;
		kseedsl[k] = lvec[seedIndices[k]];
		kseedsa[k] = avec[seedIndices[k]];
		kseedsb[k] = bvec[seedIndices[k]];
	}

	//Compute superpixels

	PerformSuperpixelSLIC(lvec, avec, bvec, kseedsl,kseedsa,kseedsb,kseedsx,kseedsy,width,height,numseeds,klabels,step,compactness);

	//Enforce connectivity
	EnforceSuperpixelConnectivity(klabels,width,height,numSuperpixels,clabels,&finalNumberOfLabels);
	
	printf("%d\n", finalNumberOfLabels);

	//建立基层超像素对象集合
	csps = new CSuperPixelSet[finalNumberOfLabels]; //调用无参构造
	hierarchicalTree = new BTreeNode[2*finalNumberOfLabels-1]; 
	createSuperPixelVector(clabels, width, height, csps, srimg, finalNumberOfLabels,hierarchicalTree);

	mAhgn = new ArrayHeadGraphNode[2*finalNumberOfLabels-1];   //严格的2n-1，最终层次树的头结点下标为2n-2
	createToplogicalGraph(clabels, width, height, mAhgn,numSuperpixels);

	createHierarchicalTree(mAhgn, hierarchicalTree, srimg, 200, finalNumberOfLabels);

	//超像素分割效果
	Mat imgSuperpixel = srimg.clone();
	for (int i = 1; i<height-1; i++)
		for (int j = 1;j<width-1; j++)
		{
			//不考虑图像边缘
			if (clabels[i*width + j] != clabels[(i-1)*width +j] || clabels[i*width + j] != clabels[(i+1)*width +j] || clabels[i*width + j] != clabels[i*width +j+1] || clabels[i*width + j] != clabels[i*width + j-1])
			{
				imgSuperpixel.data[(i*width+j)*4] = 0;
				imgSuperpixel.data[(i*width+j)*4+1] = 0;
				imgSuperpixel.data[(i*width+j)*4+2] = 255;
				imgSuperpixel.data[(i*width+j)*4+3] = 0;
			}	
		}
}

DLLTEST_API void ShowHTree(int level)
{
	setValue = -1;
	setAllNodeValue(newLabels, level, &hierarchicalTree[2*finalNumberOfLabels-2], setValue, csps);
	objectNum = setValue + 1;
	Mat imgMerge = srimg.clone();
	for (int i = 1; i<height-1; i++)
		for (int j = 1;j<width-1; j++)
		{
			//不考虑图像边缘
			if (newLabels[i*width + j] != newLabels[(i-1)*width +j] || newLabels[i*width + j] != newLabels[(i+1)*width +j] || newLabels[i*width + j] != newLabels[i*width +j+1] || newLabels[i*width + j] != newLabels[i*width + j-1])
			{

					imgMerge.data[(i*width+j)*4] = 0;
					imgMerge.data[(i*width+j)*4+1] = 0;
					imgMerge.data[(i*width+j)*4+2] = 255;
					imgMerge.data[(i*width+j)*4+3] = 0;
			}	
		}

		imwrite("ImgMerge.bmp", imgMerge);
}


DLLTEST_API void ConfirmLevel()
{
	oNode = new ObjectNode[objectNum];
	createNewObjectSet(newLabels, srimg, oNode, objectNum, width, height);

	for (int i = 0; i< objectNum; i++)
	{
		oNode[i].formFeatureInit(width);
		oNode[i].spectralFeatureInit();
		oNode[i].haveInit = 1;
	}

}

DLLTEST_API void Classify(int fromWhatType, int toWhatType, int whatFeature, double lowerLimit, double upperLimit)
{
	switch(whatFeature)
	{
		case 0: {changeNodeTypes(fromWhatType, classifyByArea, toWhatType, objectNum, width, oNode, lowerLimit, upperLimit);break;}
		case 1:	{changeNodeTypes(fromWhatType, classifyByDensity, toWhatType, objectNum, width, oNode, lowerLimit, upperLimit);break;}
		case 2:	{changeNodeTypes(fromWhatType, classifyByBrightnessBGR, toWhatType, objectNum, width, oNode, lowerLimit, upperLimit);break;}
		case 3:	{changeNodeTypes(fromWhatType, classifyByBrightnessBGRNIR, toWhatType, objectNum, width, oNode, lowerLimit, upperLimit);break;}
		case 4:	{changeNodeTypes(fromWhatType, classifyByNDVI, toWhatType, objectNum, width, oNode, lowerLimit, upperLimit);break;}
		case 5:	{changeNodeTypes(fromWhatType, classifyByNDWI, toWhatType, objectNum, width, oNode, lowerLimit, upperLimit);break;}
		case 6:	{changeNodeTypes(fromWhatType, classifyBySBI, toWhatType, objectNum, width, oNode, lowerLimit, upperLimit);break;}
		case 7:	{changeNodeTypes(fromWhatType, classifyByBAI, toWhatType, objectNum, width, oNode, lowerLimit, upperLimit);break;}
		case 8:	{changeNodeTypes(fromWhatType, classifyByShapeIndex, toWhatType, objectNum, width, oNode, lowerLimit, upperLimit);break;}
	}

	imgClassify = srimg.clone();

	for(int i= 0; i<height; i++)
		for (int j = 0; j<width; j++)
		{
			switch(oNode[newLabels[i*width+j]].objectTypes)
			{
				case 0:
					{
						//未分类颜色不变
						imgClassify.data[(i*width + j)*4] = srimg.data[(i*width + j)*4];
						imgClassify.data[(i*width + j)*4+1] = srimg.data[(i*width + j)*4 + 1];
						imgClassify.data[(i*width + j)*4+2] = srimg.data[(i*width + j)*4 + 2];
						imgClassify.data[(i*width + j)*4+3] = srimg.data[(i*width + j)*4 + 3];
						break;
					}
				case 1:
					{
						//水体颜色为蓝色
						imgClassify.data[(i*width + j)*4] = 255;
						imgClassify.data[(i*width + j)*4+1] = 0;
						imgClassify.data[(i*width + j)*4+2] = 0;
						imgClassify.data[(i*width + j)*4+3] = 0;
						break;
					}
				case 2:
					{
						//植被绿色
						imgClassify.data[(i*width + j)*4] = 0;
						imgClassify.data[(i*width + j)*4+1] = 255;
						imgClassify.data[(i*width + j)*4+2] = 0;
						imgClassify.data[(i*width + j)*4+3] = 0;
						break;
					}
				case 3:
					{
						//裸土黄色
						imgClassify.data[(i*width + j)*4] = 0;
						imgClassify.data[(i*width + j)*4+1] = 255;
						imgClassify.data[(i*width + j)*4+2] = 255;
						imgClassify.data[(i*width + j)*4+3] = 0;
						break;
					}
				case 4:
					{
						//建筑红色
						imgClassify.data[(i*width + j)*4] = 0;
						imgClassify.data[(i*width + j)*4+1] = 0;
						imgClassify.data[(i*width + j)*4+2] = 255;
						imgClassify.data[(i*width + j)*4+3] = 0;
						break;
					}
				case 5:
					{
						//道路白
						imgClassify.data[(i*width + j)*4] = 255;
						imgClassify.data[(i*width + j)*4+1] = 255;
						imgClassify.data[(i*width + j)*4+2] = 255;
						imgClassify.data[(i*width + j)*4+3] = 0;
						break;
					}

			}	
		}
	imwrite("imgClassify.bmp", imgClassify);
}