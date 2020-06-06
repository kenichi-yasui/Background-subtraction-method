
/*----------�C���N���[�h---------------------*/
#include <windows.h>						
#include <dShow.h>						
	
#include <stdio.h>							
#include "CAnimation.h"
#define SZ_CLASSNAME		"sample4"

/*----------�O���[�o���ϐ�---------------------*/
int g_Com=0;

/****************************
	�R���X�g���N�^
	����
	hInst	�C���X�^���X�n���h��
*****************************/
CAnimation::CAnimation(HINSTANCE hInst)
{
	HRESULT		hResult;				// COM�n API�̖߂�l�ۑ��p
	if(g_Com==0)	//COM���C�u�����̏��������s���Ă��Ȃ��ꍇ
	{
		hResult = CoInitialize( NULL );	// COM���C�u�����̏�����
		if( FAILED(hResult) )
			MessageBox( NULL, "COM���C�u�������������s", "Error", MB_OK | MB_ICONSTOP );
	}
	g_Com++;
	m_hInst=hInst;
	m_IsPlay = FALSE;			// Play()���R�[���������_��TRUE, Stop()�R�[����FALSE
// DirectShow�֌W///////////
	m_pGrph    = NULL;			// �O���t �r���_
	m_pMdaCtrl = NULL;			// ���f�B�A �R���g���[��
	m_pVdoWin  = NULL;			// �r�f�I �E�B���h�E
	m_pSampleGrabberFilter=NULL;
	m_pSampleGrabber=NULL;
	m_pMediaPosition=NULL;
	mFlag=0;

}
/*********************************
	�f�X�g���N�^

******************************/
CAnimation::~CAnimation()
{
	endStop();
	g_Com--;
	if(g_Com==0)
		CoUninitialize();	// COM�̏I��
}

/*********************************
	DebuMsg
	�f�o�b�N�p
	hList ���b�Z�[�W�o�͐�̃��X�g�{�b�N�X�̃E�B���h�E �n���h��
	lpMsg ���b�Z�[�W
******************************/
void CAnimation::DebugMsg( HWND hList, LPCSTR lpMsg )
{
	LONG	lCount;
	SendMessage( hList, LB_INSERTSTRING, (WPARAM)-1, (LPARAM)lpMsg );
	lCount = SendMessage( hList, LB_GETCOUNT, 0, 0 );
	SendMessage( hList, LB_SETCURSEL, (WPARAM)lCount-1L, (LPARAM)0 );
	UpdateWindow( hList );
	// �f�o�b�O���b�Z�[�W�́A�������u���b�L���O����O�ɕ\�������K�v�����邽�߁A���b�Z�[�W��\�������炷���ɍX�V
}

#define RELEASE_SET(v) if(v){v->Release(); v=NULL;}
/*************************************************
	ReleaseAll
	NULL�łȂ��C���X�^���X��������ANULL�ɂ���B
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
	�w�肳�ꂽ���f�B�A�t�@�C�����Đ�����B
	����
	wszFileName   �Đ����郁�f�B�A�t�@�C����	
	hWnd          ���[�r�[���Đ�����E�B���h�E�̃n���h��
*************************************************/
HRESULT CAnimation::Load(LPCWSTR wszFileName ,HWND hWnd)
{
	endStop();
	HRESULT		hResult;
	// �J�n
	DebugMsg( hWnd, "�t�B���^ �O���t���\�z���Ă��܂��B" );

	// �t�B���^ �O���t �}�l�[�W���̃C���X�^���X�擾
	hResult = CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (LPVOID *)&m_pGrph );
	if( FAILED(hResult) ){
		MessageBox( hWnd, "�C���X�^���X�擾���s", "Error", MB_OK | MB_ICONSTOP );
		return hResult;
	}
	AddToRot(m_pGrph);
	// �t�B���^ �O���t�̍\�z
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
	//	SampleGrabber(Filter)�𐶐�
	CoCreateInstance(CLSID_SampleGrabber,
		NULL,
		CLSCTX_INPROC,
		IID_IBaseFilter,
		(LPVOID	*)&m_pSampleGrabberFilter);
	//	Filter����ISampleGrabber�C���^�[�t�F�[�X���擾
	m_pSampleGrabberFilter->QueryInterface(IID_ISampleGrabber,
		(LPVOID	*)&m_pSampleGrabber);
//	SampleGrabber��ڑ�����t�H�[�}�b�g���w��B
	//	�����̎w��̎d���ɂ��SampleGrabber�̑}���ӏ�������B
	AM_MEDIA_TYPE	am_media_type;
	ZeroMemory(&am_media_type,	sizeof(am_media_type));
	am_media_type.majortype	=	MEDIATYPE_Video;
	am_media_type.subtype	=	MEDIASUBTYPE_RGB24;
	am_media_type.formattype	=	FORMAT_VideoInfo;
	m_pSampleGrabber->SetMediaType(&am_media_type);
	//	Graph��SampleGrabber	Filter��ǉ�
	m_pGrph->AddFilter(m_pSampleGrabberFilter,
		L"Sample	Grabber");
	///////////////
	m_pGrph->QueryInterface(IID_IMediaPosition,
	(LPVOID *)&m_pMediaPosition);
	DebugMsg( hWnd, "�t�@�C�����J���Ă��܂��B" );
	// �O���t�Ƀ����_����ǉ�
	hResult = m_pGrph->RenderFile( wszFileName, NULL );
	if( FAILED(hResult) ){
		ReleaseAll();
		return hResult;
	}

	mFlag=1;//�ǂݍ���ł���B
	return S_OK;
}
void CAnimation::ErrorChekGetCurrentBuffer(HRESULT Flag,long nBufferSize)
{
	char str[256]="�s��";
	/*
	E_INVALIDARG �T���v���̓o�b�t�@�����O����Ă��Ȃ��B 
	E_OUTOFMEMORY �w�肳�ꂽ�o�b�t�@�̑傫�����s�\���B 
	E_POINTER NULL �|�C���^�����B 
	S_OK  �����B 
	VFW_E_NOT_CONNECTED �t�B���^���ڑ�����Ă��Ȃ��B 
	VFW_E_WRONG_STATE �t�B���^���܂��T���v�����󂯎���Ă��Ȃ��B�T���v�����o�͂���ɂ́A�O���t�����s���邩�|�[�Y����B 
		*/
		switch(Flag)
		{
		case E_INVALIDARG:
			strcpy(str,"�T���v���̓o�b�t�@�����O����Ă��Ȃ��B");
			break;	
		case E_OUTOFMEMORY:
			sprintf(str,"%d �w�肳�ꂽ�o�b�t�@�̑傫�����s�\���B",nBufferSize);
			break;
		case E_POINTER:
			strcpy(str,"NULL �|�C���^�����B");
			break;
		case VFW_E_NOT_CONNECTED :
			strcpy(str,"�t�B���^���ڑ�����Ă��Ȃ�");
			break;
		case VFW_E_WRONG_STATE:
			strcpy(str,"�B���^���܂��T���v�����󂯎���Ă��Ȃ��B�T���v�����o�͂���ɂ́A�O���t�����s���邩�|�[�Y����B ");
			break;

		}
		MessageBox(NULL,str,"error",MB_OK);
}
/***************************************************
	getFrame
	�`�撆�̃t���[���摜���擾����֐�
	����
	pBmp	�摜�����N���X�ւ̃|�C���^
			�i���̃N���X�Ƀt���[���̉摜������B�j
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
		//	�o�b�t�@��p��
	long	nBufferSize	=	am_media_type.lSampleSize;
	long	*pBuffer	=	(long	*)malloc(nBufferSize);

	if(pBuffer==NULL)
		return NULL;
	static int no=0;
	//	���ݕ\������Ă���f����Î~��Ƃ��Ď擾
	HRESULT Flag=m_pSampleGrabber->GetCurrentBuffer(&nBufferSize,pBuffer);
	char str[256]="�s��";
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
	////�摜�N���X�ɉ�f����///////////
	pBmp->CreateBmp(pVideoInfoHeader->bmiHeader.biWidth,
		pVideoInfoHeader->bmiHeader.biHeight);
	CopyMemory(pBmp->getRgbP(),pBuffer,nBufferSize);

	free(pBuffer);

	return pBmp;

}

/****************************************
	Play
	�ǂݍ���ł��郁�f�B�A���Đ�����B
	����
	hWnd �`����window�n���h��
	x,y  �`�悷����W
	�߂�l: (HRESULT)�e���\�b�h�̖߂�l�A�������ɂ�S_OK
****************************************/
HRESULT CAnimation::Play( HWND hWnd,int x,int y)
{
	HRESULT		hResult;
	  if(!m_pGrph || !m_pMdaCtrl || !m_pVdoWin)
		return TRUE;
	if( m_IsPlay )		return TRUE;		// ���Ƀv���C���Ă�
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
	DebugMsg( hWnd, "�����_�����O���J�n���܂��B" );
	// �t�B���^ �O���t�̊J�n
	hResult = m_pMdaCtrl->Run();
	if( FAILED(hResult) ){
		ReleaseAll();
		return hResult;
	}
	m_pSampleGrabber->SetBufferSamples(TRUE);
	m_IsPlay = TRUE;
	mFlag=2;	//�Đ����̃t���O
	
	return S_OK;
}

/***********************************
	Stop
	�Đ����Ă��郁�f�B�A���~����B
	�߂�l: (HRESULT)�������ɂ� S_OK��Ԃ�
***********************************/
HRESULT CAnimation::Stop( void )
{
	HRESULT	hResult;

	if( !m_IsPlay )		return (HRESULT)1000L;	// �܂��Đ�����Ă��Ȃ�

	// �t�B���^ �O���t�̒�~
	hResult = m_pMdaCtrl->Stop();

	m_IsPlay = FALSE;
	mFlag=3;
	return hResult;
}
/***********************************
	endStop
	���f�B�A�̍Đ����~���A�O���t���������
	�߂�l: (HRESULT)�������ɂ� S_OK��Ԃ�
***********************************/
HRESULT CAnimation::endStop(void)
{
	HRESULT	h=Stop();
	ReleaseAll();
	return h;
}
/*************************************
	PlaySpeed
	�Đ����x����
	speed �Đ����x
		  1���ʏ�̍Đ����x
		  1�������X���[�Đ�
		  1�ȏオ�{���Đ�
*************************************/
void CAnimation::PlaySpeed(double speed)
{ 
	  if(!m_pGrph || !m_pMdaCtrl || !m_pVdoWin)
		return;
	 m_pMediaPosition->put_Rate(speed);
}
/******************************
	getTime
	���ݍĐ������f�B�A�̍Đ����Ԃ�Ԃ��B
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
	���ݍĐ������f�B�A�̏I�����Ԃ���Ԃ��B
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
	�Đ��ʒu�ύX
	����
	times �ύX��̍Đ��ʒu
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
