/*****************************************************************************
*           移動物体検出プログラム                                           *                                              *
*                                    製作者：安井健一                        *
******************************************************************************/
#define GAMEN_WIDTH 320
#define GAMEN_HEIGHT 240

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480+170
#define NOT_FILE "not file"
#define ID_MYTIMER    32767
#define DETECT_TIME 33
#define VERSION_STR "移動物体検出プログラム(OneShot)ノルム Ver7.8"

#define THREAD_ON 1
#define THREAD_OFF 0
#define THREAD_STOP 2
#define THREAD_WM_PAINT 3

/*----------インクルード---------------------*/
#include <windows.h>
#include <process.h>
#include "CAnimationOneShot.h"
#include "resource.h"
#include "CDetectNorme.h"
#include  "CGraphRGBY.h"
#include "CBmpLabel.h"

#pragma	comment(lib,"winmm.lib")
#include <mmsystem.h>
/*----------プロトタイプ宣言-----------------*/
char *ReadFileOpen(HWND hWnd,const char *Str);
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);//メインウィンドウのプロシージャ
ATOM InitApp(HINSTANCE);
BOOL InitInstance(HINSTANCE,int);	
void Wm_Paint(HWND hWnd,CAnimationOneShot *pA,char *pFileName);		//WM_PAINTメッセージ処理用
void BackDraw(HDC hdc);											//背景描画用
void WmCommand(HWND hWnd,WPARAM wp,LPARAM lp,CAnimationOneShot *pCa);	//WM_COMMANDメッセージ処理用
HDC VirualCreate(HWND hWnd,HDC hdc,int width,int height);		//仮想画面(HDC)作成用
void PaintFrame(CAnimationOneShot *pCa,HDC hdc,HWND hWnd);				//フレーム描画用
BOOL CALLBACK MyDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);//設定値ダイヤログボックスのプロシージャ
VOID ThreadFilmingMain(void *data);
void SetXY(int x,int y);
void WmCreate(HWND hWnd);
char *WriteFileOpen(HWND hWnd,char *szTiTle);
void OpenMpeg(HWND hWnd,CAnimationOneShot *pCa);
int Play(HWND hWnd,CAnimationOneShot *pCa);
int SaveBmp(HWND hWnd,CAnimationOneShot *pCa,int flag);
int GetRadio(HWND hDlg);
HWND GetHRadio(int no,HWND hDlg);
BOOL CALLBACK StopTimeSetDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
BOOL CALLBACK CBaiDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
bool NewCDetect(void); //mCDetectをnewする関数
void DeleteCDetect(void);//mCDetectをdeleteする関数
void TextOutData(double nowTime,double MaxTime);//TextOutで表示する関数
/*----------グローバル変数-------------------*/
char m_ClassName[]="DetectWindow";	//登録するクラス名
HINSTANCE g_hInst;	
char *g_LoadFileName; //読み込んでいるmpegファイル名
HDC g_hDetect=NULL;	//検出した移動物体描画用	
HDC mhBack=NULL;	//背景画像描画用
HDC mhMpeg=NULL;	//読み込んでいるmpeg描画用
//CNormeDetect *mCDetect=NULL;	//移動物体検出用
//CNormeDetect *mCDetect=NULL;	//移動物体検出用
CDetectBase *mCDetect=NULL;
int g_La=10;					//λ値
int g_N=50;					//N値
int g_DType=0;				//0:従来手法 1:非線形処理
int g_Loop=THREAD_OFF;
HWND g_hWnd;
CGraphRGBY g_Graph(320,240);
VOID ThreadOff(void);
VOID ThreadStop(void);
int g_X=0,g_Y=0;
int g_Fps=0;
HDC g_hdcVirual;
double g_TimeNo=0;
#define TIME_PLUS 0.033333
int g_M=3;
int g_Noise=10;
int g_Blank=10;
RemovaType g_NoiseFlag=Not;
//適当な変数
LONGLONG g_TimeFast=0;
unsigned long int g_AllFrame=0;
CReadBmp g_Bmp;	//フレーム画像を格納する
CReadBmp g_BmpDetect;
CReadBmp g_BmpBack;
const char DTypeStr[3][32]={"従来手法","非線形処理付加","ノルム"};
double g_Stop_Time=300;
double g_CBai =1.0;
double g_Start_Time=0;
CLabelRemovaNoise *g_CLabel=NULL;

/********************************************************
                  WinMain関数
*********************************************************/
int WINAPI WinMain(HINSTANCE hCurInst,HINSTANCE hPrevInst,LPSTR lpsCmdLine,int nCmdShow)
{
	MSG msg;
	BOOL bRet;
	g_hInst=hCurInst;
	if(!InitApp(hCurInst))
		return FALSE; 
	if(!InitInstance(hCurInst,nCmdShow))
		return FALSE;

	while((bRet=GetMessage(&msg,NULL,0,0) != 0))
	{

		if(bRet == -1)
		{
			MessageBox(NULL,"GetMessageエラー","Error",MB_OK);
			break;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}
/*************************************************************
InitApp関数
ウィンドウの登録
戻り値：失敗するとFALSEが返る
***************************************************************/
ATOM InitApp(HINSTANCE hInst)
{
	/*--------WINDCALSSEX構造体に格値を代入*/
	WNDCLASSEX wc;
	wc.cbSize		= sizeof(WNDCLASSEX); 
	wc.style		= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc	= WndProc;			//プロシージャ名
	wc.cbClsExtra	= 0;
	wc.cbWndExtra	= 0;
	wc.hInstance	= hInst;			//インスタンス
	wc.hIcon		=(HICON)LoadIcon(hInst, "MYICON");
	wc.hCursor		= (HCURSOR)LoadImage(NULL,
										MAKEINTRESOURCE(IDC_ARROW),
										IMAGE_CURSOR,
										0,
										0,
										LR_DEFAULTSIZE | LR_SHARED);

	wc.hbrBackground= NULL;
	wc.lpszMenuName	= "MYMENU";				//メニュー名
	wc.lpszClassName= (LPCSTR)m_ClassName;
	wc.hIconSm		= (HICON)LoadIcon(hInst, "MYICON");

	return (RegisterClassEx(&wc)); 
}
/*************************************************
InitInstance関数
ウィンドウを作成する。
戻り値：失敗するとFALSEが返る
***************************************************/
BOOL InitInstance(HINSTANCE hInst,int nCmdShow)
{
	HWND hWnd;

	hWnd = CreateWindow(m_ClassName,
						//タイトルバーに表示される文字列
						VERSION_STR,
						WS_OVERLAPPEDWINDOW &~WS_MAXIMIZEBOX,				//ウィンドウの種類
						CW_USEDEFAULT,						//Ｘ座標
						CW_USEDEFAULT,						//Ｙ座標
						WINDOW_WIDTH,						//幅	
						WINDOW_HEIGHT,						//高さ
						NULL,								//新ウィンドウのハンドル、親を作るときはNULL
						NULL,								//メニューハンドル、クラスメニューを使うときはNULL
						hInst,								//インスタンスハンドル
						NULL);

	if(!hWnd)
		return FALSE;
	ShowWindow(hWnd,nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}

/*************************************************************
	WndProc
	メインウィンドウのウィンドウプロシージャ

**************************************************************/
LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wp,LPARAM lp)
{
	static CAnimationOneShot CA(g_hInst);
	int x,y;
	switch(msg)
	{
	case WM_CREATE:
		WmCreate(hWnd);
		break;
	case WM_LBUTTONDOWN:
		x=lp & 0xFFFF;
		y= (lp >> 16) & 0xFFFF;
		SetXY(x,y);
		break;
	case WM_PAINT:
		Wm_Paint(hWnd,&CA,g_LoadFileName);
		break;
	case WM_COMMAND:
         WmCommand(hWnd,wp,lp,&CA);
         break;
	case WM_CLOSE:
		if(g_LoadFileName)
			delete [] g_LoadFileName;
		if(g_hDetect)
			DeleteDC(g_hDetect);
		if(mhBack)
			DeleteDC(mhBack);
		if(g_hdcVirual)
			DeleteDC(g_hdcVirual);
		if(mhMpeg)
			DeleteDC(mhMpeg);
		DeleteCDetect();
		ThreadOff();
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return (DefWindowProc(hWnd,msg,wp,lp));
	}
	return 0;
}
void WmCreate(HWND hWnd)
{
	HDC hdc;
	HWND hButtonWnd0,hButtonWnd1,hButtonWnd2,hButtonWnd3,hButtonWnd4;
	g_hDetect=NULL;
	mhBack=NULL;
	mhMpeg=NULL;
	g_hWnd=hWnd;
	g_LoadFileName= new char[strlen(NOT_FILE)+1];
	strcpy(g_LoadFileName,NOT_FILE);
			
	hButtonWnd0 = CreateWindow(
            "BUTTON", "<<", 
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            370, WINDOW_HEIGHT-120, 20,20, hWnd, (HMENU)IDM_REWINDING, g_hInst ,NULL);

	hButtonWnd1 = CreateWindow(
            "BUTTON", "|>", 
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            400, WINDOW_HEIGHT-120, 20,20, hWnd, (HMENU)IDM_PLAY, g_hInst ,NULL);
	hButtonWnd2 = CreateWindow(
            "BUTTON", "∥", 
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            430, WINDOW_HEIGHT-120, 20,20, hWnd, (HMENU)IDM_STOP, g_hInst ,NULL);
	hButtonWnd3 = CreateWindow(
            "BUTTON", "設定", 
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            460, WINDOW_HEIGHT-120, 50,20, hWnd, (HMENU)IDM_KEN, g_hInst ,NULL);
	hButtonWnd4 = CreateWindow(
            "BUTTON", "保存", 
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            520, WINDOW_HEIGHT-120, 50,20, hWnd, (HMENU)IDM_KEEP_DETECT, g_hInst ,NULL);
	g_Loop=THREAD_OFF;
	hdc=GetDC(hWnd);
	g_hdcVirual=VirualCreate(hWnd,hdc,WINDOW_WIDTH,WINDOW_HEIGHT);
	
	ReleaseDC(hWnd,hdc);
}
/*******************************
	Wm_Paint
	WM_PAINTメッセージ処理関数
*******************************/
void Wm_Paint(HWND hWnd,CAnimationOneShot *pA,char *pFileName)
{
	HDC hdc;
    PAINTSTRUCT paint;
	CReadBmp Bmp;
	switch(pA->getFlag())
	{
	//ファイルを読み込んでいません。
	case 0:
		hdc = BeginPaint(hWnd, &paint);
		BackDraw(paint.hdc);
		TextOut(hdc,0,490,NOT_FILE,strlen(NOT_FILE));
		EndPaint(hWnd, &paint);	
		break;
	//読み込み完了
	case 1:	
		hdc = BeginPaint(hWnd, &paint);
		BackDraw(paint.hdc);	
		TextOut(hdc,0,490,pFileName,strlen(pFileName));
		TextOut(hdc,0,510,"読み込み完了",strlen("読み込み完了"));

		EndPaint(hWnd, &paint);	
		break;
	//再生中
	case 2:
	//停止中
	case 3:
	case 4:
		ThreadStop();
		hdc = BeginPaint(hWnd, &paint);
		BackDraw(paint.hdc);	
		BitBlt(hdc,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,g_hdcVirual,0,0,SRCCOPY);
		TextOut(hdc,0,490,pFileName,strlen(pFileName));
		g_Loop=THREAD_ON;
		EndPaint(hWnd, &paint);	
		break;
	default:
		ThreadStop();
		hdc = BeginPaint(hWnd, &paint);
		TextOut(hdc,30,490+50,"test",strlen("test"));
		g_Loop=THREAD_ON;
		EndPaint(hWnd, &paint);	
		break;
	}

}
/*******************************
	BackDraw
	背景描画関数
*******************************/
void BackDraw(HDC hdc)
{
	HBRUSH hBrush, hOldBrush;
	HPEN hPen, hOldPen;
	hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	hOldPen = (HPEN)SelectObject(hdc, hPen); 
	hBrush = CreateSolidBrush(RGB(255,255,255));
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	Rectangle(hdc,    0,   0,  WINDOW_WIDTH,WINDOW_HEIGHT);
	SelectObject(hdc, hOldPen);
	SelectObject(hdc, hOldBrush);  
	DeleteObject(hPen);
	DeleteObject(hBrush);
	MoveToEx(hdc,0,480,NULL);
	LineTo(hdc,WINDOW_WIDTH,480);

}
/********************************************
   WmCommand
   WM_COMMANDメッセージ処理用関数
********************************************/
void WmCommand(HWND hWnd,WPARAM wp,LPARAM lp,CAnimationOneShot *pCa)
{
	switch (LOWORD(wp)) 
	{
	//メニューの「開く」
	case IDM_OPEN:
		OpenMpeg(hWnd,pCa);
		break;
	//速度変更//////////
		/*
	case IDM_QUICK:
		pCa->PlaySpeed(2.0);
		break;
	case IDM_NORMAL:
		pCa->PlaySpeed(1.0);
		break;
	case IDM_SLOW:
		pCa->PlaySpeed(0.5);
		break;
		*/
	/////////////////////
	case IDM_KEN:
		pCa->OneShotStop();
		::g_TimeNo=-TIME_PLUS;
		if(!DialogBox(g_hInst,"MYDLG", hWnd, (DLGPROC)MyDlgProc))
			break;
		DeleteCDetect();
		if(pCa->getFlag()!=0)
		{
			pCa->setTime(g_Start_Time);
			g_TimeNo=g_Start_Time;
		}
		InvalidateRect(hWnd,NULL,TRUE);
		break;
	case IDM_REWINDING://巻き戻し
		ThreadOff();
		pCa->OneShotRewinding();
		g_TimeNo=-g_TimeNo;
		break;
	//メニュー「再生」
	case IDM_PLAY:
		Play(hWnd,pCa);
		break;
	case IDM_GRAPH:
		//下策、いずれクラスに取り込む
		g_Graph.CreateDlg(hWnd,"GRAPH_DLG");
		break;
	//メニュー「停止」
	case IDM_STOP:
		pCa->OneShotStop();
		Sleep(300);
		break;
	//メニュー「バージョン情報表示」
	case IDM_VERSION:
		MessageBox(hWnd,VERSION_STR,"version情報",MB_OK);
		break;
	case IDM_KEEP_ORIGINAL:
		SaveBmp(hWnd,pCa,0);
		break;
	case IDM_KEEP_BACK:
		SaveBmp(hWnd,pCa,2);	
		break;
	case IDM_KEEP_ALL:
		SaveBmp(hWnd,pCa,3);	
		break;
	//画像を保存
	case IDM_KEEP_DETECT:	
		SaveBmp(hWnd,pCa,1);
		break;
	//停止タイム
	case IDM_STOP_TIME_SET:
		pCa->OneShotStop();
		Sleep(300);
		DialogBox(g_hInst,"STOP_TIME_DLG", NULL, (DLGPROC)StopTimeSetDlgProc);
	}
}
char mNotFileName[1024]="C:\\a.bmp";
#define WBMP_SET(Bs) Bs.WriteBmp(mNotFileName);
//#define WBMP_SET(Bs)
int SaveBmp(HWND hWnd,CAnimationOneShot *pCa,int flag)
{
	char *pName;
	pCa->OneShotStop();
	g_Loop=THREAD_STOP;
	while(g_Loop==THREAD_STOP)
		Sleep(100);
	if(flag==0 || flag==3)
	{
		if((pName=WriteFileOpen(hWnd,"原画像を保存します。"))==NULL)
		{
			WBMP_SET(g_Bmp)	
			return -1;
		}
		else
			g_Bmp.WriteBmp(pName);
	}
	if(flag==1 || flag==3)
	{
		if((pName=WriteFileOpen(hWnd,"検出画像を保存します。"))==NULL)
		{
			WBMP_SET(g_BmpDetect)	
			return -1;
		}
		g_BmpDetect.WriteBmp(pName);
	}
	if(flag==2 || flag==3)
	{
		if((pName=WriteFileOpen(hWnd,"背景画像を保存します。"))==NULL)
		{
			WBMP_SET(g_BmpBack)	
			return -1;
		}
		g_BmpBack.WriteBmp(pName);
	}
//	Play(hWnd,pCa);
	return 0;
	
}
int Play(HWND hWnd,CAnimationOneShot *pCa)
{
	ThreadOff();
	if(g_La==0 && g_N==0)
	{
		if(!DialogBox(g_hInst,"MYDLG", hWnd, (DLGPROC)MyDlgProc))
		{
			MessageBox(NULL,"再生ダイアログミス！！","エラー",MB_OK);
			return -1;
		}
	}
	if(pCa->getTime() == pCa->getLastTime() || pCa->getTime() == 0)
	{
	
		DeleteCDetect();
		mCDetect=NULL;
		pCa->setTime(g_Start_Time);
		g_TimeNo=g_Start_Time;

	}
	pCa->OneShotPlayStart(NULL,0,0);
//	pCa->OneShotPlayStart(hWnd,WINDOW_WIDTH/2,0);
	g_Loop=THREAD_ON;
	g_TimeFast=timeGetTime();
	g_AllFrame=0;
	_beginthread(ThreadFilmingMain,0,pCa);
	return 0;
}
/*
	動画ファイルを開く
*/
void OpenMpeg(HWND hWnd,CAnimationOneShot *pCa)
{
	char *FileP;
	WCHAR			wszFileName[1024];
	FileP=ReadFileOpen(hWnd,"mpegファイルを開いてください。");
	MultiByteToWideChar( CP_ACP, 0, FileP, -1, wszFileName, 260 );
	if(pCa->Load( wszFileName ,hWnd)!=S_OK)
	{
		MessageBox(hWnd,"読み込めません。",FileP,MB_OK);
		if(g_LoadFileName)
			delete [] g_LoadFileName;
		g_LoadFileName= new char[strlen(NOT_FILE)+1];
		strcpy(g_LoadFileName,NOT_FILE);
	}
	else
	{
		if(g_hDetect)
			DeleteDC(g_hDetect);
		if(mhBack)
			DeleteDC(mhBack);
		if(mhMpeg)
			DeleteDC(mhMpeg);
		g_hDetect=NULL;
		mhBack=NULL;
		mhMpeg=NULL;
		if(g_LoadFileName)
			delete [] g_LoadFileName;
		g_LoadFileName= new char[strlen(FileP)+1];
		strcpy(g_LoadFileName,FileP);
	}
	InvalidateRect(hWnd,NULL,TRUE);
}
/************VirualCreate関数*************************
仮想画面を作成する。

引数：
　hdc:最終的に画像を描画するために使う、GetDCやBeginPaint関数で、
　ウィンドウのクライアント領域に対するデバイスコンテキストのハンドルをhdcに格納しておく必要がある。
　それがなければこの関数は失敗する。
　HDC hGet:仮想画面になるハンドル
　int width:横幅
　int height:高さ
戻り値：hGet,仮想画面となった状態で戻る

**************************************/
HDC VirualCreate(HWND hWnd,HDC hdc,int width,int height)
{
	HDC hGet;
	hGet=CreateCompatibleDC(hdc);
	HBITMAP hBmp;
	hBmp=CreateCompatibleBitmap(hdc,width,height);
	SelectObject(hGet,hBmp);
	DeleteObject(hBmp);
	return 	hGet;
}
/************************************
	PaintFrame
	１フレーム毎の描画をする関数
	引数
	pCa CAnimationOneShotクラスへのポインタ
	hdc window描画用デバイスコンテキスト
	hWnd windowハンドル
*************************************/
void PaintFrame(CAnimationOneShot *pCa,HDC hdc,HWND hWnd)
{
	int Flag=pCa->getFlag();
	if(Flag<2)
		return;
	
	g_TimeNo+=TIME_PLUS;
//	pCa->OneShotPlay(hWnd,WINDOW_WIDTH/2,0,g_TimeNo);
	pCa->OneShotPlay(NULL,WINDOW_WIDTH/2,0,g_TimeNo);

	BITMAPINFO BI;
	double nowTime,MaxTime;
	unsigned char *pB=NULL;
	nowTime = pCa->getTime();		//現在再生中時間
	MaxTime = pCa->getLastTime();	//終了時間
	if(pCa->getFrame(&g_Bmp)==NULL)
	{
	//	MessageBox(NULL,"getFrame error","",MB_OK);
		return;
	}
	TextOutData(nowTime,MaxTime);
/*
	TextOut(g_hdcVirual,0,490,g_LoadFileName,strlen(g_LoadFileName));
	sprintf(DataStr,"time : %6.2f / %6.2f [秒]   %lf",nowTime,MaxTime,g_TimeNo);
	TextOut(g_hdcVirual,0,510,DataStr,strlen(DataStr));
	sprintf(DataStr,"λ:%d N:%d %s M:%d N機能:%s (%d)",g_La,g_N,DTypeStr[g_DType],g_M
		,(g_NoiseFlag ? "On":"Off"),g_Noise);
	TextOut(g_hdcVirual,0,530, DataStr,strlen(DataStr));

	sprintf(DataStr,"停止予定時間  (%.2lf)",g_Stop_Time);
	TextOut(g_hdcVirual,0,550, DataStr,strlen(DataStr));
	//
	if(mCDetect)
	{
		sprintf(DataStr,"検出画素 %8d/%8d * 100 = %3.2lf %%　　",mCDetect->getCount(),g_Bmp.getWidth()*g_Bmp.getHeight(),(mCDetect->getCount()/(double)(g_Bmp.getWidth()*g_Bmp.getHeight()))*100);
		TextOut(g_hdcVirual,0,550, DataStr,strlen(DataStr));
		sprintf(DataStr,"平均輝度値 %.2lf  ",mCDetect->getAveDY());
		TextOut(g_hdcVirual,0,570, DataStr,strlen(DataStr));
	}
	sprintf(DataStr,"FPS：%00005d    検出時間: %.3lf(msec/frame) 合計:%.2lf",g_Fps
		,(double)(timeGetTime()-g_TimeFast)/g_AllFrame,(double)(timeGetTime()-g_TimeFast) );
	TextOut(g_hdcVirual,0,590, DataStr,strlen(DataStr));
*/
	if(nowTime==0)
		return;
	if(!g_hDetect)
		g_hDetect=VirualCreate(hWnd,hdc,g_Bmp.getWidth(),g_Bmp.getHeight());
	if(!mhBack)
		mhBack=VirualCreate(hWnd,hdc,g_Bmp.getWidth(),g_Bmp.getHeight());
	if(!mhMpeg)
		mhMpeg=VirualCreate(hWnd,hdc,g_Bmp.getWidth(),g_Bmp.getHeight());
	BI.bmiHeader=*g_Bmp.getBmpInfoHeader();
	
	//mpeg画像HDC
	SetDIBitsToDevice(mhMpeg,0,0,
			g_Bmp.getWidth(),g_Bmp.getHeight(),
			0,0,0,g_Bmp.getHeight(),
			g_Bmp.getRgbP(),&BI,
			DIB_RGB_COLORS );
		
	if(!mCDetect)
	{
		NewCDetect();

	}
	else
		g_BmpDetect.BmpCopy(&g_Bmp);
	//各フレームの移動物体検出および背景画像の作成
	//	if(mCDetect && Flag==2) //急ごしらえの修正
//	if(mCDetect && (Flag==2 || Flag==3) && nowTime!=MaxTime)
	if(mCDetect && (Flag==2 || Flag==4) && nowTime!=MaxTime)
	{
		mCDetect->Detect(g_Bmp.getRgbP());
		mCDetect->DetectExtraction(g_BmpDetect.getRgbP(),g_Bmp.getWidth(),g_Bmp.getHeight());
		mCDetect->getBack(g_BmpBack.getRgbP());
		
		if(g_NoiseFlag)
		{
		//	CBmpLabel2 CLabel(g_Bmp.getWidth(),g_Bmp.getHeight());
		//	g_CLabel->LabelPuttingBmp(&g_BmpDetect);
		//	g_CLabel->RemovalNoise(g_Noise,&g_BmpDetect);

		}
	
		SetDIBitsToDevice(g_hDetect,0,0,
			g_Bmp.getWidth(),g_Bmp.getHeight(),
			0,0,0,g_Bmp.getHeight(),
			g_BmpDetect.getRgbP(),&BI,
			DIB_RGB_COLORS );
		/*
		SetDIBitsToDevice(mhBack,0,0,
			g_Bmp.getWidth(),g_Bmp.getHeight(),
			0,0,0,g_Bmp.getHeight(),
			pB,&BI,
			DIB_RGB_COLORS );
			*/
			SetDIBitsToDevice(mhBack,0,0,
			g_Bmp.getWidth(),g_Bmp.getHeight(),
			0,0,0,g_Bmp.getHeight(),
			g_BmpBack.getRgbP(),&BI,
			DIB_RGB_COLORS );
			g_AllFrame++;
		
	}

	//移動物体検出画像描画
	StretchBlt(
				g_hdcVirual,    // 転送先デバイスコンテキストハンドル 
				0,
				0,
				640/2,// 転送先長方形の幅 
				480/2,// 転送先長方形の高さ 
				g_hDetect,    // 転送元デバイスコンテキストハンドル 
				0,0,g_Bmp.getWidth(),g_Bmp.getHeight(),
				SRCCOPY     // ラスターオペレーション  
				);

	//背景画像描画
	StretchBlt(
				g_hdcVirual,    // 転送先デバイスコンテキストハンドル 
				0,
				480/2,
				640/2,// 転送先長方形の幅 
				480/2,// 転送先長方形の高さ 
				mhBack,    // 転送元デバイスコンテキストハンドル 
				0,0,g_Bmp.getWidth(),g_Bmp.getHeight(),
				SRCCOPY     // ラスターオペレーション  
				);
	
	StretchBlt(
				g_hdcVirual,    // 転送先デバイスコンテキストハンドル 
				640/2,
				0,
				640/2,// 転送先長方形の幅 
				480/2,// 転送先長方形の高さ 
				mhMpeg,    // 転送元デバイスコンテキストハンドル 
				0,0,g_Bmp.getWidth(),g_Bmp.getHeight(),
				SRCCOPY     // ラスターオペレーション  
				);
	HPEN hPen, hOldPen;
	hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
	hOldPen = (HPEN)SelectObject(g_hdcVirual, hPen); 
   	MoveToEx(g_hdcVirual, g_X-3, g_Y, NULL);
  	LineTo(g_hdcVirual, g_X+3, g_Y);
   	MoveToEx(g_hdcVirual, g_X, g_Y-3, NULL);
    LineTo(g_hdcVirual, g_X, g_Y+3);
   	MoveToEx(g_hdcVirual, g_X-3+320, g_Y, NULL);
  	LineTo(g_hdcVirual, g_X+3+320, g_Y);
   	MoveToEx(g_hdcVirual, g_X+320, g_Y-3, NULL);
    LineTo(g_hdcVirual, g_X+320, g_Y+3);
  	MoveToEx(g_hdcVirual, g_X-3, g_Y+240, NULL);
  	LineTo(g_hdcVirual, g_X+3, g_Y+240);
   	MoveToEx(g_hdcVirual, g_X, g_Y-3+240, NULL);
    LineTo(g_hdcVirual, g_X, g_Y+3+240);
	SelectObject(g_hdcVirual, hOldPen);
	DeleteObject(hPen);
	if(mCDetect && (Flag==2 || Flag==3))
	{
		if(nowTime!=MaxTime)
		{
			/*
			CReadBmp *pBmp=NULL;
			switch(g_Graph.getRadioFlag())
			{
				case IDC_DETECT_G:
					pBmp=&g_BmpDetect;
					break;
				case IDC_MPEG_G:
					pBmp=&g_Bmp;
					break;
				case IDC_BACK_G:
					mCDetect->getBack(g_Bmp.getWidth(),g_Bmp.getHeight(),&g_BmpDetect);
					//g_BmpDetect.CreateBmp(g_Bmp.getWidth(),g_Bmp.getHeight(),mCDetect->getBack(g_Bmp.getWidth(),g_Bmp.getHeight()));
					pBmp=&g_BmpDetect;
					break;
				default:
					return;
			}
			g_Graph.PrintRGB(NULL,pBmp,0,240,400,400,30);
			*/
		}
	}
	if(pB)
		delete [] pB;

	BitBlt(hdc,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,g_hdcVirual,0,0,SRCCOPY);
	
	if(nowTime>=g_Stop_Time)
	{
		pCa->OneShotStop();
		Sleep(300);
	}
	
	if(g_TimeNo>=MaxTime)
	{
		pCa->OneShotStop();
	}
}
/****************************************
  MyDlgProc
  検出設定値用ダイアログボックスのプロシージャ
****************************************/
BOOL CALLBACK MyDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
//	static HWND hCheck, hLa, hN,hM;
	HWND hCheck, hLa, hN,hM,hNCheck,hBCheck,hNoise,hBlank,hIcoBai,hFtime;
	char szBuf[64];
	switch (msg) 
	{

	case WM_INITDIALOG:
			//ボタン等の初期化
			hLa = GetDlgItem(hDlg, IDC_LA);
			hN  = GetDlgItem(hDlg, IDC_N);
			hM  = GetDlgItem(hDlg, IDC_M);
			hFtime = GetDlgItem(hDlg, IDC_EDIT_TS);
			hNCheck = GetDlgItem(hDlg, IDC_NOISE_CHECK);
			hBCheck=GetDlgItem(hDlg,IDC_BLANK_CHECK );
			hNoise =GetDlgItem(hDlg,IDC_NOISE);
			hBlank=GetDlgItem(hDlg,IDC_BLANK);
			wsprintf(szBuf,"%d",g_La);
			Edit_SetText(hLa,szBuf);
			wsprintf(szBuf,"%d",g_N);
			Edit_SetText(hN,szBuf);
			wsprintf(szBuf,"%d",g_M);
			Edit_SetText(hM,szBuf);
			wsprintf(szBuf,"%d",g_Noise);
			Edit_SetText(hNoise,szBuf);	
			wsprintf(szBuf,"%d",g_Blank);
			Edit_SetText(hBlank,szBuf);	
			hCheck=GetHRadio(g_DType,hDlg);
			Button_SetCheck(hCheck,TRUE);
			sprintf(szBuf,"%lf",g_Start_Time);
			Edit_SetText(hFtime,szBuf);	
			
			hIcoBai	= GetDlgItem(hDlg, IDC_EDIT1_IROBAI);
			sprintf(szBuf,"%.2lf",g_CBai);
			Edit_SetText(hIcoBai,szBuf);	
			if(g_NoiseFlag==NoiseOnly || g_NoiseFlag==NoiseAndBlank)
				Button_SetCheck(hNCheck,TRUE);
			if(g_NoiseFlag==BlankOnly || g_NoiseFlag==NoiseAndBlank)
				Button_SetCheck(hBCheck,TRUE);
			break;
	case WM_COMMAND:
			switch (LOWORD(wp)) 
			{
			case IDCANCEL:
				EndDialog(hDlg, FALSE);
				return TRUE;
			case IDOK:
				hLa = GetDlgItem(hDlg, IDC_LA);
				hN  = GetDlgItem(hDlg, IDC_N);
				hM  = GetDlgItem(hDlg, IDC_M);
				hNCheck = GetDlgItem(hDlg, IDC_NOISE_CHECK);
				hBCheck=GetDlgItem(hDlg,IDC_BLANK_CHECK );
				hNoise =GetDlgItem(hDlg,IDC_NOISE);
				hBlank=GetDlgItem(hDlg,IDC_BLANK);				
				hCheck = GetDlgItem(hDlg, IDC_HISEN);
				g_DType=GetRadio(hDlg);
				g_NoiseFlag=Not;
				if (Button_GetCheck(hNCheck) == BST_CHECKED) 
					g_NoiseFlag=NoiseOnly;
				if(Button_GetCheck(hBCheck) == BST_CHECKED)
					if(g_NoiseFlag==NoiseOnly)
						g_NoiseFlag=NoiseAndBlank;
					else
						g_NoiseFlag=BlankOnly;
				

				Edit_GetText(hLa, szBuf, sizeof(szBuf));
				g_La = atoi(szBuf);
				Edit_GetText(hN, szBuf, sizeof(szBuf));
				g_N = atoi(szBuf);
				Edit_GetText(hM, szBuf, sizeof(szBuf));
				g_M = atoi(szBuf);
				Edit_GetText(hNoise, szBuf, sizeof(szBuf));
				g_Noise= atoi(szBuf);
				Edit_GetText(hBlank, szBuf, sizeof(szBuf));
				g_Blank= atoi(szBuf);
		
				hIcoBai	= GetDlgItem(hDlg, IDC_EDIT1_IROBAI);	
				Edit_GetText(hIcoBai, szBuf, sizeof(szBuf));
				g_CBai= atof(szBuf);
				hFtime = GetDlgItem(hDlg, IDC_EDIT_TS);
				Edit_GetText(hFtime, szBuf, sizeof(szBuf));
				g_Start_Time=atof(szBuf);
				EndDialog(hDlg, IDOK);
				return TRUE;
			}
	}
	return FALSE;
}
/************************
通常、非線形、ノルムのいずれかのハンドルを返す関数
//g_DType
************************/
HWND GetHRadio(int no,HWND hDlg)
{
	HWND hCheck;
	switch(no)
	{
		case 1:		//非線形
			hCheck = GetDlgItem(hDlg, IDC_HISEN);
			break;
		case 2:	
			//ノルム
			hCheck = GetDlgItem(hDlg, IDC_NORUMU);
			break;
		case 0:
		default:
			hCheck = GetDlgItem(hDlg, IDC_NORMAL);
		}
	return hCheck;
}
/*********************
	どのラジオボタンが押されてるかチェックする関数
************************/
int GetRadio(HWND hDlg)
{
	HWND hCheck;
	int i;
	for(i=0;i<3;i++)
	{
		hCheck=GetHRadio(i,hDlg);
		if(Button_GetCheck(hCheck)==BST_CHECKED)
			return i;
	}
	return 0;
}

VOID ThreadStop(void)
{
	if(g_Loop==THREAD_ON)
	{
		g_Loop=THREAD_STOP;
		while(g_Loop==THREAD_STOP)
			Sleep(400);
	}
}
VOID ThreadOff(void)
{
	if(g_Loop==THREAD_ON)
	{
		g_Loop=THREAD_OFF;
		while(g_Loop==THREAD_OFF)
			Sleep(400);
		g_Loop=THREAD_OFF;
	}
}
/*****ThreadMain*************************
もう一つのMain関数
（スレッド)
*******************************************/

VOID ThreadFilmingMain(void *data)
{	
	HDC hdc;
	CAnimationOneShot *pCA=(CAnimationOneShot*)data;
	LONGLONG next_time,now_time;
	next_time=timeGetTime()+1000; 	
	g_Fps=0;
	int count=0;
	hdc=GetDC(g_hWnd);
	BitBlt(g_hdcVirual,0,0,WINDOW_WIDTH,WINDOW_HEIGHT,hdc,0,0,SRCCOPY);
	ReleaseDC(g_hWnd,hdc);
	int flag;
	while(g_Loop!=THREAD_OFF)
	{
//		Sleep(DETECT_TIME);
		now_time=timeGetTime(); 
		count++;
		if (now_time>=next_time) 
		{
			g_Fps=count;
			count=0;
			next_time = now_time + 1000; 
		}
		if(g_Loop==THREAD_STOP)
		{
			g_Loop=THREAD_WM_PAINT;
			continue;
		}
		if(g_Loop==THREAD_WM_PAINT)
			continue;
		flag=pCA->getFlag();
	//	if(flag==2 || flag==3 || flag==4)
		if(flag==2 || flag==4)
		{
		
			hdc=GetDC(g_hWnd);
			PaintFrame(pCA,hdc,g_hWnd);	
			ReleaseDC(g_hWnd,hdc);
		}
	}
	g_Loop=-1;
	_endthread();
}
void SetXY(int x,int y)
{
	if(x>640)
		return;
	if(y>480)
		return;
	if(x>=320)
		x-=320;
	if(y>=240)
		y-=240;
	g_X=x;
	g_Y=y;
	g_Graph.setXY(x,y);
}


char *WriteFileOpen(HWND hWnd,char *szTiTle)
{
	static char szFileName[1024*64]="";//パスも含めたファイルの名前が入る
	char szFile[1024]="";	//ファイルの名前が入る
    OPENFILENAME ofn;
	char strCustom[1024]="";
	static int nFilterIndex=1;
	memset(szFileName,0,sizeof(szFileName));
    memset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = "bitmap(*.bmp)\0*.bmp\0All files(*.*)\0*.*\0\0";
	/*
	lpstrFilterは、その名の通りフィルタです。
	"テキストファイル(*.txt)\0*.txt\0すべて(*.*)\0*.*\0\0"
	などのように使います。文字列の区切りにはヌル文字（\0）を入れます。最後はヌル文字２つ（\0\0）を使います。実際使ってみるとすぐにわかります。
	*/
    ofn.lpstrFile = szFileName;
	//lpstrFileには、GetOpenFileName関数が呼ばれるとここにフルパス付きのファイル名が格納されます。
    ofn.lpstrFileTitle = szFile;
	//lpstrFileTitleには、選択されたファイル名が格納されます。
    ofn.nFilterIndex = nFilterIndex;
    ofn.nMaxFile = sizeof(szFileName);
	//nMaxFileは、lpstrFileの大きさです。なおMAX_PATHはwindef.hで２６０と定義されています。
    ofn.nMaxFileTitle = sizeof(szFile);
	//nMaxFileTitleは、lpstrFileTitleの大きさです
	ofn.Flags =   OFN_OVERWRITEPROMPT ;

    ofn.lpstrCustomFilter= strCustom;
	ofn.nMaxCustFilter=sizeof(strCustom);
    ofn.lpstrDefExt = "bmp";
    ofn.lpstrTitle = szTiTle;
	
    if(GetSaveFileName(&ofn) == 0)
	{
		char Buf[64];
		sprintf(Buf,"%x %d",CommDlgExtendedError(),ofn.lpstrFile);
		MessageBox(hWnd,"error",Buf,MB_OK);
        return NULL;
	}	
	
	nFilterIndex=ofn.nFilterIndex;//選んだファイル形式を保存

    return szFileName;
}


/*****************************
	FileOpen
	画像ファイルを開く関数
引数
　hWnd windowハンドル
  Str　ファイルを開くダイアログボックスのタイトル

*****************************/
char *ReadFileOpen(HWND hWnd,const char *Str)
{
    DWORD dwSize = 0L;
    OPENFILENAME ofn;
	char szFileTitle[512];
	static char szFile[512];
	static int nFilterIndex=1;
    memset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = "mpeg(*.mpeg)\0*.mpeg\0mpg(*.mpg)\0*.mpg\0wmv(*.wmv)\0*.wmv\0avi(*.avi)\0*.avi\0All files(*.*)\0*.*\0\0";
    ofn.lpstrFile = szFile;
    ofn.lpstrFileTitle = szFileTitle;
    ofn.nMaxFile = MAX_PATH;
    ofn.nMaxFileTitle = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "mpeg";
    ofn.lpstrTitle = Str;
    ofn.nFilterIndex = nFilterIndex;
    if(GetOpenFileName(&ofn) == 0)
        return NULL;
	nFilterIndex=ofn.nFilterIndex;//選んだファイル形式を保存
	return szFile;
}


/****************************************
  MyDlgProc
  時間設定値用ダイアログボックスのプロシージャ
****************************************/
BOOL CALLBACK StopTimeSetDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
//	static HWND hCheck, hLa, hN,hM;
	HWND hLa;
	char szBuf[64];


	switch (msg) {

	case WM_INITDIALOG:
			//各コントロールのハンドルを取得
			hLa = GetDlgItem(hDlg, IDC_EDIT_T);
			sprintf(szBuf,"%.2lf",g_Stop_Time);
			Edit_SetText(hLa,szBuf);
	
			break;
	case WM_COMMAND:
			switch (LOWORD(wp)) 
			{
			case IDCANCEL:
				EndDialog(hDlg, FALSE);
				return TRUE;
			case IDOK:
				hLa = GetDlgItem(hDlg, IDC_EDIT_T);
				Edit_GetText(hLa, szBuf, sizeof(szBuf));
				g_Stop_Time = atof(szBuf);
			
			
			//	EndDialog(hDlg, IDOK);
				return TRUE;
			}
	}
	return FALSE;
}


/****************************************
  MyDlgProc
  検出設定値用ダイアログボックスのプロシージャ
****************************************/
BOOL CALLBACK CBaiDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	HWND hLa;
	char szBuf[64];
	
	switch (msg) {

	case WM_INITDIALOG:
			//各コントロールのハンドルを取得
			hLa = GetDlgItem(hDlg, IDC_C_BAI);
			sprintf(szBuf,"%.2lf",g_CBai);
			Edit_SetText(hLa,szBuf);
					
			break;
	case WM_COMMAND:
			switch (LOWORD(wp)) 
			{
			case IDCANCEL:
				EndDialog(hDlg, FALSE);
				return TRUE;
			case IDOK:
				hLa = GetDlgItem(hDlg, IDC_C_BAI);
				Edit_GetText(hLa, szBuf, sizeof(szBuf));
				g_CBai = atof(szBuf);
				EndDialog(hDlg, IDOK);
				return TRUE;
			}
	}
	return FALSE;
}

void DeleteCDetect(void)
{
	if(mCDetect)
	{
		switch(g_DType)
		{
		case 0:
			delete (CDetectDynamicBack*)mCDetect;
			break;
		case 1:
			delete (CDetectNonlinear*)mCDetect;
			break;
		case 2:
			delete	(CDetectNorme*)	mCDetect;	
			break;
		default:
			delete mCDetect;

		}
	}
	if(g_CLabel)
		delete g_CLabel;
	mCDetect=NULL;
	g_CLabel=NULL;
}
bool NewCDetect(void)
{
	switch(g_DType)
	{
		case 0:
			mCDetect = new CDetectDynamicBack(g_La,g_N,g_Bmp.getWidth(),g_Bmp.getHeight());
			mCDetect->BackReset();
			break;
		case 1:
			mCDetect = new CDetectNonlinear(g_La,g_N,g_M,DynamicBack,g_Bmp.getWidth(),g_Bmp.getHeight());
			mCDetect->BackReset();
			break;
		case 2:
			mCDetect = new	CDetectNorme(g_La,g_N,g_M,g_CBai,DynamicBack,g_Bmp.getWidth(),g_Bmp.getHeight());
			mCDetect->BackReset();
			break;
		default:
			mCDetect =  NULL;
			return false;
	}
	mCDetect->setBack(g_Bmp.getRgbP(),g_Bmp.getWidth(),g_Bmp.getHeight());
	g_BmpBack.CreateBmp(g_Bmp.getWidth(),g_Bmp.getHeight());
	g_BmpDetect.CreateBmp(g_Bmp.getWidth(),g_Bmp.getHeight(),g_Bmp.getRgbP());
	switch(g_NoiseFlag)
	{
	case NoiseOnly:
		g_CLabel = new CLabelRemovaNoise(g_Bmp.getWidth(),g_Bmp.getHeight(),g_Noise);
		break;
	case BlankOnly:
	case NoiseAndBlank:
		g_CLabel = new CLabelRemovaNoiseBlank(g_Bmp.getWidth(),g_Bmp.getHeight(),g_NoiseFlag,g_Noise,g_Blank);
		break;
	default:
		g_CLabel=NULL;
	}
	if(g_CLabel)
		mCDetect->setPostprocessing(g_CLabel);
	return true;
}
/*****************************************
	文字列描画用
*******************************************/
void TextOutData(double nowTime,double MaxTime)
{
	char DataStr[1024];
	char Buf[128];
	TextOut(g_hdcVirual,0,490,g_LoadFileName,strlen(g_LoadFileName));
	sprintf(DataStr,"time : %6.2f / %6.2f [秒]   %lf",nowTime,MaxTime,g_TimeNo);
	TextOut(g_hdcVirual,0,510,DataStr,strlen(DataStr));
	switch(g_NoiseFlag)
	{
	case NoiseOnly:
		sprintf(Buf,"ノイズ除去 : %d",g_Noise);
		break;
	case BlankOnly:
		sprintf(Buf,"空白除去 : %d ",g_Blank);
		break;
	case NoiseAndBlank:
		sprintf(Buf,"ノイズ除去　: %d  空白除去 :%d",g_Noise,g_Blank);
		break;
	default:
		sprintf(Buf," ");
	}
	sprintf(DataStr,"λ:%d N:%d %s M:%d %s ",g_La,g_N,DTypeStr[g_DType],g_M,Buf);
	TextOut(g_hdcVirual,0,530, DataStr,strlen(DataStr));

	sprintf(DataStr,"停止予定時間  (%.2lf)",g_Stop_Time);
	TextOut(g_hdcVirual,0,550, DataStr,strlen(DataStr));
	//
	if(mCDetect)
	{
		sprintf(DataStr,"検出画素 %8d/%8d * 100 = %3.2lf %%　　",mCDetect->getCount(),g_Bmp.getWidth()*g_Bmp.getHeight(),(mCDetect->getCount()/(double)(g_Bmp.getWidth()*g_Bmp.getHeight()))*100);
		TextOut(g_hdcVirual,0,550, DataStr,strlen(DataStr));
		sprintf(DataStr,"平均輝度値 %.2lf  ",mCDetect->getAveDY());
		TextOut(g_hdcVirual,0,570, DataStr,strlen(DataStr));
	}
	sprintf(DataStr,"FPS：%00005d    検出時間: %.3lf(msec/frame) 合計:%.2lf",g_Fps
		,(double)(timeGetTime()-g_TimeFast)/g_AllFrame,(double)(timeGetTime()-g_TimeFast) );
	TextOut(g_hdcVirual,0,590, DataStr,strlen(DataStr));
}