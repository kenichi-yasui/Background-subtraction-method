
#ifndef C_ANIMATION_H_
#define C_ANIMATION_H_

#pragma	comment(lib,"strmiids.lib")

#include <windows.h>						
#include <dShow.h>
#pragma include_alias( "dxtrans.h", "qedit.h" )
#define __IDxtCompositor_INTERFACE_DEFINED__
#define __IDxtAlphaSetter_INTERFACE_DEFINED__
#define __IDxtJpeg_INTERFACE_DEFINED__
#define __IDxtKey_INTERFACE_DEFINED__
#include <qedit.h>
#include <stdio.h>							
#include "CReadBmp2006.h"

class CAnimation
{
protected :
	void ErrorChekGetCurrentBuffer(HRESULT Flag,long nBufferSize);
	int mFlag;//状態 0:読み込みなし、1:動画読み込み完了,2:再生中
	///////////////////
// グローバル変数
	HINSTANCE		m_hInst;				// インスタンスをとりあえず使うかもしれないから WinMain()でコピっとく。
	BOOL			m_IsPlay ;				// Play()をコールした時点でTRUE, Stop()コールでFALSE
	// DirectShow関係
	IGraphBuilder	*m_pGrph   ;			// グラフ ビルダ
	IMediaControl	*m_pMdaCtrl ;			// メディア コントロール
	IVideoWindow	*m_pVdoWin  ;			// ビデオ ウィンドウ
///////////
	IBaseFilter	*m_pSampleGrabberFilter;
	ISampleGrabber	*m_pSampleGrabber;
	IMediaPosition *m_pMediaPosition;
//ROT
	 DWORD m_dwRegister;
	HRESULT AddToRot(IUnknown *pUnkGraph);
	void RemoveFromRot();
public:

	CAnimation(HINSTANCE hInst);
	~CAnimation();
	void DebugMsg( HWND hList, LPCSTR lpMsg );
	void ReleaseAll( void );
	HRESULT Load(LPCWSTR wszFileName,HWND hWnd);

	//
	CReadBmp* getFrame(CReadBmp *pBmp=NULL);
	void setTime(double times);

	void PlaySpeed(double speed);
	double getTime(void);
	double getLastTime(void);


	HRESULT Play(HWND hWnd,int x=0,int y=0);		
	HRESULT Stop(void);
	HRESULT endStop( void );
	int getFlag(void){ return mFlag;};


};
#endif