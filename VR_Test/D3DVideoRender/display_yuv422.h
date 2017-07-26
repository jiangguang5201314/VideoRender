/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/


#pragma once
#include "Display.h"
#include "Shader_YUV422.h"
#include "CommonFunctions.h"

class CDisplay_YUV422 :
	public CDisplay
{
	struct RENDER2FRAME_VERTEX_st
	{
		D3DXVECTOR4 pos ;
		DWORD color ;
		D3DXVECTOR2 tex1 ;
	} ;

	struct RENDER2TEXTURE_VERTEX_st
	{
		D3DXVECTOR4 pos ;
		DWORD color ;

		D3DXVECTOR2 tex1 ;
		D3DXVECTOR2 tex2 ;
	} ;

	enum
	{
		D3DFVF_RENDER2FRAME_VERTEX_macro   = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1,
		D3DFVF_RENDER2TEXTURE_VERTEX_macro = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2
	} ;

	enum
	{
		RENDER2TEXTURE = 0,
		RENDER2FRAME = 1,
		RENDER_COUNT = 2
	} ;

	enum
	{
		TEX_SURFACE = 0,
		FRM_SURFACE = 1,
		SURFACE_COUNT = 2
	} ;

public:

	CDisplay_YUV422(HRESULT &hr, LPDIRECT3DDEVICE9 pDevice, CPixelShader **pShader)
		: m_pShader(NULL)
		, m_pTexture_Parity(NULL)
		, m_lWidth(0)
		, m_lHeight(0)
		, m_lSize(0)
	{
		m_pDevice = pDevice ;

		Init_Array<LPDIRECT3DVERTEXBUFFER9>( m_pVertices, RENDER_COUNT, NULL ) ;
		Init_Array<LPDIRECT3DTEXTURE9>( m_pTexture, RENDER_COUNT, NULL ) ;
		Init_Array<LPDIRECT3DSURFACE9>( m_pSurface, SURFACE_COUNT, NULL ) ;
		
		Init_Array ( m_YUVdx, 9, 0.0f ) ;
		
		if ( pShader[CD3DManager::SHADER_YUV422] == NULL )
		{
			pShader[CD3DManager::SHADER_YUV422] = new CShader_YUV422 ( hr, pDevice ) ;
		}
		
		if ( pShader[CD3DManager::SHADER_YUV422] == NULL || FAILED(hr) )
		{
			SAFE_DELETE(pShader[CD3DManager::SHADER_YUV422]) ;
			return ;
		}

		m_pShader = (CShader_YUV422*)pShader[CD3DManager::SHADER_YUV422] ;
	}

	~CDisplay_YUV422(void)
	{
		InvalidateDeviceObjects ();
	}

	static CDisplay *Create (LPDIRECT3DDEVICE9 pDevice, CPixelShader **pShader)
	{
		HRESULT hr = S_OK ;
		CDisplay_YUV422 *p = new CDisplay_YUV422 ( hr, pDevice, pShader ) ;
		if ( FAILED(hr) )
		{
			SAFE_DELETE ( p ) ;
		}
		return p ;
	}

public:
	virtual bool SetImageBuffer ( long lWidth, long lHeight, ID3DVRInterface::COLOR_SPACE cs, RECT *rect )
	{
		m_ColorSpace = cs ;

		m_SrcRect.left = m_SrcRect.top = 0 ;
		m_SrcRect.right = lWidth ;
		m_SrcRect.bottom = lHeight ;

		m_Tex_dx = 1.0f / lWidth ;
		m_Tex_dy = 1.0f / lHeight ;

		if ( !CreateRender2Texture ( lWidth, lHeight ) )
		{
			return false ;
		}
		
		if ( !CreateRender2Frame ( lWidth, lHeight, rect ) )
		{
			return false ;
		}
		return true ;
	}
	
	virtual bool UpdateImage ( BYTE *pData )
	{
		D3DLOCKED_RECT lrect ;
		HRESULT hr = m_pTexture[RENDER2TEXTURE]->LockRect ( 0, &lrect, NULL, D3DLOCK_DISCARD/*|D3DLOCK_NO_DIRTY_UPDATE*/ ) ;
		if ( SUCCEEDED(hr) )
		{
			BYTE *pDest = (BYTE*)lrect.pBits ;
			if ( lrect.Pitch == m_lWidth )
			{
				memcpyMMX ( pDest, pData, m_lSize ) ;
			}
			else
			{
				for ( int j = 0 ; j < m_lHeight ; j++ )
				{
					memcpyMMX ( pDest, pData, m_lWidth ) ;
					pDest += lrect.Pitch ;
					pData += m_lWidth ;
				}
			}

			hr = m_pTexture[RENDER2TEXTURE]->UnlockRect ( 0 ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pTexture[i]->UnlockRect FAILED!\nhr=%x"), hr ) ;
			}
		}
		else
		{
			__asm emms ;
			g_pDebug->Log_Error ( _T("m_pTexture[i]->LockRect FAILED!\nhr=%x"), hr ) ;
			return false ;
		}

		__asm emms ;
		return true ;
	}

	virtual bool DrawImage ( float Transparent, ID3DVRInterface::GEOMETRIC_TRANSFORMATION Transformation, RECT *pDstRect, RECT *pSrcRect )
	{
		if ( Transparent <= 0.001f )
		{
			return true ;
		}

		SetImage ( Transformation, pDstRect, pSrcRect ) ;

		HRESULT hr = m_pDevice->SetFVF ( D3DFVF_RENDER2TEXTURE_VERTEX_macro ) ;
		hr = m_pDevice->SetPixelShader ( m_pShader->m_pPixelShader ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetPixelShader failed!\nhr=%x"), hr ) ;
		}

		hr = m_pDevice->SetRenderTarget( 0 , m_pSurface[TEX_SURFACE] ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetRenderTarget failed!\nhr=%x"), hr ) ;
		}
		
		hr = m_pShader->m_pPixelConstTable->SetFloatArray ( m_pDevice, m_pShader->m_YUVdxHandle, m_YUVdx, sizeof(m_YUVdx) / sizeof(float) ) ;
	
		hr = m_pDevice->SetTexture ( m_pShader->m_TextureDesc[0].RegisterIndex, m_pTexture[RENDER2TEXTURE] ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetTexture [Y_TEX] failed!\nhr=%x"), hr ) ;
		}

		hr = m_pShader->m_pPixelConstTable->SetFloat ( m_pDevice, m_pShader->m_transparentHandle, Transparent ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pShader->m_pPixelConstTable->SetFloat failed!\nhr=%x"), hr ) ;
		}

		hr = m_pDevice->SetTexture ( m_pShader->m_TextureDesc[1].RegisterIndex, m_pTexture_Parity ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetTexture [Y_TEX] failed!\nhr=%x"), hr ) ;
		}
			
		
		hr = m_pDevice->SetStreamSource ( 0, m_pVertices[RENDER2TEXTURE], 0, sizeof(RENDER2TEXTURE_VERTEX_st) ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetStreamSource failed!\nhr=%x"), hr ) ;
		}

		hr = m_pDevice->DrawPrimitive ( D3DPT_TRIANGLEFAN, 0 , 2 ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->DrawPrimitive FAILED!\nhr=%x"), hr ) ;
		}

		hr = m_pDevice->SetRenderTarget( 0 , m_pSurface[FRM_SURFACE] ) ;
		hr = m_pDevice->SetPixelShader ( NULL ) ;
		hr = m_pDevice->SetFVF ( D3DFVF_RENDER2FRAME_VERTEX_macro ) ;
		hr = m_pDevice->SetStreamSource ( 0, m_pVertices[RENDER2FRAME], 0, sizeof(RENDER2FRAME_VERTEX_st) ) ;
		hr = m_pDevice->SetTexture ( 0, m_pTexture[RENDER2FRAME] ) ;


		bool bAlpha = ( Transparent <= 0.999f ) ;
		if ( bAlpha )
		{
			m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE) ;
					// Set the source blend state.
			m_pDevice->SetRenderState(D3DRS_SRCBLEND, 
										D3DBLEND_SRCALPHA) ;

			// Set the destination blend state.
			m_pDevice->SetRenderState(D3DRS_DESTBLEND, 
										D3DBLEND_INVSRCALPHA) ;
		}

		hr = m_pDevice->DrawPrimitive ( D3DPT_TRIANGLEFAN, 0 , 2 ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->DrawPrimitive FAILED!\nhr=%x"), hr ) ;
		}

		if ( bAlpha )
		{
			m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE ) ;
		}

		return true ;
	}

	virtual bool InvalidateDeviceObjects ()
	{
		for ( int i = 0 ; i < SURFACE_COUNT ; i++ )
		{
			SAFE_RELEASE ( m_pSurface[i] ) ;
		}

		for ( int i = 0 ; i < RENDER_COUNT ; i++ )
		{
			SAFE_RELEASE ( m_pVertices[i] ) ;
			SAFE_RELEASE ( m_pTexture[i] ) ;
		}
		SAFE_RELEASE ( m_pTexture_Parity ) ;
	
		return true ;
	}

	virtual bool RestoreDeviceObjects ()
	{
		if ( !CreateRender2Texture ( m_lWidth / 2, m_lHeight ) )
		{
			return false ;
		}
		
		if ( !CreateRender2Frame ( m_lWidth / 2, m_lHeight, &m_DstRect ) )
		{
			return false ;
		}

		return true ;
	}

	virtual bool Fill ( BYTE r, BYTE g, BYTE b )
	{
		if ( m_pTexture[RENDER2TEXTURE] == NULL )
		{
			return false ;
		}

		BYTE y, u, v ;
		rgb2yuv ( r, g, b, &y, &u, &v ) ;
		
		BYTE yuv422[4] ;
		if ( m_ColorSpace == ID3DVRInterface::CS_YUY2 )
		{
			yuv422[0] = y ;
			yuv422[1] = u ;
			yuv422[2] = y ;
			yuv422[3] = v ;
		}
		else if ( m_ColorSpace == ID3DVRInterface::CS_UYVY )
		{
			yuv422[0] = u ;
			yuv422[1] = y ;
			yuv422[2] = v ;
			yuv422[3] = y ;
		}
		else
		{
			return false ;
		}

		D3DLOCKED_RECT lrect ;
		HRESULT hr = m_pTexture[RENDER2TEXTURE]->LockRect ( 0, &lrect, NULL, D3DLOCK_DISCARD/*|D3DLOCK_NO_DIRTY_UPDATE*/ ) ;
		if ( SUCCEEDED(hr) )
		{
			BYTE *pDest = (BYTE*)lrect.pBits ;
			
			for ( int j = 0 ; j < m_lHeight ; j++ )
			{
				BYTE *p = pDest ;
				for ( int i = 0 ; i < m_lWidth / 4 ; i++ )
				{
					CopyMemory ( p, yuv422, 4 ) ;
					p += 4 ;
				}
				pDest += lrect.Pitch ;
			}

			hr = m_pTexture[RENDER2TEXTURE]->UnlockRect ( 0 ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pTexture[i]->UnlockRect FAILED!\nhr=%x"), hr ) ;
			}
		}
		else
		{
			g_pDebug->Log_Error ( _T("m_pTexture[i]->LockRect FAILED!\nhr=%x"), hr ) ;
			return false ;
		}

		return true ;
	}
	
private:
	bool CreateRender2Texture ( long lWidth, long lHeight )
	{
		BYTE *p = NULL ;
		LPDIRECT3DTEXTURE9 m_pTexture_temp = NULL ;
		do
		{
			HRESULT hr = m_pDevice->CreateVertexBuffer ( 4 * sizeof(RENDER2TEXTURE_VERTEX_st), 0, D3DFVF_RENDER2TEXTURE_VERTEX_macro, D3DPOOL_DEFAULT, &m_pVertices[RENDER2TEXTURE], NULL ) ;
			if ( FAILED(hr) )
			{
				break ;
			}
			
			RENDER2TEXTURE_VERTEX_st *pVB ;

			hr = m_pVertices[RENDER2TEXTURE]->Lock ( 0, 0, (void**)&pVB, 0 ) ;
			if ( FAILED(hr) )
			{
				break ;
			}

			float shift = -0.5f / 2.0f ;
			pVB[0].pos = D3DXVECTOR4 ( shift,		   shift,		  0.0f,   1.0f ) ;
			pVB[1].pos = D3DXVECTOR4 ( lWidth + shift,  shift,		  0.0f, 1.0f ) ;
			pVB[2].pos = D3DXVECTOR4 ( lWidth + shift,  lHeight + shift, 0.0f, 1.0f ) ;
			pVB[3].pos = D3DXVECTOR4 ( shift,          lHeight + shift, 0.0f, 1.0f ) ;

			pVB[0].color = pVB[1].color = pVB[2].color = pVB[3].color = 0xffffffff ;

			pVB[0].tex1 = pVB[0].tex2 = D3DXVECTOR2 ( 0.0f, 0.0f ) ;
			pVB[1].tex1 = pVB[1].tex2 = D3DXVECTOR2 ( 1.0f, 0.0f ) ;
			pVB[2].tex1 = pVB[2].tex2 = D3DXVECTOR2 ( 1.0f, 1.0f ) ;
			pVB[3].tex1 = pVB[3].tex2 = D3DXVECTOR2 ( 0.0f, 1.0f ) ;

			hr = m_pVertices[RENDER2TEXTURE]->Unlock () ;
			if ( FAILED(hr) )
			{
				break ;
			}

			hr = m_pDevice->CreateTexture ( lWidth * 2, lHeight, 1, D3DUSAGE_DYNAMIC, D3DFMT_L8, D3DPOOL_DEFAULT, &m_pTexture[RENDER2TEXTURE], NULL ) ;

			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pDevice->CreateTexture failed!\nhr=%x"), hr ) ;
				break ;
			}

			m_lWidth = lWidth * 2 ;
			m_lHeight = lHeight ;
			m_lSize = m_lWidth * m_lHeight ;

			float dx = 1.0f / m_lWidth ;
			if ( m_ColorSpace == ID3DVRInterface::CS_YUY2 )
			{
				m_YUVdx[0] = 0.0f ;
				m_YUVdx[1] = -dx ;
				m_YUVdx[2] =  3 * dx ;
				m_YUVdx[3] =  -5 * dx ;
				m_YUVdx[4] =  7 * dx ;
				m_YUVdx[5] =  dx ;
				m_YUVdx[6] =  5 * dx ;
				m_YUVdx[7] =  -3 * dx ;
				m_YUVdx[8] =  9 * dx ;
			}
			else if ( m_ColorSpace == ID3DVRInterface::CS_UYVY )
			{
				m_YUVdx[0] = dx ;
				m_YUVdx[1] = -2 * dx ;
				m_YUVdx[2] =  2 * dx ;
				m_YUVdx[3] =  -6 * dx ;
				m_YUVdx[4] =  6 * dx ;
				m_YUVdx[5] =  0.0f ;
				m_YUVdx[6] =  4 * dx ;
				m_YUVdx[7] =  -4 * dx ;
				m_YUVdx[8] =  8 * dx ;
			}
			else
			{
				_ASSERTE(m_ColorSpace != ID3DVRInterface::CS_YUY2 && m_ColorSpace != ID3DVRInterface::CS_UYVY) ;
				break ;
			}

			hr = m_pDevice->CreateTexture ( lWidth, lHeight, 1, 0, D3DFMT_L8, D3DPOOL_SYSTEMMEM, &m_pTexture_temp, NULL ) ;

			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pDevice->CreateTexture failed!\nhr=%x"), hr ) ;
				break ;
			}

			hr = m_pDevice->CreateTexture ( lWidth, lHeight, 1, 0, D3DFMT_L8, D3DPOOL_DEFAULT, &m_pTexture_Parity, NULL ) ;

			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pDevice->CreateTexture failed!\nhr=%x"), hr ) ;
				break ;
			}
			
			p = new BYTE[lWidth] ;
			if ( p == NULL )
			{
				break ;
			}

			for ( int i = 0 ; i< lWidth ; i++ )
			{
				p[i] = ( i % 2 ) * 255 ;
			}

			D3DLOCKED_RECT lrect ;
			hr = m_pTexture_temp->LockRect ( 0, &lrect, NULL, D3DLOCK_DISCARD/*|D3DLOCK_NO_DIRTY_UPDATE*/ ) ;
			if ( SUCCEEDED(hr) )
			{
				BYTE *pDest = (BYTE*)lrect.pBits ;
				for ( int j = 0 ; j < lHeight ; j++ )
				{
					CopyMemory ( pDest, p, lWidth ) ;
					pDest += lrect.Pitch ;
				}

				hr = m_pTexture_temp->UnlockRect ( 0 ) ;
				if ( FAILED(hr) )
				{
					g_pDebug->Log_Error ( _T("m_pTexture_Parity->UnlockRect FAILED!\nhr=%x"), hr ) ;
				}
			}
			else
			{
				g_pDebug->Log_Error ( _T("m_pTexture_Parity->LockRect FAILED!\nhr=%x"), hr ) ;
				break ;
			}

			hr = m_pDevice->UpdateTexture ( m_pTexture_temp, m_pTexture_Parity ) ;
			if ( FAILED(hr) )
			{
				break ;
			}
			
			SAFE_DELETEARRAY ( p ) ;
			SAFE_RELEASE ( m_pTexture_temp ) ;
			return true ;

		} while ( false ) ;

		SAFE_DELETEARRAY ( p ) ;
		SAFE_RELEASE ( m_pTexture_temp ) ;

		InvalidateDeviceObjects () ;

		return false ;
	}

	bool CreateRender2Frame ( long lWidth, long lHeight, RECT *rect )
	{
		HRESULT hr ;

		do
		{
			long x = rect->left ;
			long y = rect->top ;
			long dx = rect->right ;
			long dy = rect->bottom ;
			m_DstRect.left = x ;
			m_DstRect.top = y ;
			m_DstRect.right = dx ;
			m_DstRect.bottom = dy ;

			m_Ver_Point[0] = D3DXVECTOR4 ( m_DstRect.left-0.5f,  m_DstRect.top-0.5f,	0.0f, 1.0f ) ;
			m_Ver_Point[1] = D3DXVECTOR4 ( m_DstRect.right-0.5f, m_DstRect.top-0.5f,	0.0f, 1.0f ) ;
			m_Ver_Point[2] = D3DXVECTOR4 ( m_DstRect.right-0.5f, m_DstRect.bottom-0.5f, 0.0f, 1.0f ) ;
			m_Ver_Point[3] = D3DXVECTOR4 ( m_DstRect.left-0.5f,  m_DstRect.bottom-0.5f, 0.0f, 1.0f ) ;

		
			hr = m_pDevice->CreateVertexBuffer ( 4 * sizeof(RENDER2FRAME_VERTEX_st), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, D3DFVF_RENDER2FRAME_VERTEX_macro, D3DPOOL_DEFAULT, &m_pVertices[RENDER2FRAME], NULL ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pDevice->CreateVertexBuffer failed!\nhr=%x"), hr ) ;
				break ;
			}
			
			RENDER2FRAME_VERTEX_st *pVB ;

			hr = m_pVertices[RENDER2FRAME]->Lock ( 0, 0, (void**)&pVB, 0 ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pVertices->Lock failed!\nhr=%x"), hr ) ;
				break ;
			}

			for ( int i = 0 ; i < 4 ; i++ )
			{
				pVB[i].pos = m_Ver_Point[i] ;
				pVB[i].color = m_dwColor ;
				pVB[i].tex1 = m_Tex_Point[i] ;
			}
			
			hr = m_pVertices[RENDER2FRAME]->Unlock () ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pVertices->Unlock failed!\nhr=%x"), hr ) ;
				break ;
			}
			
			hr = m_pDevice->CreateTexture ( lWidth, lHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pTexture[RENDER2FRAME], NULL ) ;

			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pDevice->CreateTexture failed!\nhr=%x"), hr ) ;
				break ;
			}

			hr = m_pTexture[RENDER2FRAME]->GetSurfaceLevel ( 0, &m_pSurface[TEX_SURFACE] ) ;
			if ( FAILED(hr) )
			{
				break ;
			}

			hr = m_pDevice->GetRenderTarget ( 0 , &m_pSurface[FRM_SURFACE] ) ;
			if ( FAILED(hr) )
			{
				break ;
			}
			
			return true ;

		} while ( false ) ;

		InvalidateDeviceObjects () ;
		return false ;
	}

	bool SetImage ( ID3DVRInterface::GEOMETRIC_TRANSFORMATION Flip, RECT *pDstRect, RECT *pSrcRect )
	{
		RECT d, s ;
		if ( pDstRect == NULL )
		{
			pDstRect = &d ;
			pDstRect->left = pDstRect->top = 0 ;
			pDstRect->right = m_lWidth ;
			pDstRect->bottom = m_lHeight ;
		}
		if ( pSrcRect == NULL )
		{
			pSrcRect = &s ;
			pSrcRect->left = pSrcRect->top = 0 ;
			pSrcRect->right = m_lWidth ;
			pSrcRect->bottom = m_lHeight ;
		}

		if ( !SetImageArgs ( 1.0f, Flip, pDstRect, pSrcRect ) )
		{
			return true ;
		}
				
		RENDER2FRAME_VERTEX_st *pVB ;

		HRESULT hr = m_pVertices[RENDER2FRAME]->Lock ( 0, 0, (void**)&pVB, D3DLOCK_DISCARD ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pVertices->Lock failed!\nhr=%x"), hr ) ;
			return false ;
		}

		for ( int i = 0 ; i < 4 ; i++ )
		{
			pVB[i].pos = m_Ver_Point[i] ;
			pVB[i].color = m_dwColor ;
			pVB[i].tex1 = m_Tex_Point[i] ;
		}

		hr = m_pVertices[RENDER2FRAME]->Unlock () ;

		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pVertices->Unlock failed!\nhr=%x"), hr ) ;
			return false ;
		}
	
		return true ;
	}

private:

	LPDIRECT3DVERTEXBUFFER9 m_pVertices[RENDER_COUNT] ;
	LPDIRECT3DTEXTURE9 m_pTexture[RENDER_COUNT] ;
	LPDIRECT3DTEXTURE9 m_pTexture_Parity ;

	LPDIRECT3DSURFACE9 m_pSurface[SURFACE_COUNT] ;
	
	CShader_YUV422 *m_pShader ;

	long m_lWidth, m_lHeight, m_lSize ;
	float m_YUVdx[9] ;
};
