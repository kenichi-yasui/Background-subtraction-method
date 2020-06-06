#ifndef _CDETECT_DYNAMIC_BACK_H_
#include "CDetectBase.h"
#include <math.h>
#include <stdio.h>

class CDetectDynamicBack : public CDetectBase
{
public:
	CDetectDynamicBack(int Lambda,int n,int Width,int Height);
	int setBack(unsigned char *pBack,int Width,int Height);
	unsigned char *getBack(unsigned char *pBack=NULL);
	unsigned char *setBack(unsigned char *pBack);
	int getN(void){return mN;};
protected:
	int mN;
	int OnePixelUpdatedBack(unsigned char *pPixel,int *pBack,int x,int y,bool bDetect);
	bool OnePixelDetect(unsigned char *pPixel,int *pBack,int x,int y);

};
#endif