/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/

#pragma once

#include "d3dfont.h"
#include "Myd3dfont.h"
#include "D3DVideoRenderInterface.h"
#include "RenderFactory.h"
#include "Display.h"
#include "Picture.h"
#include <list>

class CD3DManager : public ID3DVRInterface
{

	class SWAP_CHAIN_INFO
	{
	public:
		SWAP_CHAIN_INFO()
		{
			pSwapChain = NULL ;
			ZeroMemory ( &d3dpp, sizeof(d3dpp) ) ;
		}
		~SWAP_CHAIN_INFO()
		{
			SAFE_RELEASE ( pSwapChain ) ;
		}
		bool InvalidateDeviceObjects ()
		{
			SAFE_RELEASE ( pSwapChain ) ;
			return true ;
		}

		bool RestoreDeviceObjects(LPDIRECT3DDEVICE9 pDevice)
		{
			HRESULT hr = pDevice->CreateAdditionalSwapChain( &d3dpp, &pSwapChain ) ;
			if ( FAILED(hr) )
			{
				return false ;
			}	
			return true ;
		}

		LPDIRECT3DSWAPCHAIN9 pSwapChain ;
		D3DPRESENT_PARAMETERS d3dpp ;
	} ;

public:
	enum
	{
		SHADER_YUV420 = 0,
		SHADER_YUV422 = 1,
		SHADER_NV12 = 2,
		SHADER_RGB24 = 3,
		SHADER_COUNT = 4 
	} ;

public:
	HCANVAS CreateCanvas ( HWND hMainWnd, long lWidth, long lHeight, DWORD dwBackBufferCount ) ;
	bool SaveCanvas ( HCANVAS hCanvas, const char *pFileName, IMAGE_FILEFORMAT FileFormat, RECT *pSrcRect ) ;
	bool DestroyCanvas ( HCANVAS hCanvas ) ;
	
	HIMAGE CreateImage ( long lWidth, long lHeight, COLOR_SPACE cs, RECT *rect ) ;
	bool DestroyImage ( HIMAGE hImage ) ;

	HPICTURE CreatePicture ( const char *pSrcFile, RECT *DstRect ) ;
	bool DestroyPicture ( HPICTURE hPicture ) ;


	HTEXT CreateOverlayText ( const char *strText, LOGFONT *logfont ) ;
	bool DestroyOverlayText ( HTEXT hText ) ;
	bool GetOverlayTextWH ( HTEXT hText, long *lWidth, long *lHeight ) ;

	bool CreateDynamicFont ( const char* strFontName, DWORD dwHeight ) ;
	bool DestroyDynamicFont ( void ) ;

	bool ColorFill ( HIMAGE hImage, BYTE r, BYTE g, BYTE b ) ;

	bool UpdateImage ( HIMAGE hImage, BYTE *pData ) ;

	bool BeginRender ( HCANVAS hCanvas, bool bClearBackBuffer, DWORD dwBKColor ) ;

	bool DrawImage ( HIMAGE hImage, float Transparent, GEOMETRIC_TRANSFORMATION Transformation, RECT *pDstRect, RECT *pSrcRect ) ;
	bool DrawPicture ( HPICTURE hPicture, float Transparent, GEOMETRIC_TRANSFORMATION Transformation, RECT *pDstRect, RECT *pSrcRect ) ;
	bool DrawOverlayText ( HTEXT hText, DWORD dwColor, RECT *rect ) ;
	bool DrawDynamicText ( long x, long y, DWORD dwColor, const char* strText ) ;
		
	bool EndRender (HWND hDestWindow) ;
		
	CD3DManager(void);
	~CD3DManager(void);

private:	

	static CDisplay *CreateRender ( ID3DVRInterface::COLOR_SPACE cs, LPDIRECT3DDEVICE9 pDevice, CPixelShader **pShader )
	{
		extern CRenderFactory g_RenderFactory ;
		return g_RenderFactory.CreateRender ( cs, pDevice, pShader ) ;
	}

	bool CreateDevice ( HWND hMainWnd, long lWidth, long lHeight, DWORD dwBufferCount ) ;
	
	void Destroy(void);
	void DestroyAllFont(void);
	void DestroyAllDisplay(void);
	void DestroyAllPicture(void);
	void DestroyAllShader(void);
	void DestroyDevice(void);
	void DestroyAdditionalSwapChain(void) ;

	bool SetDefault (void) ;

	bool HandleDeviceLost(void);
	bool RestoreDeviceObjects(void);
	void InvalidateDeviceObjects(void);

private:
	
	LPDIRECT3D9 m_pD3D ;
	LPDIRECT3DDEVICE9 m_pDevice ;
	
	CPixelShader *m_pPixelShader[SHADER_COUNT] ;

	SWAP_CHAIN_INFO m_DefaultSwapChain ;
	LPDIRECT3DSWAPCHAIN9 m_pCurrentSwapChain ;
	std::list<SWAP_CHAIN_INFO*> m_SwapChainList ;

	std::list<CDisplay*> m_DisplayList ;
	std::list<CPicture*> m_PictureList ;
	std::list<CMYD3DFont*> m_MyFontList ;
	CD3DFont *m_pFont ;

	bool m_bDeviceLost ;
	DWORD m_dwPrevTime ;
};