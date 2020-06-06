#include <stdio.h>
#include "CReadBmp2006.h"

/****************************
	コンストラクタ

*****************************/
CReadBmp::CReadBmp()
{
	mRGBQuad = NULL;
	mColorP = NULL;
	HeaderCheck((WORD)(('B'<<8) | 'M'));
}
/****************************
	デストラクタ

*****************************/
CReadBmp::~CReadBmp()
{
	AllDelete();
}
/****************************
	getWidth :　画像の横幅を返す関数
	getHeight:  画像の縦幅を返す関数
	getBitCount: 画像のビット数を返す関数
*****************************/
int CReadBmp::getWidth(void){return mBmpInfoHeader.biWidth;}
int CReadBmp::getHeight(void){return mBmpInfoHeader.biHeight;}
int CReadBmp::getBitCount(void){return mBmpInfoHeader.biBitCount;}
/****************************
	AllDelete
	メモリを全て解放する。
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
	画素値を返す関数
	引数 :	x,y  画素値の座標(x,y)
			type 画素の種類(0=赤、1=緑、2=青)
	戻り値 : 画素値
****************/
unsigned char CReadBmp::getRGB(int x,int y,int type)
{
	unsigned char color;
	if(mColorP==NULL)
		return 0;
	if(mBmpInfoHeader.biBitCount!=24)
	{
		//24ビット以外は今は非対応
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
	指定した座標の画素を取得する
	引数
	 x,y　取得する画素の座標
	 R,G,B　取得する画素のRGBの格納先
	戻り値
	 -1:画像未読み込み
	 -2:非対応画像
	  0:正常動作
******************************/
int CReadBmp::getRGB(int x,int y,unsigned char *R,unsigned char *G,unsigned char *B)
{
	if(mColorP==NULL)
		return -1;
	if(mBmpInfoHeader.biBitCount!=24)
	{
		//24ビット以外は今は非対応
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
	指定した座標の画素を変更する
	引数
	 x,y　変更する画素の座標
	 R,G,B　変更後の画素のRGB
	戻り値
	 -1:画像未読み込み
	 -2:非対応画像
	  0:正常動作
******************************/
int CReadBmp::setRGB(int x,int y,unsigned char R,unsigned char G,unsigned char B)
{
	if(mColorP==NULL)
		return -1;
	if(mBmpInfoHeader.biBitCount!=24)
	{
		//24ビット以外は今は非対応
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
	特定の画素のYIQを返す関数
	引数
	x,y YIQを求める座標
	type 0:Y成分,1:I成分,2:Q成分を返す。
	戻り値：求めたYIQの値
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
	特定の画素のYCbCrを返す関数
	引数
	x,y YCbCrを求める座標
	type 0:Y成分,1:Cb成分,2:Cr成分を返す。
	戻り値：求めたYCbCrの値
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
	指定した座標の画素を取得する
	引数
	 x,y　取得する画素の座標
	 Y,Cb,Cr　取得する画素のYCbCrの格納先
	戻り値
	  0:正常動作
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
	指定した座標の画素を変更する
	引数
	 x,y　変更する画素の座標
	 Y,Cb,Cr　変更後の画素のYCbCr
	戻り値
	 -1:画像未読み込み
	 -2:非対応画像
	  0:正常動作
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
	ビットマップファイルを読み込む関数
	引数
	　FileName 読み込むファイル名
	戻り値
	  -1:読み込みファイル名エラー
	  -2:ファイルオープンエラー
	  -11～-19:ヘッダー読み込みエラー
	  -3:非対応ビットマップエラー
	  -21～-29:画素読み込みエラー
	  0:正常動作
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
	//24bit以外は今んとこ対応してない。
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
	ビットマップ画素読み込み
	引数
	 fp 読込先
*************************************************/
int CReadBmp::ReadPixel(FILE *fp)
{
	int y,x,i;
	int yo;
	if(mBmpInfoHeader.biBitCount==24)
	{
//		mImgSize= mBmpInfoHeader.biWidth* mBmpInfoHeader.biHeight*3;
		//仮修正
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
	else if(mBmpInfoHeader.biBitCount<24)	//現在のところ24bit以外の画像は非対応
	{
		//24bit以外の処理予定
	}

	return 0;
}
/******************************
	ReadHeader
	ビットマップファイルヘッダーを読み込む
	引数
	 fp 読込先
	戻り値
	 -1:読み込んだファイルはビットマップではありません。（エラー）
	 他:読み込んだビットマップのビット数
******************************/
int CReadBmp::ReadHeader(FILE *fp)
{
//-----ヘッダー１--------------------------
//---BMPHEADER1-------------------------------
	fread(&mBmpFileHeader.bfType,sizeof(WORD),1,fp);
	if(!HeaderCheck(mBmpFileHeader.bfType))
		return -1;
	fread(&mBmpFileHeader.bfSize,sizeof(int),1,fp);
	fread(&mBmpFileHeader.bfReserved1,sizeof(WORD),1,fp);
	fread(&mBmpFileHeader.bfReserved2,sizeof(WORD),1,fp);
	fread(&mBmpFileHeader.bfOffBits,sizeof(int),1,fp);
//-----ヘッダー２--------------------------
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
	ヘッダーの先頭にあるマーカーをチェック
	引数: Type マーカー
	戻り値:ﾋﾞｯﾄﾏｯﾌﾟのマーカーならtrue,違うならfalse
	Type の中が　B M の並びならOK

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
	ビットマップを書き込む
	引数
	　FileName 書き込み先ファイル名
	戻り値
	  0:正常書き込み
	  -1:ファイルオープン失敗（エラー）
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
	ビットマップファイルヘッダー書き込み
	引数
	 fp 書き込み先
*************************/
int CReadBmp::WriteHeader(FILE *fp)
{
	//-----ヘッダー１--------------------------
	fwrite(&mBmpFileHeader.bfType,sizeof(char),2,fp);
	fwrite(&mBmpFileHeader.bfSize,sizeof(int),1,fp);
	fwrite(&mBmpFileHeader.bfReserved1,sizeof(WORD),1,fp);
	fwrite(&mBmpFileHeader.bfReserved2,sizeof(WORD),1,fp);
	fwrite(&mBmpFileHeader.bfOffBits,sizeof(int),1,fp);
	
	//-----ヘッダー2--------------------------
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
	ビットマップファイルの情報を表示する関数
	引数
	 fp 情報出力先
	戻り値
	　0:正常処理
	  1:ファイル未読み込み（エラー）
	  2:ビットマップファイル以外を読み込み（エラー）
*************************************************************/
int CReadBmp::printBmp(FILE *fp)
{
	if(mColorP==NULL)
	{
		fprintf(fp,"ファイルを読み込んでいません。\n");
		return -1;
	}
	char Bt[2];
	if(!HeaderCheck(mBmpFileHeader.bfType,&Bt[0],&Bt[1]))
	{
		fprintf(fp,"ビットマップファイルではありません。\n");
		return -2;
	}
	/****ヘッダー1***********************************************************************
	bfType;//ファイルの種類
	bfSize;//ファイルサイズ
	bfReserved1;//拡張用（0)
	bfReserved2;//拡張用（0)
	bfOffBits;//ＢＩＴＭＡＰＦＩＬＥＨＥＡＤＥＲからビットマップデータまでのバイト数
	************************************************************************************/
	fprintf(fp,"ビットマップ情報\n");
	fprintf(fp,"------------------------------------------\n");
	fprintf(fp,"ファイルの種類: %c %c\n",Bt[0],Bt[1]);
	fprintf(fp,"ファイルサイズ: %d\n",mBmpFileHeader.bfSize);
	fprintf(fp,"拡張用1:%d \n",mBmpFileHeader.bfReserved1);
	fprintf(fp,"拡張用2:%d \n",mBmpFileHeader.bfReserved2);
	fprintf(fp,"BMPHEADER1からビットマップデータまでのバイト数: %d\n",mBmpFileHeader.bfOffBits);
	fprintf(fp,"------------------------------------------\n");
	/****ヘッダー2***********************************************************************
	DWORD biSize;//この構造体のバイト数
	LONG biWidth;//ビットマップの幅
	LONG biHeight;//ビットマップの高さ
	WORD biPlanes;//ビットマップのカラープレーン数(通常1)
	WORD biBitCount;//ピクセル当たりのビット数(1,4,8,16,24,32)
	DWORD biCompression;//圧縮形式（ＢＩ＿ＲＧＢ＝非圧縮）（ＢＩ＿ＪＰＥＧ＝ＪＰＥＧ）
	DWORD biSizeImage;//イメージサイズ(バイト単位)（通常０）
	LONG biXPelsPerMeter;//水平解像度（ピクセル/メートル）
	LONG biYPelsPerMeter;//垂直解像度（ピクセル/メートル)
	DWORD biClrUsed;//カラーインデックスの個数(通常０）
	DWORD biClrImportant;//重要なカラーインデックスの個数　
	*************************************************************************************/
	fprintf(fp,"この構造体のバイト数: %d\n",mBmpInfoHeader.biSize);
	fprintf(fp,"幅:%d 高さ:%d\n",mBmpInfoHeader.biWidth,mBmpInfoHeader.biHeight);
	fprintf(fp,"ビットマップのカラープレーン数:%d\n",mBmpInfoHeader.biPlanes);
	fprintf(fp,"ピクセル当たりのビット数: %d\n",mBmpInfoHeader.biBitCount);
	fprintf(fp,"圧縮形式: %d\n",mBmpInfoHeader.biCompression);
	fprintf(fp,"イメージサイズ（バイト単位) : %d\n",mBmpInfoHeader.biSizeImage);
	fprintf(fp,"水平解像度(ピクセル/メートル):%d\n",mBmpInfoHeader.biXPelsPerMeter);
	fprintf(fp,"垂直解像度(ピクセル/メートル):%d\n",mBmpInfoHeader.biYPelsPerMeter);
	fprintf(fp,"カラーインデックスの個数:%d\n",mBmpInfoHeader.biClrUsed);
	fprintf(fp,"重要なカラーインデックスの個数:%d\n",mBmpInfoHeader.biClrImportant);
	fprintf(fp,"------------------------------------------\n");

	return 0;
}
/************************
	CreateBmp
	指定した横幅・縦幅のビットマップ画像を作成
	引数
	 width 作成するビットマップの横幅
	 height 作成するビットマップ縦幅
	 Gp     作成するビットマップ画素郡
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
	
//-----ヘッダー2--------------------------
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
	指定された画像をコピーする
	引数
	　Bmp コピー元ビットマップ
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
最大値・最小値チェック用
	引数
	　num　チェックする変数
	　max  最大値
	　min  最小値
*****************************/
int CReadBmp::Jougen(int num,int max,int min)
{
	if(num<=min)
		return min;
	else if(max<=num)
		return max;
	return num;
}