#include "CDetectNorme.h"

CDetectNorme::CDetectNorme (int Lambda,int n,int m,double ColorTwice,
							UpdateMethod Flag,int Width,int Height)
: CDetectNonlinear(Lambda,n,m,Flag,Width,Height)
{
	mColorTwice=ColorTwice;
	mPixelPlus=3;
}


/************************************
	1ƒsƒNƒZƒ‹‚¾‚¯ˆÚ“®•¨‘Ì‚ğŒŸo‚·‚é
	ˆø”
		pPixel ŒŸo‘ÎÛ‰æ‘f(Blue,Green,Blue)
		pBack  ”wŒiiY¬•ª‚Ì‚İ)
		x,y    À•W
	–ß‚è’l
	@true  ˆÚ“®•¨‘Ì—Ìˆæ
	  false ”wŒi—Ìˆæ
***********************************/
bool CDetectNorme::OnePixelDetect(unsigned char *pPixel,int *pBack,int x,int y)
{
	if(mFrameCount==0)
		return false;
	unsigned char r[2],g[2],b[2];
	int Y[2],Cb[2],Cr[2];
	unsigned char *pP=pPixel;
	int *pB=pBack;
	int YCbCr;
	b[0]=*pP++;
	g[0]=*pP++;
	r[0]=*pP++;

	b[1]=*pB++/mN;
	g[1]=*pB++/mN;
	r[1]=*pB++/mN;

	int delta,i;
	for(i=0;i<2;i++)
	{
		Y[i]=mNonlinearY[ getY(r[i],g[i],b[i])];//‚x¬•ª’Šo
		Cb[i]=getCb(r[i],g[i],b[i]);
		Cr[i]=getCr(r[i],g[i],b[i]);
	}
	YCbCr=(Y[0]-Y[1])*(Y[0]-Y[1]);
	if(mColorTwice==1)
		YCbCr+=(Cb[0]-Cb[1])*(Cb[0]-Cb[1])+(Cr[0]-Cr[1])*(Cr[0]-Cr[1]);
	else if(mColorTwice!=0)
		YCbCr+=(int)(mColorTwice*(Cb[0]-Cb[1])*(Cb[0]-Cb[1])+mColorTwice*(Cr[0]-Cr[1])*(Cr[0]-Cr[1]));
	delta=(int)sqrt((double)YCbCr);
	if(delta>mLambda)//‰æ‘f‚Æ”wŒi‚Æ‚ÌŒë·‚ªè‡’lˆÈã‚Ì—Ìˆæ‚ğˆÚ“®•¨‘Ì‚Æ‚µ‚ÄŒŸo
		return true;
	else
		return false;
	return false;
}
/************************************
	1ƒsƒNƒZƒ‹‚¾‚¯”wŒi‚ğXV‚·‚é
	ˆø”
		pPixel ‰æ‘f(Blue,Green,Blue)
		pBack  ”wŒiiY¬•ª‚Ì‚İ)
		x,y    À•W
	–ß‚è’l
	@true  ˆÚ“®•¨‘Ì—Ìˆæ
	  false ”wŒi—Ìˆæ
***********************************/
int CDetectNorme::OnePixelUpdatedBack(unsigned char *pPixel,int *pBack,int x,int y,bool bDetect)
{
	if(mFrameCount==0)
		return 0;
	if(bDetect)
		return 0;
	if(mUpdateMethodFlag==DynamicBack)
	{
		int r,g,b;
		b=*pPixel++;
		g=*pPixel++;
		r=*pPixel++;
		*pBack++=b+(*pBack)*(mN-1)/mN;
		*pBack++=g+(*pBack)*(mN-1)/mN;
		*pBack++=r+(*pBack)*(mN-1)/mN;
	}
	return 0;
}
/********************************
	setBack
	”wŒi‰æ‘œ‚ğİ’è
	ˆø”
		pBack@İ’è‚³‚ê‚é”wŒi‰æ‘œ
		Width Height ”wŒi‰æ‘œ‚Ì‰¡•Ac•
***********************************/
int CDetectNorme::setBack(unsigned char *pBack,int Width,int Height)
{
	mPixelPlus=3;
	//Œ³‚Ì”wŒi‰æ‘œ‚ÆƒTƒCƒY‚ªˆá‚¤ê‡AV‹K‚Åì¬
	if(Width!=mWidth || mHeight!=Height || mpBack==NULL)
		BackNew(Width,Height);
	int *pNewBack=mpBack;
	int i,j;
	for(i=0;i<mHeight*mWidth;i++)
	{
		for(j=0;j<3;j++,pBack++)
			*pNewBack++=*pBack*mN;
	}	
	return 0;
}