#include "CBmpLabel.h"
#include <stdio.h>

CBmpLabel::CBmpLabel(int Width,int Height)
{
	mWidth=Width;
	mHeight=Height;
	mpLabel = new unsigned int [mWidth*mHeight];
	Reset();

	
}
CBmpLabel::~CBmpLabel()
{
	Delete();
}
void CBmpLabel::Delete()
{
	delete [] mpLabel;
	mpLabel=NULL;
	mMaxLabelNo=0;
	mMaxLabelData=0;
	mDMax=0;
}
void CBmpLabel::Reset()
{
	mMaxLabelNo=0;
	mMaxLabelData=0;
	mDMax=0;
	int x,y;
	unsigned int* p=mpLabel;
	for(y=0;y<mHeight;y++)
		for(x=0;x<mWidth;x++)
			*p++=0;
}
void CBmpLabel::LabelPutting(bool *pMark)
{
	if(!pMark)
		return;
	Reset();
	bool *pM=pMark;
	unsigned int *pLa=NULL,*pLb=NULL,*pL;
	pL=mpLabel;
	int x,y;
	mDMax=0;
	for(y=0;y<mHeight;y++)
	{
		pLb=NULL;
		for(x=0;x<mWidth;x++,pL++,pM++)
		{
			if(x==1)
				pLb=pL-1;
			if(*pM)
			{
				*pL=getLabelNo(pLa,pLb,x,y);
				mDMax++;
			}
			else
				*pL=0;
			if(pLb)
				pLb++;
			if(pLa)
				pLa++;
			}
		pLa=pL-mWidth;
	}



}
unsigned int CBmpLabel::getLabelNo(unsigned int *pLa,unsigned int *pLb,int x,int y)
{
	if(mMaxLabelNo==0)
		return 	setLabelFastData(x,y);
	if(pLa==NULL)
	{	
		if(*pLb==0)
			return 	setLabelFastData(x,y);
		else
			return 	setLabelAdd(*pLb,x,y);
	}
	else if(pLb==NULL)
	{
		
		if(*pLa==0)
			return setLabelFastData(x,y);
		else
			return 	setLabelAdd(*pLa,x,y);
	}
	else
	{
		if(*pLa==0 && *pLb==0)
			return setLabelFastData(x,y);
		else if(*pLa==0)
			return 	setLabelAdd(*pLb,x,y);
		else if(*pLb==0)
			return 	setLabelAdd(*pLa,x,y);
		else if(*pLa==*pLb)
			return 	setLabelAdd(*pLa,x,y);
		else	//*pLa>0 *pLb>0
		{
			LabelReset(*pLb,*pLa,x,y);
			return 	setLabelAdd(*pLa,x,y);
		}
	}
}
void CBmpLabel::LabelReset(unsigned int Before,unsigned int After,int LastX,int LastY)
{
	unsigned int *pL=mpLabel;
	int x,y;
	int MinY,MaxY,MinX,MaxX;
	MinY=getLabeMinY(Before);
	MaxY=getLabeMaxY(Before);
	MinX=getLabeMinX(Before);
	MaxX=getLabeMaxX(Before);
	for(y=MinY,pL+=MinY*mWidth;y<=MaxY;y++)
	{
		for(x=MinX,pL+=MinX;x<=MaxX;x++,pL++)
		{
			if(*pL==Before)
			{
				setLabelAdd(After,x,y);
				*pL=After;
			}
			if(x==LastX && y==LastY)
				return;
		}
		pL+=mWidth-MaxX-1;
	}	
	setLabelDelete(Before);
}
//LNoのmLabelDataの要素番号をゲッツ
int CBmpLabel::getLabelDataNo(int LNo)
{
	int i;
	for(i=0;i<mMaxLabelData;i++)
	{
		if(mLabelData[i].No==LNo)
			return i;
	}
	return mMaxLabelData;
}

//初期セット
int CBmpLabel::setLabelFastData(int X,int Y)
{
	++mMaxLabelNo;
	mLabelData[mMaxLabelData].No=mMaxLabelNo;
	mLabelData[mMaxLabelData].Max=1;
	mLabelData[mMaxLabelData].minX=mLabelData[mMaxLabelData].maxX=X;
	mLabelData[mMaxLabelData].minY=mLabelData[mMaxLabelData].maxY=Y;
	++mMaxLabelData;
	if(mMaxLabelData>=L_DATA_MAX)
	{
		printf("mMaxLabelData Max\n");
		--mMaxLabelData;
	}
	mLabelData[mMaxLabelData].No=0;
	return mMaxLabelNo;
}
//追加
int CBmpLabel::setLabelAdd(int LNo,int X,int Y)
{
	int LDNo=getLabelDataNo(LNo);
	mLabelData[LDNo].Max++;
	if(mLabelData[LDNo].maxX<X)
		mLabelData[LDNo].maxX=X;
	if(mLabelData[LDNo].minX>X)
		mLabelData[LDNo].minX=X;
	if(mLabelData[LDNo].maxY<Y)
		mLabelData[LDNo].maxY=Y;
	if(mLabelData[LDNo].minY>Y)
		mLabelData[LDNo].minY=Y;
	return LNo;
}
int CBmpLabel::setLabelDelete(int LNo)
{
	int LDNo=getLabelDataNo(LNo);
	int i;
	--mMaxLabelData;
	for(i=LDNo;i<mMaxLabelData;i++)
	{
		mLabelData[i].No=mLabelData[i+1].No;
		mLabelData[i].Max=mLabelData[i+1].Max;
		mLabelData[i].minX=mLabelData[i+1].minX;
		mLabelData[i].maxX=mLabelData[i+1].maxX;
		mLabelData[i].minY=mLabelData[i+1].minY;
		mLabelData[i].maxY=mLabelData[i+1].maxY;
	}
	return LNo;
}

int CBmpLabel::getLabeMinY(int LNo)
{
	return mLabelData[getLabelDataNo(LNo)].minY;
}
int CBmpLabel::getLabeMaxY(int LNo)
{
	return mLabelData[getLabelDataNo(LNo)].maxY;
}
int CBmpLabel::getLabeMinX(int LNo)
{
	return mLabelData[getLabelDataNo(LNo)].minX;
}
int CBmpLabel::getLabeMaxX(int LNo)
{
	return mLabelData[getLabelDataNo(LNo)].maxX;
}
int CBmpLabel::getLabeMax(int LNo)
{
	return mLabelData[getLabelDataNo(LNo)].Max;
}
////////////////
void CLabelRemovaNoise::RemovalNoise(int NWidth,int NHeight,int NMax)
{
	int i;	
	mDMax=0;
	for(i=0;i<mMaxLabelData;i++)
	{
		if(mLabelData[i].maxX-mLabelData[i].minX<NWidth)
			setLabelDelete(mLabelData[i--].No);
		else if(mLabelData[i].maxY-mLabelData[i].minY <NHeight)
			setLabelDelete(mLabelData[i--].No);
		else if(mLabelData[i].Max<NMax)
			setLabelDelete(mLabelData[i--].No);
		else
			mDMax+=mLabelData[i].Max;
		}
	unsigned int *pL;
	pL=mpLabel;
	int x,y;
	for(y=0;y<mHeight;y++)
		for(x=0;x<mWidth;x++,pL++)
			if(getLabelDataNo(*pL)==mMaxLabelData && *pL!=0)
				*pL=0;
}


void CLabelRemovaNoise::LabelPuttingBmp(CReadBmp *CBmp)
{
	if(CBmp->getRgbP()==NULL)
		return;
	Reset();
//	bool *pM=pMark;
	unsigned char M;
	unsigned int *pLa=NULL,*pLb=NULL,*pL;
	pL=mpLabel;
	int x,y;
	mDMax=0;
	for(y=0;y<mHeight;y++)
	{
		pLb=NULL;
		for(x=0;x<mWidth;x++,pL++)
		{
			if(x==1)
				pLb=pL-1;
			M=(int)CBmp->getYCbCr(x,y,0);
			if(M!=255)
			{
				*pL=getLabelNo(pLa,pLb,x,y);
				mDMax++;
			}
			else
				*pL=0;
			if(pLb)
				pLb++;
			if(pLa)
				pLa++;
		}
		pLa=pL-mWidth;
	}
}
void CLabelRemovaNoise::RemovalNoise(int LMax,CReadBmp *CBmp)
{
	int x,y;
	unsigned int *pL=mpLabel;
	unsigned int L;
	for(y=0;y<mHeight;y++)
	{
		for(x=0;x<mWidth;x++,pL++)
		{
			L=*pL;
			if(L==0)
			{
		//		CBmp.setRGB(x,y,255,255,255);
			}
			else
			{
			//	printf("%d %d\n",L,CLabel.getLabeMaxX(L));
				if(LMax>=getLabeMax(L))
				{			
					CBmp->setRGB(x,y,255,255,255);
				}
			}
		
		}
	}
}

void CLabelRemovaNoise::RemovalNoise(int LMax,bool *pMark)
{
	if(pMark==NULL)
		return;
	int x,y;
	bool *pM=pMark;
	unsigned int *pL=mpLabel;
	unsigned int L;

	for(y=0;y<mHeight;y++)
	{
		for(x=0;x<mWidth;x++,pL++)
		{
			L=*pL;
			if(L==0){}
			else if(LMax>=getLabeMax(L))
				*pM=false;
			pM++;
			
		}
	}
}
int CLabelRemovaNoise::Postprocessing(CDetectBase *pDetect,unsigned char *pPixel)
{
	if( ! ( pDetect->getWidth()==mWidth && pDetect->getHeight() == mHeight))
		return -1;
	bool *pbM=pDetect->getMark();
	if(pbM==NULL)
		return -2;
	if(mLMax<1)
		return -3;
	Reset();
	LabelPutting(pbM);
	RemovalNoise(mLMax,pbM);
	return 0;
}
CLabelRemovaNoiseBlank::CLabelRemovaNoiseBlank(int Width,int Height,RemovaType RemovaFlag,
		int LMax,int LMax2):CLabelRemovaNoise(Width,Height,LMax)
{
	mRemovaFlag=RemovaFlag;
	mLBlankMax=LMax2;
}
int CLabelRemovaNoiseBlank::Postprocessing(CDetectBase *pDetect,unsigned char *pPixel)
{
	if(mRemovaFlag==Not)
		return 0;
	bool *pbM=pDetect->getMark();
	int id=0;
	if(mRemovaFlag==BlankOnly)	//空白除去のみ
	{
		if( ! ( pDetect->getWidth()==mWidth && pDetect->getHeight() == mHeight))
			return -1;
		if(pbM==NULL)
			return -2;
		if(mLBlankMax<1)
			return -3;
	}
	else
	{	//ノイズ除去 NoiseOnly or NoiseAndBlank
		id=CLabelRemovaNoise::Postprocessing(pDetect,pPixel);
		if(id!=0)
			return id;
	}
	if(mRemovaFlag==NoiseOnly)
		return id;
	Reset();
	ReversesMark(pbM);
	LabelPutting(pbM);
	RemovalNoise(mLBlankMax,pbM);
	ReversesMark(pbM);
	return 0;
}
/*******************************
	ReversesMark
	マークを反転させる。
*******************************/
int CLabelRemovaNoiseBlank::ReversesMark(bool *pMark)
{
	if(pMark==NULL)
		return -1;
	bool* p=pMark;
	int x,y;
	for(y=0;y<mHeight;y++)
	{
		for(x=0;x<mWidth;x++,p++)
		{
			if(*p)
				*p=false;
			else
				*p=true;
		}
	}
	return 0;
}