
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
	0�`255�܂ŋP�x���������`�����ɂ����A
	�z��ɑ��
*************************************/
void CDetectNonlinear::NonlinearFormat(void)
{
	int y;
	for(y=0;y<256;y++)
		mNonlinearY[y]=getNonlinearY(y);

}
/********************************************
	����`����
	 ������ (Y/255)^(1/M) *255
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
	1�s�N�Z�������ړ����̂����o����
	����
		pPixel ���o�Ώۉ�f(Blue,Green,Blue)
		pBack  �w�i�iY�����̂�)
		x,y    ���W
	�߂�l
	�@true  �ړ����̗̈�
	  false �w�i�̈�
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
	Y[0]=mNonlinearY[ getY(r,g,b)];//�x�������o
	Y[1]=mNonlinearY[ CReadBmp::Jougen(*pBack/mN,255,0)];
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
int CDetectNonlinear::OnePixelUpdatedBack(unsigned char *pPixel,int *pBack,int x,int y,bool bDetect)
{
	if(mUpdateMethodFlag==DynamicBack)
		return  CDetectDynamicBack::OnePixelUpdatedBack(pPixel,pBack,x,y,bDetect);
	return 0;
}
