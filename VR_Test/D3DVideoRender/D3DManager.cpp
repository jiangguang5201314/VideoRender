/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/

#include "stdafx.h"
#include "D3DManager.h"

#include "Picture.h"

#pragma comment ( lib, "d3d9.lib" )
#pragma comment ( lib, "d3dx9.lib" )

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern IDebug *g_pDebug ;

CD3DManager::CD3DManager(void)
	: m_pD3D(NULL)
	, m_pDevice(NULL)
	, m_pCurrentSwapChain(NULL)
	, m_bDeviceLost(false)
	, m_dwPrevTime(0)
	, m_pFont(NULL)
{	
	for ( int i = 0 ; i < SHADER_COUNT ; i++ )
	{
		m_pPixelShader[i] = NULL ;
	}
}

CD3DManager::~CD3DManager(void)
{
	Destroy () ;
}

bool CD3DManager::CreateDevice ( HWND hMainWnd, long lWidth, long lHeight, DWORD dwBufferCount )
{
	do
	{
		m_pD3D = Direct3DCreate9 ( D3D_SDK_VERSION ) ;
		if ( m_pD3D == NULL )
		{
			g_pDebug->Log_Error ( _T("Direct3DCreate9() failed!") ) ;
			break ;
		}

		//*

		// window MODE
				
		m_DefaultSwapChain.d3dpp.BackBufferWidth  = lWidth ;
		m_DefaultSwapChain.d3dpp.BackBufferHeight = lHeight ;
		
		m_DefaultSwapChain.d3dpp.BackBufferFormat = D3DFMT_UNKNOWN ;
		m_DefaultSwapChain.d3dpp.BackBufferCount = dwBufferCount ;
		
		m_DefaultSwapChain.d3dpp.Windowed = TRUE ;
		m_DefaultSwapChain.d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD ;
		
		m_DefaultSwapChain.d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE ;
		//*/


/*
		// fullscreen MODE
		ZeroMemory ( &m_PresentParams, sizeof(m_PresentParams) ) ;

		UINT count = m_pD3D->GetAdapterModeCount ( 0, D3DFMT_X8R8G8B8 ) ;
		D3DDISPLAYMODE Mode ;
		for ( int i = 0 ; i < count ; i++ )
		{
			m_pD3D->EnumAdapterModes ( 0, D3DFMT_X8R8G8B8, i, &Mode ) ;
		}
		
		m_PresentParams.BackBufferWidth  = 1024 ;
		m_PresentParams.BackBufferHeight = 768 ;
		
		m_PresentParams.BackBufferFormat = D3DFMT_X8R8G8B8;
		m_PresentParams.BackBufferCount = dwBufferCount ;
		
		m_PresentParams.Windowed = FALSE ;
		m_PresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD ;
		
		m_PresentParams.PresentationInterval = D3DPRESENT_INTERVAL_ONE ;

		m_PresentParams.FullScreen_RefreshRateInHz = 60 ;
		m_PresentParams.hDeviceWindow = hWnd ;
		//*/

		D3DCAPS9 caps ;
		DWORD BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING ;
		HRESULT hr = m_pD3D->GetDeviceCaps ( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps ) ;
		if ( SUCCEEDED(hr) )
		{
			if ( !(caps.Caps2 & D3DCAPS2_DYNAMICTEXTURES) )
			{
				g_pDebug->Log_Error ( _T("VIDEO CARD NOT SUPPORT D3DCAPS2_DYNAMICTEXTURES!") ) ;
				return false ;
			}

			if ( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
			{
				BehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING ;
			}
		}

		UINT Adapter = D3DADAPTER_DEFAULT ;
		D3DDEVTYPE DeviceType = D3DDEVTYPE_HAL ;

		hr = m_pD3D->CreateDevice ( Adapter, DeviceType, hMainWnd, BehaviorFlags, &m_DefaultSwapChain.d3dpp, &m_pDevice ) ;
		if ( FAILED(hr ) )
		{
			g_pDebug->Log_Error ( _T("m_pD3D->CreateDevice failed!\nhr=%x"), hr ) ;
			break ;
		}

		return true ;

	} while ( false ) ;

	DestroyDevice () ;
	return false ;
}

ID3DVRInterface::HCANVAS CD3DManager::CreateCanvas ( HWND hMainWnd, long lWidth, long lHeight, DWORD dwBackBufferCount )
{
	if ( hMainWnd == NULL || lWidth <= 0 || lHeight <= 0 )
	{
		_ASSERTE(hMainWnd != NULL) ;
		_ASSERTE(lWidth > 0) ;
		_ASSERTE(lHeight > 0) ;
		g_pDebug->Log_Error ( _T("CD3DManager::CreateCanvas() Invalid args!") ) ;

		return NULL ;
	}

	SWAP_CHAIN_INFO *pInfo = NULL ;
	
	do
	{
		LPDIRECT3DSWAPCHAIN9 pSwapChain ;
		if ( m_pDevice == NULL )
		{
			if ( !CreateDevice ( hMainWnd, lWidth, lHeight, dwBackBufferCount ) )
			{
				Destroy () ;
				break ;
			}

			HRESULT hr = m_pDevice->GetSwapChain( 0, &m_DefaultSwapChain.pSwapChain ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("CD3DManager::CreateCanvas() GetSwapChain failed!") ) ;
				Destroy () ;
				break ;
			}
			pInfo = &m_DefaultSwapChain ;
		}
		else
		{
			pInfo = new SWAP_CHAIN_INFO ;
			if ( pInfo == NULL )
			{
				g_pDebug->Log_Error ( _T("CD3DManager::CreateCanvas() alloc memory failed!") ) ;
				break ;
			}
					
			pInfo->d3dpp.BackBufferWidth  = lWidth ;
			pInfo->d3dpp.BackBufferHeight = lHeight ;
			pInfo->d3dpp.BackBufferFormat = D3DFMT_UNKNOWN ;
			pInfo->d3dpp.BackBufferCount = dwBackBufferCount ;
			pInfo->d3dpp.Windowed = TRUE ;
			pInfo->d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD ;
			pInfo->d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE ;

			HRESULT hr = m_pDevice->CreateAdditionalSwapChain( &pInfo->d3dpp, &pSwapChain ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("CD3DManager::CreateCanvas() CreateAdditionalSwapChain failed!") ) ;
				break ;
			}
			pInfo->pSwapChain = pSwapChain ;
			m_SwapChainList.push_back ( pInfo ) ;
		}

		return reinterpret_cast<HCANVAS>(pInfo) ;
		
	} while ( false ) ;

	delete pInfo ;

	return NULL ;
}

bool CD3DManager::SaveCanvas ( HCANVAS hCanvas, const char *pFileName, IMAGE_FILEFORMAT FileFormat, RECT *pSrcRect )
{
	if ( m_pDevice == NULL )
	{
		_ASSERTE(m_pDevice != NULL) ;
		g_pDebug->Log_Error ( _T("CD3DManager::SaveCanvas() D3D Device wasn't created!") ) ;
		return false ;
	}
	if ( pFileName == NULL )
	{
		_ASSERTE(pFileName != NULL) ;
		g_pDebug->Log_Error ( _T("CD3DManager::SaveCanvas() pFileName!=NULL !") ) ;
		return false ;
	}

	if ( FileFormat < 0 || FileFormat > 3 )
	{
		_ASSERTE(FileFormat >= 0) ;
		_ASSERTE(FileFormat <= 3) ;
		g_pDebug->Log_Error ( _T("CD3DManager::SaveCanvas() invalid format!") ) ;
		return false ;
	}

	const D3DXIMAGE_FILEFORMAT format[] = { D3DXIFF_BMP, D3DXIFF_JPG, D3DXIFF_PNG, D3DXIFF_DIB } ;

	LPDIRECT3DSWAPCHAIN9 p = NULL ;
	if ( hCanvas != NULL )
	{
		if ( ((SWAP_CHAIN_INFO*)hCanvas)->pSwapChain == m_DefaultSwapChain.pSwapChain )
		{
			p = m_DefaultSwapChain.pSwapChain ;
		}
		else
		{
			std::list<SWAP_CHAIN_INFO*>::iterator it ;
			for ( it = m_SwapChainList.begin() ; it != m_SwapChainList.end() ; it++ )
			{
				if ( (*it)->pSwapChain == ((SWAP_CHAIN_INFO*)hCanvas)->pSwapChain )
				{
					p = (*it)->pSwapChain ;
					break ;
				}
			}
		}
	}

	LPDIRECT3DSURFACE9 pSur ;
	HRESULT hr ;

	if ( p == NULL )
	{		
		RECT rcWorkArea ;
		if ( !SystemParametersInfo ( SPI_GETWORKAREA, 0, &rcWorkArea, 0 ) )
		{
			return false ;
		}

		int Width  = rcWorkArea.right - rcWorkArea.left ;
		int Height = rcWorkArea.bottom - rcWorkArea.top ;

		hr = m_pDevice->CreateOffscreenPlainSurface ( Width, Height, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &pSur, NULL ) ;
		if ( FAILED(hr) )
		{
			return false ;
		}
		hr = m_pDevice->GetFrontBufferData ( 0, pSur ) ;
		if ( FAILED(hr) )
		{
			pSur->Release () ;
			return false ;
		}
	}
	else
	{
		hr = p->GetBackBuffer ( 0, D3DBACKBUFFER_TYPE_MONO, &pSur ) ;
		if ( FAILED(hr) )
		{
			return false ;
		}
	}
		
	hr = D3DXSaveSurfaceToFile ( pFileName, format[FileFormat], pSur, NULL, pSrcRect ) ;
	if ( FAILED(hr) )
	{
		pSur->Release () ;
		return false ;
	}
	pSur->Release () ;
	
	return true ;
}

bool CD3DManager::DestroyCanvas ( HCANVAS hCanvas )
{
	if ( hCanvas == NULL )
	{
		return false ;
	}

	SWAP_CHAIN_INFO *p = reinterpret_cast<SWAP_CHAIN_INFO*>(hCanvas) ;
	if ( p->pSwapChain == m_DefaultSwapChain.pSwapChain )
	{
		g_pDebug->Log_Error ( _T("CD3DManager::DestroyCanvas() cannot destroy default canvas!") ) ;
		return false ;
	}

	std::list<SWAP_CHAIN_INFO*>::iterator it ;
	for ( it = m_SwapChainList.begin() ; it != m_SwapChainList.end() ; it++ )
	{
		if ( (*it)->pSwapChain == p->pSwapChain )
		{
			delete *it ;
			m_SwapChainList.erase ( it ) ;
			return true ;
		}
	}
	return false ;
}

ID3DVRInterface::HIMAGE CD3DManager::CreateImage( long lWidth, long lHeight, COLOR_SPACE cs, RECT *rect )
{
	CDisplay *pDisplay = NULL ;

	do
	{
		if ( m_pDevice == NULL )
		{
			_ASSERTE(m_pDevice != NULL) ;
			g_pDebug->Log_Error ( _T("CD3DManager::SetImageBuffer() D3D Device wasn't created!") ) ;
			break ;
		}

		if ( rect == NULL || lWidth <= 0 || lHeight <= 0 || cs < CS_YV12 || cs > CS_RGB32 )
		{
			_ASSERTE(rect != NULL ) ;
			_ASSERTE(lWidth > 0) ;
			_ASSERTE(lHeight > 0) ;
			_ASSERTE(cs >= CS_YV12) ;
			_ASSERTE(cs <= CS_RGB32) ;
			
			g_pDebug->Log_Error ( _T("CD3DManager::CreateImage() invalid args!") ) ;
			break ;
		}

		pDisplay = CD3DManager::CreateRender ( cs, m_pDevice, m_pPixelShader ) ;

		bool bret = pDisplay->SetImageBuffer ( lWidth, lHeight, cs, rect ) ;
		if ( !bret )
		{
			break ;
		}
		
		m_DisplayList.push_back ( pDisplay ) ;
		return reinterpret_cast<HIMAGE>(pDisplay) ;

	} while ( false ) ;
	
	SAFE_DELETE ( pDisplay ) ;
	return NULL ;
}

bool CD3DManager::DestroyImage ( HIMAGE hImage )
{
	CDisplay *p = reinterpret_cast<CDisplay*>(hImage) ;

	std::list<CDisplay*>::iterator it ;
	for ( it = m_DisplayList.begin() ; it != m_DisplayList.end() ; it++ )
	{
		if ( *it == p )
		{
			delete *it ;
			m_DisplayList.erase ( it ) ;
			return true ;
		}
	}
	return false ;
}

ID3DVRInterface::HPICTURE CD3DManager::CreatePicture ( const char *pSrcFile, RECT *DstRect )
{
	CPicture *p = NULL ;

	do
	{
		if ( m_pDevice == NULL )
		{
			_ASSERTE(m_pDevice != NULL) ;
			g_pDebug->Log_Error ( _T("CD3DManager::CreatePicture() D3D Device wasn't created!") ) ;
			break ;
		}

		if ( pSrcFile == NULL )
		{
			_ASSERTE(pSrcFile != NULL ) ;			
			g_pDebug->Log_Error ( _T("CD3DManager::CreatePicture() invalid args!") ) ;
			break ;
		}

		HRESULT hr ;
		p = new CPicture ( hr, m_pDevice, pSrcFile, DstRect ) ;
		if ( FAILED(hr) || p == NULL )
		{
			g_pDebug->Log_Error ( _T("Create Picture failed!") ) ;
			break ;
		}

		m_PictureList.push_back ( p ) ;
		return reinterpret_cast<HPICTURE>(p) ;
	}
	while ( false ) ;

	SAFE_DELETE ( p ) ;
	return p ;
}

bool CD3DManager::DestroyPicture ( HPICTURE hPicture )
{
	CPicture *p = reinterpret_cast<CPicture*>(hPicture) ;

	std::list<CPicture*>::iterator it ;
	for ( it = m_PictureList.begin() ; it != m_PictureList.end() ; it++ )
	{
		if ( *it == p )
		{
			delete *it ;
			m_PictureList.erase ( it ) ;
			return true ;
		}
	}
	return false ;
}

ID3DVRInterface::HTEXT CD3DManager::CreateOverlayText ( const char *strText, LOGFONT *logfont )
{
	if ( m_pDevice == NULL )
	{
		_ASSERTE(m_pDevice != NULL) ;
		g_pDebug->Log_Error ( _T("CD3DManager::CreateOverlayText() D3D Device wasn't created!") ) ;
		return false ;
	}

	if ( strText == NULL || logfont == NULL )
	{
		_ASSERTE(strText != NULL) ;
		_ASSERTE(logfont != NULL) ;
		g_pDebug->Log_Error ( _T("CD3DManager::CreateOverlayText() invalid args!") ) ;
		return false ;
	}

	CMYD3DFont *pMyFont = NULL ;
	do
	{
		CMYD3DFont *pMyFont = new CMYD3DFont () ;
		if ( pMyFont == NULL )
		{
			break ;
		}
		pMyFont->InitDeviceObjects ( m_pDevice ) ;

		HRESULT hr = pMyFont->SetOverlayText ( strText, *logfont ) ;
		if ( FAILED(hr) )
		{
			break ;
		}

		m_MyFontList.push_back ( pMyFont ) ;
		return reinterpret_cast<HTEXT>(pMyFont) ;

	} while ( false ) ;

	SAFE_DELETE ( pMyFont ) ;
	return NULL ;
}

bool CD3DManager::GetOverlayTextWH ( HTEXT hText, long *lWidth, long *lHeight )
{
	if ( m_pDevice == NULL )
	{
		_ASSERTE(m_pDevice != NULL) ;
		g_pDebug->Log_Error ( _T("CD3DManager::GetOverlayTextWH() D3D Device wasn't created!") ) ;
		return false ;
	}

	CMYD3DFont *pMyFont = reinterpret_cast<CMYD3DFont*>(hText) ;
	
	std::list<CMYD3DFont*>::iterator it ;
	for ( it = m_MyFontList.begin() ; it != m_MyFontList.end() ; it++ )
	{
		if ( *it == pMyFont )
		{
			return pMyFont->GetOverlayTextWH ( *lWidth, *lHeight ) ;
		}
	}
	return false ;
}

bool CD3DManager::CreateDynamicFont ( const char* strFontName, DWORD dwHeight )
{
	if ( m_pDevice == NULL )
	{
		_ASSERTE(m_pDevice != NULL) ;
		g_pDebug->Log_Error ( _T("CD3DManager::CreateDynamicFont() D3D Device wasn't created!") ) ;
		return false ;
	}

	if ( strFontName == NULL || dwHeight == 0 )
	{
		_ASSERTE(strFontName != NULL) ;
		_ASSERTE(dwHeight != NULL) ;
		g_pDebug->Log_Error ( _T("CD3DManager::CreateDynamicFont() invalid args!") ) ;
		return false ;
	}

	SAFE_DELETE ( m_pFont ) ;
	m_pFont = new CD3DFont ( strFontName, dwHeight ) ;
	if ( m_pFont == NULL )
	{
		return false ;
	}
	HRESULT hr = m_pFont->InitDeviceObjects ( m_pDevice ) ;
	if ( FAILED(hr) )
	{
		return false ;
	}
	hr = m_pFont->RestoreDeviceObjects () ;
	if ( FAILED(hr) )
	{
		return false ;
	}
	return true ;
}

bool CD3DManager::DestroyDynamicFont ( void )
{
	SAFE_DELETE ( m_pFont ) ;
	return true ;
}

bool CD3DManager::SetDefault (void)
{
	HRESULT hr = m_pDevice->SetRenderState ( D3DRS_LIGHTING, FALSE ) ;
	hr = m_pDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CCW ) ;
	return true ;
}

bool CD3DManager::ColorFill ( HIMAGE hImage, BYTE r, BYTE g, BYTE b )
{
	if ( m_pDevice == NULL )
	{
		g_pDebug->Log_Error ( _T("CD3DManager::ColorFill() D3D Device wasn't created!") ) ;
		return false ;
	}

	CDisplay *p = reinterpret_cast<CDisplay*>(hImage) ;

	std::list<CDisplay*>::iterator it ;
	for ( it = m_DisplayList.begin() ; it != m_DisplayList.end() ; it++ )
	{
		if ( *it == p )
		{
			return (*it)->Fill ( r, g, b ) ;
		}
	}
	return false ;
}

void CD3DManager::InvalidateDeviceObjects(void)
{
	std::list<CDisplay*>::const_iterator it ;
	for ( it = m_DisplayList.begin() ; it != m_DisplayList.end() ; it++ )
	{
		(*it)->InvalidateDeviceObjects () ;
	}

	std::list<CMYD3DFont*>::const_iterator it1 ;
	for ( it1 = m_MyFontList.begin() ; it1 != m_MyFontList.end() ; it1++ )
	{
		(*it1)->InvalidateDeviceObjects () ;
	}

	if ( m_pFont != NULL )
	{
		m_pFont->InvalidateDeviceObjects () ;
	}

	SAFE_RELEASE ( m_DefaultSwapChain.pSwapChain ) ;
	m_pCurrentSwapChain = NULL ;

	std::list<SWAP_CHAIN_INFO*>::const_iterator it2 ;
	for ( it2 = m_SwapChainList.begin() ; it2 != m_SwapChainList.end() ; it2++ )
	{
		(*it2)->InvalidateDeviceObjects () ;
	}
}


bool CD3DManager::RestoreDeviceObjects(void)
{
	HRESULT hr = m_pDevice->GetSwapChain ( 0, &m_DefaultSwapChain.pSwapChain ) ;
	if ( FAILED( hr ) )
	{	
		return false ;
	}

	std::list<SWAP_CHAIN_INFO*>::const_iterator it2 ;
	for ( it2 = m_SwapChainList.begin() ; it2 != m_SwapChainList.end() ; it2++ )
	{
		if ( !(*it2)->RestoreDeviceObjects ( m_pDevice ) )
		{
			return false ;
		}
	}

	std::list<CDisplay*>::const_iterator it ;
	for ( it = m_DisplayList.begin() ; it != m_DisplayList.end() ; it++ )
	{
		if ( !(*it)->RestoreDeviceObjects () )
		{
			return false ;
		}
	}

	std::list<CMYD3DFont*>::const_iterator it1 ;
	for ( it1 = m_MyFontList.begin() ; it1 != m_MyFontList.end() ; it1++ )
	{
		hr = (*it1)->RestoreDeviceObjects () ;
		if ( FAILED(hr) )
		{
			return false ;
		}
	}


	if ( m_pFont != NULL )
	{
		hr = m_pFont->RestoreDeviceObjects () ;
		if ( FAILED(hr) )
		{
			return false ;
		}
	}

	SetDefault () ;
	return true ;
}

bool CD3DManager::HandleDeviceLost(void)
{
	if ( !m_bDeviceLost )
	{
		return true ;
	}
	
	HRESULT hr = m_pDevice->TestCooperativeLevel () ;
	
	if ( FAILED(hr) )
	{
		if ( hr == D3DERR_DEVICELOST )
		{
			return false ;
		}
		else if ( hr == D3DERR_DEVICENOTRESET )
		{
			InvalidateDeviceObjects () ;
			
			hr = m_pDevice->Reset ( &m_DefaultSwapChain.d3dpp ) ;
			if ( FAILED( hr ) )
			{	
				return false ;
			}
		}
		else
		{
			g_pDebug->Log_Error ( _T("VideoCard UNKNOWN DEVICE ERROR!\nhr=%x"), hr ) ;
			return false ;
		}
	}

	InvalidateDeviceObjects () ;

	if ( RestoreDeviceObjects () )
	{
		m_bDeviceLost = false ;
	}
	else
	{
		g_pDebug->Log_Error ( _T("HandleDeviceLost RestoreDeviceObjects failed!") ) ;
		return false ;
	}
	
	return true ;
}

bool CD3DManager::UpdateImage ( HIMAGE hImage, BYTE *pBuf )
{
	return (reinterpret_cast<CDisplay*>(hImage))->UpdateImage ( pBuf ) ;
}


bool CD3DManager::BeginRender ( HCANVAS hCanvas, bool bClearBackBuffer, DWORD dwBKColor )
{
	HRESULT hr ;

	if ( hCanvas == NULL )
	{
		m_pCurrentSwapChain = m_DefaultSwapChain.pSwapChain ;
	}
	else
	{
		m_pCurrentSwapChain = (reinterpret_cast<SWAP_CHAIN_INFO*>(hCanvas))->pSwapChain ;
	}

	LPDIRECT3DSURFACE9 pBackBuffer ;
	hr = m_pCurrentSwapChain->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer ) ;
	if ( SUCCEEDED(hr) )
	{
		hr = m_pDevice->SetRenderTarget( 0, pBackBuffer ) ;
		pBackBuffer->Release() ;
	}
	
	if ( bClearBackBuffer )
	{
		hr = m_pDevice->Clear ( 0, NULL, D3DCLEAR_TARGET, dwBKColor, 1.0f, 0 ) ;
	}
	hr = m_pDevice->BeginScene () ;
	if ( SUCCEEDED(hr) )
	{
		return true ;
	}
	else
	{
		g_pDebug->Log_Error ( _T("m_pDevice->BeginScene() failed!\nhr=%x"), hr ) ;
		return false ;
	}
}

bool CD3DManager::EndRender (HWND hDestWindow)
{
	HRESULT hr = m_pDevice->EndScene () ;
	hr = m_pCurrentSwapChain->Present ( NULL, NULL, hDestWindow, NULL, 0 ) ;
	if ( hr != D3D_OK )
	{
		DWORD dw = GetTickCount () ;
		if ( dw - m_dwPrevTime >= 10000 ) // 10s
		{
			m_dwPrevTime = dw ;
			g_pDebug->Log_Error ( _T("m_pDevice->Present FAILED!\nhr=%x"), hr ) ;
		}		

		if ( hr == D3DERR_DEVICELOST )
		{
			m_bDeviceLost = true ;
		}
		return false ;
	}
	return true ;
}

bool CD3DManager::DrawImage ( HIMAGE hImage, float Transparent, GEOMETRIC_TRANSFORMATION Transformation, RECT *pDstRect, RECT *pSrcRect )
{
	return (reinterpret_cast<CDisplay*>(hImage))->DrawImage ( Transparent, Transformation, pDstRect, pSrcRect ) ;
}

bool CD3DManager::DrawPicture ( HPICTURE hPicture, float Transparent , GEOMETRIC_TRANSFORMATION Transformation, RECT *pDstRect, RECT *pSrcRect )
{
	return (reinterpret_cast<CPicture*>(hPicture))->DrawPicture ( Transparent, Transformation, pDstRect, pSrcRect ) ;
}

bool CD3DManager::DrawOverlayText ( HTEXT hText, DWORD dwColor, RECT *rect )
{
	(reinterpret_cast<CMYD3DFont*>(hText))->DrawText ( dwColor, rect ) ;
	return true ;
}

bool CD3DManager::DrawDynamicText ( long x, long y, DWORD dwColor, const TCHAR* strText )
{
	m_pFont->DrawText ( x, y, dwColor, strText ) ;
	return true ;
}


void CD3DManager::Destroy(void)
{
	DestroyAllFont () ;
	DestroyAllDisplay () ;
	DestroyAllPicture () ;
	DestroyAllShader () ;
	
	DestroyAdditionalSwapChain () ;

	DestroyDevice () ;
}

void CD3DManager::DestroyAllFont(void)
{
	SAFE_DELETE ( m_pFont ) ;

	std::list<CMYD3DFont*>::const_iterator it ;
	for ( it = m_MyFontList.begin() ; it != m_MyFontList.end() ; it++ )
	{
		delete *it ;
	}
	m_MyFontList.clear () ;
}

void CD3DManager::DestroyAllDisplay(void)
{
	std::list<CDisplay*>::const_iterator it ;
	for ( it = m_DisplayList.begin() ; it != m_DisplayList.end() ; it++ )
	{
		delete *it ;
	}
	m_DisplayList.clear () ;
}

void CD3DManager::DestroyAllPicture(void)
{
	std::list<CPicture*>::const_iterator it ;
	for ( it = m_PictureList.begin() ; it != m_PictureList.end() ; it++ )
	{
		delete *it ;
	}
	m_PictureList.clear () ;
}

void CD3DManager::DestroyAllShader(void)
{
	for ( int i = 0 ; i < SHADER_COUNT ; i++ )
	{
		SAFE_DELETE ( m_pPixelShader[i] ) ;
	}
}

void CD3DManager::DestroyAdditionalSwapChain(void)
{
	std::list<SWAP_CHAIN_INFO*>::const_iterator it ;
	for ( it = m_SwapChainList.begin() ; it != m_SwapChainList.end() ; it++ )
	{
		delete *it ;
	}
	m_SwapChainList.clear () ;
}

void CD3DManager::DestroyDevice(void)
{
	SAFE_RELEASE ( m_DefaultSwapChain.pSwapChain ) ;
	SAFE_RELEASE ( m_pDevice ) ;
	SAFE_RELEASE ( m_pD3D ) ;
}

bool CD3DManager::DestroyOverlayText ( HTEXT hText )
{
	CMYD3DFont *p = reinterpret_cast<CMYD3DFont*>(hText) ;

	std::list<CMYD3DFont*>::iterator it ;
	for ( it = m_MyFontList.begin() ; it != m_MyFontList.end() ; it++ )
	{
		if ( *it == p )
		{
			delete *it ;
			m_MyFontList.erase ( it ) ;
			return true ;
		}
	}
	return false ;
}