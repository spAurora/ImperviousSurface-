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