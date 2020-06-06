
#include "CDetectNonlinear.h"

CDetectNonlinear::CDetectNonlinear(int Lambda,int n,int m,UpdateMethod Flag,int Width,int Height) 
: CDetectDynamicBack(Lambda,n,Width,Height) 
{
	mUpdateMethodFlag=Flag;
	mM=m;
	NonlinearFormat();
}
/********************************************
	NonlinearFormat
	0～255まで輝度成分を非線形処理にかけ、
	配列に代入
*************************************/
void CDetectNonlinear::NonlinearFormat(void)
{
	int y;
	for(y=0;y<256;y++)
		mNonlinearY[y]=getNonlinearY(y);

}
/********************************************
	非線形処理
	 処理式 (Y/255)^(1/M) *255
/********************************************/
unsigned char CDetectNonlinear::getNonlinearY(unsigned char Y)
{
	int newY=(int)(pow(Y/255.0,1.0/mM)*255);	
	if(newY>255)
		return 255;
	else if(newY<0)
		return 0;
	return newY;
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
bool CDetectNonlinear::OnePixelDetect(unsigned char *pPixel,int *pBack,int x,int y)
{
	if(mFrameCount==0)
		return false;
	unsigned char r,g,b;
	int Y[2];
	unsigned char *pP=pPixel;
	b=*pP++;
	g=*pP++;
	r=*pP++;
	Y[0]=mNonlinearY[ getY(r,g,b)];//Ｙ成分抽出
	Y[1]=mNonlinearY[ CReadBmp::Jougen(*pBack/mN,255,0)];
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
int CDetectNonlinear::OnePixelUpdatedBack(unsigned char *pPixel,int *pBack,int x,int y,bool bDetect)
{
	if(mUpdateMethodFlag==DynamicBack)
		return  CDetectDynamicBack::OnePixelUpdatedBack(pPixel,pBack,x,y,bDetect);
	return 0;
}
