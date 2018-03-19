using namespace std;
class ObjectNode : public CSuperPixelSet
{
public:
	int objectTypes;  //�������
	
	//Э�������
	double varxx;
	double varyy;
	double covxy;
	int borderLength;
	double shapeIndex;
	double density;

	ObjectNode()
	{
		borderLength = 0;
		objectTypes = 0;//0δ���࣬1ˮ�壬2ֲ����3������4������5��·  
	}

	//��Ա����
	//��ȡ��̬��Ϣ
	//��ʼ��Э�������
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




protected:
private:
};


//������ĳ���ȼ���ȫ�����
//��ͷ��㿪ʼ���κ�С�ڵ��ڸõȼ��Ľ���Ϊ���õȼ���㡱
void searchTreeNodeWithLevel(BTreeNode* hierarchicalTree, int level, int superPixelNum)
{
	if(hierarchicalTree->level <= level)
		printf("%d -> ",hierarchicalTree->ID);
	if(hierarchicalTree->level > level && hierarchicalTree->left != NULL)
		searchTreeNodeWithLevel(hierarchicalTree->left, level, superPixelNum);
	if(hierarchicalTree->level > level && hierarchicalTree->right != NULL)
		searchTreeNodeWithLevel(hierarchicalTree->right, level, superPixelNum);
};

//����ĳ������л������ֵ
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

//�����¶����㼯��
void createNewObjectSet(int* newLabels, cv::Mat &srimg, ObjectNode* oNode, int objectNum, int width, int height)
{
	for (int i = 0;i<height;i++)
		for (int j = 0;j<width;j++)
		{
			oNode[newLabels[i*width+j]].pixelLocation.push_back(i*width+j);  //�������������������صĹ�ϵ
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
			oNode[i].objectTypes = 0; //���������Ϊδ����
			//oNode[i].formFeatureInit(width);
		}
}

//�����µ��ڽ�ͼ
//ͬʱ�����������߳�
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
				int check = 0; //0Ϊ������
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
				int check = 0; //0Ϊ������
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