
#include <math.h>
#include "resource.h"
#include "CGraphRGBY.h"

/*
#define GRAPH_POINT 30
#define GRAPH_START_X 400
#define GRAPH_START_Y 400
*/
#define GRAPH_END_X m_GraphStartX+m_Graph_Point*10
#define GRAPH_END_Y m_GraphStartY-m_Graph_Point*10

#define LOG_START 1
#define LOG_STOP  0
CGraphRGBY *g_pCGraph;

CGraphRGBY::CGraphRGBY(int MaxX,int MaxY)
{
	m_X=0;
	m_Y=0;
	ZeroMemory(m_exR,10);
	ZeroMemory(m_exG,10);
	ZeroMemory(m_exB,10);
	ZeroMemory(m_exV,10);
	ZeroMemory(m_exY,10);
	ZeroMemory(m_exYGosa,10);
	m_exNo=-1;
	m_FlagLogSave=0;//ログ保存するかどうか
	m_LogFileP=NULL;
	g_pCGraph=this;
	m_MaxX=MaxX;
	m_MaxY=MaxY;
	m_hDlg=NULL;
#ifdef RADIO_BUTTON_ON
	m_RadioFlag=IDC_MPEG_G;
#endif
}
void CGraphRGBY::Reset(void)
{
	ZeroMemory(m_exR,10);
	ZeroMemory(m_exG,10);
	ZeroMemory(m_exB,10);
	ZeroMemory(m_exV,10);
	ZeroMemory(m_exY,10);
	ZeroMemory(m_exYGosa,10);
	m_exNo=-1;
	m_FlagLogSave=0;//ログ保存するかどうか
	if(m_LogFileP)
		fclose(m_LogFileP);
	m_LogFileP=NULL;
}
/******************
	PrintRGB
	RGB変化を表示
******************/
void CGraphRGBY::PrintRGB(HDC hdc,CReadBmp *Bmp,int RGBPaintX,int RGBPaintY,
		int GraphStartX,int GraphStartY,int Graph_Point)
{
	//下策
	if(hdc==NULL)
	{
		HDC hDdc;
		if(m_hDlg==NULL)
			return;
		hDdc=GetDC(m_hDlg);
		PrintRGB(hDdc,Bmp,RGBPaintX,RGBPaintY,
		GraphStartX,GraphStartY,Graph_Point);
		ReleaseDC(m_hDlg,hDdc);
		return;
	}

	int x=m_X,y=m_Y;
	SetPrintXY( RGBPaintX, RGBPaintY,
		 GraphStartX, GraphStartY, Graph_Point);

	char str[256];
	sprintf(str,"座標(%003d , %003d)",x,y);
	int Height=Bmp->getHeight();
	TextOut(hdc,m_RGBPaintX, m_RGBPaintY+GYOU*2,str,strlen(str));

	ChangeExRGB(Bmp);
	BackGraphDraw(hdc);

	sprintf(str,"exNo : %003d",m_exNo);
	TextOut(hdc,m_RGBPaintX,m_RGBPaintY+GYOU*9,str,strlen(str));
	if(m_FlagLogSave)
	{
		sprintf(str,"ログ保存中。",m_exNo);
		TextOut(hdc,m_RGBPaintX,m_RGBPaintY+GYOU*10,str,strlen(str));
	}
	else //下作
	{
		sprintf(str,"                       ",m_exNo);
		TextOut(hdc,m_RGBPaintX,m_RGBPaintY+GYOU*10,str,strlen(str));
	}
	sprintf(str,"R : %003d",m_exR[m_exNo]);
	TextOut(hdc,m_RGBPaintX,m_RGBPaintY+GYOU*3,str,strlen(str));
	sprintf(str,"G : %003d",m_exG[m_exNo]);
	TextOut(hdc,m_RGBPaintX,m_RGBPaintY+GYOU*4,str,strlen(str));
	sprintf(str,"B : %003d",m_exB[m_exNo]);
	TextOut(hdc,m_RGBPaintX,m_RGBPaintY+GYOU*5,str,strlen(str));
	sprintf(str,"Y : %003d",m_exY[m_exNo]);
	TextOut(hdc,m_RGBPaintX,m_RGBPaintY+GYOU*6,str,strlen(str));
	sprintf(str,"V : %003d",m_exV[m_exNo]);
	TextOut(hdc,m_RGBPaintX,m_RGBPaintY+GYOU*7,str,strlen(str));
	sprintf(str,"Y : %003d",m_exYGosa[m_exNo]);
	TextOut(hdc,m_RGBPaintX,m_RGBPaintY+GYOU*8,str,strlen(str));
	int PenStartX=80+m_RGBPaintX,LastY;
	HPEN hPen, hOldPen,hPenBack;
	/*
    
	hPen = CreatePen(PS_SOLID, 3, RGB(0,0, 0));
	hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, x-5,y, NULL);
    LineTo(hdc, x+5,y);
    MoveToEx(hdc, x,y+5, NULL);
    LineTo(hdc, x,y-5);
    DeleteObject(hPen);
	*/
	//Back
	hPenBack = CreatePen(PS_SOLID, GYOU-4, RGB(100,100, 100));
    hOldPen = (HPEN)SelectObject(hdc, hPenBack);
	LastY=m_RGBPaintY+GYOU*3+8;
	MoveToEx(hdc, PenStartX, LastY, NULL);
    LineTo(hdc, 255+PenStartX, LastY);
	LastY=m_RGBPaintY+GYOU*4+8;
	MoveToEx(hdc, PenStartX, LastY, NULL);
    LineTo(hdc, 255+PenStartX, LastY);
	LastY=m_RGBPaintY+GYOU*5+8;
	MoveToEx(hdc, PenStartX, LastY, NULL);
    LineTo(hdc, 255+PenStartX, LastY);
	LastY=m_RGBPaintY+GYOU*6+8;
	MoveToEx(hdc, PenStartX, LastY, NULL);
    LineTo(hdc, 255+PenStartX, LastY);
	LastY=m_RGBPaintY+GYOU*7+8;
	MoveToEx(hdc, PenStartX, LastY, NULL);
    LineTo(hdc, 255+PenStartX, LastY);
	LastY=m_RGBPaintY+GYOU*8+8;
	MoveToEx(hdc, PenStartX, LastY, NULL);
    LineTo(hdc, 255+PenStartX, LastY);



	//赤ペン先生
    hPen = CreatePen(PS_SOLID, GYOU-7, RGB(255,0, 0));
    SelectObject(hdc, hPen);
	LastY=m_RGBPaintY+GYOU*3+8;
    MoveToEx(hdc, PenStartX, LastY, NULL);
    LineTo(hdc, m_exR[m_exNo]+PenStartX, LastY);

    DeleteObject(hPen);
	//緑
    hPen = CreatePen(PS_SOLID, GYOU-7, RGB(0,255, 0));
    SelectObject(hdc, hPen);
	LastY=m_RGBPaintY+GYOU*4+8;
    MoveToEx(hdc, PenStartX, LastY, NULL);
    LineTo(hdc, m_exG[m_exNo]+PenStartX, LastY);
    DeleteObject(hPen);
	//青
	hPen = CreatePen(PS_SOLID, GYOU-7, RGB(0,0, 255));
    SelectObject(hdc, hPen);
	LastY=m_RGBPaintY+GYOU*5+8;
    MoveToEx(hdc, PenStartX, LastY, NULL);
    LineTo(hdc, m_exB[m_exNo]+PenStartX, LastY);
    DeleteObject(hPen);
	//輝度
	hPen = CreatePen(PS_SOLID, GYOU-7, RGB(255,255, 255));
    (HPEN)SelectObject(hdc, hPen);
	LastY=m_RGBPaintY+GYOU*6+8;
    MoveToEx(hdc, PenStartX, LastY, NULL);
    LineTo(hdc, m_exY[m_exNo]+PenStartX, LastY);
    DeleteObject(hPen);


	hPen = CreatePen(PS_SOLID, GYOU-7, RGB(255,255, 0));
    (HPEN)SelectObject(hdc, hPen);
	LastY=m_RGBPaintY+GYOU*7+8;
    MoveToEx(hdc, PenStartX, LastY, NULL);
    LineTo(hdc, m_exV[m_exNo]+PenStartX, LastY);
    DeleteObject(hPen);

	hPen = CreatePen(PS_SOLID, GYOU-7, RGB(230,230, 230));
    (HPEN)SelectObject(hdc, hPen);
	LastY=m_RGBPaintY+GYOU*8+8;
    MoveToEx(hdc, PenStartX, LastY, NULL);
    LineTo(hdc, m_exYGosa[m_exNo]+PenStartX, LastY);
    DeleteObject(hPen);

	///////////////
	hPen = CreatePen(PS_SOLID, 1, RGB(255,0, 0));
    (HPEN)SelectObject(hdc, hPen);
	PaintGraph(hdc,m_exR);
    DeleteObject(hPen);
	hPen = CreatePen(PS_SOLID, 1, RGB(0,255, 0));
    (HPEN)SelectObject(hdc, hPen);
	PaintGraph(hdc,m_exG);
    DeleteObject(hPen);
	hPen = CreatePen(PS_SOLID, 1, RGB(0,0,255));
    (HPEN)SelectObject(hdc, hPen);
	PaintGraph(hdc,m_exB);
    DeleteObject(hPen);
	hPen = CreatePen(PS_SOLID, 1, RGB(255,255, 255));
    (HPEN)SelectObject(hdc, hPen);
	PaintGraph(hdc,m_exY);
    DeleteObject(hPen);
	hPen = CreatePen(PS_SOLID, 1, RGB(255,255, 0));
    (HPEN)SelectObject(hdc, hPen);
	PaintGraph(hdc,m_exV);

	hPen = CreatePen(PS_SOLID, 1, RGB(230,230, 230));
    (HPEN)SelectObject(hdc, hPen);
	PaintGraph(hdc,m_exYGosa);

    SelectObject(hdc, hOldPen);

}
bool CGraphRGBY::setXY(int x,int y)
{
	if(m_MaxX<=x || x<0)
		return false;
	else if(m_MaxY<=y || y<0)
		return false;
	m_Y=y;
	m_X=x;
	return true;
}
void CGraphRGBY::PaintGraph(HDC hdc,unsigned int *p)
{
	int i;
	int no;

	for(i=0;i<9;i++)
	{
		no=( (m_exNo-i)<0 ? (m_exNo-i)+10 : m_exNo-i );
		MoveToEx(hdc, m_GraphStartX+m_Graph_Point*i, m_GraphStartY-p[no], NULL);
		no=( (no-1)<0 ? no-1+10 : no-1);
		LineTo(hdc, m_GraphStartX+m_Graph_Point*(i+1), m_GraphStartY-p[no]);
	}

}


//グラフ
void CGraphRGBY::BackGraphDraw(HDC hdc)
{
	HBRUSH hBrush, hOldBrush;
	HPEN hPen, hOldPen;

	hBrush = CreateSolidBrush(RGB(100,100,100));
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
	Rectangle(hdc,    m_GraphStartX-20,  m_GraphStartY+20,  GRAPH_END_X+20,GRAPH_END_Y-20);
	SelectObject(hdc, hOldBrush);  
	DeleteObject(hBrush);
//  グラフ
	hPen = CreatePen(PS_SOLID, 10, RGB(0,0, 0));
    hOldPen = (HPEN)SelectObject(hdc, hPen);
	MoveToEx(hdc, m_GraphStartX, m_GraphStartY, NULL);
    LineTo(hdc, m_GraphStartX,GRAPH_END_Y);	
	MoveToEx(hdc, m_GraphStartX, m_GraphStartY, NULL);
    LineTo(hdc, GRAPH_END_X,m_GraphStartY);		
    DeleteObject(hPen);
	SelectObject(hdc, hOldPen);

}
void CGraphRGBY::ChangeExRGB(CReadBmp *Bmp)
{
	unsigned char R,G,B,Y,ex_Y;
	int V;
	int x=m_X,y=Bmp->getHeight()-m_Y;

	Bmp->getRGB(x,y,&R,&G,&B);
	Y=Bmp->getYIQ(x,y,0);
	ex_Y=m_exY[m_exNo];
	double s_data=(m_exR[m_exNo]-R)*(m_exR[m_exNo]-R)+ (m_exG[m_exNo]-G) * (m_exG[m_exNo]-G) + (m_exB[m_exNo]-B)*(m_exB[m_exNo]-B);
	V=sqrt(s_data);
//	V=sqrt( (m_exR[m_exNo]-R)*(m_exR[m_exNo]-R)+ (m_exG[m_exNo]-G) * (m_exG[m_exNo]-G) + (m_exB[m_exNo]-B)*(m_exB[m_exNo]-B) );
	m_exNo++;
	if(m_exNo>=10)
		m_exNo=0;
	m_exR[m_exNo]=R;
	m_exG[m_exNo]=G;
	m_exB[m_exNo]=B;
	m_exY[m_exNo]=Y;
	m_exV[m_exNo]=V;
	m_exYGosa[m_exNo]=abs(ex_Y-Y);
	//ログ保存
	switch(m_FlagLogSave)
	{
	case LOG_START:
		fprintf(m_LogFileP,"%d\t",m_exR[m_exNo]);
		fprintf(m_LogFileP,"%d\t",m_exG[m_exNo]);
		fprintf(m_LogFileP,"%d\t",m_exB[m_exNo]);
		fprintf(m_LogFileP,"%d\t",m_exY[m_exNo]);
		fprintf(m_LogFileP,"%d\t",m_exV[m_exNo]);
		fprintf(m_LogFileP,"%d\t",m_exYGosa[m_exNo]);	
		fprintf(m_LogFileP,"\n");
		break;
	case LOG_STOP:
		if(m_LogFileP!=NULL)
		{
			fclose(m_LogFileP);
			m_LogFileP=NULL;
		}
		break;

	}
	
}
void CGraphRGBY::setHWnd(HWND hWnd)
{
	m_hDlg=hWnd;
}

BOOL CALLBACK CtlCamDlgProc(HWND hDlg,UINT msg,WPARAM wp,LPARAM lp)
{
	static char Buf[MAX_PATH];
	char *FNameP;
	switch(msg)
	{
	//ダイヤログ版のWM_CREATEみたいなもん WM_CREATEはこないのでキヲツケロ
	case WM_INITDIALOG:
		g_pCGraph->setHWnd(hDlg);
		ZeroMemory(Buf,MAX_PATH);
		GetCurrentDirectory(MAX_PATH,Buf);
		strcat(Buf,"\\Log.txt");
		SetDlgItemText(hDlg,ID_SAVE_PASS,Buf);
	#ifdef RADIO_BUTTON_ON
		IsDlgButtonChecked(hDlg,g_pCGraph->getRadioFlag()); 
	#endif
		break;
	case WM_COMMAND:
		///ボタンなどのメッセージ
		switch(LOWORD(wp))
		{
		case IDC_LOG_SAVE_PASS_GET:
			FNameP=FileSave(hDlg,"色成分ログ保存先指定","txt");
			if(!FNameP)
				break;
			strcpy(Buf,FNameP);
			SetDlgItemText(hDlg,ID_SAVE_PASS,Buf);
			if(g_pCGraph->m_FlagLogSave==LOG_START)
			{
				g_pCGraph->m_FlagLogSave=LOG_STOP;
			}
			break;
		case IDC_LOG_START:

			g_pCGraph->m_LogFileP=fopen(Buf,"w");
			if(!g_pCGraph->m_LogFileP)
			{
				MessageBox(hDlg,"ファイルが開けませんでした。","Error",MB_OK);
				break;
			}
			fprintf(g_pCGraph->m_LogFileP,"R\tG\tB\tY\tV\tY(誤差)\n");
			g_pCGraph->m_FlagLogSave=LOG_START;	//ログ保存スタート
			break;
		case IDC_LOG_STOP:
			g_pCGraph->m_FlagLogSave=LOG_STOP;   //ログ保存ストップ
			break;
		case IDCANCEL:
			g_pCGraph->setHWnd(NULL);
			EndDialog(hDlg,IDCANCEL);
			return TRUE;
#ifdef RADIO_BUTTON_ON
		case IDC_DETECT_G:
		case IDC_MPEG_G:
		case IDC_BACK_G:
			g_pCGraph->SetRadioFlag(LOWORD(wp));
			break;

#endif
		}
		return FALSE;
	}
	return FALSE;
}
void CGraphRGBY::CreateDlg(HWND hWnd,LPCTSTR DlgStr)
{	
	if(m_hDlg)
		return;
	HINSTANCE hInst = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	DialogBox(hInst,DlgStr,NULL,CtlCamDlgProc);
}
/*****************************
	FileSave
	ファイルを保存関数
引数
　hWnd windowハンドル
  Str　ファイルを保存するダイアログボックスのタイトル
  FileType ファイル形式(例,"txt",",mpeg")
*****************************/
char *FileSave(HWND hWnd,const char *Str,const char *FileType)
{
    DWORD dwSize = 0L;
    OPENFILENAME ofn;
	char szFileTitle[512];
	char szFilter[512];
	static char szFile[512];
	int len;
	sprintf(szFilter,"%s(*.%s)",FileType,FileType);
	len=strlen(szFilter)+1;
	sprintf(&szFilter[len],"*.%s",FileType);
    memset(&ofn, 0, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
//    ofn.lpstrFilter = "txt(*.txt)\0All files(*.*)\0*.*\0\0";
	ofn.lpstrFilter = szFilter;
    ofn.lpstrFile = szFile;
    ofn.lpstrFileTitle = szFileTitle;
    ofn.nMaxFile = sizeof(szFile);
    ofn.nMaxFileTitle = sizeof(szFileTitle);
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
 //   ofn.lpstrDefExt = "txt";
	ofn.lpstrDefExt = FileType;
    ofn.lpstrTitle = Str;

    if(GetSaveFileName(&ofn) == 0)
        return NULL;
	return szFile;
}
int CGraphRGBY::getRadioFlag(void)
{
	return m_RadioFlag;
}
void CGraphRGBY::SetPrintXY(int RGBPaintX,int RGBPaintY,
		int GraphStartX,int GraphStartY,int Graph_Point)
{
	m_RGBPaintX=RGBPaintX;
	m_RGBPaintY=RGBPaintY;
	m_GraphStartX=GraphStartX;
	m_GraphStartY=GraphStartY;
	m_Graph_Point=Graph_Point;
}