#include "CDetectDynamicBack.h"

CDetectDynamicBack::CDetectDynamicBack(int Lambda,int n,int Width,int Height) :CDetectBase(Lambda,Width,Height) 
{
	mN=n;
	mPixelPlus=1;
}

/********************************
	setBack
	”wŒi‰æ‘œ‚ğİ’è
	ˆø”
		pBack@İ’è‚³‚ê‚é”wŒi‰æ‘œ
		Width Height ”wŒi‰æ‘œ‚Ì‰¡•Ac•
***********************************/
int CDetectDynamicBack::setBack(unsigned char *pBack,int Width,int Height)
{
	//Œ³‚Ì”wŒi‰æ‘œ‚ÆƒTƒCƒY‚ªˆá‚¤ê‡AV‹K‚Åì¬
	if(Width!=mWidth || mHeight!=Height || mpBack==NULL)
		BackNew(Width,Height);
	int *pNewBack=mpBack;
	int r,g,b,i;
	for(i=0;i<mHeight*mWidth;i++)
	{
		if(mPixelPlus==1)	//‹P“x¬•ª‚Ì‚İ•Û‚µ‚Ä‚¢‚éê‡
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
	”wŒi‰æ‘œ‚ğ•Ô‚·ŠÖ”
	”wŒi‰æ‘œ‚ğŠi”[‚·‚é”z—ñ
	–ß‚è’lF”wŒi‰æ‘œ
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
		if(mPixelPlus==1)	//‹P“x¬•ª‚Ì‚İ•Û‚µ‚Ä‚¢‚éê‡
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
	1ƒsƒNƒZƒ‹‚¾‚¯ˆÚ“®•¨‘Ì‚ğŒŸo‚·‚é
	ˆø”
		pPixel ŒŸo‘ÎÛ‰æ‘f(Blue,Green,Blue)
		pBack  ”wŒiiY¬•ª‚Ì‚İ)
		x,y    À•W
	–ß‚è’l
	@true  ˆÚ“®•¨‘Ì—Ìˆæ
	  false ”wŒi—Ìˆæ
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
	Y[0]=getY(r,g,b);//‚x¬•ª’Šo
	Y[1]=*pBack/mN;
	if(abs(Y[0]-Y[1])>mLambda)//‰æ‘f‚Æ”wŒi‚Æ‚ÌŒë·‚ªè‡’lˆÈã‚Ì—Ìˆæ‚ğˆÚ“®•¨‘Ì‚Æ‚µ‚ÄŒŸo
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