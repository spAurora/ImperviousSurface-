
class CSuperPixelSet  //������ʵ����
{
public:
	int level;
	int id;
	double avgB;   //BGR��ֵ
	double avgG;
	double avgR;
	int pixelnum;
	vector<int> pixelLocation;  //���ɹ��ɳ����ص�����

	//list<CSuperPixelSet> childspslist; //���ӽ������

	CSuperPixelSet()  //�޲ι���
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
	//CSuperPixelSet(CSuperPixelSet &c)  //���ƹ���
	//{
	//	level = c.level;
	//	id = c.id;
	//}
protected:
private:
};

//��������
class BTreeNode
{
public:
	BTreeNode* left;
	BTreeNode* right;

	//������
	int ID;
	//�ȼ���Ϣ
	int level;   //1Ϊ����������
	//������Ϣ
	double avgB;   
	double avgG;
	double avgR;
	//������Ϣ
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
	int ID; //*��ʼ��
	//BTreeNode* pBTnode; //����������û��
	GraphNode(int mID)
	{
		ID = mID;
	}
};

//����ͼͷ���
//�ڽӱ�洢
//ͷ��㱾��Ҳ��֯Ϊ����
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

class ArrayHeadGraphNode  //ͷ�������
{
public:
	BTreeNode* pBTnode;							//ָ�������Ľ��
	forward_list<GraphNode> pGraphNodeList;		//�ڽ����˵�
	bool hadRemove;

	ArrayHeadGraphNode()
	{
		pBTnode = NULL;
		hadRemove = false; 
	}
protected:
private:
};



//����������ʵ�弯���Լ�������ײ���
//������ͼ�㡢���ߡ������ؽ���顢ԭͼ�񡢳������������������
void createSuperPixelVector(int* label, int width, int height, CSuperPixelSet *csps, cv::Mat& srimg, int superPixelNum, BTreeNode* hierarchicalTree)
{
	//int*a = new int[superPixelnum] ();  //���ĳ���������Ƿ����������У���ʼ��Ϊ0
	//int col = 0;

	//xΪheight yΪwidth
	//��������label���������������ض��󼯺ϲ���ȡ�����Ϣ
	//c++vector˳�����Ч��̫��
	//for (int i = 0;i<height;i++)
	//	for (int j = 0;j<width;j++)
	//	{
	//		if (a[label[i*width+j]] == 0) //δ����������
	//		{
	//			printf("%d\n",col++);
	//			CSuperPixelSet* csps = new CSuperPixelSet(1, label[i*width+j]);
	//			//*csps = CSuperPixelSet(1, lable[i*width+j]);
	//			csps->pixelLocation.push_back(i*width+j); 
	//			vcsps.push_back(*csps);
	//			a[label[i*width+j]] = 1;
	//			delete csps;
	//		}
	//		else  //����������
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
		
	//����

	for (int i = 0;i<height;i++)
		for (int j = 0;j<width;j++)
		{
			csps[label[i*width+j]].pixelLocation.push_back(i*width+j);  //�������������������صĹ�ϵ
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

//������ʼ������ͼ
//������ͼ�㡢ͼƬ���ߡ�ͼͷ������顢��������Ŀ
void createToplogicalGraph(int*clabels, int width, int height, ArrayHeadGraphNode* mAhgn ,int superPixelnum)
{
	//int* HadThisNodeBeenCreated = new int[superPixelnum] (); //0Ϊ���룬1Ϊ����
	
	//��ʼ��vHeadGraphList
	
	int show = 0;


	//ArrayHeadGraphNode *mAhgn = new ArrayHeadGraphNode[superPixelnum];
	//ArrayHeadGraphNode mAhgn[100000];
	for (int i = 0; i<height - 1; i++)
		for (int j = 0; j < width - 1; j++)
		{
			if (clabels[i*width + j] != clabels[i*width + j + 1])
			{
				forward_list<GraphNode>::iterator it;
				int check = 0; //0Ϊ������
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
				int check = 0; //0Ϊ������
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
	//		//���ұߵĵ�
	//		if(clabels[i*width+j] != clabels[i*width+j+1])
	//		{
	//			//Has 2 node in 1 node list?(�����˹�ϵ�Ƿ��Ѿ�����)
	//			//yes: pass
	//			//No:����
	//			forward_list<HeadGraphNode>::iterator headit;
	//			forward_list<HeadGraphNode>::iterator saveheadit[2];
	//			forward_list<GraphNode>::iterator it;
	//			int check = 0; //0Ϊ������
	//			int hadcheck = 0; //0Ϊδ�������
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
	//							if (it->ID == clabels[i*width+j+1]) //����
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
	//							if (it->ID == clabels[i*width+j]) //����
	//							{
	//								check = 1;
	//								hadcheck = 1;
	//								break;
	//							}
	//				}
	//			}
	//			
	//			if(check == 0) //�����ڣ��������˹�ϵ
	//			{
	//				saveheadit[0]->pGraphNodeList.push_front(GraphNode(clabels[i*width+j+1]));
	//				saveheadit[1]->pGraphNodeList.push_front(GraphNode(clabels[i*width+j]));
	//				printf("%d\n", show++);
	//			}
	//		}

	//		//�õ����±ߵĵ�
	//		if(clabels[i*width+j] != clabels[(i+1)*width+j])
	//		{
	//			
	//			forward_list<HeadGraphNode>::iterator headit;
	//			forward_list<HeadGraphNode>::iterator saveheadit[2];
	//			forward_list<GraphNode>::iterator it;
	//			int check = 0; //0Ϊ������
	//			int hadcheck = 0; //0Ϊδ�������
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
	//							if (it->ID == clabels[(i+1)*width+j]) //����
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
	//							if (it->ID == clabels[i*width+j]) //����
	//							{
	//								check = 1;
	//								hadcheck = 1;
	//								break;
	//							}
	//				}
	//			}
	//				
	//				//printf("%d\-----d\n",saveheadit[0]->ID, saveheadit[1]->ID);
	//				if(check == 0) //�����ڣ��������˹�ϵ
	//				{
	//					saveheadit[0]->pGraphNodeList.push_front(GraphNode(clabels[(i+1)*width+j]));
	//					saveheadit[1]->pGraphNodeList.push_front(GraphNode(clabels[i*width+j]));
	//					printf("%d ++ \n", show++);
	//				}
	//			
	//		}
	//	}
}

//������ײ�
double calculateDifference(BTreeNode t1, BTreeNode t2)
{
	return(sqrt( (t1.avgB-t2.avgB)*(t1.avgB-t2.avgB) + (t1.avgG-t2.avgG)*(t1.avgG-t2.avgG) + (t1.avgR-t2.avgR)*(t1.avgR-t2.avgR) ));
}

//����Ƿ��ظ�
bool whetherThisValueInTheOtherSet(GraphNode mNode, forward_list<GraphNode> list)
{
	forward_list<GraphNode>::iterator it;
	for (it = list.begin(); it != list.end(); it++)
		if (it->ID == mNode.ID)
			return true;
	return false;
}

//ɾ�����
void delNode(ArrayHeadGraphNode* mAhgn, int delInId, int delId_1, int delId_2, int graphAndTreeEnd)
{
	//printf("�ѽ���\n");
	if(mAhgn[delInId].pGraphNodeList.empty())
		printf("�ڽ�Ϊ�գ������߼�����!");
	int sum = 0;
	forward_list<GraphNode>::iterator prev=mAhgn[delInId].pGraphNodeList.before_begin();  //��ʾflst�ġ���ǰԪ�ء�
	forward_list<GraphNode>::iterator curr=mAhgn[delInId].pGraphNodeList.begin();  //��ʾflst�еĵ�һ��Ԫ��
	while(curr!=mAhgn[delInId].pGraphNodeList.end())
	{
		if(curr->ID == delId_1 || curr->ID == delId_2)
		{
			curr=mAhgn[delInId].pGraphNodeList.erase_after(prev);// ɾ�������ƶ�curr
			//printf("��ɾ��\n");
		}
		else
		{
			prev=curr;  //�ƶ�������curr��ָ����һ��Ԫ�أ�prevָ��curr֮ǰ��Ԫ��
			++curr;
		}
	}
	mAhgn[delInId].pGraphNodeList.push_front(GraphNode(graphAndTreeEnd));
}

//���㲢��
void calculateUnion(int childNodeLoc_1, int childNodeLoc_2, int graphAndTreeEnd, ArrayHeadGraphNode* mAhgn, BTreeNode* hierarchicalTree)
{
	mAhgn[childNodeLoc_1].hadRemove = true;
	mAhgn[childNodeLoc_2].hadRemove = true;
	//ȡ����
	forward_list<GraphNode>::iterator it;
	for (it = mAhgn[childNodeLoc_1].pGraphNodeList.begin(); it!= mAhgn[childNodeLoc_1].pGraphNodeList.end(); it++)
		if (it->ID != childNodeLoc_2)
		mAhgn[graphAndTreeEnd].pGraphNodeList.push_front(*it);
	for (it = mAhgn[childNodeLoc_2].pGraphNodeList.begin(); it != mAhgn[childNodeLoc_2].pGraphNodeList.end(); it++)
		if(whetherThisValueInTheOtherSet(*it, mAhgn[childNodeLoc_1].pGraphNodeList) == false && it->ID != childNodeLoc_1)
			mAhgn[graphAndTreeEnd].pGraphNodeList.push_front(*it);

	//check
	if(mAhgn[childNodeLoc_1].pGraphNodeList.empty() == true)
		printf("��1\n");
	if(mAhgn[childNodeLoc_2].pGraphNodeList.empty() == true)
		printf("��2\n");

	//�޸������ڽӽ������˱�
	forward_list<GraphNode>::iterator itt;
	itt = mAhgn[1].pGraphNodeList.begin();
	//printf("WTF %d\n", itt->ID);
	for (itt = mAhgn[childNodeLoc_1].pGraphNodeList.begin(); itt!= mAhgn[childNodeLoc_1].pGraphNodeList.end(); itt++)
	{
		//printf("%d\n", itt->ID);
		if (itt->ID != childNodeLoc_2)  //������˸��ֺţ�������
		{
			//printf("��ʼɾ��2���\n");
			//printf("������ID:it->ID = %d\n", itt->ID);
			delNode(mAhgn, itt->ID, childNodeLoc_1, childNodeLoc_2, graphAndTreeEnd);
		}
	}
	//printf("���1���ڽӵ��������(������Ҫ�ںϵ�������)\n");
	for (itt = mAhgn[childNodeLoc_2].pGraphNodeList.begin(); itt!= mAhgn[childNodeLoc_2].pGraphNodeList.end(); itt++)
	{
		//printf("%d\n", itt->ID);
		if (itt->ID != childNodeLoc_1)
		{
			//printf("��ʼɾ��2���\n");
			//printf("������ID:it->ID = %d\n", itt->ID);
			delNode(mAhgn, itt->ID, childNodeLoc_1, childNodeLoc_2, graphAndTreeEnd);
		}
	}
	//printf("���2���ڽӵ��������(������Ҫ�ںϵ�������)\n");
	/*for (itt = mAhgn[childNodeLoc_2].pGraphNodeList.begin(); itt!= mAhgn[childNodeLoc_2].pGraphNodeList.end(); itt++)
		if (itt->ID != childNodeLoc_1);
		{
			delNode(mAhgn, itt->ID, childNodeLoc_1, childNodeLoc_2, graphAndTreeEnd);
			printf("��ʼɾ��2���\n");
		}*/
}

//�ݹ�����
void DFS(int location,int *vnum, ArrayHeadGraphNode* mAhgn,BTreeNode* hierarchicalTree,int &graphAndTreeEnd, int nowLevel, double & allowDifference, bool& NodeMerge, int superPixelNum)
{
	if (NodeMerge == true)   //�Ѿ��������ں���ֱ�ӷ���
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
	//------ �ں�
	forward_list<GraphNode>::iterator mit;
	for(mit = mAhgn[location].pGraphNodeList.begin(); mit!= mAhgn[location].pGraphNodeList.end(); mit++)
		if (mAhgn[mit->ID].hadRemove == false)
		{
			if(calculateDifference(hierarchicalTree[mit->ID], hierarchicalTree[location]) < allowDifference) //С����ֵ��ں�
			{
				//�ںϰ��� �����½ڵ�
				//ɾ���������ڵ�
				//����Ѿ��������ں�
				//ֱ���˳��ú���
				NodeMerge = true;
				graphAndTreeEnd++;

				printf("�ںϼ�����ʼ,�����c1 c2�±�Ϊ%d, %d\n, level: %d\n", location, mit->ID, nowLevel);
				//printf("��ʼ�ںϣ�\n");
				calculateUnion(location, mit->ID, graphAndTreeEnd, mAhgn, hierarchicalTree); //����ͼȡ����********

				//printf("��չ�����±꣺%d\n", graphAndTreeEnd);
				
				//mAhgn[mit->ID].hadRemove = true;
				//mAhgn[location].hadRemove = true;
				//д�������������
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
				//printf("��ֵ����̫�ϸ��޷��ں� code 0\n��ǰ��ֵ��%lf, ������ֵ: %lf\n", calculateDifference(hierarchicalTree[mit->ID], hierarchicalTree[location]), allowDifference);
			}
		}
		else
		{
			//printf("����ѱ��Ƴ����޷��ںϣ�code 1\n");
		}
	//------
	//printf("\n��ǰ������ܱ������ڽӵĺ���û���ںϣ�������һ��(�ݹ�):\n**************************************\n");
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


	for(itp = mAhgn[location].pGraphNodeList.begin(); itp != mAhgn[location].pGraphNodeList.end(); itp++)    //mAhgn�����������ᷢ���仯������ʹ�õ�����ʧЧ����������������
	{
		
		//printf("������û��\n");
		//printf("it ID:%d,  vnum[it->ID]:%d,   mAhgn.hadRemove:%d\n", itp->ID, vnum[itp->ID], mAhgn[itp->ID].hadRemove);
		if (vnum[itp->ID] == 0 && mAhgn[itp->ID].hadRemove == false)
		{	int temp;
			temp = itp->ID;
			//printf("temp(Ҳ����it->ID):%d\n");
			DFS(temp, vnum, mAhgn, hierarchicalTree, graphAndTreeEnd, nowLevel, allowDifference, NodeMerge, superPixelNum);
		}
		if (NodeMerge == true)
			return;   //!!!!!!!!!!!
	}
}

//�����Լ��ں�
//ͼͷ������顢��������飬ͼ�����������յ㣬��ǰ�����ȼ�������Ĺ��ײ�,�����Ƿ����ں�,��������Ŀ
void traversalAndMerge(ArrayHeadGraphNode* mAhgn,BTreeNode* hierarchicalTree,int &graphAndTreeEnd, int nowLevel, double& allowDifference, bool& NodeMerge, int superPixelNum)  //double�����int��
{
	if (NodeMerge == true)
	{
		printf("something wrong!\n");
		exit(-1);
	}
	int *vnum = new int[2*superPixelNum-1] (); //vnum,0Ϊ��ǰδ������
	for (int i = 0; i<=graphAndTreeEnd;i++)
		if(vnum[i] == 0 && mAhgn[i].hadRemove == false)
		{
			//printf("�ӵ�%d��ʼ����\n", i);
			DFS(i, vnum, mAhgn, hierarchicalTree, graphAndTreeEnd, nowLevel, allowDifference, NodeMerge, superPixelNum); //ͼ����ȫ��ͨ��
			break;
		}

	//������ȱ���ͨ��
	//for (int i = 0; i<=graphAndTreeEnd;i++)
	//{
	//	if(vnum[i] == 0)
	//	printf("wrong!!\n");
	//}
	//
	delete vnum;
}


//���������
//ͼͷ������顢��������顢ԭͼ�񡢷ֲ�������������
void createHierarchicalTree(ArrayHeadGraphNode* mAhgn,BTreeNode* hierarchicalTree, cv::Mat& srimg, int levelindex,int superPixelnum)
{
	int nowLevel = 1;
	bool NodeMerge = false;
	double allowDifference = 0;
	int graphAndTreeEnd = superPixelnum - 1;
	//traversalAndMerge(mAhgn, hierarchicalTree, graphAndTreeEnd, nowLevel, allowDifference, NodeMerge, superPixelnum);  ??????????��Ϊʲôûע�͵�������
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
			//���������������
			//printf("allDifference:%lf\n", allowDifference);
			//system("pause");
			traversalAndMerge(mAhgn, hierarchicalTree, graphAndTreeEnd, nowLevel, allowDifference, NodeMerge, superPixelnum);
			if (NodeMerge == false)
				printf("û�з����ں�...\n\n");
			/*if (NodeMerge == true)
				graphAndTreeEnd++;*/
		} while (NodeMerge == true);

		printf("������һ�ȼ�.....\n");
	}
}