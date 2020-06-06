#include "CDetectDynamicBack.h"

CDetectDynamicBack::CDetectDynamicBack(int Lambda,int n,int Width,int Height) :CDetectBase(Lambda,Width,Height) 
{
	mN=n;
	mPixelPlus=1;
}

/********************************
	setBack
	�w�i�摜��ݒ�
	����
		pBack�@�ݒ肳���w�i�摜
		Width Height �w�i�摜�̉����A�c��
***********************************/
int CDetectDynamicBack::setBack(unsigned char *pBack,int Width,int Height)
{
	//���̔w�i�摜�ƃT�C�Y���Ⴄ�ꍇ�A�V�K�ō쐬
	if(Width!=mWidth || mHeight!=Height || mpBack==NULL)
		BackNew(Width,Height);
	int *pNewBack=mpBack;
	int r,g,b,i;
	for(i=0;i<mHeight*mWidth;i++)
	{
		if(mPixelPlus==1)	//�P�x�����̂ݕێ����Ă���ꍇ
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
	�w�i�摜��Ԃ��֐�
	�w�i�摜���i�[����z��
	�߂�l�F�w�i�摜
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
			*pBack++=(unsigned char)(rgb[j]/mN);
	}
	return pBackFirst;
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
	Y[0]=getY(r,g,b);//�x�������o
	Y[1]=*pBack/mN;
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