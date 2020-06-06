#include "CDetectNonlinear.h"

class CDetectNorme  : public CDetectNonlinear
{
public:
	CDetectNorme(int Lambda,int n,int m,double ColorTwice,UpdateMethod Flag,int Width,int Height);
	int OnePixelUpdatedBack(unsigned char *pPixel,int *pBack,int x,int y,bool bDetect);
	bool OnePixelDetect(unsigned char *pPixel,int *pBack,int x,int y);
	int setBack(unsigned char *pBack,int Width,int Height);
protected:
	double mColorTwice;
};
