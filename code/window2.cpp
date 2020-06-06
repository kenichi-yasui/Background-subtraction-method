/*****************************************************************************
*           �ړ����̌��o�v���O����                                           *                                              *
*                                    ����ҁF���䌒��                        *
******************************************************************************/
#define GAMEN_WIDTH 320
#define GAMEN_HEIGHT 240

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480+170
#define NOT_FILE "not file"
#define ID_MYTIMER    32767
#define DETECT_TIME 33
#define VERSION_STR "�ړ����̌��o�v���O����(OneShot)�m���� Ver7.8"

#define THREAD_ON 1
#define THREAD_OFF 0
#define THREAD_STOP 2
#define THREAD_WM_PAINT 3

/*----------�C���N���[�h---------------------*/
#include <windows.h>
#include <process.h>
#include "CAnimationOneShot.h"
#include "resource.h"
#include "CDetectNorme.h"
#include  "CGraphRGBY.h"
#include "CBmpLabel.h"

#pragma	comment(lib,"winmm.lib")
#include <mmsystem.h>
/*----------�v���g�^�C�v�錾-----------------*/
char *ReadFileOpen(HWND hWnd,const char *Str);
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);//���C���E�B���h�E�̃v���V�[�W��
ATOM InitApp(HINSTANCE);
BOOL InitInstance(HINSTANCE,int);	
void Wm_Paint(HWND hWnd,CAnimationOneShot *pA,char *pFileName);		//WM_PAINT���b�Z�[�W�����p
void BackDraw(HDC hdc);											//�w�i�`��p
void WmCommand(HWND hWnd,WPARAM wp,LPARAM lp,CAnimationOneShot *pCa);	//WM_COMMAND���b�Z�[�W�����p
HDC VirualCreate(HWND hWnd,HDC hdc,int width,int height);		//���z���(HDC)�쐬�p
void PaintFrame(CAnimationOneShot *pCa,HDC hdc,HWND hWnd);				//�t���[���`��p
BOOL CALLBACK MyDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);//�ݒ�l�_�C�����O�{�b�N�X�̃v���V�[�W��
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
bool NewCDetect(void); //mCDetect��new����֐�
void DeleteCDetect(void);//mCDetect��delete����֐�
void TextOutData(double nowTime,double MaxTime);//TextOut�ŕ\������֐�
/*----------�O���[�o���ϐ�-------------------*/
char m_ClassName[]="DetectWindow";	//�o�^����N���X��
HINSTANCE g_hInst;	
char *g_LoadFileName; //�ǂݍ���ł���mpeg�t�@�C����
HDC g_hDetect=NULL;	//���o�����ړ����̕`��p	
HDC mhBack=NULL;	//�w�i�摜�`��p
HDC mhMpeg=NULL;	//�ǂݍ���ł���mpeg�`��p
//CNormeDetect *mCDetect=NULL;	//�ړ����̌��o�p
//CNormeDetect *mCDetect=NULL;	//�ړ����̌��o�p
CDetectBase *mCDetect=NULL;
int g_La=10;					//�ɒl
int g_N=50;					//N�l
int g_DType=0;				//0:�]����@ 1:����`����
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
//�K���ȕϐ�
LONGLONG g_TimeFast=0;
unsigned long int g_AllFrame=0;
CReadBmp g_Bmp;	//�t���[���摜���i�[����
CReadBmp g_BmpDetect;
CReadBmp g_BmpBack;
const char DTypeStr[3][32]={"�]����@","����`�����t��","�m����"};
double g_Stop_Time=300;
double g_CBai =1.0;
double g_Start_Time=0;
CLabelRemovaNoise *g_CLabel=NULL;

/********************************************************
                  WinMain�֐�
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
			MessageBox(NULL,"GetMessage�G���[","Error",MB_OK);
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
InitApp�֐�
�E�B���h�E�̓o�^
�߂�l�F���s�����FALSE���Ԃ�
***************************************************************/
ATOM InitApp(HINSTANCE hInst)
{
	/*--------WINDCALSSEX�\���̂Ɋi�l����*/
	WNDCLASSEX wc;
	wc.cbSize		= sizeof(WNDCLASSEX); 
	wc.style		= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc	= WndProc;			//�v���V�[�W����
	wc.cbClsExtra	= 0;
	wc.cbWndExtra	= 0;
	wc.hInstance	= hInst;			//�C���X�^���X
	wc.hIcon		=(HICON)LoadIcon(hInst, "MYICON");
	wc.hCursor		= (HCURSOR)LoadImage(NULL,
										MAKEINTRESOURCE(IDC_ARROW),
										IMAGE_CURSOR,
										0,
										0,
										LR_DEFAULTSIZE | LR_SHARED);

	wc.hbrBackground= NULL;
	wc.lpszMenuName	= "MYMENU";				//���j���[��
	wc.lpszClassName= (LPCSTR)m_ClassName;
	wc.hIconSm		= (HICON)LoadIcon(hInst, "MYICON");

	return (RegisterClassEx(&wc)); 
}
/*************************************************
InitInstance�֐�
�E�B���h�E���쐬����B
�߂�l�F���s�����FALSE���Ԃ�
***************************************************/
BOOL InitInstance(HINSTANCE hInst,int nCmdShow)
{
	HWND hWnd;

	hWnd = CreateWindow(m_ClassName,
						//�^�C�g���o�[�ɕ\������镶����
						VERSION_STR,
						WS_OVERLAPPEDWINDOW &~WS_MAXIMIZEBOX,				//�E�B���h�E�̎��
						CW_USEDEFAULT,						//�w���W
						CW_USEDEFAULT,						//�x���W
						WINDOW_WIDTH,						//��	
						WINDOW_HEIGHT,						//����
						NULL,								//�V�E�B���h�E�̃n���h���A�e�����Ƃ���NULL
						NULL,								//���j���[�n���h���A�N���X���j���[���g���Ƃ���NULL
						hInst,								//�C���X�^���X�n���h��
						NULL);

	if(!hWnd)
		return FALSE;
	ShowWindow(hWnd,nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}

/*************************************************************
	WndProc
	���C���E�B���h�E�̃E�B���h�E�v���V�[�W��

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
            "BUTTON", "�a", 
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            430, WINDOW_HEIGHT-120, 20,20, hWnd, (HMENU)IDM_STOP, g_hInst ,NULL);
	hButtonWnd3 = CreateWindow(
            "BUTTON", "�ݒ�", 
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            460, WINDOW_HEIGHT-120, 50,20, hWnd, (HMENU)IDM_KEN, g_hInst ,NULL);
	hButtonWnd4 = CreateWindow(
            "BUTTON", "�ۑ�", 
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            520, WINDOW_HEIGHT-120, 50,20, hWnd, (HMENU)IDM_KEEP_DETECT, g_hInst ,NULL);
	g_Loop=THREAD_OFF;
	hdc=GetDC(hWnd);
	g_hdcVirual=VirualCreate(hWnd,hdc,WINDOW_WIDTH,WINDOW_HEIGHT);
	
	ReleaseDC(hWnd,hdc);
}
/*******************************
	Wm_Paint
	WM_PAINT���b�Z�[�W�����֐�
*******************************/
void Wm_Paint(HWND hWnd,CAnimationOneShot *pA,char *pFileName)
{
	HDC hdc;
    PAINTSTRUCT paint;
	CReadBmp Bmp;
	switch(pA->getFlag())
	{
	//�t�@�C����ǂݍ���ł��܂���B
	case 0:
		hdc = BeginPaint(hWnd, &paint);
		BackDraw(paint.hdc);
		TextOut(hdc,0,490,NOT_FILE,strlen(NOT_FILE));
		EndPaint(hWnd, &paint);	
		break;
	//�ǂݍ��݊���
	case 1:	
		hdc = BeginPaint(hWnd, &paint);
		BackDraw(paint.hdc);	
		TextOut(hdc,0,490,pFileName,strlen(pFileName));
		TextOut(hdc,0,510,"�ǂݍ��݊���",strlen("�ǂݍ��݊���"));

		EndPaint(hWnd, &paint);	
		break;
	//�Đ���
	case 2:
	//��~��
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
	�w�i�`��֐�
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
   WM_COMMAND���b�Z�[�W�����p�֐�
********************************************/
void WmCommand(HWND hWnd,WPARAM wp,LPARAM lp,CAnimationOneShot *pCa)
{
	switch (LOWORD(wp)) 
	{
	//���j���[�́u�J���v
	case IDM_OPEN:
		OpenMpeg(hWnd,pCa);
		break;
	//���x�ύX//////////
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
	case IDM_REWINDING://�����߂�
		ThreadOff();
		pCa->OneShotRewinding();
		g_TimeNo=-g_TimeNo;
		break;
	//���j���[�u�Đ��v
	case IDM_PLAY:
		Play(hWnd,pCa);
		break;
	case IDM_GRAPH:
		//����A������N���X�Ɏ�荞��
		g_Graph.CreateDlg(hWnd,"GRAPH_DLG");
		break;
	//���j���[�u��~�v
	case IDM_STOP:
		pCa->OneShotStop();
		Sleep(300);
		break;
	//���j���[�u�o�[�W�������\���v
	case IDM_VERSION:
		MessageBox(hWnd,VERSION_STR,"version���",MB_OK);
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
	//�摜��ۑ�
	case IDM_KEEP_DETECT:	
		SaveBmp(hWnd,pCa,1);
		break;
	//��~�^�C��
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
		if((pName=WriteFileOpen(hWnd,"���摜��ۑ����܂��B"))==NULL)
		{
			WBMP_SET(g_Bmp)	
			return -1;
		}
		else
			g_Bmp.WriteBmp(pName);
	}
	if(flag==1 || flag==3)
	{
		if((pName=WriteFileOpen(hWnd,"���o�摜��ۑ����܂��B"))==NULL)
		{
			WBMP_SET(g_BmpDetect)	
			return -1;
		}
		g_BmpDetect.WriteBmp(pName);
	}
	if(flag==2 || flag==3)
	{
		if((pName=WriteFileOpen(hWnd,"�w�i�摜��ۑ����܂��B"))==NULL)
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
			MessageBox(NULL,"�Đ��_�C�A���O�~�X�I�I","�G���[",MB_OK);
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
	����t�@�C�����J��
*/
void OpenMpeg(HWND hWnd,CAnimationOneShot *pCa)
{
	char *FileP;
	WCHAR			wszFileName[1024];
	FileP=ReadFileOpen(hWnd,"mpeg�t�@�C�����J���Ă��������B");
	MultiByteToWideChar( CP_ACP, 0, FileP, -1, wszFileName, 260 );
	if(pCa->Load( wszFileName ,hWnd)!=S_OK)
	{
		MessageBox(hWnd,"�ǂݍ��߂܂���B",FileP,MB_OK);
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
/************VirualCreate�֐�*************************
���z��ʂ��쐬����B

�����F
�@hdc:�ŏI�I�ɉ摜��`�悷�邽�߂Ɏg���AGetDC��BeginPaint�֐��ŁA
�@�E�B���h�E�̃N���C�A���g�̈�ɑ΂���f�o�C�X�R���e�L�X�g�̃n���h����hdc�Ɋi�[���Ă����K�v������B
�@���ꂪ�Ȃ���΂��̊֐��͎��s����B
�@HDC hGet:���z��ʂɂȂ�n���h��
�@int width:����
�@int height:����
�߂�l�FhGet,���z��ʂƂȂ�����ԂŖ߂�

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
	�P�t���[�����̕`�������֐�
	����
	pCa CAnimationOneShot�N���X�ւ̃|�C���^
	hdc window�`��p�f�o�C�X�R���e�L�X�g
	hWnd window�n���h��
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
	nowTime = pCa->getTime();		//���ݍĐ�������
	MaxTime = pCa->getLastTime();	//�I������
	if(pCa->getFrame(&g_Bmp)==NULL)
	{
	//	MessageBox(NULL,"getFrame error","",MB_OK);
		return;
	}
	TextOutData(nowTime,MaxTime);
/*
	TextOut(g_hdcVirual,0,490,g_LoadFileName,strlen(g_LoadFileName));
	sprintf(DataStr,"time : %6.2f / %6.2f [�b]   %lf",nowTime,MaxTime,g_TimeNo);
	TextOut(g_hdcVirual,0,510,DataStr,strlen(DataStr));
	sprintf(DataStr,"��:%d N:%d %s M:%d N�@�\:%s (%d)",g_La,g_N,DTypeStr[g_DType],g_M
		,(g_NoiseFlag ? "On":"Off"),g_Noise);
	TextOut(g_hdcVirual,0,530, DataStr,strlen(DataStr));

	sprintf(DataStr,"��~�\�莞��  (%.2lf)",g_Stop_Time);
	TextOut(g_hdcVirual,0,550, DataStr,strlen(DataStr));
	//
	if(mCDetect)
	{
		sprintf(DataStr,"���o��f %8d/%8d * 100 = %3.2lf %%�@�@",mCDetect->getCount(),g_Bmp.getWidth()*g_Bmp.getHeight(),(mCDetect->getCount()/(double)(g_Bmp.getWidth()*g_Bmp.getHeight()))*100);
		TextOut(g_hdcVirual,0,550, DataStr,strlen(DataStr));
		sprintf(DataStr,"���ϋP�x�l %.2lf  ",mCDetect->getAveDY());
		TextOut(g_hdcVirual,0,570, DataStr,strlen(DataStr));
	}
	sprintf(DataStr,"FPS�F%00005d    ���o����: %.3lf(msec/frame) ���v:%.2lf",g_Fps
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
	
	//mpeg�摜HDC
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
	//�e�t���[���̈ړ����̌��o����єw�i�摜�̍쐬
	//	if(mCDetect && Flag==2) //�}�����炦�̏C��
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

	//�ړ����̌��o�摜�`��
	StretchBlt(
				g_hdcVirual,    // �]����f�o�C�X�R���e�L�X�g�n���h�� 
				0,
				0,
				640/2,// �]���撷���`�̕� 
				480/2,// �]���撷���`�̍��� 
				g_hDetect,    // �]�����f�o�C�X�R���e�L�X�g�n���h�� 
				0,0,g_Bmp.getWidth(),g_Bmp.getHeight(),
				SRCCOPY     // ���X�^�[�I�y���[�V����  
				);

	//�w�i�摜�`��
	StretchBlt(
				g_hdcVirual,    // �]����f�o�C�X�R���e�L�X�g�n���h�� 
				0,
				480/2,
				640/2,// �]���撷���`�̕� 
				480/2,// �]���撷���`�̍��� 
				mhBack,    // �]�����f�o�C�X�R���e�L�X�g�n���h�� 
				0,0,g_Bmp.getWidth(),g_Bmp.getHeight(),
				SRCCOPY     // ���X�^�[�I�y���[�V����  
				);
	
	StretchBlt(
				g_hdcVirual,    // �]����f�o�C�X�R���e�L�X�g�n���h�� 
				640/2,
				0,
				640/2,// �]���撷���`�̕� 
				480/2,// �]���撷���`�̍��� 
				mhMpeg,    // �]�����f�o�C�X�R���e�L�X�g�n���h�� 
				0,0,g_Bmp.getWidth(),g_Bmp.getHeight(),
				SRCCOPY     // ���X�^�[�I�y���[�V����  
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
  ���o�ݒ�l�p�_�C�A���O�{�b�N�X�̃v���V�[�W��
****************************************/
BOOL CALLBACK MyDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
//	static HWND hCheck, hLa, hN,hM;
	HWND hCheck, hLa, hN,hM,hNCheck,hBCheck,hNoise,hBlank,hIcoBai,hFtime;
	char szBuf[64];
	switch (msg) 
	{

	case WM_INITDIALOG:
			//�{�^�����̏�����
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
�ʏ�A����`�A�m�����̂����ꂩ�̃n���h����Ԃ��֐�
//g_DType
************************/
HWND GetHRadio(int no,HWND hDlg)
{
	HWND hCheck;
	switch(no)
	{
		case 1:		//����`
			hCheck = GetDlgItem(hDlg, IDC_HISEN);
			break;
		case 2:	
			//�m����
			hCheck = GetDlgItem(hDlg, IDC_NORUMU);
			break;
		case 0:
		default:
			hCheck = GetDlgItem(hDlg, IDC_NORMAL);
		}
	return hCheck;
}
/*********************
	�ǂ̃��W�I�{�^����������Ă邩�`�F�b�N����֐�
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
�������Main�֐�
�i�X���b�h)
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
	static char szFileName[1024*64]="";//�p�X���܂߂��t�@�C���̖��O������
	char szFile[1024]="";	//�t�@�C���̖��O������
    OPENFILENAME ofn;
	char strCustom[1024]="";
	static int nFilterIndex=1;
	memset(szFileName,0,sizeof(szFileName));
    memset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = "bitmap(*.bmp)\0*.bmp\0All files(*.*)\0*.*\0\0";
	/*
	lpstrFilter�́A���̖��̒ʂ�t�B���^�ł��B
	"�e�L�X�g�t�@�C��(*.txt)\0*.txt\0���ׂ�(*.*)\0*.*\0\0"
	�Ȃǂ̂悤�Ɏg���܂��B������̋�؂�ɂ̓k�������i\0�j�����܂��B�Ō�̓k�������Q�i\0\0�j���g���܂��B���ێg���Ă݂�Ƃ����ɂ킩��܂��B
	*/
    ofn.lpstrFile = szFileName;
	//lpstrFile�ɂ́AGetOpenFileName�֐����Ă΂��Ƃ����Ƀt���p�X�t���̃t�@�C�������i�[����܂��B
    ofn.lpstrFileTitle = szFile;
	//lpstrFileTitle�ɂ́A�I�����ꂽ�t�@�C�������i�[����܂��B
    ofn.nFilterIndex = nFilterIndex;
    ofn.nMaxFile = sizeof(szFileName);
	//nMaxFile�́AlpstrFile�̑傫���ł��B�Ȃ�MAX_PATH��windef.h�łQ�U�O�ƒ�`����Ă��܂��B
    ofn.nMaxFileTitle = sizeof(szFile);
	//nMaxFileTitle�́AlpstrFileTitle�̑傫���ł�
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
	
	nFilterIndex=ofn.nFilterIndex;//�I�񂾃t�@�C���`����ۑ�

    return szFileName;
}


/*****************************
	FileOpen
	�摜�t�@�C�����J���֐�
����
�@hWnd window�n���h��
  Str�@�t�@�C�����J���_�C�A���O�{�b�N�X�̃^�C�g��

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
	nFilterIndex=ofn.nFilterIndex;//�I�񂾃t�@�C���`����ۑ�
	return szFile;
}


/****************************************
  MyDlgProc
  ���Ԑݒ�l�p�_�C�A���O�{�b�N�X�̃v���V�[�W��
****************************************/
BOOL CALLBACK StopTimeSetDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
//	static HWND hCheck, hLa, hN,hM;
	HWND hLa;
	char szBuf[64];


	switch (msg) {

	case WM_INITDIALOG:
			//�e�R���g���[���̃n���h�����擾
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
  ���o�ݒ�l�p�_�C�A���O�{�b�N�X�̃v���V�[�W��
****************************************/
BOOL CALLBACK CBaiDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
	HWND hLa;
	char szBuf[64];
	
	switch (msg) {

	case WM_INITDIALOG:
			//�e�R���g���[���̃n���h�����擾
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
	������`��p
*******************************************/
void TextOutData(double nowTime,double MaxTime)
{
	char DataStr[1024];
	char Buf[128];
	TextOut(g_hdcVirual,0,490,g_LoadFileName,strlen(g_LoadFileName));
	sprintf(DataStr,"time : %6.2f / %6.2f [�b]   %lf",nowTime,MaxTime,g_TimeNo);
	TextOut(g_hdcVirual,0,510,DataStr,strlen(DataStr));
	switch(g_NoiseFlag)
	{
	case NoiseOnly:
		sprintf(Buf,"�m�C�Y���� : %d",g_Noise);
		break;
	case BlankOnly:
		sprintf(Buf,"�󔒏��� : %d ",g_Blank);
		break;
	case NoiseAndBlank:
		sprintf(Buf,"�m�C�Y�����@: %d  �󔒏��� :%d",g_Noise,g_Blank);
		break;
	default:
		sprintf(Buf," ");
	}
	sprintf(DataStr,"��:%d N:%d %s M:%d %s ",g_La,g_N,DTypeStr[g_DType],g_M,Buf);
	TextOut(g_hdcVirual,0,530, DataStr,strlen(DataStr));

	sprintf(DataStr,"��~�\�莞��  (%.2lf)",g_Stop_Time);
	TextOut(g_hdcVirual,0,550, DataStr,strlen(DataStr));
	//
	if(mCDetect)
	{
		sprintf(DataStr,"���o��f %8d/%8d * 100 = %3.2lf %%�@�@",mCDetect->getCount(),g_Bmp.getWidth()*g_Bmp.getHeight(),(mCDetect->getCount()/(double)(g_Bmp.getWidth()*g_Bmp.getHeight()))*100);
		TextOut(g_hdcVirual,0,550, DataStr,strlen(DataStr));
		sprintf(DataStr,"���ϋP�x�l %.2lf  ",mCDetect->getAveDY());
		TextOut(g_hdcVirual,0,570, DataStr,strlen(DataStr));
	}
	sprintf(DataStr,"FPS�F%00005d    ���o����: %.3lf(msec/frame) ���v:%.2lf",g_Fps
		,(double)(timeGetTime()-g_TimeFast)/g_AllFrame,(double)(timeGetTime()-g_TimeFast) );
	TextOut(g_hdcVirual,0,590, DataStr,strlen(DataStr));
}