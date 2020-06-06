#include "CAdditionalProcessing.h"
#include "CDetectBase.h"
#include <math.h>
#include <stdio.h>
#define DELETE_P(D) if(D){ delete [] D;  D=NULL;}

/******************************
	�R���X�g���N�^
	����
	Width  ����
	Height �c��
******************************/
CDetectBase::CDetectBase(int Lambda,int Width,int Height)
{	
	mLambda=Lambda;	//臒l�ݒ�
	mWidth=Width;
	mHeight=Height;
	mPixelPlus=1;
	mpBack  =  NULL;//�w�i�摜�@�������m��
	mbMarks = NULL;//���o�̈�  �@�������m��
	mDetectCount=0;//���o������f��
	mFrameCount=0;//���o�����t���[����
	mCAdditionalProcessing=NULL;	//�㏈���N���X
}
/******************************
	�f�X�g���N�^
******************************/
CDetectBase::~CDetectBase()
{
	Clear();
}
/***************************
	�w�i�摜���Z�b�g
	�w�i�摜mpBack�̂q�f�a��
	�����Ŏw�肵���l�ŏ���������B
	�w�肵�Ȃ������ꍇ�AR=0,G=0,B=0�ŏ����������B
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
			else if(mPixelPlus==1)	//�P�x�����̂ݕێ����Ă���ꍇ
				*p++=Y;
		}
	}
	MarkReset(false);
	mFrameCount=0;
	return 0;
}
/***************************
	���o�̈�����Z�b�g
	���o�̈�mbMarks�������Ŏw�肵���l�ɏ���������B
	�w�肵�Ȃ������ꍇfalse�ŏ����������
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
	����������p
*********************/
void CDetectBase::Clear()
{
	DELETE_P(mpBack)
	DELETE_P(mbMarks)
}
/******************************
	Detect
	�ړ����̌��o
	����
	Gp	���o�Ώۉ摜
	Width�@����
	Height �c��
	�߂�l�F�w�i�摜
*****************************/
int CDetectBase::Detect(unsigned char *pPixel)
{
	if(!pPixel)
		return NULL;
	if(mpBack==NULL)
		 BackReset();
	if(mbMarks==NULL)	//���o�̈�̏�����
		MarkReset();
	int *pBack=mpBack;
	unsigned char *pPixelFirst=pPixel;
	int x,y;
	bool *pbMarks=mbMarks;
	mDetectCount=0;	//���o��������
    Preprocessing(pPixelFirst);	//�O����
	for(y=0;y<mHeight;y++)
	{
		for(x=0;x<mWidth;x++)
		{
			*pbMarks=OnePixelDetect(pPixel,pBack,x,y); //1�s�N�Z���ړ����̌��o
			if(*pbMarks)
				mDetectCount++;
			OnePixelUpdatedBack(pPixel,pBack,x,y,*pbMarks);    //�w�i�X�V
			pBack+=mPixelPlus;
			pPixel+=3;
			pbMarks++;
		}//for(x=0;x<mWidth;x++)
	}//for(y=0;y<mHeight;y++)
	mFrameCount++;//���o�����t���[�����J�E���g
	Postprocessing(pPixelFirst);//�㏈��
	return mDetectCount;
}
/************************************
	RGB������Y�����ɕϊ�
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
	RGB������Cb�����ɕϊ�
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
	RGB������Cr�����ɕϊ�
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
	1�s�N�Z�������ړ����̂����o����
	����
		pPixel ���o�Ώۉ�f(Blue,Green,Blue)
		pBack  �w�i�iY�����̂�)
		x,y    ���W
	�߂�l
	�@true  �ړ����̗̈�
	  false �w�i�̈�
***********************************/
bool CDetectBase::OnePixelDetect(unsigned char *pPixel,int *pBack,int x,int y)
{
	unsigned char r,g,b;
	int Y[2];
	unsigned char *pP=pPixel;
	b=*pP++;
	g=*pP++;
	r=*pP++;
	Y[0]=getY(r,g,b);//�x�������o
	Y[1]=*pBack;
	if(abs(Y[0]-Y[1])>mLambda)//��f�Ɣw�i�Ƃ̌덷��臒l�ȏ�̗̈���ړ����̂Ƃ��Č��o
		return true;
	else
		return false;
	return false;
}
/************************************
	1�s�N�Z�������w�i���X�V����
	����
		pPixel ��f(Blue,Green,Blue)
		pBack  �w�i�iY�����̂�)
		x,y    ���W
	�߂�l
	�@true  �ړ����̗̈�
	  false �w�i�̈�
***********************************/
int CDetectBase::OnePixelUpdatedBack(unsigned char *pPixel,int *pBack,int x,int y,bool bDetect)
{
	return 0;
}
/******************************
	DetectMark
	�l���Ɣw�i�̍���������֐�
	����
	BackBmp	�w�i�摜
	pChar	�l���摜
	BackWidth �w�i�摜����
	BackHeight�@�w�i�摜�c��
	Width�@�l���摜����
	Height �l���摜�c��
	�߂�l�@�F�@�����摜
*******************************/
unsigned char *CDetectBase::DetectMark(unsigned char *pBackBmp,unsigned char *pChar,int BackWidth,int BackHeight,int CharWidth,int CharHeight)
{
	if(mbMarks==NULL)
		return NULL;
	int j=0;
	unsigned char *pBackFirst=pBackBmp;	//�w�i�摜�����ʒu
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
	//�K���ȃG���[�����i����C���\��j
	if(pM==NULL || pChar==NULL || pBackBmp==NULL)
		return NULL;
	for(y=0;y<Height;y++)
	{
		for(x=0;x<Width;x++,pM++)
		{
			if(*pM)	//�@�l������
			{
				for(j=0;j<3;j++)
					*pBackBmp++=*pChar++;
			}
			else	//�w�i�̈�
			{
				pBackBmp+=3;
				pChar+=3;
			}
		}
		if(sa>0)	//�����̌덷
			pChar+=(3*sa);
		else if(sa<0)
			pBackBmp+=(-3*sa);
	}
	return pBackFirst;//�w�i�摜�����ʒu
}
/*********************************
	DetectExtraction
	�ړ����̌��o�̈�ȊO�𔒂ɂ���֐�
	pChar	�摜
	Width�@����
	Height �c��
	�߂�l�F������̉摜
********************************/
unsigned char *CDetectBase::DetectExtraction(unsigned char *pChar,int Width,int Height)
{
	if(mbMarks==NULL)
		return NULL;
	int j,x,y;
	unsigned char *pCharFirst=pChar;
	bool *pM=mbMarks;
	//�K���ȃG���[�����i����C���\��j
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
			if(*pM)		//�ړ����̗̈�
			{
				pChar+=3;
			}
			else		//�w�i�̈�
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
	�w�i�摜��Ԃ��֐�
	�w�i�摜���i�[����z��
	�߂�l�F�w�i�摜
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
		if(mPixelPlus==1)	//�P�x�����̂ݕێ����Ă���ꍇ
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
	�w�i�摜��Ԃ��֐�
	�w�i�摜���i�[����CReadBmp�N���X
	�߂�l�F�w�i�摜
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
	�ړ����̌��o�̑O����
***********************************/
int CDetectBase::Preprocessing(unsigned char *pPixel)
{
	if(mCAdditionalProcessing)
			return mCAdditionalProcessing->Preprocessing(this,pPixel);
	return 0;
}
/********************************
	Preprocessing
	�ړ����̌��o�̌㏈��
***********************************/
int CDetectBase::Postprocessing(unsigned char *pPixel)
{
	if(mCAdditionalProcessing)
			return mCAdditionalProcessing->Postprocessing(this,pPixel);
	return 0;
}
/********************************
	setBack
	�w�i�摜��ݒ�
	����
		pBack�@�ݒ肳���w�i�摜
		Width Height �w�i�摜�̉����A�c��
***********************************/
int CDetectBase::setBack(unsigned char *pBack,int Width,int Height)
{
	//���̔w�i�摜�ƃT�C�Y���Ⴄ�ꍇ�A�V�K�ō쐬
	if(Width!=mWidth || mHeight!=Height || mpBack==NULL)
		BackNew(Width,Height);
	int *pNewBack=mpBack;
	int r,g,b,i,j;
	for(i=0;i<mHeight*mWidth;i++)
	{
		if(mPixelPlus==1)	//�P�x�����̂ݕێ����Ă���ꍇ
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
