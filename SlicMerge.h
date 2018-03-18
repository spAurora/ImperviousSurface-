
class CSuperPixelSet  //超像素实体类
{
public:
	int level;
	int id;
	double avgB;   //BGR均值
	double avgG;
	double avgR;
	int pixelnum;
	vector<int> pixelLocation;  //容纳构成超像素的像素

	//list<CSuperPixelSet> childspslist; //孩子结点链表

	CSuperPixelSet()  //无参构造
	{
		level = 0;
		id = -1;
		avgB = 0;
		avgG = 0;
		avgR = 0;
		pixelnum = 0;
	}
	CSuperPixelSet(int flevel, int fid)
	{
		level = flevel;
		id = fid;
	}
	//CSuperPixelSet(CSuperPixelSet &c)  //复制构造
	//{
	//	level = c.level;
	//	id = c.id;
	//}
protected:
private:
};

//层次树结点
class BTreeNode
{
public:
	BTreeNode* left;
	BTreeNode* right;

	//数据域
	int ID;
	//等级信息
	int level;   //1为基础超像素
	//光谱信息
	double avgB;   
	double avgG;
	double avgR;
	//像素信息
	int pixelnum;

	BTreeNode()
	{
		ID = 0;
		level = 1;
		avgB = 0;
		avgG = 0;
		avgR = 0;
		left = NULL;
		right = NULL;
	}

protected:
private:
};

class GraphNode
{
public:
	int ID; //*初始化
	//BTreeNode* pBTnode; //这个或许可以没有
	GraphNode(int mID)
	{
		ID = mID;
	}
};

//拓扑图头结点
//邻接表存储
//头结点本身也组织为链表
//class HeadGraphNode
//{
//public:
//	int ID;
//	BTreeNode* pBTnode;
//	forward_list<GraphNode> pGraphNodeList;
//
//	HeadGraphNode(int mID, forward_list<GraphNode> mpGraphNodeList)
//	{
//		ID = mID;
//		pGraphNodeList = mpGraphNodeList;
//		pBTnode = NULL;
//	}
//protected:
//private:
//};

class ArrayHeadGraphNode  //头结点数组
{
public:
	BTreeNode* pBTnode;							//指向层次树的结点
	forward_list<GraphNode> pGraphNodeList;		//邻接拓扑点
	bool hadRemove;

	ArrayHeadGraphNode()
	{
		pBTnode = NULL;
		hadRemove = false; 
	}
protected:
private:
};



//创建超像素实体集合以及层次树底层结点
//超像素图层、宽、高、超像素结点组、原图像、超像素数、层次数数组
void createSuperPixelVector(int* label, int width, int height, CSuperPixelSet *csps, cv::Mat& srimg, int superPixelNum, BTreeNode* hierarchicalTree)
{
	//int*a = new int[superPixelnum] ();  //标记某个超像素是否纳入容器中，初始化为0
	//int col = 0;

	//x为height y为width
	//遍历整个label，建立基础超像素对象集合并提取相关信息
	//c++vector顺序遍历效率太低
	//for (int i = 0;i<height;i++)
	//	for (int j = 0;j<width;j++)
	//	{
	//		if (a[label[i*width+j]] == 0) //未纳入容器中
	//		{
	//			printf("%d\n",col++);
	//			CSuperPixelSet* csps = new CSuperPixelSet(1, label[i*width+j]);
	//			//*csps = CSuperPixelSet(1, lable[i*width+j]);
	//			csps->pixelLocation.push_back(i*width+j); 
	//			vcsps.push_back(*csps);
	//			a[label[i*width+j]] = 1;
	//			delete csps;
	//		}
	//		else  //已在容器中
	//		{
	//			vector<CSuperPixelSet>::iterator it;
	//			for(it=vcsps.end()-1;it>vcsps.begin();it--)
	//				if(it->id == label[label[i*width+j]])
	//				{
	//					it->pixelLocation.push_back(i*width+j);
	//					break;
	//				}
	//			if(it->id == label[label[i*width+j]])
	//				it->pixelLocation.push_back(i*width+j);
	//		}
		
	//排序

	for (int i = 0;i<height;i++)
		for (int j = 0;j<width;j++)
		{
			csps[label[i*width+j]].pixelLocation.push_back(i*width+j);  //建立超像素与其中像素的关系
			csps[label[i*width+j]].pixelnum++;
			csps[label[i*width+j]].avgB += srimg.data[(i*width+j)*3];
			csps[label[i*width+j]].avgG += srimg.data[(i*width+j)*3+1];
			csps[label[i*width+j]].avgR += srimg.data[(i*width+j)*3+2];
		}
	for(int i = 0; i< superPixelNum;i++)
	{
		csps[i].avgB /= csps[i].pixelnum;
		csps[i].avgG /= csps[i].pixelnum;
		csps[i].avgR /= csps[i].pixelnum;
	}
	printf("-----------\n %lf \n ----------", csps[100].avgB);

	for (int i = 0; i< superPixelNum; i++)
	{
		hierarchicalTree[i].level = 1;
		hierarchicalTree[i].ID = i;
		hierarchicalTree[i].avgB = csps[i].avgB;
		hierarchicalTree[i].avgG = csps[i].avgG;
		hierarchicalTree[i].avgR = csps[i].avgR;
		hierarchicalTree[i].pixelnum = csps[i].pixelnum;
	}
	//delete a;
}

//创建初始化拓扑图
//超像素图层、图片宽、高、图头结点数组、超像素数目
void createToplogicalGraph(int*clabels, int width, int height, ArrayHeadGraphNode* mAhgn ,int superPixelnum)
{
	//int* HadThisNodeBeenCreated = new int[superPixelnum] (); //0为纳入，1为纳入
	
	//初始化vHeadGraphList
	
	int show = 0;


	//ArrayHeadGraphNode *mAhgn = new ArrayHeadGraphNode[superPixelnum];
	//ArrayHeadGraphNode mAhgn[100000];
	for (int i = 0; i<height - 1; i++)
		for (int j = 0; j < width - 1; j++)
		{
			if (clabels[i*width + j] != clabels[i*width + j + 1])
			{
				forward_list<GraphNode>::iterator it;
				int check = 0; //0为不存在
				for (it = mAhgn[clabels[i*width + j]].pGraphNodeList.begin(); it != mAhgn[clabels[i*width + j]].pGraphNodeList.end(); it++)
					if (it->ID == clabels[i*width+j+1])
					{
						check = 1;
						break;
					}
				if(check == 0)
				{
					mAhgn[clabels[i*width + j]].pGraphNodeList.push_front(clabels[i*width + j + 1]);
					mAhgn[clabels[i*width + j + 1]].pGraphNodeList.push_front(clabels[i*width + j]);
				}
			}

			if (clabels[i*width + j] != clabels[(i+1)*width+j])
			{
				forward_list<GraphNode>::iterator it;
				int check = 0; //0为不存在
				for (it = mAhgn[clabels[i*width + j]].pGraphNodeList.begin(); it != mAhgn[clabels[i*width + j]].pGraphNodeList.end(); it++)
					if (it->ID == clabels[(i+1)*width+j])
					{
						check = 1;
						break;
					}
					if(check == 0)
					{
						mAhgn[clabels[i*width + j]].pGraphNodeList.push_front(clabels[(i+1)*width+j]);
						mAhgn[clabels[(i+1)*width+j]].pGraphNodeList.push_front(clabels[i*width + j]);
					}
			}
		}
		/*for (int i = 0; i<superPixelnum; i++)
			vHeadGraphList.push_front(HeadGraphNode(i, mAhgn[i].pGraphNodeList));*/

		//delete mAhgn;
		//check
		

		

	//for (int i = 0; i<height-1; i++)
	//	for (int j = 0;j<width-1; j++)
	//	{
	//		//if (clabels[i*width + j] != clabels[(i-1)*width +j] || clabels[i*width + j] != clabels[(i+1)*width +j] || clabels[i*width + j] != clabels[i*width +j+1] || clabels[i*width + j] != clabels[i*width + j-1])
	//		//与右边的点
	//		if(clabels[i*width+j] != clabels[i*width+j+1])
	//		{
	//			//Has 2 node in 1 node list?(即拓扑关系是否已经建立)
	//			//yes: pass
	//			//No:建立
	//			forward_list<HeadGraphNode>::iterator headit;
	//			forward_list<HeadGraphNode>::iterator saveheadit[2];
	//			forward_list<GraphNode>::iterator it;
	//			int check = 0; //0为不存在
	//			int hadcheck = 0; //0为未经过检查
	//			int col = 0;
	//			for (headit = vHeadGraphList.begin(); headit!=vHeadGraphList.end(); headit++)
	//			{
	//				if (col == 2)
	//					break;
	//				if (headit->ID == clabels[i*width+j])
	//				{
	//					saveheadit[0] = headit;
	//					col = col+1;
	//					if (hadcheck == 0)
	//						for (it = headit->pGraphNodeList.begin(); it!=headit->pGraphNodeList.end(); it++)
	//							if (it->ID == clabels[i*width+j+1]) //存在
	//							{
	//								check = 1;
	//								hadcheck = 1;
	//								break;
	//							}
	//				}
	//				if (headit->ID == clabels[i*width+j+1])
	//				{
	//					saveheadit[1] = headit;
	//					col = col+1;
	//					if (hadcheck == 0)
	//						for (it = headit->pGraphNodeList.begin(); it!=headit->pGraphNodeList.end(); it++)
	//							if (it->ID == clabels[i*width+j]) //存在
	//							{
	//								check = 1;
	//								hadcheck = 1;
	//								break;
	//							}
	//				}
	//			}
	//			
	//			if(check == 0) //不存在，建立拓扑关系
	//			{
	//				saveheadit[0]->pGraphNodeList.push_front(GraphNode(clabels[i*width+j+1]));
	//				saveheadit[1]->pGraphNodeList.push_front(GraphNode(clabels[i*width+j]));
	//				printf("%d\n", show++);
	//			}
	//		}

	//		//该点与下边的点
	//		if(clabels[i*width+j] != clabels[(i+1)*width+j])
	//		{
	//			
	//			forward_list<HeadGraphNode>::iterator headit;
	//			forward_list<HeadGraphNode>::iterator saveheadit[2];
	//			forward_list<GraphNode>::iterator it;
	//			int check = 0; //0为不存在
	//			int hadcheck = 0; //0为未经过检查
	//			int col = 0;
	//			for (headit = vHeadGraphList.begin(); headit!=vHeadGraphList.end(); headit++)
	//			{
	//				if (col == 2)
	//					break;
	//				if (headit->ID == clabels[i*width+j])
	//				{
	//					saveheadit[0] = headit;
	//					//printf("%d\n ttt", saveheadit[1]->ID);
	//					col = col+1;
	//					if (hadcheck == 0)
	//						for (it = headit->pGraphNodeList.begin(); it!=headit->pGraphNodeList.end(); it++)
	//							if (it->ID == clabels[(i+1)*width+j]) //存在
	//							{
	//								check = 1;
	//								hadcheck = 1;
	//								break;
	//							}
	//				}
	//				if (headit->ID == clabels[(i+1)*width+j])
	//				{
	//					saveheadit[1] = headit;
	//					col = col+1;
	//					if (hadcheck == 0)
	//						for (it = headit->pGraphNodeList.begin(); it!=headit->pGraphNodeList.end(); it++)
	//							if (it->ID == clabels[i*width+j]) //存在
	//							{
	//								check = 1;
	//								hadcheck = 1;
	//								break;
	//							}
	//				}
	//			}
	//				
	//				//printf("%d\-----d\n",saveheadit[0]->ID, saveheadit[1]->ID);
	//				if(check == 0) //不存在，建立拓扑关系
	//				{
	//					saveheadit[0]->pGraphNodeList.push_front(GraphNode(clabels[(i+1)*width+j]));
	//					saveheadit[1]->pGraphNodeList.push_front(GraphNode(clabels[i*width+j]));
	//					printf("%d ++ \n", show++);
	//				}
	//			
	//		}
	//	}
}

//计算光谱差
double calculateDifference(BTreeNode t1, BTreeNode t2)
{
	return(sqrt( (t1.avgB-t2.avgB)*(t1.avgB-t2.avgB) + (t1.avgG-t2.avgG)*(t1.avgG-t2.avgG) + (t1.avgR-t2.avgR)*(t1.avgR-t2.avgR) ));
}

//检查是否重复
bool whetherThisValueInTheOtherSet(GraphNode mNode, forward_list<GraphNode> list)
{
	forward_list<GraphNode>::iterator it;
	for (it = list.begin(); it != list.end(); it++)
		if (it->ID == mNode.ID)
			return true;
	return false;
}

//删除结点
void delNode(ArrayHeadGraphNode* mAhgn, int delInId, int delId_1, int delId_2, int graphAndTreeEnd)
{
	//printf("已进入\n");
	if(mAhgn[delInId].pGraphNodeList.empty())
		printf("邻接为空，存在逻辑错误!");
	int sum = 0;
	forward_list<GraphNode>::iterator prev=mAhgn[delInId].pGraphNodeList.before_begin();  //表示flst的“首前元素”
	forward_list<GraphNode>::iterator curr=mAhgn[delInId].pGraphNodeList.begin();  //表示flst中的第一个元素
	while(curr!=mAhgn[delInId].pGraphNodeList.end())
	{
		if(curr->ID == delId_1 || curr->ID == delId_2)
		{
			curr=mAhgn[delInId].pGraphNodeList.erase_after(prev);// 删除它并移动curr
			//printf("已删除\n");
		}
		else
		{
			prev=curr;  //移动迭代器curr，指向下一个元素，prev指向curr之前的元素
			++curr;
		}
	}
	mAhgn[delInId].pGraphNodeList.push_front(GraphNode(graphAndTreeEnd));
}

//计算并集
void calculateUnion(int childNodeLoc_1, int childNodeLoc_2, int graphAndTreeEnd, ArrayHeadGraphNode* mAhgn, BTreeNode* hierarchicalTree)
{
	mAhgn[childNodeLoc_1].hadRemove = true;
	mAhgn[childNodeLoc_2].hadRemove = true;
	//取并集
	forward_list<GraphNode>::iterator it;
	for (it = mAhgn[childNodeLoc_1].pGraphNodeList.begin(); it!= mAhgn[childNodeLoc_1].pGraphNodeList.end(); it++)
		if (it->ID != childNodeLoc_2)
		mAhgn[graphAndTreeEnd].pGraphNodeList.push_front(*it);
	for (it = mAhgn[childNodeLoc_2].pGraphNodeList.begin(); it != mAhgn[childNodeLoc_2].pGraphNodeList.end(); it++)
		if(whetherThisValueInTheOtherSet(*it, mAhgn[childNodeLoc_1].pGraphNodeList) == false && it->ID != childNodeLoc_1)
			mAhgn[graphAndTreeEnd].pGraphNodeList.push_front(*it);

	//check
	if(mAhgn[childNodeLoc_1].pGraphNodeList.empty() == true)
		printf("空1\n");
	if(mAhgn[childNodeLoc_2].pGraphNodeList.empty() == true)
		printf("空2\n");

	//修改其余邻接结点的拓扑表
	forward_list<GraphNode>::iterator itt;
	itt = mAhgn[1].pGraphNodeList.begin();
	//printf("WTF %d\n", itt->ID);
	for (itt = mAhgn[childNodeLoc_1].pGraphNodeList.begin(); itt!= mAhgn[childNodeLoc_1].pGraphNodeList.end(); itt++)
	{
		//printf("%d\n", itt->ID);
		if (itt->ID != childNodeLoc_2)  //后面多了个分号，见鬼了
		{
			//printf("开始删除2结点\n");
			//printf("清理结点ID:it->ID = %d\n", itt->ID);
			delNode(mAhgn, itt->ID, childNodeLoc_1, childNodeLoc_2, graphAndTreeEnd);
		}
	}
	//printf("结点1的邻接点清理完毕(不包括要融合的两个点)\n");
	for (itt = mAhgn[childNodeLoc_2].pGraphNodeList.begin(); itt!= mAhgn[childNodeLoc_2].pGraphNodeList.end(); itt++)
	{
		//printf("%d\n", itt->ID);
		if (itt->ID != childNodeLoc_1)
		{
			//printf("开始删除2结点\n");
			//printf("清理结点ID:it->ID = %d\n", itt->ID);
			delNode(mAhgn, itt->ID, childNodeLoc_1, childNodeLoc_2, graphAndTreeEnd);
		}
	}
	//printf("结点2的邻接点清理完毕(不包括要融合的两个点)\n");
	/*for (itt = mAhgn[childNodeLoc_2].pGraphNodeList.begin(); itt!= mAhgn[childNodeLoc_2].pGraphNodeList.end(); itt++)
		if (itt->ID != childNodeLoc_1);
		{
			delNode(mAhgn, itt->ID, childNodeLoc_1, childNodeLoc_2, graphAndTreeEnd);
			printf("开始删除2结点\n");
		}*/
}

//递归深搜
void DFS(int location,int *vnum, ArrayHeadGraphNode* mAhgn,BTreeNode* hierarchicalTree,int &graphAndTreeEnd, int nowLevel, double & allowDifference, bool& NodeMerge, int superPixelNum)
{
	if (NodeMerge == true)   //已经发生过融合则直接返回
		return;
	if (location == 2*superPixelNum-2)
	{
		NodeMerge = false;
		return;
	}
	if(vnum[location]!=0)
		return;

	vnum[location] = 1;
	//printf("%d\n", location);
	//------ 融合
	forward_list<GraphNode>::iterator mit;
	for(mit = mAhgn[location].pGraphNodeList.begin(); mit!= mAhgn[location].pGraphNodeList.end(); mit++)
		if (mAhgn[mit->ID].hadRemove == false)
		{
			if(calculateDifference(hierarchicalTree[mit->ID], hierarchicalTree[location]) < allowDifference) //小于阈值差，融合
			{
				//融合包括 创建新节点
				//删除这两个节点
				//标记已经发生过融合
				//直接退出该函数
				NodeMerge = true;
				graphAndTreeEnd++;

				printf("融合即将开始,两结点c1 c2下标为%d, %d\n, level: %d\n", location, mit->ID, nowLevel);
				//printf("开始融合：\n");
				calculateUnion(location, mit->ID, graphAndTreeEnd, mAhgn, hierarchicalTree); //拓扑图取并集********

				//printf("扩展到的下标：%d\n", graphAndTreeEnd);
				
				//mAhgn[mit->ID].hadRemove = true;
				//mAhgn[location].hadRemove = true;
				//写入新树结点数据
				hierarchicalTree[graphAndTreeEnd].left = &hierarchicalTree[mit->ID];
				hierarchicalTree[graphAndTreeEnd].right = &hierarchicalTree[location];
				hierarchicalTree[graphAndTreeEnd].pixelnum = hierarchicalTree[location].pixelnum + hierarchicalTree[mit->ID].pixelnum;
				hierarchicalTree[graphAndTreeEnd].level = nowLevel;
				hierarchicalTree[graphAndTreeEnd].avgB = (hierarchicalTree[location].pixelnum*hierarchicalTree[location].avgB + hierarchicalTree[mit->ID].pixelnum*hierarchicalTree[mit->ID].avgB) / hierarchicalTree[graphAndTreeEnd].pixelnum;
				hierarchicalTree[graphAndTreeEnd].avgG = (hierarchicalTree[location].pixelnum*hierarchicalTree[location].avgG + hierarchicalTree[mit->ID].pixelnum*hierarchicalTree[mit->ID].avgG) / hierarchicalTree[graphAndTreeEnd].pixelnum;
				hierarchicalTree[graphAndTreeEnd].avgR = (hierarchicalTree[location].pixelnum*hierarchicalTree[location].avgR + hierarchicalTree[mit->ID].pixelnum*hierarchicalTree[mit->ID].avgR) / hierarchicalTree[graphAndTreeEnd].pixelnum;
				hierarchicalTree[graphAndTreeEnd].ID = graphAndTreeEnd;
				return;
			}
			else
			{
				//printf("阈值限制太严格，无法融合 code 0\n当前阈值：%lf, 允许阈值: %lf\n", calculateDifference(hierarchicalTree[mit->ID], hierarchicalTree[location]), allowDifference);
			}
		}
		else
		{
			//printf("结点已被移除，无法融合：code 1\n");
		}
	//------
	//printf("\n当前结点与周边所有邻接的好像都没法融合，进行下一步(递归):\n**************************************\n");
	//forward_list<GraphNode>::iterator itt;
	//itt = mAhgn[1].pGraphNodeList.begin();
	////printf("WTF %d\n", itt->ID);
	//for (itt = mAhgn[childNodeLoc_1].pGraphNodeList.begin(); itt!= mAhgn[childNodeLoc_1].pGraphNodeList.end(); itt++)
	forward_list<GraphNode>::iterator itp;
	itp = mAhgn[0].pGraphNodeList.begin();
	//printf("WTF %d\n", itp->ID);
	//for(it = mAhgn[location].pGraphNodeList.begin(); it!= mAhgn[location].pGraphNodeList.end(); it++)
	//for(itp = mAhgn[location].pGraphNodeList.begin(); itp != mAhgn[location].pGraphNodeList.end(); itp++)
	//{
		//printf("FUCK %d\n", itp->ID);
	//}


	for(itp = mAhgn[location].pGraphNodeList.begin(); itp != mAhgn[location].pGraphNodeList.end(); itp++)    //mAhgn的链表容器会发生变化，进而使得迭代器失效！！！！！！！！
	{
		
		//printf("进来了没？\n");
		//printf("it ID:%d,  vnum[it->ID]:%d,   mAhgn.hadRemove:%d\n", itp->ID, vnum[itp->ID], mAhgn[itp->ID].hadRemove);
		if (vnum[itp->ID] == 0 && mAhgn[itp->ID].hadRemove == false)
		{	int temp;
			temp = itp->ID;
			//printf("temp(也就是it->ID):%d\n");
			DFS(temp, vnum, mAhgn, hierarchicalTree, graphAndTreeEnd, nowLevel, allowDifference, NodeMerge, superPixelNum);
		}
		if (NodeMerge == true)
			return;   //!!!!!!!!!!!
	}
}

//遍历以及融合
//图头结点数组、层次树数组，图与层次数遍历终点，当前树结点等级，允许的光谱差,遍历是否发生融合,超像素数目
void traversalAndMerge(ArrayHeadGraphNode* mAhgn,BTreeNode* hierarchicalTree,int &graphAndTreeEnd, int nowLevel, double& allowDifference, bool& NodeMerge, int superPixelNum)  //double定义成int了
{
	if (NodeMerge == true)
	{
		printf("something wrong!\n");
		exit(-1);
	}
	int *vnum = new int[2*superPixelNum-1] (); //vnum,0为当前未遍历到
	for (int i = 0; i<=graphAndTreeEnd;i++)
		if(vnum[i] == 0 && mAhgn[i].hadRemove == false)
		{
			//printf("从第%d开始遍历\n", i);
			DFS(i, vnum, mAhgn, hierarchicalTree, graphAndTreeEnd, nowLevel, allowDifference, NodeMerge, superPixelNum); //图是完全连通的
			break;
		}

	//深度优先遍历通过
	//for (int i = 0; i<=graphAndTreeEnd;i++)
	//{
	//	if(vnum[i] == 0)
	//	printf("wrong!!\n");
	//}
	//
	delete vnum;
}


//创建层次树
//图头结点数组、层次树数组、原图像、分层数、超像素数
void createHierarchicalTree(ArrayHeadGraphNode* mAhgn,BTreeNode* hierarchicalTree, cv::Mat& srimg, int levelindex,int superPixelnum)
{
	int nowLevel = 1;
	bool NodeMerge = false;
	double allowDifference = 0;
	int graphAndTreeEnd = superPixelnum - 1;
	//traversalAndMerge(mAhgn, hierarchicalTree, graphAndTreeEnd, nowLevel, allowDifference, NodeMerge, superPixelnum);  ??????????我为什么没注释掉？？？
	for (int i = 1; i <= levelindex; i++)
	{
		nowLevel++;
		allowDifference =allowDifference + ((double)450/(double)levelindex);
		printf("allDifference:%lf\n", allowDifference);
		//system("pause");
		NodeMerge = false;
		do 
		{	
			NodeMerge = false;
			//遍历并创建层次树
			//printf("allDifference:%lf\n", allowDifference);
			//system("pause");
			traversalAndMerge(mAhgn, hierarchicalTree, graphAndTreeEnd, nowLevel, allowDifference, NodeMerge, superPixelnum);
			if (NodeMerge == false)
				printf("没有发生融合...\n\n");
			/*if (NodeMerge == true)
				graphAndTreeEnd++;*/
		} while (NodeMerge == true);

		printf("处理下一等级.....\n");
	}
}