
#ifndef _CBMP_LABEL_H_
#define _CBMP_LABEL_H_
#define CBMP_LABEL_VERSION 2.1
#include "CReadBmp2006.h"
#include "CAdditionalProcessing.h"

struct SLabelData
{
	int No;
	int Max;
	int minX;
	int maxX;
	int minY;
	int maxY;
};
#define L_DATA_MAX 1000
class CBmpLabel
{

protected:
	unsigned int *mpLabel;
	int mWidth;
	int mHeight;
	void Delete(void);
	void LabelReset(unsigned int Before,unsigned int After,int x,int y);
	unsigned int getLabelNo(unsigned int *pLa,unsigned int *pLb,int LastX,int LastY);
	unsigned int mMaxLabelNo;
	int mMaxLabelData;
	SLabelData mLabelData[L_DATA_MAX];
	int getLabelDataNo(int LNo);
	int mDMax;
public:
	CBmpLabel(int Width,int Height);
~CBmpLabel();
	unsigned int *getLabel(void){return mpLabel;};
	int getWidth(void){return mWidth;};
	int getHeight(void){return mHeight;};
	void Reset(void);
	void LabelPutting(bool *pMark);
	int getMaxLabelNo(void){return mMaxLabelNo;};
	int setLabelFastData(int X,int Y);
	int setLabelAdd(int LNo,int X,int Y);
	int setLabelDelete(int LNo);
	int getLabeMinY(int LNo);
	int getLabeMaxY(int LNo);
	int getLabeMinX(int LNo);
	int getLabeMaxX(int LNo);
	int getLabeMax(int LNo);
	int getDMax(void){return mDMax;}
	int getTotalLabel(void){return mMaxLabelData;};

};
class CLabelRemovaNoise : public CBmpLabel ,public CAdditionalProcessing
{
public:
	CLabelRemovaNoise(int Width,int Height,int LMax=1):CBmpLabel(Width,Height){mLMax=LMax;};
	void RemovalNoise(int NWidth,int NHeight,int NMax);
	void LabelPuttingBmp(CReadBmp *CBmp);
	void RemovalNoise(int LMax,CReadBmp *CBmp);
	void RemovalNoise(int LMax,bool *pMark);
	~CLabelRemovaNoise(){};
	int Postprocessing(CDetectBase *pDetect,unsigned char *pPixel);
	void setLMax(int LMax){mLMax=LMax;};
protected:
	int mLMax;
};
enum RemovaType{NoiseOnly=0,BlankOnly,NoiseAndBlank,Not=-1};

class CLabelRemovaNoiseBlank: public CLabelRemovaNoise 
{
public:
	CLabelRemovaNoiseBlank(int Width,int Height,RemovaType RemovaFlag,int LMax=1,int LMax2=1);
	int Postprocessing(CDetectBase *pDetect,unsigned char *pPixel);
protected:
	RemovaType mRemovaFlag;
	int ReversesMark(bool *pMark);
	int mLBlankMax;
};
#endif