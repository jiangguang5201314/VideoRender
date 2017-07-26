// VR_TestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VR_Test.h"
#include "VR_TestDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#pragma comment ( lib, "D3DVideoRender.lib" )


#define IMAGE_WIDTH 352
#define IMAGE_HEIGHT 288


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CVR_TestDlg dialog




CVR_TestDlg::CVR_TestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVR_TestDlg::IDD, pParent)
	, m_pD3DRender(NULL)
	, m_LBtnDown(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVR_TestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

#define WM_KICKIDLE         0x036A

BEGIN_MESSAGE_MAP(CVR_TestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// CVR_TestDlg message handlers

BOOL CVR_TestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	AdjustWindow ( IMAGE_WIDTH, IMAGE_HEIGHT ) ;
	if ( !CreateRender () )
	{
		D3D_VIDEO_RENDER::D3D_Video_Render_Destroy () ;
		m_pD3DRender = NULL ;
		MessageBox ( "CreateRender FAILED!" ) ;
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CVR_TestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVR_TestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		OnKickIdle ( 0, 0 ) ;
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVR_TestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL CVR_TestDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	if ( pMsg->message == WM_KEYDOWN )
	{
		if ( pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN || pMsg->wParam == VK_SPACE )
		{
			m_FullScreen.ToggleFullScreen ( this->GetSafeHwnd() ) ;
			return TRUE ;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CVR_TestDlg::AdjustWindow(unsigned long dwWidth, unsigned long dwHeight)
{	
	RECT  rcWork ;
    RECT  rc ;
    DWORD dwStyle ;

	HWND hWnd = GetSafeHwnd () ;
    // If we are still a WS_POPUP window we should convert to a normal app
    // window so we look like a windows app.
    dwStyle  = GetWindowLong( hWnd, GWL_STYLE ) ;
    dwStyle &= ~WS_POPUP ;
    dwStyle |= WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX;
    SetWindowLong ( hWnd, GWL_STYLE, dwStyle ) ;

    // Aet window size
    SetRect( &rc, 0, 0, dwWidth, dwHeight );

    AdjustWindowRectEx( &rc, GetWindowLong( hWnd, GWL_STYLE ), GetMenu() != NULL,
                        GetWindowLong( hWnd, GWL_EXSTYLE ) ) ;

    SetWindowPos( NULL, 0, 0, rc.right-rc.left, rc.bottom-rc.top,
                  SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE );

	
    SetWindowPos( NULL, 0, 0, 0, 0,
                  SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE );

    //  Make sure our window does not hang outside of the work area
    SystemParametersInfo( SPI_GETWORKAREA, 0, &rcWork, 0 );
    GetWindowRect( &rc );
    if( rc.left < rcWork.left ) rc.left = rcWork.left;
    if( rc.top  < rcWork.top )  rc.top  = rcWork.top;
    SetWindowPos( NULL, rc.left, rc.top, 0, 0,
                  SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
}

void CVR_TestDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	m_LBtnDown = true ;

	CDialog::OnLButtonDown(nFlags, point);
}

void CVR_TestDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	m_LBtnDown = false ;

	CDialog::OnLButtonUp(nFlags, point);
}

ID3DVRInterface::HCANVAS hCanvas[3] ;
ID3DVRInterface::HIMAGE  hImage[3] ;
ID3DVRInterface::HTEXT   hText[3] ;

long lTextW[2], lTextH[2] ;

bool CVR_TestDlg::CreateRender(void)
{
	LOGFONT font ;
	memset ( &font, 0, sizeof(font) ) ;

	font.lfHeight = 20 ;
	font.lfWidth = 10 ;
	font.lfEscapement = 0 ;
	font.lfOrientation = 0 ;
	font.lfWeight = FW_BOLD ;
	font.lfItalic = FALSE ;
	font.lfUnderline = FALSE ;
	font.lfStrikeOut = FALSE ;
	font.lfCharSet = DEFAULT_CHARSET ;
	font.lfOutPrecision = OUT_DEFAULT_PRECIS ;
	font.lfClipPrecision = CLIP_DEFAULT_PRECIS ;
	font.lfQuality = ANTIALIASED_QUALITY ;
	font.lfPitchAndFamily = VARIABLE_PITCH ;
	strcpy_s ( font.lfFaceName, LF_FACESIZE, "ºÚÌå") ;


	HWND hWnd = this->GetSafeHwnd() ; 
	m_pD3DRender = D3D_VIDEO_RENDER::D3D_Video_Render_Create () ;
	if ( m_pD3DRender == NULL )
	{
		return false ;
	}

	for ( int i = 0 ; i < 3 ; i++ )
	{
		hCanvas[i] = m_pD3DRender->CreateCanvas ( hWnd, IMAGE_WIDTH, IMAGE_HEIGHT ) ;
		if ( hCanvas[i] == NULL )
		{
			return false ;
		}
	}

	RECT rect ;
	rect.left = 0;
	rect.top = 0;
	rect.right = rect.left + IMAGE_WIDTH ;
	rect.bottom = rect.top + IMAGE_HEIGHT ;

	hImage[0] = m_pD3DRender->CreateImage ( IMAGE_WIDTH, IMAGE_HEIGHT, ID3DVRInterface::CS_YV12, &rect ) ;
	if ( hImage[0] == NULL )
	{
		return false ;
	}

	hText[0] = m_pD3DRender->CreateOverlayText ( "°ëÍ¸Ã÷", &font ) ;
	if ( hText[0] == NULL )
	{
		return false ;
	}

	bool bret = m_pD3DRender->GetOverlayTextWH ( hText[0], &lTextW[0], &lTextH[0] ) ;
	if ( !bret )
	{
		return false ;
	}


	rect.top = 200 ;
	rect.left = 200 ;
	rect.right = 250 ;
	rect.bottom = rect.top + 50 ;
	hImage[1] = m_pD3DRender->CreateImage ( IMAGE_WIDTH, IMAGE_HEIGHT, ID3DVRInterface::CS_YV12, &rect ) ;
	if ( hImage[1] == NULL )
	{
		return false ;
	}
	
	font.lfWidth = 15 ;
	strcpy_s ( font.lfFaceName, LF_FACESIZE, "ËÎÌå") ;
	hText[1] = m_pD3DRender->CreateOverlayText ( "²âÊÔ", &font ) ;
	if ( hText[1] == NULL )
	{
		return false ;
	}
	bret = m_pD3DRender->GetOverlayTextWH ( hText[1], &lTextW[1], &lTextH[1] ) ;
	if ( !bret )
	{
		return false ;
	}

	rect.left = 100 ;
	rect.top = 200 ;
	rect.right = 150 ;
	rect.bottom = 250 ;

	hImage[2] = m_pD3DRender->CreateImage ( IMAGE_WIDTH, IMAGE_HEIGHT, ID3DVRInterface::CS_YV12, &rect ) ;
	if ( hImage[2] == NULL )
	{
		return false ;
	}

	bret = m_pD3DRender->CreateDynamicFont ( "Arial", 12 ) ;
	if ( !bret )
	{
		return false ;
	}

	return true;
}


CHDTimer Timer ( 10.0f ) ;
CFile file ( "Video.dat", CFile::modeRead ) ;
BYTE pSrc[IMAGE_WIDTH * IMAGE_HEIGHT * 3 / 2] ;
BYTE pDest[IMAGE_WIDTH * IMAGE_HEIGHT * 3 / 2] ;
CXvidDecoder Decoder ( IMAGE_WIDTH, IMAGE_HEIGHT ) ;

LRESULT CVR_TestDlg::OnKickIdle(WPARAM wp, LPARAM lCount)
{
	if ( IsIconic() )
	{
		return FALSE ;
	}
	if ( m_pD3DRender == NULL )
	{
		return FALSE ;
	}

	static RECT *pCapture = NULL ;
	static POINT Opoint = {0x1fffff, 0x1fffff} ;

	static POINT point ;
	static RECT crect ;
	static RECT I_rect1 = {100,100,250, 200 } ;
	static RECT I_rect2 = {100,200,150, 250 } ;
	static RECT overlay1 = { 80, 80, lTextW[0]+80, lTextH[0]+80};
	static RECT overlay2 = {10, 200, lTextW[1]+10, lTextH[1]+200};
	if ( m_LBtnDown )
	{
		::GetClientRect ( this->GetSafeHwnd(), &crect ) ;
		long sx = crect.right - crect.left ;
		long sy = crect.bottom - crect.top ;
		float sxs = IMAGE_WIDTH * 1.0f / sx  ;
		float sys = IMAGE_HEIGHT * 1.0f / sy ;

		::GetCursorPos ( &point ) ;

		::ScreenToClient ( this->GetSafeHwnd(), &point ) ;

		point.x *= sxs ;
		point.y *= sys ;

		if ( pCapture == NULL )
		{
			if ( point.x > overlay2.left && point.x < overlay2.right &&
				point.y > overlay2.top && point.y < overlay2.bottom )
			{
				pCapture = &overlay2 ;
			}
			else if ( point.x > overlay1.left && point.x < overlay1.right &&
				point.y > overlay1.top && point.y < overlay1.bottom )
			{
				pCapture = &overlay1 ;
			}
			else if ( point.x > I_rect2.left && point.x < I_rect2.right &&
				point.y > I_rect2.top && point.y < I_rect2.bottom )
			{
				pCapture = &I_rect2 ;
			}
			else if ( point.x > I_rect1.left && point.x < I_rect1.right &&
				point.y > I_rect1.top && point.y < I_rect1.bottom )
			{
				pCapture = &I_rect1 ;
			}
			else
			{
				Opoint.x = 0x1fffff ;
				pCapture = NULL ;
			}
		}
		else
		{
			if ( Opoint.x == 0x1fffff )
			{
				Opoint.x = point.x ;
				Opoint.y = point.y ;
			}

			int dx = pCapture->right - pCapture->left ;
			int dy = pCapture->bottom - pCapture->top ;
			int x = point.x - Opoint.x ;
			int y = point.y - Opoint.y ;

			pCapture->left += x ;
			pCapture->top += y ;
			pCapture->right = pCapture->left + dx ;
			pCapture->bottom = pCapture->top + dy ;

			Opoint.x = point.x ;
			Opoint.y = point.y ;
		}
	}
	else
	{
		Opoint.x = 0x1fffff ;
		pCapture = NULL ;
	}

	DWORD t = GetTickCount () ;
	char tim[64] ;
	sprintf ( tim, "%d", t ) ;

	static BYTE b = 0x00 ;
	static bool bUp = true ;

	DWORD dwColor = 0x00ff00 ;
	dwColor |= ( b << 24 ) ;

	if ( b == 0 )
	{
		bUp = true ;
	}
	if ( b == 0xff )
	{
		bUp = false ;
	}
	if ( bUp )
	{
		b++ ;
	}
	else
	{
		b-- ;
	}

	bool bret = m_pD3DRender->HandleDeviceLost () ;
	if ( !bret )
	{
		return TRUE ;
	}

	static long count = 0 ;

	if ( count % 4 == 0 )
	{
		long n = file.Read ( pSrc, 4 ) ;
		if ( n != 0 )
		{
			DWORD dw = pSrc[0] ;
			dw |= ( pSrc[1] << 8 ) ;
			dw |= ( pSrc[2] << 16 ) ;
			dw |= ( pSrc[3] << 24 ) ;

			n = file.Read ( pSrc, dw ) ;
			if ( n == 0 )
			{
				file.Seek ( 0, 0 ) ;
			}
			else
			{
				Decoder.Decode ( pSrc, dw, pDest ) ;
			}
		}
		else
		{
			file.Seek ( 0, 0 ) ;
		}
	}
	count++ ;
	

	m_pD3DRender->UpdateImage( hImage[0], pDest ) ;
	m_pD3DRender->UpdateImage( hImage[1], pDest ) ;
	m_pD3DRender->UpdateImage( hImage[2], pDest ) ;


	m_pD3DRender->BeginRender () ;

	m_pD3DRender->DrawImage ( hImage[0], 1.0f, ID3DVRInterface::GS_NONE ) ;
	m_pD3DRender->DrawImage ( hImage[1], 0.2f, ID3DVRInterface::GS_NONE, &I_rect1 ) ;
	m_pD3DRender->DrawImage ( hImage[2], 1.0f, ID3DVRInterface::GS_NONE, &I_rect2 ) ;

	m_pD3DRender->DrawOverlayText ( hText[0], 0x60ff0000, &overlay1 ) ;
	m_pD3DRender->DrawOverlayText ( hText[1], 0xff00ffff, &overlay2 ) ;

	m_pD3DRender->DrawDynamicText ( 2, 40, dwColor, tim ) ;
	m_pD3DRender->DrawDynamicText ( 2, 60, 0xffffff00, tim ) ;

//	m_pD3DRender->EndRender ( ::GetDlgItem ( this->GetSafeHwnd(), IDC_STATIC1 ) ) ;
	m_pD3DRender->EndRender () ;



	m_pD3DRender->BeginRender ( hCanvas[1] ) ;

	m_pD3DRender->DrawImage ( hImage[0], 1.0f, ID3DVRInterface::GS_NONE ) ;
	m_pD3DRender->DrawImage ( hImage[1], 0.2f, ID3DVRInterface::GS_NONE, &I_rect1 ) ;
	m_pD3DRender->DrawImage ( hImage[2], 1.0f, ID3DVRInterface::GS_NONE, &I_rect2 ) ;

	m_pD3DRender->DrawOverlayText ( hText[0], 0x60ff0000, &overlay1 ) ;
	m_pD3DRender->DrawOverlayText ( hText[1], 0xff00ffff, &overlay2 ) ;

	m_pD3DRender->DrawDynamicText ( 2, 40, dwColor, tim ) ;
	m_pD3DRender->DrawDynamicText ( 2, 60, 0xffffff00, tim ) ;

	m_pD3DRender->EndRender ( ::GetDlgItem ( this->GetSafeHwnd(), IDC_STATIC1 ) ) ;

	m_pD3DRender->BeginRender ( hCanvas[1] ) ;

	m_pD3DRender->DrawImage ( hImage[0], 1.0f, ID3DVRInterface::GS_NONE ) ;
	m_pD3DRender->DrawImage ( hImage[1], 0.2f, ID3DVRInterface::GS_NONE, &I_rect1 ) ;
	m_pD3DRender->DrawImage ( hImage[2], 1.0f, ID3DVRInterface::GS_NONE, &I_rect2 ) ;

	m_pD3DRender->DrawOverlayText ( hText[0], 0x60ff0000, &overlay1 ) ;
	m_pD3DRender->DrawOverlayText ( hText[1], 0xff00ffff, &overlay2 ) ;

	m_pD3DRender->DrawDynamicText ( 2, 40, dwColor, tim ) ;
	m_pD3DRender->DrawDynamicText ( 2, 60, 0xffffff00, tim ) ;

	m_pD3DRender->EndRender ( ::GetDlgItem ( this->GetSafeHwnd(), IDC_STATIC2 ) ) ;

	Timer.Timer() ;

	return TRUE ;
}

BOOL CVR_TestDlg::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	
	D3D_VIDEO_RENDER::D3D_Video_Render_Destroy () ;

	return CDialog::DestroyWindow();
}
