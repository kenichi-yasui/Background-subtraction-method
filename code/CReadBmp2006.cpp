#include <stdio.h>
#include "CReadBmp2006.h"

/****************************
	�R���X�g���N�^

*****************************/
CReadBmp::CReadBmp()
{
	mRGBQuad = NULL;
	mColorP = NULL;
	HeaderCheck((WORD)(('B'<<8) | 'M'));
}
/****************************
	�f�X�g���N�^

*****************************/
CReadBmp::~CReadBmp()
{
	AllDelete();
}
/****************************
	getWidth :�@�摜�̉�����Ԃ��֐�
	getHeight:  �摜�̏c����Ԃ��֐�
	getBitCount: �摜�̃r�b�g����Ԃ��֐�
*****************************/
int CReadBmp::getWidth(void){return mBmpInfoHeader.biWidth;}
int CReadBmp::getHeight(void){return mBmpInfoHeader.biHeight;}
int CReadBmp::getBitCount(void){return mBmpInfoHeader.biBitCount;}
/****************************
	AllDelete
	��������S�ĉ������B
*****************************/
void CReadBmp::AllDelete()
{
	if(mRGBQuad)
		delete [] mRGBQuad;
	if(mColorP)
		delete [] mColorP;
	mRGBQuad = NULL;
	mColorP = NULL;
}
/****************
	getRGB
	��f�l��Ԃ��֐�
	���� :	x,y  ��f�l�̍��W(x,y)
			type ��f�̎��(0=�ԁA1=�΁A2=��)
	�߂�l : ��f�l
****************/
unsigned char CReadBmp::getRGB(int x,int y,int type)
{
	unsigned char color;
	if(mColorP==NULL)
		return 0;
	if(mBmpInfoHeader.biBitCount!=24)
	{
		//24�r�b�g�ȊO�͍��͔�Ή�
		return 0;
	}
	if(type >= 3  || type<0)
		return 0;
	x=Jougen(x,mBmpInfoHeader.biWidth-1,0);
	y=Jougen(y,mBmpInfoHeader.biHeight-1,0);
	color=mColorP[(x+y*mBmpInfoHeader.biWidth)*3+type];
	return color;
}
/******************************
	getRGB
	�w�肵�����W�̉�f���擾����
	����
	 x,y�@�擾�����f�̍��W
	 R,G,B�@�擾�����f��RGB�̊i�[��
	�߂�l
	 -1:�摜���ǂݍ���
	 -2:��Ή��摜
	  0:���퓮��
******************************/
int CReadBmp::getRGB(int x,int y,unsigned char *R,unsigned char *G,unsigned char *B)
{
	if(mColorP==NULL)
		return -1;
	if(mBmpInfoHeader.biBitCount!=24)
	{
		//24�r�b�g�ȊO�͍��͔�Ή�
		return -2;
	}
	x=Jougen(x,mBmpInfoHeader.biWidth-1,0);
	y=Jougen(y,mBmpInfoHeader.biHeight-1,0);
	*B=mColorP[(x+y*mBmpInfoHeader.biWidth)*3];
	*G=mColorP[(x+y*mBmpInfoHeader.biWidth)*3+1];
	*R=mColorP[(x+y*mBmpInfoHeader.biWidth)*3+2];
	return 0;
}
/******************************
	setRGB
	�w�肵�����W�̉�f��ύX����
	����
	 x,y�@�ύX�����f�̍��W
	 R,G,B�@�ύX��̉�f��RGB
	�߂�l
	 -1:�摜���ǂݍ���
	 -2:��Ή��摜
	  0:���퓮��
******************************/
int CReadBmp::setRGB(int x,int y,unsigned char R,unsigned char G,unsigned char B)
{
	if(mColorP==NULL)
		return -1;
	if(mBmpInfoHeader.biBitCount!=24)
	{
		//24�r�b�g�ȊO�͍��͔�Ή�
		return -2;
	}
	x=Jougen(x,mBmpInfoHeader.biWidth-1,0);
	y=Jougen(y,mBmpInfoHeader.biHeight-1,0);
	mColorP[(x+y*mBmpInfoHeader.biWidth)*3]=B;
	mColorP[(x+y*mBmpInfoHeader.biWidth)*3+1]=G;
	mColorP[(x+y*mBmpInfoHeader.biWidth)*3+2]=R;
	return 0;
}
/***********************************
	getYIQ
	����̉�f��YIQ��Ԃ��֐�
	����
	x,y YIQ�����߂���W
	type 0:Y����,1:I����,2:Q������Ԃ��B
	�߂�l�F���߂�YIQ�̒l
***********************************/
double CReadBmp::getYIQ(int x,int y,int type)
{
	unsigned char r,g,b;
	getRGB(x,y,&r,&g,&b);
	switch(type)
	{
	case 0:
		return   (unsigned char)(0.2990*(r)+0.5870*(g)+0.1140*(b));
	case 1:
		return   (unsigned char)(0.5959*(r)-0.2750*(g)-0.3210*(b));
	case 2:
		return	 (unsigned char)(0.2065*(r)-0.4969*(g)+0.2904*(b));
	}
	return 0;
}
/***********************************
	getYCbCr
	����̉�f��YCbCr��Ԃ��֐�
	����
	x,y YCbCr�����߂���W
	type 0:Y����,1:Cb����,2:Cr������Ԃ��B
	�߂�l�F���߂�YCbCr�̒l
***********************************/
double CReadBmp::getYCbCr(int x,int y,int type)
{
	/*------------------------------------------
		Y = 0.29900 * R + 0.58700 * G + 0.11400 * B   
		Cb = -0.16874 * R - 0.33126 * G + 0.50000 * B + 0x80 
		Cr = 0.50000 * R - 0.41869 * G - 0.08131 * B + 0x80 
	------------------------------------------*/
	unsigned char R,G,B;
	getRGB(x,y,&R,&G,&B);
	switch(type)
	{
	//Y
	case 0:
		return   (0.29900 * R + 0.58700 * G + 0.11400 * B);
	//Cb
	case 1:
		return   (-0.16874 * R - 0.33126 * G + 0.50000 * B );
	//Cr
	case 2:
		return	 (0.50000 * R - 0.41869 * G - 0.08131 * B );
	}
	return 0;
}
/******************************
	getYCbCr
	�w�肵�����W�̉�f���擾����
	����
	 x,y�@�擾�����f�̍��W
	 Y,Cb,Cr�@�擾�����f��YCbCr�̊i�[��
	�߂�l
	  0:���퓮��
******************************/
int CReadBmp::getYCbCr(int x,int y,double *Y,double *Cb,double *Cr)
{
	*Y=getYCbCr(x,y,0);
	*Cb=getYCbCr(x,y,1);
	*Cr=getYCbCr(x,y,2);
	return 0;
}
/******************************
	setYCbCr
	�w�肵�����W�̉�f��ύX����
	����
	 x,y�@�ύX�����f�̍��W
	 Y,Cb,Cr�@�ύX��̉�f��YCbCr
	�߂�l
	 -1:�摜���ǂݍ���
	 -2:��Ή��摜
	  0:���퓮��
******************************/
int CReadBmp::setYCbCr(int x,int y,double Y,double Cb,double Cr)
{
	unsigned char R,G,B;
	R=(unsigned char)this->Jougen((int)(Y     + 1.40200 * (Cr )),255,0);
	G=(unsigned char)this->Jougen((int)(Y - 0.34414 * (Cb ) - 0.71414 * (Cr ) ),255,0);
	B=(unsigned char)this->Jougen((int)(Y + 1.77200 * (Cb ) ),255,0);
	return setRGB(x,y,R,G,B);
}

/*****************************
	ReadBmp
	�r�b�g�}�b�v�t�@�C����ǂݍ��ފ֐�
	����
	�@FileName �ǂݍ��ރt�@�C����
	�߂�l
	  -1:�ǂݍ��݃t�@�C�����G���[
	  -2:�t�@�C���I�[�v���G���[
	  -11�`-19:�w�b�_�[�ǂݍ��݃G���[
	  -3:��Ή��r�b�g�}�b�v�G���[
	  -21�`-29:��f�ǂݍ��݃G���[
	  0:���퓮��
*****************************/
int CReadBmp::ReadBmp(const char *FileName)
{
	if(FileName==NULL)
		return -1;
	AllDelete();
	FILE *FBmpP;
	FBmpP=fopen(FileName,"rb");
	if(FBmpP==NULL)
		return -2;
	int id;
	if((id=ReadHeader(FBmpP))<0)
	{
		fclose(FBmpP);
		return -10+id;
	}
	//24bit�ȊO�͍���Ƃ��Ή����ĂȂ��B
	if(mBmpInfoHeader.biBitCount!=24)
	{
		fclose(FBmpP);
		return -3;
	}
	if((id=ReadPixel(FBmpP))<0)
	{
		fclose(FBmpP);
		return -20+id;
	}

	fclose(FBmpP);
	return 0;
}
/*************************************************
	ReadPixel
	�r�b�g�}�b�v��f�ǂݍ���
	����
	 fp �Ǎ���
*************************************************/
int CReadBmp::ReadPixel(FILE *fp)
{
	int y,x,i;
	int yo;
	if(mBmpInfoHeader.biBitCount==24)
	{
//		mImgSize= mBmpInfoHeader.biWidth* mBmpInfoHeader.biHeight*3;
		//���C��
		mImgSize=(mBmpInfoHeader.biWidth*3
			+ ( (mBmpInfoHeader.biWidth*3)%4==0 ? 0 :(4-(mBmpInfoHeader.biWidth*3)%4)) ) 
			* mBmpInfoHeader.biHeight;
		mColorP = new unsigned char [mImgSize];
		unsigned char *p=mColorP;
		int Gomi=(mBmpInfoHeader.biWidth*3)%4==0 ? 0 : 4-(mBmpInfoHeader.biWidth*3)%4;
		for(y=mBmpInfoHeader.biHeight-1;y>=0 ;y--)
		{
			for(x=0;x<mBmpInfoHeader.biWidth*3;x++)
			{
				yo=(mBmpInfoHeader.biHeight-y-1)*mBmpInfoHeader.biWidth*3+x;
				fread(&p[yo],sizeof(unsigned char),1,fp);
			}
			for(i=0;i<Gomi;i++)
				fseek(fp,1,SEEK_CUR);
		}
	}
	else if(mBmpInfoHeader.biBitCount<24)	//���݂̂Ƃ���24bit�ȊO�̉摜�͔�Ή�
	{
		//24bit�ȊO�̏����\��
	}

	return 0;
}
/******************************
	ReadHeader
	�r�b�g�}�b�v�t�@�C���w�b�_�[��ǂݍ���
	����
	 fp �Ǎ���
	�߂�l
	 -1:�ǂݍ��񂾃t�@�C���̓r�b�g�}�b�v�ł͂���܂���B�i�G���[�j
	 ��:�ǂݍ��񂾃r�b�g�}�b�v�̃r�b�g��
******************************/
int CReadBmp::ReadHeader(FILE *fp)
{
//-----�w�b�_�[�P--------------------------
//---BMPHEADER1-------------------------------
	fread(&mBmpFileHeader.bfType,sizeof(WORD),1,fp);
	if(!HeaderCheck(mBmpFileHeader.bfType))
		return -1;
	fread(&mBmpFileHeader.bfSize,sizeof(int),1,fp);
	fread(&mBmpFileHeader.bfReserved1,sizeof(WORD),1,fp);
	fread(&mBmpFileHeader.bfReserved2,sizeof(WORD),1,fp);
	fread(&mBmpFileHeader.bfOffBits,sizeof(int),1,fp);
//-----�w�b�_�[�Q--------------------------
//------------------------------------------------------	
	fread(&mBmpInfoHeader.biSize,sizeof(DWORD),1,fp);
	fread(&mBmpInfoHeader.biWidth,sizeof(LONG),1,fp);
	fread(&mBmpInfoHeader.biHeight,sizeof(LONG),1,fp);
	fread(&mBmpInfoHeader.biPlanes,sizeof(WORD),1,fp);
	fread(&mBmpInfoHeader.biBitCount,sizeof(WORD),1,fp);
	fread(&mBmpInfoHeader.biCompression,sizeof(DWORD),1,fp);
	fread(&mBmpInfoHeader.biSizeImage,sizeof(DWORD),1,fp);
	fread(&mBmpInfoHeader.biXPelsPerMeter,sizeof(LONG),1,fp);
	fread(&mBmpInfoHeader.biYPelsPerMeter,sizeof(LONG),1,fp);
	fread(&mBmpInfoHeader.biClrUsed,sizeof(DWORD),1,fp);
	fread(&mBmpInfoHeader.biClrImportant,sizeof(DWORD),1,fp);
	
	mRGBQuadMax=(mBmpFileHeader.bfOffBits-ftell(fp))/sizeof(RGBQUAD);
#ifdef _DEBUG_D_
	printf("%d=%d-%d\n",mRGBQuadMax,mBmpFileHeader.bfOffBits,ftell(fp));
#endif
	if(mBmpInfoHeader.biBitCount==24)
	{
		mRGBQuadMax=0;
		return mBmpInfoHeader.biBitCount;
	}
	mRGBQuad = new RGBQUAD[mRGBQuadMax];
	for(int i=0;i<mRGBQuadMax;i++)
	{
		fread(&mRGBQuad[i].rgbBlue,sizeof(BYTE),1,fp);
		fread(&mRGBQuad[i].rgbGreen,sizeof(BYTE),1,fp);
		fread(&mRGBQuad[i].rgbRed,sizeof(BYTE),1,fp);
		fread(&mRGBQuad[i].rgbReserved,sizeof(BYTE),1,fp);
	}
	return (int)mBmpInfoHeader.biBitCount;
}

/****************
	HeaderCheck
	�w�b�_�[�̐擪�ɂ���}�[�J�[���`�F�b�N
	����: Type �}�[�J�[
	�߂�l:�ޯ�ϯ�߂̃}�[�J�[�Ȃ�true,�Ⴄ�Ȃ�false
	Type �̒����@B M �̕��тȂ�OK

**************/
bool CReadBmp::HeaderCheck(WORD Type,char *Bt1,char *Bt2)
{
	WORD test=('B'<<8) | 'M';
	char BT[2];
	BT[0]=(char)(Type>>8);
	BT[1]=(char)Type;
	if(Bt1!=NULL && Bt2!=NULL)
	{
		*Bt1=BT[0];
		*Bt2=BT[1];
	}
#ifdef _DEBUG_D_
	printf("%c %c\n",BT[0],BT[1]);
#endif 
	if((char)test == 'B')
	{
		mBigE=true;
		if(BT[0]=='B' && BT[1]=='M')
			return true;
		else 
			return false;
	}
	else
	{
		mBigE=false;
		if(BT[0]=='M' && BT[1]=='B')
			return true;
		else 
			return false;

	}
	return false;
}
/****************************
	WriteBmp
	�r�b�g�}�b�v����������
	����
	�@FileName �������ݐ�t�@�C����
	�߂�l
	  0:���폑������
	  -1:�t�@�C���I�[�v�����s�i�G���[�j
*****************************/
int CReadBmp::WriteBmp(const char *FileName)
{
	FILE *fp;
	fp=fopen(FileName,"wb");
	if(fp==NULL)
		return -1;
	WriteHeader(fp);
	unsigned char *p=mColorP;
	unsigned char c=0;
	int Gomi=(mBmpInfoHeader.biWidth*3)%4==0 ? 0 : 4-(mBmpInfoHeader.biWidth*3)%4;
	int y,x,yo,i;
	for(y=mBmpInfoHeader.biHeight-1;y>=0 ;y--)
	{
			for(x=0;x<mBmpInfoHeader.biWidth*3;x++)
			{
				yo=(mBmpInfoHeader.biHeight-y-1)*mBmpInfoHeader.biWidth*3+x;
				fwrite(&p[yo],sizeof(unsigned char),1,fp);
			}
			for(i=0;i<Gomi;i++)
				fwrite(&c,sizeof(unsigned char),1,fp);
	}
	fclose(fp);
	return 0;
}
/************************
	WriteHeader
	�r�b�g�}�b�v�t�@�C���w�b�_�[��������
	����
	 fp �������ݐ�
*************************/
int CReadBmp::WriteHeader(FILE *fp)
{
	//-----�w�b�_�[�P--------------------------
	fwrite(&mBmpFileHeader.bfType,sizeof(char),2,fp);
	fwrite(&mBmpFileHeader.bfSize,sizeof(int),1,fp);
	fwrite(&mBmpFileHeader.bfReserved1,sizeof(WORD),1,fp);
	fwrite(&mBmpFileHeader.bfReserved2,sizeof(WORD),1,fp);
	fwrite(&mBmpFileHeader.bfOffBits,sizeof(int),1,fp);
	
	//-----�w�b�_�[2--------------------------
	fwrite(&mBmpInfoHeader.biSize,sizeof(DWORD),1,fp);
	fwrite(&mBmpInfoHeader.biWidth,sizeof(LONG),1,fp);
	fwrite(&mBmpInfoHeader.biHeight,sizeof(LONG),1,fp);
	fwrite(&mBmpInfoHeader.biPlanes,sizeof(WORD),1,fp);
	fwrite(&mBmpInfoHeader.biBitCount,sizeof(WORD),1,fp);
	fwrite(&mBmpInfoHeader.biCompression,sizeof(DWORD),1,fp);
	fwrite(&mBmpInfoHeader.biSizeImage,sizeof(DWORD),1,fp);
	fwrite(&mBmpInfoHeader.biXPelsPerMeter,sizeof(LONG),1,fp);
	fwrite(&mBmpInfoHeader.biYPelsPerMeter,sizeof(LONG),1,fp);
	fwrite(&mBmpInfoHeader.biClrUsed,sizeof(DWORD),1,fp);
	fwrite(&mBmpInfoHeader.biClrImportant,sizeof(DWORD),1,fp);
	if(mBmpInfoHeader.biBitCount==24)
		return 0;
	for(int i=0;i<mRGBQuadMax;i++)
	{
		fwrite(&mRGBQuad[i].rgbBlue,sizeof(BYTE),1,fp);
		fwrite(&mRGBQuad[i].rgbGreen,sizeof(BYTE),1,fp);
		fwrite(&mRGBQuad[i].rgbRed,sizeof(BYTE),1,fp);
		fwrite(&mRGBQuad[i].rgbReserved,sizeof(BYTE),1,fp);
	}
	return 0;
}

/*************************************************************
	printBmp
	�r�b�g�}�b�v�t�@�C���̏���\������֐�
	����
	 fp ���o�͐�
	�߂�l
	�@0:���폈��
	  1:�t�@�C�����ǂݍ��݁i�G���[�j
	  2:�r�b�g�}�b�v�t�@�C���ȊO��ǂݍ��݁i�G���[�j
*************************************************************/
int CReadBmp::printBmp(FILE *fp)
{
	if(mColorP==NULL)
	{
		fprintf(fp,"�t�@�C����ǂݍ���ł��܂���B\n");
		return -1;
	}
	char Bt[2];
	if(!HeaderCheck(mBmpFileHeader.bfType,&Bt[0],&Bt[1]))
	{
		fprintf(fp,"�r�b�g�}�b�v�t�@�C���ł͂���܂���B\n");
		return -2;
	}
	/****�w�b�_�[1***********************************************************************
	bfType;//�t�@�C���̎��
	bfSize;//�t�@�C���T�C�Y
	bfReserved1;//�g���p�i0)
	bfReserved2;//�g���p�i0)
	bfOffBits;//�a�h�s�l�`�o�e�h�k�d�g�d�`�c�d�q����r�b�g�}�b�v�f�[�^�܂ł̃o�C�g��
	************************************************************************************/
	fprintf(fp,"�r�b�g�}�b�v���\n");
	fprintf(fp,"------------------------------------------\n");
	fprintf(fp,"�t�@�C���̎��: %c %c\n",Bt[0],Bt[1]);
	fprintf(fp,"�t�@�C���T�C�Y: %d\n",mBmpFileHeader.bfSize);
	fprintf(fp,"�g���p1:%d \n",mBmpFileHeader.bfReserved1);
	fprintf(fp,"�g���p2:%d \n",mBmpFileHeader.bfReserved2);
	fprintf(fp,"BMPHEADER1����r�b�g�}�b�v�f�[�^�܂ł̃o�C�g��: %d\n",mBmpFileHeader.bfOffBits);
	fprintf(fp,"------------------------------------------\n");
	/****�w�b�_�[2***********************************************************************
	DWORD biSize;//���̍\���̂̃o�C�g��
	LONG biWidth;//�r�b�g�}�b�v�̕�
	LONG biHeight;//�r�b�g�}�b�v�̍���
	WORD biPlanes;//�r�b�g�}�b�v�̃J���[�v���[����(�ʏ�1)
	WORD biBitCount;//�s�N�Z��������̃r�b�g��(1,4,8,16,24,32)
	DWORD biCompression;//���k�`���i�a�h�Q�q�f�a���񈳏k�j�i�a�h�Q�i�o�d�f���i�o�d�f�j
	DWORD biSizeImage;//�C���[�W�T�C�Y(�o�C�g�P��)�i�ʏ�O�j
	LONG biXPelsPerMeter;//�����𑜓x�i�s�N�Z��/���[�g���j
	LONG biYPelsPerMeter;//�����𑜓x�i�s�N�Z��/���[�g��)
	DWORD biClrUsed;//�J���[�C���f�b�N�X�̌�(�ʏ�O�j
	DWORD biClrImportant;//�d�v�ȃJ���[�C���f�b�N�X�̌��@
	*************************************************************************************/
	fprintf(fp,"���̍\���̂̃o�C�g��: %d\n",mBmpInfoHeader.biSize);
	fprintf(fp,"��:%d ����:%d\n",mBmpInfoHeader.biWidth,mBmpInfoHeader.biHeight);
	fprintf(fp,"�r�b�g�}�b�v�̃J���[�v���[����:%d\n",mBmpInfoHeader.biPlanes);
	fprintf(fp,"�s�N�Z��������̃r�b�g��: %d\n",mBmpInfoHeader.biBitCount);
	fprintf(fp,"���k�`��: %d\n",mBmpInfoHeader.biCompression);
	fprintf(fp,"�C���[�W�T�C�Y�i�o�C�g�P��) : %d\n",mBmpInfoHeader.biSizeImage);
	fprintf(fp,"�����𑜓x(�s�N�Z��/���[�g��):%d\n",mBmpInfoHeader.biXPelsPerMeter);
	fprintf(fp,"�����𑜓x(�s�N�Z��/���[�g��):%d\n",mBmpInfoHeader.biYPelsPerMeter);
	fprintf(fp,"�J���[�C���f�b�N�X�̌�:%d\n",mBmpInfoHeader.biClrUsed);
	fprintf(fp,"�d�v�ȃJ���[�C���f�b�N�X�̌�:%d\n",mBmpInfoHeader.biClrImportant);
	fprintf(fp,"------------------------------------------\n");

	return 0;
}
/************************
	CreateBmp
	�w�肵�������E�c���̃r�b�g�}�b�v�摜���쐬
	����
	 width �쐬����r�b�g�}�b�v�̉���
	 height �쐬����r�b�g�}�b�v�c��
	 Gp     �쐬����r�b�g�}�b�v��f�S
***************************/
int CReadBmp::CreateBmp(int width,int height,unsigned char *Gp)
{
	Jougen(width,10000000,1);
	Jougen(height,10000000,1);
	AllDelete();
	mImgSize=(width*3 +( (width*3)%4==0 ? 0 :(4-(width*3)%4)) ) * height;
	mBmpFileHeader.bfType = (WORD)(mBigE ? ('B'<<8) | ('M') : ('B') | ('M'<<8) ) ;
	mBmpFileHeader.bfSize = mImgSize +  sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	mBmpFileHeader.bfReserved1 = 0;
	mBmpFileHeader.bfReserved2 = 0;
	mBmpFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	
//-----�w�b�_�[2--------------------------
	mBmpInfoHeader.biSize  = sizeof(BITMAPINFOHEADER);
	mBmpInfoHeader.biWidth=width;
	mBmpInfoHeader.biHeight=height;
	mBmpInfoHeader.biPlanes = 1;
	mBmpInfoHeader.biBitCount = 24;
	mBmpInfoHeader.biCompression =0;
	mBmpInfoHeader.biSizeImage=mImgSize;
	mBmpInfoHeader.biXPelsPerMeter=0;
	mBmpInfoHeader.biYPelsPerMeter=0;
	mBmpInfoHeader.biClrUsed=0;
	mBmpInfoHeader.biClrImportant=0;
	mColorP=new unsigned char [mImgSize];
	unsigned char *p = mColorP;
	int i;
	if(Gp==NULL)
	{
		for(i=0;i<mImgSize;i++)
			*p++ = 0;
	}
	else
	{
		int x,y;
		for(y=0;y<height;y++)
		{
			for(x=0;x<width*3;x++)
				*p++ = *Gp++;
			/*
			if((width*3)%4!=0)
				for(i=0;i<4-(width*3)%4;i++)
					*p++=*Gp++;	
					*/
		}
	}
	return 0;
}
/************************
	BmpCopy
	�w�肳�ꂽ�摜���R�s�[����
	����
	�@Bmp �R�s�[���r�b�g�}�b�v
************************/
CReadBmp *CReadBmp::BmpCopy(CReadBmp *Bmp)
{
	if(Bmp==NULL)
		return NULL;
	int i;
	if(Bmp->getWidth() != getWidth() || Bmp->getHeight() != getHeight() )
	{
		AllDelete();
		mBmpFileHeader=Bmp->mBmpFileHeader;
		mBmpInfoHeader=Bmp->mBmpInfoHeader;
		if(Bmp->mRGBQuad)
		{
			mRGBQuadMax=Bmp->mRGBQuadMax;
			mRGBQuad = new RGBQUAD [mRGBQuadMax];	
			for(i=0;i<mRGBQuadMax;i++)
				mRGBQuad[i]=Bmp->mRGBQuad[i];
		}	
		if(Bmp->mColorP)
		{
			mImgSize=Bmp->mImgSize;
			mColorP=new unsigned char [mImgSize];	
		}
	}
	if(Bmp->mColorP)
		for(i=0;i<mImgSize;i++)
			mColorP[i]=Bmp->mColorP[i];	
	return 0;
}
/***************************
�ő�l�E�ŏ��l�`�F�b�N�p
	����
	�@num�@�`�F�b�N����ϐ�
	�@max  �ő�l
	�@min  �ŏ��l
*****************************/
int CReadBmp::Jougen(int num,int max,int min)
{
	if(num<=min)
		return min;
	else if(max<=num)
		return max;
	return num;
}