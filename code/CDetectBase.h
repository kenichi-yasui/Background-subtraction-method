
#ifndef _CDETECT_BASE_H_
#define _CDETECT_BASE_H_
#include "CReadBmp2006.h"
#define CDETECT_VERSION 2.0

class CAdditionalProcessing;
class CDetectBase
{
public:
	CDetectBase(int Lambda,int Width,int Height);
	~CDetectBase();
	int Detect(unsigned char *pPixel);
	unsigned char *DetectExtraction(unsigned char *pChar,int Width,int Height);
	unsigned char *DetectMark(unsigned char *pBackBmp,unsigned char *pChar,int BackWidth,int BackHeight,int CharWidth,int CharHeight);
	bool *getMark(){return mbMarks;};		//���o�̈�擾
	int getCount(void){return mDetectCount;};//���o��f���擾
	int getHeight(){ return mHeight;};		//�w�i�摜�蕝�擾
	int getWidth(){ return mWidth;};		//�w�i�摜�����擾
	int getLambda(void){return mLambda;};	//臒l�擾

	double getAveDY(){ return  mDetectCount/(mWidth*mHeight);};
	int virtual BackReset(int PixelPlus=0,int R=0,int G=0,int B=0);
	int virtual MarkReset(bool bMark=false);
	int getY(int r,int g,int b);
	int getCb(int r,int g,int b);
	int getCr(int r,int g,int b);
	void virtual Clear(void);
	unsigned char virtual *getBack(unsigned char *pBack=NULL);
	unsigned char virtual *getBack(CReadBmp *pBmp);
	int virtual setBack(unsigned char *pBack,int Width,int Height);
	void setPostprocessing(CAdditionalProcessing *pProcessing){mCAdditionalProcessing=pProcessing;};

protected :
	int mLambda;//臒l
	bool *mbMarks;//���o�����̈�
	int mDetectCount;//���o������f��
	unsigned long int mFrameCount;//���o�����t���[����
	int mHeight;//�c��
	int mWidth;//����
	int *mpBack;//�w�i�̈�
	int mPixelPlus;
	bool virtual OnePixelDetect(unsigned char *pPixel,int *pBack,int x,int y);
	int virtual OnePixelUpdatedBack(unsigned char *pPixel,int *pBack,int x,int y,bool bDetect);
	int virtual Preprocessing(unsigned char *pPixel);	//�O����
	int virtual Postprocessing(unsigned char *pPixel);	//�㏈��
	int BackNew(int Width,int Height);
	CAdditionalProcessing *mCAdditionalProcessing;
};
#endif