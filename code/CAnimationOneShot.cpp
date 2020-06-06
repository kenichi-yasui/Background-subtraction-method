
#include "CAnimationOneShot.h"
HRESULT	CAnimationOneShot::Load(LPCWSTR wszFileName ,HWND hWnd)
{
	HRESULT	hResult=CAnimation::Load(wszFileName,hWnd);
	if( FAILED(hResult) )
		return hResult;
		//�ǉ�
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
	mFlag=1;//�ǂݍ���ł���B
	return S_OK;	

}

HRESULT CAnimationOneShot::OneShotPlay(HWND hWnd,int x,int y,double PlayTime)

/****************************************
	Play
	�ǂݍ���ł��郁�f�B�A���Đ�����B
	����
	hWnd �`����window�n���h��
	x,y  �`�悷����W
	�߂�l: (HRESULT)�e���\�b�h�̖߂�l�A�������ɂ�S_OK
****************************************/
/*
{
	
	HRESULT		hResult;
	if(!m_pGrph || !m_pMdaCtrl || !m_pVdoWin)
		return TRUE;
	if( m_IsPlay )	//	return TRUE;		// ���Ƀv���C���Ă�
		Stop();

	m_pVdoWin->put_Owner( (OAHWND)hWnd );						// �r�f�I���Đ�����E�B���h�E�̎w��
	m_pVdoWin->put_WindowStyle( WS_CHILD | WS_CLIPSIBLINGS );	// �Đ��E�B���h�E�͎q�E�B���h�E�X�^�C��
	IMediaEvent	*pEvent;
	AM_MEDIA_TYPE	am_media_type;
	m_pGrph->QueryInterface(IID_IMediaEvent,
						(LPVOID	*)&pEvent);
	m_pSampleGrabber->GetConnectedMediaType(&am_media_type);
	VIDEOINFOHEADER	*pVideoInfoHeader	=
					(VIDEOINFOHEADER	*)am_media_type.pbFormat;
	
	m_pVdoWin->SetWindowPosition(
		x,y,pVideoInfoHeader->bmiHeader.biWidth,pVideoInfoHeader->bmiHeader.biHeight);


	DebugMsg( hWnd, "�����_�����O���J�n���܂��B" );
	m_pSampleGrabber->SetBufferSamples(TRUE);
	m_pMdaCtrl->StopWhenReady();
	m_pMediaPosition->put_CurrentPosition(PlayTime);
//	m_pSampleGrabber->SetOneShot(TRUE);
	// �t�B���^ �O���t�̊J�n
	hResult = m_pMdaCtrl->Run();
	if( FAILED(hResult) ){
		ReleaseAll();
		return hResult;
	}
	long eventCode;
//	m_pMediaEvent->WaitForCompletion(PlayTime, &eventCode);

 // �Đ��J�n

	m_IsPlay = TRUE;
	mFlag=2;	//�Đ����̃t���O

	return S_OK;
}
*/
{
	OAFilterState fs;
	m_pMediaPosition->put_CurrentPosition(PlayTime);	
	m_pMdaCtrl -> StopWhenReady();	
	m_pMdaCtrl -> GetState( INFINITE, & fs);		// �����_�����O
	return S_OK;
}
/*
HRESULT CAnimationOneShot::OneShotPlayStart(void)
{
	LONGLONG nn=0;
	OAFilterState fs;

		
	m_pMediaSeeking -> SetTimeFormat( &(TIME_FORMAT_FRAME) ) ;	
	m_pSampleGrabber -> SetBufferSamples(TRUE);				// �O���u�J�n
	m_pMediaSeeking -> SetPositions( &nn, AM_SEEKING_AbsolutePositioning,
					          &nn, AM_SEEKING_AbsolutePositioning );	
	m_pMdaCtrl -> StopWhenReady();	
	m_pMdaCtrl -> GetState( INFINITE, & fs);		// �����_�����O
		mFlag=3;	//�Đ����̃t���O

	return S_OK;
}
*/
HRESULT CAnimationOneShot::OneShotPlayStart(HWND hWnd,int x,int y)
{
	LONGLONG nn=0;
	OAFilterState fs;


//	m_pVdoWin->put_Owner( (OAHWND)hWnd );						// �r�f�I���Đ�����E�B���h�E�̎w��
//	m_pVdoWin->put_Owner( NULL);						// �r�f�I���Đ�����E�B���h�E�̎w��
//	m_pVdoWin->put_WindowState(SW_HIDE);
//	m_pVdoWin->put_WindowStyle( WS_CHILD | WS_CLIPSIBLINGS );	// �Đ��E�B���h�E�͎q�E�B���h�E�X�^�C��
	  // �E�B���h�E���\����
	if(hWnd!=NULL)

	{
		m_pVdoWin->put_Owner( (OAHWND)hWnd );						// �r�f�I���Đ�����E�B���h�E�̎w��
		m_pVdoWin->put_WindowStyle( WS_CHILD | WS_CLIPSIBLINGS );	// �Đ��E�B���h�E�͎q�E�B���h�E�X�^�C��
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
	m_pSampleGrabber -> SetBufferSamples(TRUE);				// �O���u�J�n
	m_pMediaSeeking -> SetPositions( &nn, AM_SEEKING_AbsolutePositioning,
					          &nn, AM_SEEKING_AbsolutePositioning );	
	m_pMdaCtrl -> StopWhenReady();	
	m_pMdaCtrl -> GetState( INFINITE, & fs);		// �����_�����O
		mFlag=4;	//�Đ����̃t���O
	//get_BackgroundPalette ���\�b�h�́A�r�f�I �E�B���h�E���p���b�g���o�b�N�O���E���h�Ŏ������邩�ǂ������Ɖ��B 
//	m_pVdoWin->get_BackgroundPalette(OAFALSE);
	// �r�f�I �����_�����r�f�I �E�B���h�E�������I�ɕ\�����邩�ǂ������w�肷��B���̂����ꂩ�̒l���w�肷��
//	m_pVdoWin->put_AutoShow(OAFALSE);
	//�r�f�I �E�B���h�E���p���b�g���o�b�N�O���E���h�Ŏ������邩�ǂ������w�肷��B
//	m_pVdoWin->put_BackgroundPalette(OAFALSE);
	//�r�f�I �E�B���h�E�̕\��/��\����؂�ւ���B �E�B���h�E���ă��j���[�Ƃ��̂��Ƃ�
//	m_pVdoWin->put_Visible(OAFALSE);
//	m_pVdoWin->put_WindowState(SW_MINIMIZE);
	//m_pVdoWin->put_WindowState(SW_HIDE);

	return S_OK;
}