#include "CDetectNorme.h"

CDetectNorme::CDetectNorme (int Lambda,int n,int m,double ColorTwice,
							UpdateMethod Flag,int Width,int Height)
: CDetectNonlinear(Lambda,n,m,Flag,Width,Height)
{
	mColorTwice=ColorTwice;
	mPixelPlus=3;
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
bool CDetectNorme::OnePixelDetect(unsigned char *pPixel,int *pBack,int x,int y)
{
	if(mFrameCount==0)
		return false;
	unsigned char r[2],g[2],b[2];
	int Y[2],Cb[2],Cr[2];
	unsigned char *pP=pPixel;
	int *pB=pBack;
	int YCbCr;
	b[0]=*pP++;
	g[0]=*pP++;
	r[0]=*pP++;

	b[1]=*pB++/mN;
	g[1]=*pB++/mN;
	r[1]=*pB++/mN;

	int delta,i;
	for(i=0;i<2;i++)
	{
		Y[i]=mNonlinearY[ getY(r[i],g[i],b[i])];//Ｙ成分抽出
		Cb[i]=getCb(r[i],g[i],b[i]);
		Cr[i]=getCr(r[i],g[i],b[i]);
	}
	YCbCr=(Y[0]-Y[1])*(Y[0]-Y[1]);
	if(mColorTwice==1)
		YCbCr+=(Cb[0]-Cb[1])*(Cb[0]-Cb[1])+(Cr[0]-Cr[1])*(Cr[0]-Cr[1]);
	else if(mColorTwice!=0)
		YCbCr+=(int)(mColorTwice*(Cb[0]-Cb[1])*(Cb[0]-Cb[1])+mColorTwice*(Cr[0]-Cr[1])*(Cr[0]-Cr[1]));
	delta=(int)sqrt((double)YCbCr);
	if(delta>mLambda)//画素と背景との誤差が閾値以上の領域を移動物体として検出
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
int CDetectNorme::OnePixelUpdatedBack(unsigned char *pPixel,int *pBack,int x,int y,bool bDetect)
{
	if(mFrameCount==0)
		return 0;
	if(bDetect)
		return 0;
	if(mUpdateMethodFlag==DynamicBack)
	{
		int r,g,b;
		b=*pPixel++;
		g=*pPixel++;
		r=*pPixel++;
		*pBack++=b+(*pBack)*(mN-1)/mN;
		*pBack++=g+(*pBack)*(mN-1)/mN;
		*pBack++=r+(*pBack)*(mN-1)/mN;
	}
	return 0;
}
/********************************
	setBack
	背景画像を設定
	引数
		pBack　設定される背景画像
		Width Height 背景画像の横幅、縦幅
***********************************/
int CDetectNorme::setBack(unsigned char *pBack,int Width,int Height)
{
	mPixelPlus=3;
	//元の背景画像とサイズが違う場合、新規で作成
	if(Width!=mWidth || mHeight!=Height || mpBack==NULL)
		BackNew(Width,Height);
	int *pNewBack=mpBack;
	int i,j;
	for(i=0;i<mHeight*mWidth;i++)
	{
		for(j=0;j<3;j++,pBack++)
			*pNewBack++=*pBack*mN;
	}	
	return 0;
}