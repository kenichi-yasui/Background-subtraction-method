#ifndef CDETECT_NONLINEAR_H_
#define _CDETECT_NONLINEAR_H_
#include "CDetectDynamicBack.h"

//�w�i�X�V���@
enum UpdateMethod{NotUpdate=0,DynamicBack,etc};

class CDetectNonlinear : public CDetectDynamicBack
{
public:
	CDetectNonlinear(int Lambda,int n,int m,UpdateMethod Flag,int Width,int Height);

protected:
	int mM;			//����`�l
	UpdateMethod mUpdateMethodFlag;	//�w�i�X�V���@

	unsigned char mNonlinearY[256];
	unsigned char virtual getNonlinearY(unsigned char Y);
	int OnePixelUpdatedBack(unsigned char *pPixel,int *pBack,int x,int y,bool bDetect);
	bool OnePixelDetect(unsigned char *pPixel,int *pBack,int x,int y);
private:
	void NonlinearFormat(void);

};
#endif