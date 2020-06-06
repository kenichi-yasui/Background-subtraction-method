
#include "CAnimationOneShot.h"
HRESULT	CAnimationOneShot::Load(LPCWSTR wszFileName ,HWND hWnd)
{
	HRESULT	hResult=CAnimation::Load(wszFileName,hWnd);
	if( FAILED(hResult) )
		return hResult;
		//追加
	hResult =m_pGrph->QueryInterface(IID_IMediaEvent,
	(LPVOID *)&m_pMediaEvent);
	if( FAILED(hResult) ){
		ReleaseAll();
		mFlag=0;
		return hResult;
	}
	hResult =m_pGrph -> QueryInterface( IID_IMediaSeeking, (void **)&m_pMediaSeeking );
	if( FAILED(hResult) ){
		ReleaseAll();
		mFlag=0;
		return hResult;
	}
	mFlag=1;//読み込んでいる。
	return S_OK;	

}

HRESULT CAnimationOneShot::OneShotPlay(HWND hWnd,int x,int y,double PlayTime)

/****************************************
	Play
	読み込んでいるメディアを再生する。
	引数
	hWnd 描画先のwindowハンドル
	x,y  描画する座標
	戻り値: (HRESULT)各メソッドの戻り値、成功時にはS_OK
****************************************/
/*
{
	
	HRESULT		hResult;
	if(!m_pGrph || !m_pMdaCtrl || !m_pVdoWin)
		return TRUE;
	if( m_IsPlay )	//	return TRUE;		// 既にプレイしてた
		Stop();

	m_pVdoWin->put_Owner( (OAHWND)hWnd );						// ビデオを再生するウィンドウの指定
	m_pVdoWin->put_WindowStyle( WS_CHILD | WS_CLIPSIBLINGS );	// 再生ウィンドウは子ウィンドウスタイル
	IMediaEvent	*pEvent;
	AM_MEDIA_TYPE	am_media_type;
	m_pGrph->QueryInterface(IID_IMediaEvent,
						(LPVOID	*)&pEvent);
	m_pSampleGrabber->GetConnectedMediaType(&am_media_type);
	VIDEOINFOHEADER	*pVideoInfoHeader	=
					(VIDEOINFOHEADER	*)am_media_type.pbFormat;
	
	m_pVdoWin->SetWindowPosition(
		x,y,pVideoInfoHeader->bmiHeader.biWidth,pVideoInfoHeader->bmiHeader.biHeight);


	DebugMsg( hWnd, "レンダリングを開始します。" );
	m_pSampleGrabber->SetBufferSamples(TRUE);
	m_pMdaCtrl->StopWhenReady();
	m_pMediaPosition->put_CurrentPosition(PlayTime);
//	m_pSampleGrabber->SetOneShot(TRUE);
	// フィルタ グラフの開始
	hResult = m_pMdaCtrl->Run();
	if( FAILED(hResult) ){
		ReleaseAll();
		return hResult;
	}
	long eventCode;
//	m_pMediaEvent->WaitForCompletion(PlayTime, &eventCode);

 // 再生開始

	m_IsPlay = TRUE;
	mFlag=2;	//再生中のフラグ

	return S_OK;
}
*/
{
	OAFilterState fs;
	m_pMediaPosition->put_CurrentPosition(PlayTime);	
	m_pMdaCtrl -> StopWhenReady();	
	m_pMdaCtrl -> GetState( INFINITE, & fs);		// レンダリング
	return S_OK;
}
/*
HRESULT CAnimationOneShot::OneShotPlayStart(void)
{
	LONGLONG nn=0;
	OAFilterState fs;

		
	m_pMediaSeeking -> SetTimeFormat( &(TIME_FORMAT_FRAME) ) ;	
	m_pSampleGrabber -> SetBufferSamples(TRUE);				// グラブ開始
	m_pMediaSeeking -> SetPositions( &nn, AM_SEEKING_AbsolutePositioning,
					          &nn, AM_SEEKING_AbsolutePositioning );	
	m_pMdaCtrl -> StopWhenReady();	
	m_pMdaCtrl -> GetState( INFINITE, & fs);		// レンダリング
		mFlag=3;	//再生中のフラグ

	return S_OK;
}
*/
HRESULT CAnimationOneShot::OneShotPlayStart(HWND hWnd,int x,int y)
{
	LONGLONG nn=0;
	OAFilterState fs;


//	m_pVdoWin->put_Owner( (OAHWND)hWnd );						// ビデオを再生するウィンドウの指定
//	m_pVdoWin->put_Owner( NULL);						// ビデオを再生するウィンドウの指定
//	m_pVdoWin->put_WindowState(SW_HIDE);
//	m_pVdoWin->put_WindowStyle( WS_CHILD | WS_CLIPSIBLINGS );	// 再生ウィンドウは子ウィンドウスタイル
	  // ウィンドウを非表示に
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
	m_pMediaSeeking -> SetTimeFormat( &(TIME_FORMAT_FRAME) ) ;	
	m_pSampleGrabber -> SetBufferSamples(TRUE);				// グラブ開始
	m_pMediaSeeking -> SetPositions( &nn, AM_SEEKING_AbsolutePositioning,
					          &nn, AM_SEEKING_AbsolutePositioning );	
	m_pMdaCtrl -> StopWhenReady();	
	m_pMdaCtrl -> GetState( INFINITE, & fs);		// レンダリング
		mFlag=4;	//再生中のフラグ
	//get_BackgroundPalette メソッドは、ビデオ ウィンドウがパレットをバックグラウンドで実現するかどうかを照会する。 
//	m_pVdoWin->get_BackgroundPalette(OAFALSE);
	// ビデオ レンダラがビデオ ウィンドウを自動的に表示するかどうかを指定する。次のいずれかの値を指定する
//	m_pVdoWin->put_AutoShow(OAFALSE);
	//ビデオ ウィンドウがパレットをバックグラウンドで実現するかどうかを指定する。
//	m_pVdoWin->put_BackgroundPalette(OAFALSE);
	//ビデオ ウィンドウの表示/非表示を切り替える。 ウィンドウってメニューとかのことな
//	m_pVdoWin->put_Visible(OAFALSE);
//	m_pVdoWin->put_WindowState(SW_MINIMIZE);
	//m_pVdoWin->put_WindowState(SW_HIDE);

	return S_OK;
}