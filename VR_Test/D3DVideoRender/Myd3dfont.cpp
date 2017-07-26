/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/


//#define STRICT
#include "stdafx.h"
#include <stdio.h>
#include <tchar.h>
#include <D3DX9.h>
#include "MyD3DFont.h"
//#include "D3DUtil.h"
//#include "DXUtil.h"

#include "debuginterface.h"

extern IDebug *g_pDebug ;

//-----------------------------------------------------------------------------
// Custom vertex types for rendering text
//-----------------------------------------------------------------------------

struct FONT2DVERTEX { D3DXVECTOR4 p;   DWORD color;     FLOAT tu, tv; };

#define D3DFVF_FONT2DVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

inline FONT2DVERTEX InitFont2DVertex( const D3DXVECTOR4& p, D3DCOLOR color,
                                      FLOAT tu, FLOAT tv )
{
    FONT2DVERTEX v;   v.p = p;   v.color = color;   v.tu = tu;   v.tv = tv;
    return v;
}


//-----------------------------------------------------------------------------
// Name: CMYD3DFont()
// Desc: Font class constructor
//-----------------------------------------------------------------------------
CMYD3DFont::CMYD3DFont()
{
    m_pd3dDevice           = NULL;
    m_pTexture             = NULL;
    m_pVB                  = NULL;

	m_strText[0] = 0 ;
	m_dwColor = 0 ;

	memset ( &m_textRect, 0, sizeof(m_textRect) ) ;

//	AllocConsole () ;
//	freopen ( "CONOUT$", "a", stdout ) ;
}




//-----------------------------------------------------------------------------
// Name: ~CMYD3DFont()
// Desc: Font class destructor
//-----------------------------------------------------------------------------
CMYD3DFont::~CMYD3DFont()
{
    InvalidateDeviceObjects();
    m_pd3dDevice = NULL;
}

//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initializes device-dependent objects, including the vertex buffer used
//       for rendering text and the texture map which stores the font image.
//-----------------------------------------------------------------------------
HRESULT CMYD3DFont::InitDeviceObjects( LPDIRECT3DDEVICE9 pd3dDevice )
{
	m_pd3dDevice = pd3dDevice ;
	return S_OK ;
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CMYD3DFont::RestoreDeviceObjects()
{
	if ( m_strText[0] == 0 )
	{
		return E_FAIL ;
	}

	if ( m_pd3dDevice == NULL )
	{
		return E_FAIL ;
	}

	InvalidateDeviceObjects () ;

	HRESULT hr = S_OK;
    HFONT hFont = NULL;
    HFONT hFontOld = NULL;
    HDC hDC = NULL;
    HBITMAP hbmBitmap = NULL;
    HGDIOBJ hbmOld = NULL;

	do
	{
		hDC = CreateCompatibleDC( NULL ) ;
		if ( hDC == NULL )
		{
			break ;
		}

		SetMapMode( hDC, MM_TEXT ) ;
		
		hFont = CreateFontIndirect ( &m_LogFont ) ;
		if( hFont == NULL )
		{
			break ;
		}
		
		hFontOld = (HFONT) SelectObject( hDC, hFont );
		
		hr = CalcuTextureSize ( hDC, m_strText ) ;
		
		if( FAILED(hr) )
		{
			break ;
		}
		
		// If requested texture is too big, use a smaller texture and smaller font,
		// and scale up when rendering.
		D3DCAPS9 d3dCaps;
		m_pd3dDevice->GetDeviceCaps( &d3dCaps );
		
		if( m_dwTexWidth > d3dCaps.MaxTextureWidth )
		{
			break ;
		}
		
		// Create a new texture for the font
		hr = m_pd3dDevice->CreateTexture( m_dwTexWidth, m_dwTexHeight, 1,
			D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8,//D3DFMT_X8R8G8B8,//D3DFMT_A4R4G4B4,
			D3DPOOL_DEFAULT/*D3DPOOL_MANAGED*/, &m_pTexture, NULL );
		if( FAILED(hr) )
		{
			break ;
		}
		
		// Prepare to create a bitmap
		DWORD*      pBitmapBits;
		BITMAPINFO bmi;
		ZeroMemory( &bmi.bmiHeader, sizeof(BITMAPINFOHEADER) );
		bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth       =  (int)m_dwTexWidth;
		bmi.bmiHeader.biHeight      = -(int)m_dwTexHeight;
		bmi.bmiHeader.biPlanes      = 1;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biBitCount    = 32;
		
		// Create a bitmap for the font
		hbmBitmap = CreateDIBSection( hDC, &bmi, DIB_RGB_COLORS,
			(void**)&pBitmapBits, NULL, 0 );
		if ( hbmBitmap == NULL )
		{
			break ;
		}
		
		hbmOld = SelectObject( hDC, hbmBitmap );
		
		// Set text properties
		
		SetTextColor( hDC, RGB(255,255,255) ) ;
		SetBkColor ( hDC, 0x00000000 ) ;	
		SetTextAlign( hDC, TA_TOP );
		
		// Paint the alphabet onto the selected bitmap
		hr = PaintText( hDC, m_strText );
		if( FAILED(hr) )
		{
			break ;
		}
		
		// Lock the surface and write the alpha values for the set pixels
		D3DLOCKED_RECT d3dlr;
		hr = m_pTexture->LockRect( 0, &d3dlr, 0, 0 );
		if( FAILED(hr) )
		{
			break ;
		}

		BYTE* pDstRow;
		pDstRow = (BYTE*)d3dlr.pBits;
		DWORD* pDst;
		DWORD x, y;
		
		for( y=0; y < m_dwTexHeight; y++ )
		{
			pDst = (DWORD*)pDstRow;
			for( x=0; x < m_dwTexWidth; x++ )
			{
				*pDst = pBitmapBits[m_dwTexWidth*y + x] ;
				
				if ( *pDst > 0 )	
				{
					*pDst |= 0xff000000 ;
				}
				pDst++ ;
			}
			pDstRow += d3dlr.Pitch;
		}

		
		hr = m_pTexture->UnlockRect(0);
		if( FAILED(hr) )
		{
			break ;
		}
		
		hr = m_pd3dDevice->CreateVertexBuffer( 4 * sizeof(FONT2DVERTEX),
			D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0,
			D3DPOOL_DEFAULT, &m_pVB, NULL ) ;
		
		if( FAILED(hr) )
		{
			break ;
		}

		{
		FONT2DVERTEX* pVertices ;

		long x = m_textRect.left ;
		long y = m_textRect.top ;
		long w = m_textRect.right ;
		long h = m_textRect.bottom ;
		
		HRESULT hr = m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD ) ;
		if ( FAILED(hr) )
		{
			return hr ;
		}

		*pVertices++ = InitFont2DVertex( D3DXVECTOR4(x-0.5f,y-0.5f,0.0f,1.0f), m_dwColor, 0.0f, 0.0f );
		*pVertices++ = InitFont2DVertex( D3DXVECTOR4(w-0.5f,y-0.5f,0.0f,1.0f), m_dwColor, 1.0f, 0.0f );
		*pVertices++ = InitFont2DVertex( D3DXVECTOR4(w-0.5f,h-0.5f,0.0f,1.0f), m_dwColor, 1.0f, 1.0f );
		*pVertices++ = InitFont2DVertex( D3DXVECTOR4(x-0.5f,h-0.5f,0.0f,1.0f), m_dwColor, 0.0f, 1.0f );

		m_pVB->Unlock();
		}
		
//		hr = D3DXSaveTextureToFile ( "e:\\ABCD.bmp", D3DXIFF_BMP, m_pTexture, NULL ) ;

		SelectObject( hDC, hbmOld );
		SelectObject( hDC, hFontOld );
		DeleteObject( hbmBitmap );
		DeleteObject( hFont );
		DeleteDC( hDC );

		return S_OK ;
		
	} while ( false ) ;
	
	InvalidateDeviceObjects () ;

	SelectObject( hDC, hbmOld );
	SelectObject( hDC, hFontOld );
	DeleteObject( hbmBitmap );
	DeleteObject( hFont );
	DeleteDC( hDC );
	
    return E_FAIL ;
}


//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Destroys all device-dependent objects
//-----------------------------------------------------------------------------
HRESULT CMYD3DFont::InvalidateDeviceObjects()
{
   if ( m_pVB != NULL )
   {
	   m_pVB->Release () ;
	   m_pVB = NULL ;
   }
   if ( m_pTexture != NULL )
   {
	   m_pTexture->Release () ;
	   m_pTexture = NULL ;
   }
    return S_OK;
}

bool CMYD3DFont::GetOverlayTextWH ( long &lWidth, long &lHeight )
{
	lWidth = m_dwTexWidth ;
	lHeight = m_dwTexHeight ;
	return true ;
}

HRESULT CMYD3DFont::CalcuTextureSize ( HDC hDC, char *strText )
{
	SIZE size ;

    if( 0 == GetTextExtentPoint32( hDC, strText, strlen(strText), &size ) )
        return E_FAIL;

	m_dwTexWidth  = ( size.cx / 16 + 1 ) * 16 ;
	m_dwTexHeight = ( size.cy / 16 + 1 ) * 16 ;
	
    return S_OK;
}

HRESULT CMYD3DFont::PaintText( HDC hDC, char *strText )
{
	RECT rect ;
	rect.left = rect.top = 0 ;
	rect.right = m_dwTexWidth ;
	rect.bottom = m_dwTexHeight ;
	
	if( 0 == ExtTextOut( hDC, 0, 0, ETO_OPAQUE, &rect, strText, strlen(strText), NULL ) )
		return E_FAIL;
	
    return S_OK;
}


HRESULT CMYD3DFont::SetOverlayTextRect ( DWORD dwColor, RECT *rect )
{
	if ( m_pVB == NULL )
	{
		return E_FAIL ;
	}

	long x = rect->left ;
	long y = rect->top ;
	long w  = rect->right ;
	long h = rect->bottom ;

	if ( x == m_textRect.left && y == m_textRect.top && w == m_textRect.right && h == m_textRect.bottom 
		&& dwColor == m_dwColor )
	{
		return S_OK ;
	}

	FONT2DVERTEX* pVertices ;

	DWORD dw = D3DCOLOR_RGBA(255,255,255,255) ;

	HRESULT hr = m_pVB->Lock( 0, 0, (void**)&pVertices, D3DLOCK_DISCARD ) ;
	if ( FAILED(hr) )
	{
		return hr ;
	}

	*pVertices++ = InitFont2DVertex( D3DXVECTOR4(x-0.5f,y-0.5f,0.0f,1.0f), dwColor, 0.0f, 0.0f );
	*pVertices++ = InitFont2DVertex( D3DXVECTOR4(w-0.5f,y-0.5f,0.0f,1.0f), dwColor, 1.0f, 0.0f );
	*pVertices++ = InitFont2DVertex( D3DXVECTOR4(w-0.5f,h-0.5f,0.0f,1.0f), dwColor, 1.0f, 1.0f );
	*pVertices++ = InitFont2DVertex( D3DXVECTOR4(x-0.5f,h-0.5f,0.0f,1.0f), dwColor, 0.0f, 1.0f );

	m_pVB->Unlock();

	m_textRect.left = x ;
	m_textRect.top = y ;
	m_textRect.right = w ;
	m_textRect.bottom = h ;
	m_dwColor = dwColor ;

	return S_OK ;
}

HRESULT CMYD3DFont::SetOverlayText ( const char *strText, LOGFONT &logfont )
{
	strcpy ( m_strText, strText ) ;

	m_LogFont.lfCharSet = logfont.lfCharSet ;
	m_LogFont.lfClipPrecision = logfont.lfClipPrecision ;
	m_LogFont.lfEscapement = logfont.lfEscapement ;
	strcpy ( m_LogFont.lfFaceName, logfont.lfFaceName ) ;
	m_LogFont.lfHeight = logfont.lfHeight ;
	m_LogFont.lfItalic = logfont.lfItalic ;
	m_LogFont.lfOrientation = logfont.lfOrientation ;
	m_LogFont.lfOutPrecision = logfont.lfOutPrecision ;
	m_LogFont.lfPitchAndFamily = logfont.lfPitchAndFamily ;
	m_LogFont.lfQuality = logfont.lfQuality ;
	m_LogFont.lfStrikeOut = logfont.lfStrikeOut ;
	m_LogFont.lfUnderline = logfont.lfUnderline ;
	m_LogFont.lfWeight = logfont.lfWeight ;
	m_LogFont.lfWidth = logfont.lfWidth ;


	InvalidateDeviceObjects () ;
	return RestoreDeviceObjects () ;
}

HRESULT CMYD3DFont::DrawText( DWORD dwColor, RECT *rect )
{
	SetOverlayTextRect ( dwColor, rect ) ;

    if( m_pd3dDevice == NULL )
        return E_FAIL ;

    // Setup renderstate
    
	if ( dwColor < 0xff000000 )
	{
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
		m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
	}

	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );

	m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );

	m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,   D3DFILL_SOLID );
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
	m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_CLIPPING,         TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND,      D3DVBF_DISABLE );
	m_pd3dDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_COLORWRITEENABLE,
		D3DCOLORWRITEENABLE_RED  | D3DCOLORWRITEENABLE_GREEN |
		D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );

	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

    m_pd3dDevice->SetFVF( D3DFVF_FONT2DVERTEX );
    m_pd3dDevice->SetPixelShader( NULL );
    m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(FONT2DVERTEX) ) ;

	m_pd3dDevice->SetTexture( 0, m_pTexture ) ;

	m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );

	if ( dwColor < 0xff000000 )
	{
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ) ;
	}

    m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  FALSE ) ;

    return S_OK ;
}