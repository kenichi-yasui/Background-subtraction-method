
#include "CAnimation.h"
#define RELEASE_SET(v) if(v){v->Release(); v=NULL;}
class CAnimationOneShot :public CAnimation
{
private:
	IMediaEvent *m_pMediaEvent;
	IMediaSeeking *m_pMediaSeeking;
public :
	CAnimationOneShot(HINSTANCE hInst):CAnimation(hInst){m_pMediaEvent=NULL;m_pMediaSeeking=NULL;};
	~CAnimationOneShot(){OneShotStop(); };
	HRESULT Load(LPCWSTR wszFileName ,HWND hWnd);
	
	HRESULT OneShotPlay(HWND hWnd,int x=0,int y=0,double PlayTime=0);
	void ReleaseAll( void ){CAnimation::ReleaseAll();RELEASE_SET(m_pMediaEvent) RELEASE_SET(m_pMediaSeeking)};
	void endStop(void){mFlag=0;ReleaseAll();};
//	void OneShotStop(void){if(mFlag==3) mFlag=0;};
	void OneShotStop(void){if(mFlag==4) mFlag=3;};
	void OneShotRewinding(void){if(mFlag==4) mFlag=0;};
	HRESULT OneShotPlayStart(void);
	HRESULT OneShotPlayStart(HWND hWnd,int x,int y);
};