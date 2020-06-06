#include <windows.h>
#include "CReadBmp2006.h"
#define GYOU 20

#define RADIO_BUTTON_ON 1


class CGraphRGBY
{
public:
	CGraphRGBY(int MaxX,int MaxY);
	void PrintRGB(HDC hdc,CReadBmp *Bmp,int RGBPaintX,int RGBPaintY,
		int GraphStartX,int GraphStartY,int Graph_Point);
	friend	BOOL CALLBACK CtlCamDlgProc(HWND hDlg,UINT msg,WPARAM wp,LPARAM lp);
	//////
	bool setXY(int x,int y);
	void Reset(void);
	void CreateDlg(HWND hWnd,LPCTSTR DlgStr);
#ifdef RADIO_BUTTON_ON 
	int getRadioFlag(void);
#endif 
protected :
	int m_X;
	int m_Y;
	unsigned int m_exR[10],m_exG[10],m_exB[10],m_exV[10],m_exY[10],m_exYGosa[10];
	int m_exNo;
	int m_FlagLogSave;//ÉçÉOï€ë∂Ç∑ÇÈÇ©Ç«Ç§Ç©
	FILE *m_LogFileP;
	void PaintGraph(HDC hdc,unsigned int *p);
	void ChangeExRGB(CReadBmp *Bmp);
	void BackGraphDraw(HDC hdc);
	int m_RGBPaintX,m_RGBPaintY;
	int m_GraphStartX;
	int m_GraphStartY;
	int m_Graph_Point;
	void SetPrintXY(int RGBPaintX,int RGBPaintY,
	int GraphStartX,int GraphStartY,int Graph_Point);
	int m_MaxX;
	int m_MaxY;
	HWND m_hDlg;
	void setHWnd(HWND hWnd);
#ifdef RADIO_BUTTON_ON 
	int m_RadioFlag;
	void SetRadioFlag(int Flag){m_RadioFlag=Flag;};

#endif

};
	char *FileSave(HWND hWnd,const char *Str,const char *FileType);