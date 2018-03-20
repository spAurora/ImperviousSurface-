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
#include "highgui.h"
#include "Slic.h"
#include "cv.h"
#include "SlicMerge.h"
#include "HierarchicalTree.h"

#pragma comment(linker, "/STACK:102400000,102400000")    //防止栈溢出
using namespace cv;


//@为可优化点，**为自行设置参数
int main()
{
    //先不考虑Alpha通道
    //Mat srimg;
    int width;
    int height;
    int sz;
    int i, ii;
    int x, y;
    //int* rin; int* gin; int* bin;
    //int* klabels;
    //int* clabels;
    //double* lvec; double* avec; double* bvec;
    int step;
    //int* seedIndices;
    int numseeds;
    //double* kseedsx; double* kseedsy;
    //double* kseedsl; double* kseedsa; double* kseedsb;
    int k;
    //const mwSize* dims;//int* dims;
    int dims[2] = {0};
    int* outputNumSuperpixels;
    int* outlabels;
    int finalNumberOfLabels;
    unsigned char* imgbytes; //等同于src.data
    int numelements;
    int numSuperpixels = 200;//default value
    double compactness = 10;//default value

    ////RGB读入
    //srimg = imread("D:\demo.bmp", 1);    //**
    //if (srimg.empty())
    //{   
    //    printf("Can not open image.\n");
    //    exit(0);
    //}

	Mat zy1, zy2, zy3, zy4, srimg;
	zy1 = imread("b.bmp",0);
	zy2 = imread("g.bmp",0);
	zy3 = imread("r.bmp",0);
	zy4 = imread("ir.bmp",0);
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
	namedWindow("srimg");
	imshow("Superpixel",srimg);
	waitKey(0);
    //分离三个RGB通道
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
    int* clabels = new int[height*width];
    int* seedIndices = new int[height*width];
    /*for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
        {
            bin[i*width + j] = srimg.data[(i*width + j)*3];
            gin[i*width + j] = srimg.data[(i*width + j)*3 + 1];
            rin[i*width + j] = srimg.data[(i*width + j)*3 + 2];
        }*/

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
    numSuperpixels = 100000; //**超像素个数,适用于demo
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
    
    //output
    //clabels为连通矩阵 finalNumberOfLabels为超像素区域个数
    //cout << finalNumberOfLabels << endl; 
	printf("%d\n", finalNumberOfLabels);
	for (int i = 0; i<20; i++)
	{
		for (int j = 0; j<9; j++)
		{
			printf("%d ", clabels[i*width + j]);
		}
		printf("\n");
	}



	//test
	//vector<int> a;
	//int b = 10;
	//a.push_back(b);
	//vector<int>::iterator it=a.begin();
	//b = 15;
	//printf("%d\n%d\n", *it, b);
	//vector成员独立，是push入成员的拷贝
	/*CSuperPixelSet *test = new CSuperPixelSet(5,5);
	printf("%d\n", test->id);*/
	/*vector<CSuperPixelSet> vcsps;
	createSuperPixelVector(clabels, width, height, vcsps, finalNumberOfLabels);*/
	
	//建立基层超像素对象集合	p
	CSuperPixelSet* csps = new CSuperPixelSet[finalNumberOfLabels]; //调用无参构造
	BTreeNode* hierarchicalTree = new BTreeNode[2*finalNumberOfLabels-1]; 
	//超像素过分割，建立超像素实体集
	createSuperPixelVector(clabels, width, height, csps, srimg, finalNumberOfLabels,hierarchicalTree);

	//建立初始拓扑图	p
	//forward_list<HeadGraphNode>  vHeadGraphList;
	ArrayHeadGraphNode *mAhgn = new ArrayHeadGraphNode[2*finalNumberOfLabels-1];   //严格的2n-1，最终层次树的头结点下标为2n-2
	createToplogicalGraph(clabels, width, height, mAhgn,numSuperpixels);

	//test
	//forward_list<GraphNode>::iterator prev=mAhgn[0].pGraphNodeList.before_begin();  //表示flst的“首前元素”
	//forward_list<GraphNode>::iterator curr=mAhgn[0].pGraphNodeList.begin();  //表示flst中的第一个元素
	//while(curr!=mAhgn[0].pGraphNodeList.end())
	//{
	//	if(curr->ID == 1)
	//	{
	//		curr=mAhgn[0].pGraphNodeList.erase_after(prev);// 删除它并移动curr
	//		printf("123\n");
	//	}
	//	else
	//	{
	//		prev=curr;  //移动迭代器curr，指向下一个元素，prev指向curr之前的元素
	//		++curr;
	//	}
	//}
	printf("%d\n", finalNumberOfLabels*2-2);
	system("pause");
	
	//构建层次树
	createHierarchicalTree(mAhgn, hierarchicalTree, srimg, 50, finalNumberOfLabels);

	int level;
	int *newLabels = new int[height*width] ();
	do{
		printf("\n\n请输入查询结点等级:");
		scanf("%d", &level);
		//searchTreeNodeWithLevel(&hierarchicalTree[2*finalNumberOfLabels-2], level, finalNumberOfLabels);
		int setValue = -1;
		setAllNodeValue(newLabels, level, &hierarchicalTree[2*finalNumberOfLabels-2], setValue, csps);
		


		printf("融合后面块数：%d\n", setValue+1);
		
		//放弃层次树结点中的其它信息，只保留层次信息
		//基于新联通图层建立新对象面块集合以及拓扑关系信息
		int objectNum = setValue + 1;
		ObjectNode* oNode = new ObjectNode[objectNum];
		ArrayHeadGraphNode *newAHGn = new ArrayHeadGraphNode[objectNum];
		createNewObjectSet(newLabels, srimg, oNode, objectNum, width, height);
		createNewToplogicalGraph(newLabels, width, height, newAHGn, objectNum,oNode);

		//融合效果展示
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

			namedWindow("Superpixel");
			imshow("Superpixel", imgMerge);
			waitKey(0);


	}while(1);

	//check
	//printf("%d  %d", hierarchicalTree[finalNumberOfLabels*2-2].left->ID, hierarchicalTree[finalNumberOfLabels*2-2].right->ID);
	
	//check
	/*forward_list<GraphNode>::iterator it;
	for (it = mAhgn[603].pGraphNodeList.begin(); it != mAhgn[603].pGraphNodeList.end(); it++)
	{
	printf("%d -> " ,it->ID);
	}
	system("pause");*/


	//check
	/*forward_list<HeadGraphNode>::iterator hit;
	for(hit = vHeadGraphList.begin(); hit!=vHeadGraphList.end();hit++)
	{
		if (hit->ID == 0)
			break;
	}
	forward_list<GraphNode>::iterator itt;
	for (itt = hit->pGraphNodeList.begin();itt != hit->pGraphNodeList.end(); itt++)
	{
		printf("%d -> " ,itt->ID);
	}
	printf("\n");
	system("pause");*/

	//test2
	/*vector<int>::iterator it;
	for (it = csps[0].pixelLocation.begin(); it != csps[0].pixelLocation.end(); it++)
		printf("%d %d\n",(*it)/width, (*it)%width);*/

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

	namedWindow("Superpixel");
	imshow("Superpixel", imgSuperpixel);
	waitKey(0);
	//cvCopy(srimg,Imgfinal,NULL);



    //Deallocate memory
    delete rin;
    delete gin;
    delete bin;
    delete lvec;
    delete avec;
    delete bvec;
    delete klabels;
    delete clabels;
    delete seedIndices;
    delete kseedsx;
    delete kseedsy;
    delete kseedsl;
    delete kseedsa;
    delete kseedsb;

    return 0;
}