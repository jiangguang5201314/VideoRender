// VR_TestDlg.h : header file
//

#pragma once

#include "D3DVideoRenderInterface.h"
#include "xviddecoder.h"

#include "mmsystem.h"
#include "winbase.h"


#pragma comment ( lib, "winmm.lib" )

class CHDTimer  
{
	#define THRESHOLD_MS 1

public:
/*
  ___________________________________________________________________
 | Average time per frame | Frame rate (fps) | Numerator Denominator |
 |  166833 59.94 (NTSC)   |    60000         |           1001        | 
 |  333667 29.97 (NTSC)   |    30000         |           1001        |
 | 417188 23.97 (NTSC)    |    24000         |           1001        |
 | 200000 50.00 (PAL)     |    50            |             1         |
 | 400000 25.00 (PAL)     |    25            |             1         |
 | 416667 24.00 (Film)    |    24            |             1         |
 |________________________|__________________|_______________________|
*/


	CHDTimer( double timer_interval )
	{
		/*TIMECAPS tc ;
		m_TimerResolution = (TIMERR_NOERROR == timeGetDevCaps ( &tc, sizeof(tc) )) ? tc.wPeriodMin : 1 ;  

		if ( TIMERR_NOERROR != timeBeginPeriod ( m_TimerResolution ) )
		{
			throw "timeBeginPeriod failed!" ;
		}//*/
		
		if ( !QueryPerformanceFrequency ( (LARGE_INTEGER*)&m_Freq ) )
		{
			throw "QueryPerformanceFrequency failed!" ;
		}

		m_Interval  = timer_interval * m_Freq / 1000 ;

		m_Start = m_End = 0 ;
		m_Threshold = THRESHOLD_MS * m_Freq / 1000 ;
	}
	~CHDTimer()
	{
		if ( m_TimerResolution ) 
		{
//			timeEndPeriod(m_TimerResolution) ; 
		}
	}

	void Timer ()
	{
		for ( ;; )
		{
			QueryPerformanceCounter ( (LARGE_INTEGER*)&m_End ) ;
			__int64 interval = m_End - m_Start ;
			if ( interval < m_Interval )
			{
				__int64 temp = m_Interval - interval ;
				if ( temp >= m_Threshold )
				{
					long lms = ( temp * 1000 / m_Freq ) - THRESHOLD_MS ;
					if ( lms > 0 )
					{
						Sleep ( lms ) ;
					}
				}
				else
				{
//					SwitchToThread () ;
				}
			}
			else
			{
				break ;
			}
		}
		m_Start = m_End ;
	//	QueryPerformanceCounter ( (LARGE_INTEGER*)&m_Start ) ;
	}

private:

	UINT m_TimerResolution ;

	__int64 m_Interval ;
	__int64 m_Freq ;

	__int64 m_Start, m_End, m_Threshold ;

};
class CFullScreen
{
	CFullScreen ( const CFullScreen &refFull ) ;
	CFullScreen &operator= ( const CFullScreen &refFull ) ;

public:
	CFullScreen ()
		: m_hWnd(NULL)	
	{
		Init () ;
	}

	~CFullScreen (){}

	bool IsFullScreen ()
	{
		return m_bFullScreen ;
	}

	void ToggleFullScreen ( HWND hWnd )
	{
		if ( hWnd == NULL )
		{
			return ;
		}

		if ( hWnd != m_hWnd )
		{
			m_hWnd = hWnd ;
			Init () ;
		}

		if ( !m_bFullScreen )
		{			
			GetWindowPlacement( hWnd, &m_windowedPWP ) ;

			if ( m_dwExStyle == 0 )
			{
				m_dwExStyle = GetWindowLong( hWnd, GWL_EXSTYLE ) ;
			}
			
			if ( m_dwStyle == 0 )
			{
				m_dwStyle = GetWindowLong( hWnd, GWL_STYLE ) ;
				m_dwStyle &= ~WS_MAXIMIZE & ~WS_MINIMIZE; // remove minimize/maximize style
			}

			if ( m_hMenu == NULL )
			{
				m_hMenu = GetMenu( hWnd ) ;
			}

			// Hide the window to avoid animation of blank windows
			ShowWindow ( m_hWnd, SW_HIDE ) ;
			// Set FS window style
			SetWindowLong ( hWnd, GWL_STYLE, WS_POPUP | WS_SYSMENU | WS_CLIPCHILDREN ) ;
			SetMenu ( hWnd, NULL ) ;
			ShowWindow ( hWnd, SW_SHOWMAXIMIZED ) ;
		}
		else
		{
			if ( m_dwStyle != 0 )
			{
				SetWindowLong( hWnd, GWL_STYLE, m_dwStyle );
			}
			if ( m_dwExStyle != 0 )
			{
				SetWindowLong( hWnd, GWL_EXSTYLE, m_dwExStyle );
			}

			if ( m_hMenu != NULL )
			{
				SetMenu( hWnd, m_hMenu );
			}

			if ( m_windowedPWP.length == sizeof( WINDOWPLACEMENT ) )
			{
				if ( m_windowedPWP.showCmd == SW_SHOWMAXIMIZED )
				{
					ShowWindow ( hWnd, SW_HIDE ) ;
					m_windowedPWP.showCmd = SW_HIDE ;
					SetWindowPlacement( hWnd, &m_windowedPWP ) ;
					ShowWindow ( hWnd, SW_SHOWMAXIMIZED ) ;
					m_windowedPWP.showCmd = SW_SHOWMAXIMIZED ;
				}
				else
				{
					SetWindowPlacement( hWnd, &m_windowedPWP ) ;
				}
			}
		}
		m_bFullScreen = !m_bFullScreen ;
	}

private:
	void Init ()
	{
		m_bFullScreen = false ;
		m_dwExStyle = 0 ;
		m_dwStyle = 0 ;
		m_hMenu = NULL ;

		ZeroMemory( &m_windowedPWP, sizeof( WINDOWPLACEMENT ) ) ;
		m_windowedPWP.length = sizeof( WINDOWPLACEMENT ) ;
	}

private:
	HWND m_hWnd ;
	bool m_bFullScreen ;
	WINDOWPLACEMENT m_windowedPWP ;
	DWORD m_dwExStyle ;
	DWORD m_dwStyle ;
	HMENU m_hMenu ;

} ;

// CVR_TestDlg dialog
class CVR_TestDlg : public CDialog
{
// Construction
public:
	CVR_TestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_VR_TEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnKickIdle(WPARAM wp, LPARAM lp);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()

private:

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void AdjustWindow(unsigned long dwWidth, unsigned long dwHeight);
	bool CreateRender(void);
	virtual BOOL DestroyWindow();

private:

	CFullScreen m_FullScreen ;
	ID3DVRInterface *m_pD3DRender ;
	bool m_LBtnDown ;	
};
