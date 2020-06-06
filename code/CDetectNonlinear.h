#ifndef CDETECT_NONLINEAR_H_
#define _CDETECT_NONLINEAR_H_
#include "CDetectDynamicBack.h"

//背景更新方法
enum UpdateMethod{NotUpdate=0,DynamicBack,etc};

class CDetectNonlinear : public CDetectDynamicBack
{
public:
	CDetectNonlinear(int Lambda,int n,int m,UpdateMethod Flag,int Width,int Height);

protected:
	int mM;			//非線形値
	UpdateMethod mUpdateMethodFlag;	//背景更新方法

	unsigned char mNonlinearY[256];
	unsigned char virtual getNonlinearY(unsigned char Y);
	int OnePixelUpdatedBack(unsigned char *pPixel,int *pBack,int x,int y,bool bDetect);
	bool OnePixelDetect(unsigned char *pPixel,int *pBack,int x,int y);
private:
	void NonlinearFormat(void);

};
#endif