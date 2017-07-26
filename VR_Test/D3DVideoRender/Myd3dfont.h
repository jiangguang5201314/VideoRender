/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/

#ifndef MYD3DFONT_H
#define MYD3DFONT_H
#include <tchar.h>
#include <D3D9.h>
#include "DebugInterface.h"


// Font creation flags
#define D3DFONT_BOLD        0x0001
#define D3DFONT_ITALIC      0x0002


#define D3DFONT_ZENABLE     0x0004

// Font rendering flags
#define D3DFONT_CENTERED_X  0x0001
#define D3DFONT_CENTERED_Y  0x0002
#define D3DFONT_TWOSIDED    0x0004
#define D3DFONT_FILTERED    0x0008





//-----------------------------------------------------------------------------
// Name: class CD3DFont
// Desc: Texture-based font class for doing text in a 3D scene.
//-----------------------------------------------------------------------------
class CMYD3DFont
{
    char m_strText[256] ;
	LOGFONT m_LogFont ;
	RECT  m_textRect ;
	DWORD m_dwColor ;

    LPDIRECT3DDEVICE9       m_pd3dDevice; // A D3DDevice used for rendering
    LPDIRECT3DTEXTURE9      m_pTexture;   // The d3d texture for this font
    LPDIRECT3DVERTEXBUFFER9 m_pVB;        // VertexBuffer for rendering text
    DWORD   m_dwTexWidth;                 // Texture dimensions
    DWORD   m_dwTexHeight;

public:
    // 2D and 3D text drawing functions

      // Initializing and destroying device-dependent objects
    HRESULT InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice );
    HRESULT RestoreDeviceObjects();
    HRESULT InvalidateDeviceObjects();

    // Constructor / destructor
    CMYD3DFont();
    ~CMYD3DFont();

	HRESULT SetOverlayText (const char *strText, LOGFONT &logfont ) ;
	HRESULT SetOverlayTextRect ( DWORD dwColor, RECT *rect ) ;
	bool GetOverlayTextWH ( long &lWidth, long &lHeight ) ;
	HRESULT DrawText ( DWORD dwColor, RECT *rect ) ;

private:
	HRESULT CalcuTextureSize ( HDC hDC, char *strText ) ;
	HRESULT PaintText ( HDC hDC, char *strText ) ;
	
};




#endif


