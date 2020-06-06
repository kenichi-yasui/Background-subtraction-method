#include "CAdditionalProcessing.h"
#include "CDetectBase.h"
#include <math.h>
#include <stdio.h>
#define DELETE_P(D) if(D){ delete [] D;  D=NULL;}

/******************************
	コンストラクタ
	引数
	Width  横幅
	Height 縦幅
******************************/
CDetectBase::CDetectBase(int Lambda,int Width,int Height)
{	
	mLambda=Lambda;	//閾値設定
	mWidth=Width;
	mHeight=Height;
	mPixelPlus=1;
	mpBack  =  NULL;//背景画像　メモリ確保
	mbMarks = NULL;//検出領域  　メモリ確保
	mDetectCount=0;//検出した画素数
	mFrameCount=0;//検出したフレーム数
	mCAdditionalProcessing=NULL;	//後処理クラス
}
/******************************
	デストラクタ
******************************/
CDetectBase::~CDetectBase()
{
	Clear();
}
/***************************
	背景画像リセット
	背景画像mpBackのＲＧＢを
	引数で指定した値で初期化する。
	指定しなかった場合、R=0,G=0,B=0で初期化される。
*******************************/
int CDetectBase::BackReset(int PixelPlus,int R,int G,int B)
{
	if(PixelPlus>0)
		mPixelPlus=PixelPlus;
	int x,y;
	if(mpBack==NULL)
		mpBack = new int   [mWidth*mHeight*mPixelPlus];
	int *p=mpBack;
	int Y=getY(R,G,B);
	for(y=0;y<mHeight;y++)
	{
		for(x=0;x<mWidth;x++)
		{
			if(mPixelPlus==3)
			{
				*p++=R;
				*p++=G;
				*p++=B;
			}
			else if(mPixelPlus==1)	//輝度成分のみ保持している場合
				*p++=Y;
		}
	}
	MarkReset(false);
	mFrameCount=0;
	return 0;
}
/***************************
	検出領域をリセット
	検出領域mbMarksを引数で指定した値に初期化する。
	指定しなかった場合falseで初期化される
*******************************/
int  CDetectBase::MarkReset(bool bMark)
{
	int x,y;
	if(mbMarks==NULL)
		mbMarks = new bool [mWidth*mHeight];
	bool *p=mbMarks;
	for(y=0;y<mHeight;y++)
		for(x=0;x<mWidth;x++)
			*p++=bMark;
	mFrameCount=0;
	return 0;
}
/*****************************
	メモリ解放用
*********************/
void CDetectBase::Clear()
{
	DELETE_P(mpBack)
	DELETE_P(mbMarks)
}
/******************************
	Detect
	移動物体検出
	引数
	Gp	検出対象画像
	Width　横幅
	Height 縦幅
	戻り値：背景画像
*****************************/
int CDetectBase::Detect(unsigned char *pPixel)
{
	if(!pPixel)
		return NULL;
	if(mpBack==NULL)
		 BackReset();
	if(mbMarks==NULL)	//検出領域の初期化
		MarkReset();
	int *pBack=mpBack;
	unsigned char *pPixelFirst=pPixel;
	int x,y;
	bool *pbMarks=mbMarks;
	mDetectCount=0;	//検出数初期化
    Preprocessing(pPixelFirst);	//前処理
	for(y=0;y<mHeight;y++)
	{
		for(x=0;x<mWidth;x++)
		{
			*pbMarks=OnePixelDetect(pPixel,pBack,x,y); //1ピクセル移動物体検出
			if(*pbMarks)
				mDetectCount++;
			OnePixelUpdatedBack(pPixel,pBack,x,y,*pbMarks);    //背景更新
			pBack+=mPixelPlus;
			pPixel+=3;
			pbMarks++;
		}//for(x=0;x<mWidth;x++)
	}//for(y=0;y<mHeight;y++)
	mFrameCount++;//検出したフレーム数カウント
	Postprocessing(pPixelFirst);//後処理
	return mDetectCount;
}
/************************************
	RGB成分をY成分に変換
*************************************/
int CDetectBase::getY(int r,int g,int b)
{
	int Y =(int)(0.2990*r+0.5870*g+0.1140*b);
	if(Y>255)
		return 255;
	else if(Y<0)
		return 0;
	return Y;
}
/************************************
	RGB成分をCb成分に変換
*************************************/
int CDetectBase::getCb(int r,int g,int b)
{
	int Cb =(int)(-0.16874 * r - 0.33126 * g + 0.50000 * b );
	if(Cb>255)
		return 255;
	else if(Cb<0)
		return 0;
	return Cb;
}
/************************************
	RGB成分をCr成分に変換
*************************************/
int CDetectBase::getCr(int r,int g,int b)
{
	int Cr =(int)(0.50000 * r - 0.41869 * g - 0.08131 * b);
	if(Cr>255)
		return 255;
	else if(Cr<0)
		return 0;
	return Cr;
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
bool CDetectBase::OnePixelDetect(unsigned char *pPixel,int *pBack,int x,int y)
{
	unsigned char r,g,b;
	int Y[2];
	unsigned char *pP=pPixel;
	b=*pP++;
	g=*pP++;
	r=*pP++;
	Y[0]=getY(r,g,b);//Ｙ成分抽出
	Y[1]=*pBack;
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
int CDetectBase::OnePixelUpdatedBack(unsigned char *pPixel,int *pBack,int x,int y,bool bDetect)
{
	return 0;
}
/******************************
	DetectMark
	人物と背景の合成をする関数
	引数
	BackBmp	背景画像
	pChar	人物画像
	BackWidth 背景画像横幅
	BackHeight　背景画像縦幅
	Width　人物画像横幅
	Height 人物画像縦幅
	戻り値　：　合成画像
*******************************/
unsigned char *CDetectBase::DetectMark(unsigned char *pBackBmp,unsigned char *pChar,int BackWidth,int BackHeight,int CharWidth,int CharHeight)
{
	if(mbMarks==NULL)
		return NULL;
	int j=0;
	unsigned char *pBackFirst=pBackBmp;	//背景画像初期位置
	bool *pM=mbMarks;
	int sa=CharWidth-BackWidth;
	int x,y;
	int Height,Width;
	
	if(CharHeight<=BackHeight)
		Height=CharHeight;
	else
		Height=BackHeight;
	if(CharWidth<=BackWidth)
		Width=CharWidth;
	else
		Width=BackWidth;
	//適当なエラー処理（後日修正予定）
	if(pM==NULL || pChar==NULL || pBackBmp==NULL)
		return NULL;
	for(y=0;y<Height;y++)
	{
		for(x=0;x<Width;x++,pM++)
		{
			if(*pM)	//　人物合成
			{
				for(j=0;j<3;j++)
					*pBackBmp++=*pChar++;
			}
			else	//背景領域
			{
				pBackBmp+=3;
				pChar+=3;
			}
		}
		if(sa>0)	//横幅の誤差
			pChar+=(3*sa);
		else if(sa<0)
			pBackBmp+=(-3*sa);
	}
	return pBackFirst;//背景画像初期位置
}
/*********************************
	DetectExtraction
	移動物体検出領域以外を白にする関数
	pChar	画像
	Width　横幅
	Height 縦幅
	戻り値：処理後の画像
********************************/
unsigned char *CDetectBase::DetectExtraction(unsigned char *pChar,int Width,int Height)
{
	if(mbMarks==NULL)
		return NULL;
	int j,x,y;
	unsigned char *pCharFirst=pChar;
	bool *pM=mbMarks;
	//適当なエラー処理（後日修正予定）
	if(pM==NULL || pChar==NULL )
		return NULL;
	if(mWidth<Width)
		Width=mWidth;
	if(mHeight<Height)
		Height=mHeight;
	int sa=mHeight-Height;
	for(y=0;y<Height;y++)
	{
		for(x=0;x<Width;x++,pM++)
		{
			if(*pM)		//移動物体領域
			{
				pChar+=3;
			}
			else		//背景領域
			{		
				for(j=0;j<3;j++)
					*pChar++=255;	
			}
		}
		if(sa>0)
			pM+=sa;
		else if(sa<0)
			pChar+=(-3*sa);
	}
	return pCharFirst;
}
/**************************************
	getBack
	背景画像を返す関数
	背景画像を格納する配列
	戻り値：背景画像
**************************************/
unsigned char *CDetectBase::getBack(unsigned char *pBack)
{
	if(pBack==NULL)
		pBack=new  unsigned char[mHeight*mWidth*3];
	unsigned char *pBackFirst=pBack;
	int *pBackY;
	int rgb[3];
	int i,j;

	pBackY=this->mpBack;
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
			*pBack++=(unsigned char)rgb[j];
	}
	return pBackFirst;
}
/********************************
	getBack
	背景画像を返す関数
	背景画像を格納するCReadBmpクラス
	戻り値：背景画像
***********************************/
unsigned char *CDetectBase::getBack(CReadBmp *pBmp)
{
	if(pBmp==NULL)
		return NULL;
	unsigned char *pF=getBack();
	pBmp->CreateBmp(mWidth,mHeight,pF);
	delete [] pF;

	return pBmp->getRgbP();
}
/********************************
	Preprocessing
	移動物体検出の前処理
***********************************/
int CDetectBase::Preprocessing(unsigned char *pPixel)
{
	if(mCAdditionalProcessing)
			return mCAdditionalProcessing->Preprocessing(this,pPixel);
	return 0;
}
/********************************
	Preprocessing
	移動物体検出の後処理
***********************************/
int CDetectBase::Postprocessing(unsigned char *pPixel)
{
	if(mCAdditionalProcessing)
			return mCAdditionalProcessing->Postprocessing(this,pPixel);
	return 0;
}
/********************************
	setBack
	背景画像を設定
	引数
		pBack　設定される背景画像
		Width Height 背景画像の横幅、縦幅
***********************************/
int CDetectBase::setBack(unsigned char *pBack,int Width,int Height)
{
	//元の背景画像とサイズが違う場合、新規で作成
	if(Width!=mWidth || mHeight!=Height || mpBack==NULL)
		BackNew(Width,Height);
	int *pNewBack=mpBack;
	int r,g,b,i,j;
	for(i=0;i<mHeight*mWidth;i++)
	{
		if(mPixelPlus==1)	//輝度成分のみ保持している場合
		{
			b=*pBack++;
			g=*pBack++;
			r=*pBack++;
			*pNewBack++=getY(r,g,b);
		}
		else
			for(j=0;j<3;j++)
				*pNewBack++=*pBack++;

	}
	return 0;
}
int CDetectBase::BackNew(int Width,int Height)
{
	mWidth=Width;
	mHeight=Height;
	if(mpBack)
		delete [] mpBack;
	if(mbMarks)
		delete [] mbMarks;
	mpBack = new int [mWidth*mHeight*mPixelPlus];
	MarkReset();
	return 0;
}
