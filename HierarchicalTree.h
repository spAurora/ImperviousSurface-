extern int width;
extern int height;

using namespace std;
class ObjectNode : public CSuperPixelSet
{
public:
	int objectTypes;  //地物类别
	int haveInit;
	
	//协方差矩阵
	//形态学相关
	double varxx;
	double varyy;
	double covxy;
	int borderLength;
	double shapeIndex;
	double density;

	//光谱相关
	double brightnessBGRNIR;
	double brightnessBGR;
	double NDVI;
	double NDWI;
	double SBI;
	double BAI;

	ObjectNode()
	{
		borderLength = 0;
		objectTypes = 0;//0未分类，1水体，2植被，3裸土，4建筑，5道路
		haveInit = 0;
	}

	//成员函数

	//展示信息
	void showInformation()
	{
		this->formFeatureInit(width);
		this->spectralFeatureInit();
		printf("ID：%d\n", this->id);
		printf("Area: %d\n", this->pixelnum);
		printf("borderLength:%d\n", this->borderLength);
		printf("shapeIndex: %lf\n", this->shapeIndex);
		printf("density:%lf\n", this->density);
		printf("brightnessBGRNIR:%lf\n", this->brightnessBGRNIR);
		printf("brightnessBGR:%lf\n", this->brightnessBGR);
		printf("NDVI:%lf\n", this->NDVI);
		printf("NDWI:%lf\n", this->NDWI);
		printf("BAI:%lf\n", this->BAI);
		printf("SBI:%lf\n\n", this->SBI);
		
	}

	//初始化形态信息
	//计算依赖协方差矩阵
	void formFeatureInit(int width)
	{
		vector<int>::iterator it;
		int sumx = 0;
		int sumy = 0;
		for(it = this->pixelLocation.begin(); it != this->pixelLocation.end(); it++)
		{
			sumx += (*it)/width;
			sumy += (*it)%width;
		}
		double meanx = 0, meany = 0;
		meanx = sumx/(double)this->pixelLocation.size();
		meany = sumy/(double)this->pixelLocation.size();
		double finx = 0, finy = 0, finxy = 0;
		for(it = this->pixelLocation.begin(); it != this->pixelLocation.end(); it++)
		{
			finx += ((*it)/width - meanx)*((*it)/width - meanx);
			finy += ((*it)%width - meany)*((*it)%width - meany);
			finxy += ((*it)/width - meanx) * ((*it)%width - meany);
		}
		this->varxx = finx/(this->pixelLocation.size()-1);
		this->varyy = finy/(this->pixelLocation.size()-1);
		this->covxy = finxy/(this->pixelLocation.size()-1);

		this->shapeIndex = (double)borderLength/(4*sqrt((double)this->pixelnum));
		this->density = sqrt((double)this->pixelnum)/(1+sqrt(varxx+varyy));
	}

	//初始化光谱信息
	void spectralFeatureInit()
	{
		this->NDVI = (this->avgNIR - this->avgR)/(double)(this->avgNIR + this->avgR);
		this->NDWI = (this->avgG - this->avgNIR)/(double)(this->avgG + this->avgNIR);
		this->brightnessBGRNIR = (this->avgB + this->avgG + this->avgR + this->avgNIR)/4;
		this->brightnessBGR = (this->avgB + this->avgG + this->avgR)/3;
		this->BAI = (this->avgB - this ->avgNIR)/(this->avgB + this->avgNIR);
		this->SBI = sqrt(this->avgR*this->avgR + this->avgNIR*this->avgNIR);
	}



protected:
private:
};


//遍历出某个等级的全部结点
//从头结点开始，任何小于等于该等级的结点均为“该等级结点”
void searchTreeNodeWithLevel(BTreeNode* hierarchicalTree, int level, int superPixelNum)
{
	if(hierarchicalTree->level <= level)
		printf("%d -> ",hierarchicalTree->ID);
	if(hierarchicalTree->level > level && hierarchicalTree->left != NULL)
		searchTreeNodeWithLevel(hierarchicalTree->left, level, superPixelNum);
	if(hierarchicalTree->level > level && hierarchicalTree->right != NULL)
		searchTreeNodeWithLevel(hierarchicalTree->right, level, superPixelNum);
};

//设置某结点所有基层结点的子像素值
void setNowLevelNodeValue(int *labels, int &setValue, BTreeNode* htNode, CSuperPixelSet* csps)
{
	if (htNode->level == 1)
	{
		vector<int>::iterator it;
		for(it = csps[htNode->ID].pixelLocation.begin(); it != csps[htNode->ID].pixelLocation.end(); it++)
			labels[*it] = setValue;
	}
	if(htNode->level > 1 && htNode->left != NULL)
		setNowLevelNodeValue(labels, setValue, htNode->left, csps);
	if(htNode->level > 1 && htNode->right != NULL)
		setNowLevelNodeValue(labels, setValue, htNode->right, csps);
}

//遍历某等级下的所有结点
void setAllNodeValue(int *labels, int level, BTreeNode* hierarchicalTree, int &setValue, CSuperPixelSet* csps)
{
	if(hierarchicalTree->level <= level)
	{
		setValue++;
		setNowLevelNodeValue(labels, setValue, hierarchicalTree, csps);
	}
	if(hierarchicalTree->level > level && hierarchicalTree->left != NULL)
		setAllNodeValue(labels, level, hierarchicalTree->left, setValue, csps);
	if(hierarchicalTree->level > level && hierarchicalTree->right != NULL)
		setAllNodeValue(labels, level, hierarchicalTree->right, setValue, csps);
}

//建立新对象结点集合
void createNewObjectSet(int* newLabels, cv::Mat &srimg, ObjectNode* oNode, int objectNum, int width, int height)
{
	for (int i = 0;i<height;i++)
		for (int j = 0;j<width;j++)
		{
			oNode[newLabels[i*width+j]].pixelLocation.push_back(i*width+j);  //建立超像素与其中像素的关系
			oNode[newLabels[i*width+j]].pixelnum++;
			oNode[newLabels[i*width+j]].avgB += srimg.data[(i*width+j)*4];
			oNode[newLabels[i*width+j]].avgG += srimg.data[(i*width+j)*4+1];
			oNode[newLabels[i*width+j]].avgR += srimg.data[(i*width+j)*4+2];
			oNode[newLabels[i*width+j]].avgNIR += srimg.data[(i*width+j)*4+3];
		}
	for(int i = 0; i< objectNum;i++)
		{
			oNode[i].avgB /= oNode[i].pixelnum;
			oNode[i].avgG /= oNode[i].pixelnum;
			oNode[i].avgR /= oNode[i].pixelnum;
			oNode[i].avgNIR /= oNode[i].pixelnum;
			oNode[i].id = i;
			oNode[i].objectTypes = 0; //定义地物类为未定义
			//oNode[i].formFeatureInit(width);
		}
}

//建立新的邻接图
//同时计算各个对象边长
void createNewToplogicalGraph(int *newLabels, int width, int height, ArrayHeadGraphNode* newAHGn, int objectNum, ObjectNode* oNode)
{
	for (int i = 0; i<height; i++)
		oNode[newLabels[i*width+width-1]].borderLength++;
	for(int j = 0; j<width-1; j++)
		oNode[newLabels[(height-1)*width+j]].borderLength++;
	for (int i = 0; i<height - 1; i++)
		for (int j = 0; j < width - 1; j++)
		{
			if(i == 0 || j == 0)
				oNode[newLabels[i*width+j]].borderLength++;
			if (newLabels[i*width + j] != newLabels[i*width + j + 1])
			{
				oNode[newLabels[i*width+j]].borderLength++;
				forward_list<GraphNode>::iterator it;
				int check = 0; //0为不存在
				for (it = newAHGn[newLabels[i*width + j]].pGraphNodeList.begin(); it != newAHGn[newLabels[i*width + j]].pGraphNodeList.end(); it++)
					if (it->ID == newLabels[i*width+j+1])
					{
						check = 1;
						break;
					}
					if(check == 0)
					{
						newAHGn[newLabels[i*width + j]].pGraphNodeList.push_front(newLabels[i*width + j + 1]);
						newAHGn[newLabels[i*width + j + 1]].pGraphNodeList.push_front(newLabels[i*width + j]);
					}
			}

			if (newLabels[i*width + j] != newLabels[(i+1)*width+j])
			{
				oNode[newLabels[i*width+j]].borderLength++;
				forward_list<GraphNode>::iterator it;
				int check = 0; //0为不存在
				for (it = newAHGn[newLabels[i*width + j]].pGraphNodeList.begin(); it != newAHGn[newLabels[i*width + j]].pGraphNodeList.end(); it++)
					if (it->ID == newLabels[(i+1)*width+j])
					{
						check = 1;
						break;
					}
					if(check == 0)
					{
						newAHGn[newLabels[i*width + j]].pGraphNodeList.push_front(newLabels[(i+1)*width+j]);
						newAHGn[newLabels[(i+1)*width+j]].pGraphNodeList.push_front(newLabels[i*width + j]);
					}
			}
		}
}

