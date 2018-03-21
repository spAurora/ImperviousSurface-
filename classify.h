
bool classifyByNDVI(ObjectNode* oNode, double lowerLimit, double upperLimit)
{
	return (oNode->NDVI>=lowerLimit && oNode->NDVI <= upperLimit) ? true : false;
}

bool classifyByNDWI(ObjectNode* oNode, double lowerLimit, double upperLimit)
{
	//return (oNode->NDWI>=lowerLimit && oNode->NDWI <= upperLimit)? true : false;
	//printf(" get in NDWI fun\n");
	if(oNode->NDWI >= lowerLimit && oNode->NDWI <= upperLimit)
		return true;
	return false;
}

bool classifyByBAI(ObjectNode* oNode, double lowerLimit, double upperLimit)
{
	return (oNode->BAI >= lowerLimit && oNode->BAI <= upperLimit)? true : false;
}

bool classifyBySBI(ObjectNode* oNode, double lowerLimit, double upperLimit)
{
	return (oNode->SBI >= lowerLimit && oNode->SBI <= upperLimit)? true : false;
}

bool classifyByBrightnessBGR(ObjectNode* oNode, double lowerLimit, double upperLimit)
{
	return (oNode->brightnessBGR >= lowerLimit && oNode->brightnessBGR <= upperLimit)? true : false;
}

bool classifyByBrightnessBGRNIR(ObjectNode* oNode, double lowerLimit, double upperLimit)
{
	return (oNode->brightnessBGRNIR >= lowerLimit && oNode->brightnessBGRNIR <= upperLimit)? true : false;
}

bool classifyByArea(ObjectNode* oNode, double lowerLimit, double upperLimit)
{
	return (oNode->pixelnum >= lowerLimit && oNode->pixelnum <= upperLimit)? true : false;
}

bool classifyByShapeIndex(ObjectNode* oNode, double lowerLimit, double upperLimit)
{
	return (oNode->shapeIndex >= lowerLimit && oNode->shapeIndex <= upperLimit)? true : false;
}

bool classifyByDensity(ObjectNode* oNode, double lowerLimit, double upperLimit)
{
	return (oNode->density >= lowerLimit && oNode->density <= upperLimit)? true : false;
}
//����ĳ��������ĳ�����ת������һ�����
void changeNodeTypes(int changeFrom, bool (*fp)(ObjectNode*, double, double), int changeTo, int objectNum, int width, ObjectNode* oNode, double lowerLimit, double upperLimit)
{
	int check = 0;
	
	for (int i = 0; i<objectNum; i++)
		if (oNode[i].objectTypes == changeFrom)
		{
			
			//���֮ǰû�г�ʼ���ͳ�ʼ��
			if (oNode[i].haveInit == 0)
			{
				oNode[i].formFeatureInit(width);
				oNode[i].spectralFeatureInit();
				oNode[i].haveInit = 1;
			}
			if (fp(&oNode[i], lowerLimit, upperLimit) == true)
			{
				oNode[i].objectTypes = changeTo;
				printf("%lf\n", oNode[i].NDWI);
				check++;
			}
		}
		printf("ObjectNum in change fun: %d\n", check);
}
