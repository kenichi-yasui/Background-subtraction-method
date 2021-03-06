#include "CDetectDynamicBack.h"

CDetectDynamicBack::CDetectDynamicBack(int Lambda,int n,int Width,int Height) :CDetectBase(Lambda,Width,Height) 
{
	mN=n;
	mPixelPlus=1;
}

/********************************
	setBack
	背景画像を設定
	引数
		pBack　設定される背景画像
		Width Height 背景画像の横幅、縦幅
***********************************/
int CDetectDynamicBack::setBack(unsigned char *pBack,int Width,int Height)
{
	//元の背景画像とサイズが違う場合、新規で作成
	if(Width!=mWidth || mHeight!=Height || mpBack==NULL)
		BackNew(Width,Height);
	int *pNewBack=mpBack;
	int r,g,b,i;
	for(i=0;i<mHeight*mWidth;i++)
	{
		if(mPixelPlus==1)	//輝度成分のみ保持している場合
		{
			b=*pBack++;
			g=*pBack++;
			r=*pBack++;
			*pNewBack++=getY(r,g,b)*mN;
		}
	}
	return 0;
}
/**************************************
	getBack
	背景画像を返す関数
	背景画像を格納する配列
	戻り値：背景画像
**************************************/
unsigned char *CDetectDynamicBack::getBack(unsigned char *pBack)
{
	if(pBack==NULL)
		pBack=new  unsigned char[mHeight*mWidth*3];
	unsigned char *pBackFirst=pBack;
	int *pBackY;
	int rgb[3];
	int i,j;
	pBackY=this->mpBack;
	static int no=0;
	for(i=0;i<mHeight*mWidth;i++)
	{
		if(mPixelPlus==1)	//輝度成分のみ保持している場合
		{
			for(j=0;j<3;j++)
				rgb[j]=*pBackY;
			pBackY++;
		}
		else
		{
			for(j=0;j<3;j++)
				rgb[j]=*pBackY++;
		}
		for(j=0;j<3;j++)
			*pBack++=(unsigned char)(rgb[j]/mN);
	}
	return pBackFirst;
}

/************************************
	1ピクセルだけ移動物体を検出する
	引数
		pPixel 検出対象画素(Blue,Green,Blue)
		pBack  背景（Y成分のみ)
		x,y    座標
	戻り値
	　true  移動物体領域
	  false 背景領域
***********************************/
bool CDetectDynamicBack::OnePixelDetect(unsigned char *pPixel,int *pBack,int x,int y)
{
	if(mFrameCount==0)
		return false;
	unsigned char r,g,b;
	int Y[2];
	unsigned char *pP=pPixel;
	b=*pP++;
	g=*pP++;
	r=*pP++;
	Y[0]=getY(r,g,b);//Ｙ成分抽出
	Y[1]=*pBack/mN;
	if(abs(Y[0]-Y[1])>mLambda)//画素と背景との誤差が閾値以上の領域を移動物体として検出
		return true;
	else
		return false;
	return false;
}

/************************************
	1ピクセルだけ背景を更新する
	引数
		pPixel 画素(Blue,Green,Blue)
		pBack  背景（Y成分のみ)
		x,y    座標
	戻り値
	　true  移動物体領域
	  false 背景領域
***********************************/
int CDetectDynamicBack::OnePixelUpdatedBack(unsigned char *pPixel,int *pBack,int x,int y,bool bDetect)
{
	if(mFrameCount==0)
		return 0;
	if(bDetect)
		return 0;
	
	int r,g,b;
	b=*pPixel++;
	g=*pPixel++;
	r=*pPixel++;
	int Y=getY(r,g,b);
	*pBack=Y+(*pBack)*(mN-1)/mN;
	return 0;
}