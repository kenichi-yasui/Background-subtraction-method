/***************
	�X�V����
		setRGB�֐���ǉ�
		setYCbCr,getYCbCr�֐���ǉ�
***************/

/*---�C���N���[�h-------*/
#ifndef _CREADBMP2006_VERSION_
#define _CREADBMP2006_VERSION_ 1.9
#define RED 0
#define GREEN 1
#define BLUE 2
#include <windows.h>
#include <stdio.h>
/*----�v���g�^�C�v�錾---*/
class CReadBmp
{
public:

	CReadBmp();
	~CReadBmp();
	int CreateBmp(int width,int height,unsigned char *Gp=NULL);
	int ReadBmp(const char *FileName);
	int WriteBmp(const char *FileName);
	unsigned char getRGB(int x,int y,int type);
	int getRGB(int x,int y,unsigned char *R,unsigned char *G,unsigned char *B);
	int setRGB(int x,int y,unsigned char R,unsigned char G,unsigned char B);
	double getYIQ(int x,int y,int type);
	double getYCbCr(int x,int y,int type);
	int getYCbCr(int x,int y,double *Y,double *Cb,double *Cr);
	int setYCbCr(int x,int y,double Y,double Cb,double Cr);
	int printBmp(FILE *fp=stdout);
	void AllDelete();
	int getWidth(void);
	int getHeight(void);
	unsigned char *getRgbP(void){return mColorP;};
	int getBitCount(void);
	static int Jougen(int num,int max,int min);
	CReadBmp *BmpCopy(CReadBmp *Bmp);
	friend CReadBmp;
	BITMAPINFOHEADER *getBmpInfoHeader(){return &mBmpInfoHeader;};
	RGBQUAD			*getRGBQUAD(){return mRGBQuad;};
protected:
	BITMAPFILEHEADER	mBmpFileHeader;
	BITMAPINFOHEADER	mBmpInfoHeader;
	RGBQUAD				*mRGBQuad;
	int mRGBQuadMax;
	int mImgSize;
	unsigned char *mColorP;
	int ReadPixel(FILE *fp);
	int ReadHeader(FILE *fp);
	int WriteHeader(FILE *fp);
	bool mBigE;

private:
	bool HeaderCheck(WORD Type,char *Bt1=NULL,char *Bt2=NULL);
};

int MaxMin(int Max,int Min,int num);
#endif 