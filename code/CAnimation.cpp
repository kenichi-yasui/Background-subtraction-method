
/*----------インクルード---------------------*/
#include <windows.h>						
#include <dShow.h>						
	
#include <stdio.h>							
#include "CAnimation.h"
#define SZ_CLASSNAME		"sample4"

/*----------グローバル変数---------------------*/
int g_Com=0;

/****************************
	コンストラクタ
	引数
	hInst	インスタンスハンドル
*****************************/
CAnimation::CAnimation(HINSTANCE hInst)
{
	HRESULT		hResult;				// COM系 APIの戻り値保存用
	if(g_Com==0)	//COMライブラリの初期化が行われていない場合
	{
		hResult = CoInitialize( NULL );	// COMライブラリの初期化
		if( FAILED(hResult) )
			MessageBox( NULL, "COMライブラリ初期化失敗", "Error", MB_OK | MB_ICONSTOP );
	}
	g_Com++;
	m_hInst=hInst;
	m_IsPlay = FALSE;			// Play()をコールした時点でTRUE, Stop()コールでFALSE
// DirectShow関係///////////
	m_pGrph    = NULL;			// グラフ ビルダ
	m_pMdaCtrl = NULL;			// メディア コントロール
	m_pVdoWin  = NULL;			// ビデオ ウィンドウ
	m_pSampleGrabberFilter=NULL;
	m_pSampleGrabber=NULL;
	m_pMediaPosition=NULL;
	mFlag=0;

}
/*********************************
	デストラクタ

******************************/
CAnimation::~CAnimation()
{
	endStop();
	g_Com--;
	if(g_Com==0)
		CoUninitialize();	// COMの終了
}

/*********************************
	DebuMsg
	デバック用
	hList メッセージ出力先のリストボックスのウィンドウ ハンドル
	lpMsg メッセージ
******************************/
void CAnimation::DebugMsg( HWND hList, LPCSTR lpMsg )
{
	LONG	lCount;
	SendMessage( hList, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)lpMsg );
	lCount = SendMessage( hList, LB_GETCOUNT, 0, 0 );
	SendMessage( hList, LB_SETCURSEL, (WPARAM)lCount-1L, (LPARAM)0 );
	UpdateWindow( hList );
	// デバッグメッセージは、処理がブロッキングする前に表示される必要があるため、メッセージを表示したらすぐに更新
}

#define RELEASE_SET(v) if(v){v->Release(); v=NULL;}
/*************************************************
	ReleaseAll
	NULLでないインスタンスを解放し、NULLにする。
*************************************************/
void CAnimation::ReleaseAll( void )
{
	RELEASE_SET( m_pVdoWin )
	RELEASE_SET( m_pMdaCtrl )
	RELEASE_SET( m_pGrph )
	RELEASE_SET(m_pSampleGrabberFilter)
	RELEASE_SET(m_pSampleGrabber)
	RELEASE_SET(m_pMediaPosition)
	mFlag=0;
	RemoveFromRot();

}



/*************************************************
	Load
	指定されたメディアファイルを再生する。
	引数
	wszFileName   再生するメディアファイル名	
	hWnd          ムービーを再生するウィンドウのハンドル
*************************************************/
HRESULT CAnimation::Load(LPCWSTR wszFileName ,HWND hWnd)
{
	endStop();
	HRESULT		hResult;
	// 開始
	DebugMsg( hWnd, "フィルタ グラフを構築しています。" );

	// フィルタ グラフ マネージャのインスタンス取得
	hResult = CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (LPVOID *)&m_pGrph );
	if( FAILED(hResult) ){
		MessageBox( hWnd, "インスタンス取得失敗", "Error", MB_OK | MB_ICONSTOP );
		return hResult;
	}
	AddToRot(m_pGrph);
	// フィルタ グラフの構築
	hResult = m_pGrph->QueryInterface( IID_IMediaControl, (LPVOID *)&m_pMdaCtrl );
	if( FAILED(hResult) ){
		ReleaseAll();
		return hResult;
	}
	
	hResult = m_pGrph->QueryInterface( IID_IVideoWindow, (LPVOID *)&m_pVdoWin );
	if( FAILED(hResult) ){
		ReleaseAll();
		return hResult;
	}
	
/////////////////
	//	SampleGrabber(Filter)を生成
	CoCreateInstance(CLSID_SampleGrabber,
		NULL,
		CLSCTX_INPROC,
		IID_IBaseFilter,
		(LPVOID	*)&m_pSampleGrabberFilter);
	//	FilterからISampleGrabberインターフェースを取得
	m_pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber,
		(LPVOID	*)&m_pSampleGrabber);
//	SampleGrabberを接続するフォーマットを指定。
	//	ここの指定の仕方によりSampleGrabberの挿入箇所を決定。
	AM_MEDIA_TYPE	am_media_type;
	ZeroMemory(&am_media_type,	sizeof(am_media_type));
	am_media_type.majortype	=	MEDIATYPE_Video;
	am_media_type.subtype	=	MEDIASUBTYPE_RGB24;
	am_media_type.formattype	=	FORMAT_VideoInfo;
	m_pSampleGrabber->SetMediaType(&am_media_type);
	//	GraphにSampleGrabber	Filterを追加
	m_pGrph->AddFilter(m_pSampleGrabberFilter,
		L"Sample	Grabber");
	///////////////
	m_pGrph->QueryInterface(IID_IMediaPosition,
	(LPVOID *)&m_pMediaPosition);
	DebugMsg( hWnd, "ファイルを開いています。" );
	// グラフにレンダラを追加
	hResult = m_pGrph->RenderFile( wszFileName, NULL );
	if( FAILED(hResult) ){
		ReleaseAll();
		return hResult;
	}

	mFlag=1;//読み込んでいる。
	return S_OK;
}
void CAnimation::ErrorChekGetCurrentBuffer(HRESULT Flag,long nBufferSize)
{
	char str[256]="不明";
	/*
	E_INVALIDARG サンプルはバッファリングされていない。 
	E_OUTOFMEMORY 指定されたバッファの大きさが不十分。 
	E_POINTER NULL ポインタ引数。 
	S_OK  成功。 
	VFW_E_NOT_CONNECTED フィルタが接続されていない。 
	VFW_E_WRONG_STATE フィルタがまだサンプルを受け取っていない。サンプルを出力するには、グラフを実行するかポーズする。 
		*/
		switch(Flag)
		{
		case E_INVALIDARG:
			strcpy(str,"サンプルはバッファリングされていない。");
			break;	
		case E_OUTOFMEMORY:
			sprintf(str,"%d 指定されたバッファの大きさが不十分。",nBufferSize);
			break;
		case E_POINTER:
			strcpy(str,"NULL ポインタ引数。");
			break;
		case VFW_E_NOT_CONNECTED :
			strcpy(str,"フィルタが接続されていない");
			break;
		case VFW_E_WRONG_STATE:
			strcpy(str,"ィルタがまだサンプルを受け取っていない。サンプルを出力するには、グラフを実行するかポーズする。 ");
			break;

		}
		MessageBox(NULL,str,"error",MB_OK);
}
/***************************************************
	getFrame
	描画中のフレーム画像を取得する関数
	引数
	pBmp	画像処理クラスへのポインタ
			（このクラスにフレームの画像が入る。）
**************************************************/
CReadBmp* CAnimation::getFrame(CReadBmp *pBmp)
{
	int BmpNewFlag=0;
	if(!m_pGrph || !m_pMdaCtrl || !m_pVdoWin)
		return NULL;
	if(!pBmp)
	{
		pBmp=new CReadBmp();
		BmpNewFlag++;
	}
	IMediaEvent	*pEvent;
	AM_MEDIA_TYPE	am_media_type;
	m_pGrph->QueryInterface(IID_IMediaEvent,
						(LPVOID	*)&pEvent);
	m_pSampleGrabber->GetConnectedMediaType(&am_media_type);
	VIDEOINFOHEADER	*pVideoInfoHeader	=
					(VIDEOINFOHEADER	*)am_media_type.pbFormat;
		//	バッファを用意
	long	nBufferSize	=	am_media_type.lSampleSize;
	long	*pBuffer	=	(long	*)malloc(nBufferSize);

	if(pBuffer==NULL)
		return NULL;
	static int no=0;
	//	現在表示されている映像を静止画として取得
	HRESULT Flag=m_pSampleGrabber->GetCurrentBuffer(&nBufferSize,pBuffer);
	char str[256]="不明";
	if(Flag!=S_OK )
	{
		free(pBuffer);
		if( BmpNewFlag==1)
			delete pBmp;
		if(no==0)
			ErrorChekGetCurrentBuffer(Flag,nBufferSize);
		return NULL;
	}
	else
	{
	}
	if(pVideoInfoHeader==NULL)
	{
		free(pBuffer);	
		if( BmpNewFlag==1)
			delete pBmp;
		return NULL;
	}
	////画像クラスに画素を代入///////////
	pBmp->CreateBmp(pVideoInfoHeader->bmiHeader.biWidth,
		pVideoInfoHeader->bmiHeader.biHeight);
	CopyMemory(pBmp->getRgbP(),pBuffer,nBufferSize);

	free(pBuffer);

	return pBmp;

}

/****************************************
	Play
	読み込んでいるメディアを再生する。
	引数
	hWnd 描画先のwindowハンドル
	x,y  描画する座標
	戻り値: (HRESULT)各メソッドの戻り値、成功時にはS_OK
****************************************/
HRESULT CAnimation::Play( HWND hWnd,int x,int y)
{
	HRESULT		hResult;
	  if(!m_pGrph || !m_pMdaCtrl || !m_pVdoWin)
		return TRUE;
	if( m_IsPlay )		return TRUE;		// 既にプレイしてた
	if(hWnd!=NULL)
	{
		m_pVdoWin->put_Owner( (OAHWND)hWnd );						// ビデオを再生するウィンドウの指定
		m_pVdoWin->put_WindowStyle( WS_CHILD | WS_CLIPSIBLINGS );	// 再生ウィンドウは子ウィンドウスタイル
	}
	else
		m_pVdoWin->put_AutoShow(0);
	IMediaEvent	*pEvent;
	AM_MEDIA_TYPE	am_media_type;
	m_pGrph->QueryInterface(IID_IMediaEvent,
						(LPVOID	*)&pEvent);
	m_pSampleGrabber->GetConnectedMediaType(&am_media_type);
	VIDEOINFOHEADER	*pVideoInfoHeader	=
					(VIDEOINFOHEADER	*)am_media_type.pbFormat;
	if(hWnd!=NULL)
	{
		m_pVdoWin->SetWindowPosition(
		x,y,pVideoInfoHeader->bmiHeader.biWidth,pVideoInfoHeader->bmiHeader.biHeight);
	}
	DebugMsg( hWnd, "レンダリングを開始します。" );
	// フィルタ グラフの開始
	hResult = m_pMdaCtrl->Run();
	if( FAILED(hResult) ){
		ReleaseAll();
		return hResult;
	}
	m_pSampleGrabber->SetBufferSamples(TRUE);
	m_IsPlay = TRUE;
	mFlag=2;	//再生中のフラグ
	
	return S_OK;
}

/***********************************
	Stop
	再生しているメディアを停止する。
	戻り値: (HRESULT)成功時には S_OKを返す
***********************************/
HRESULT CAnimation::Stop( void )
{
	HRESULT	hResult;

	if( !m_IsPlay )		return (HRESULT)1000L;	// まだ再生されていない

	// フィルタ グラフの停止
	hResult = m_pMdaCtrl->Stop();

	m_IsPlay = FALSE;
	mFlag=3;
	return hResult;
}
/***********************************
	endStop
	メディアの再生を停止し、グラフを解放する
	戻り値: (HRESULT)成功時には S_OKを返す
***********************************/
HRESULT CAnimation::endStop(void)
{
	HRESULT	h=Stop();
	ReleaseAll();
	return h;
}
/*************************************
	PlaySpeed
	再生速度調整
	speed 再生速度
		  1が通常の再生速度
		  1未満がスロー再生
		  1以上が倍速再生
*************************************/
void CAnimation::PlaySpeed(double speed)
{ 
	  if(!m_pGrph || !m_pMdaCtrl || !m_pVdoWin)
		return;
	 m_pMediaPosition->put_Rate(speed);
}
/******************************
	getTime
	現在再生中メディアの再生時間を返す。
******************************/
double CAnimation::getTime()
{
	double llTime;
	  if(!m_pGrph || !m_pMdaCtrl || !m_pVdoWin)
		return 0;
	if(m_pMediaPosition->get_CurrentPosition(&llTime)==S_OK)
		return llTime;
	return 0;
}
/******************************
	getTime
	現在再生中メディアの終了時間をを返す。
******************************/
double CAnimation::getLastTime()
{
	double llTime;
	  if(!m_pGrph || !m_pMdaCtrl || !m_pVdoWin)
		return 0;
	if(m_pMediaPosition->get_Duration(&llTime)==S_OK)
		return llTime;
	return 0;
}
/***********************************
	setTime
	再生位置変更
	引数
	times 変更後の再生位置
************************************/
void CAnimation::setTime(double times)
{
	m_pMediaPosition->put_CurrentPosition(times);
}

HRESULT CAnimation::AddToRot(IUnknown *pUnkGraph) 
{
    IMoniker * pMoniker;
    IRunningObjectTable *pROT;
    if (FAILED(GetRunningObjectTable(0, &pROT))) {
        return E_FAIL;
    }
    WCHAR wsz[256];
    wsprintfW(wsz, L"FilterGraph %08p pid %08x", (DWORD_PTR)pUnkGraph, GetCurrentProcessId());
    HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
    if (SUCCEEDED(hr)) {
        hr = pROT->Register(0, pUnkGraph, pMoniker, &m_dwRegister);
        pMoniker->Release();
    }
    pROT->Release();
    return hr;
}

void CAnimation::RemoveFromRot()
{
    IRunningObjectTable *pROT;
    if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) {
        pROT->Revoke(m_dwRegister);
        pROT->Release();
    }
}
