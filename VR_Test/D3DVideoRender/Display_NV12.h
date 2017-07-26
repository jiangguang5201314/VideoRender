/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/

#pragma once
#include "display.h"
#include "Shader_YUV420.h"
#include "CommonFunctions.h"

class CDisplay_NV12 :
	public CDisplay
{	
	struct YUV_VERTEX_st
	{
		D3DXVECTOR4 pos ;
		DWORD color ;
		D3DXVECTOR2 tex1 ;
		D3DXVECTOR2 tex2 ;
		D3DXVECTOR2 tex3 ;
	} ;

	struct RENDERTARGET_VERTEX_st
	{
		D3DXVECTOR4 pos ;
		DWORD color ;
		D3DXVECTOR2 tex1 ;
	} ;

	enum
	{
		D3DFVF_YUV_VERTEX_macro		= D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX3,
		D3DFVF_RENDER_VERTEX_macro	= D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1,
	} ;

	enum
	{
		TEXTURE_Y  = 0,
		TEXTURE_UV = 1,
		TEXTURE_U  = 2,
		TEXTURE_V  = 3,
		TEXTURE_COUNT = 4
	} ;

	enum
	{
		VERTEX_Y = 0,
		VERTEX_U = 1,
		VERTEX_V = 2,
		VERTEX_COUNT = 3
	} ;

	enum
	{
		SURFACE_U = 0,
		SURFACE_V = 1,
		SURFACE_FRM = 2,
		SURFACE_COUNT = 3
	} ;

	enum
	{
		PLANE_Y  = 0,
		PLANE_UV = 1,
		PLANE_COUNT = 2
	} ;

public:

	CDisplay_NV12(HRESULT &hr, LPDIRECT3DDEVICE9 pDevice, CPixelShader **pShader)
		: m_pShader(NULL)
	{
		m_pDevice = pDevice ;

		Init_Array<LPDIRECT3DVERTEXBUFFER9>( m_pVertices, VERTEX_COUNT, NULL ) ;
		Init_Array<LPDIRECT3DTEXTURE9>( m_pTexture, TEXTURE_COUNT, NULL ) ;
		Init_Array<LPDIRECT3DSURFACE9>( m_pSurface, SURFACE_COUNT, NULL ) ;

		Init_Array<long>( m_lWidth, PLANE_COUNT, 0 ) ;
		Init_Array<long>( m_lHeight, PLANE_COUNT, 0 ) ;
		Init_Array<long>( m_lSize, PLANE_COUNT, 0 ) ;
				
		if ( pShader[CD3DManager::SHADER_YUV420] == NULL )
		{
			pShader[CD3DManager::SHADER_YUV420] = new CShader_YUV420 ( hr, pDevice ) ;
		}
		
		if ( pShader[CD3DManager::SHADER_YUV420] == NULL || FAILED(hr) )
		{
			SAFE_DELETE(pShader[CD3DManager::SHADER_YUV420]) ;
			return ;
		}

		m_pShader = (CShader_YUV420*)pShader[CD3DManager::SHADER_YUV420] ;
	}
public:

	~CDisplay_NV12(void)
	{
		InvalidateDeviceObjects () ;
	}

	static CDisplay *Create (LPDIRECT3DDEVICE9 pDevice, CPixelShader **pShader)
	{
		HRESULT hr = S_OK ;
		CDisplay_NV12 *p = new CDisplay_NV12 ( hr, pDevice, pShader ) ;
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

		return CreateVT ( lWidth, lHeight, rect ) ;
	}

	virtual bool UpdateImage ( BYTE *pData )
	{
		BYTE *pSrc = pData ;
		for ( int i = TEXTURE_Y ; i <= TEXTURE_UV ; i++ )
		{
			D3DLOCKED_RECT lrect ;
			HRESULT hr = m_pTexture[i]->LockRect ( 0, &lrect, NULL, D3DLOCK_DISCARD/*|D3DLOCK_NO_DIRTY_UPDATE*/ ) ;
			if ( SUCCEEDED(hr) )
			{
				BYTE *pDest = (BYTE*)lrect.pBits ;
				if ( lrect.Pitch == m_lWidth[i] )
				{
					memcpyMMX ( pDest, pSrc, m_lSize[i] ) ;
					pSrc += m_lSize[i] ;
				}
				else
				{
					for ( int j = 0 ; j < m_lHeight[i] ; j++ )
					{
						memcpyMMX ( pDest, pSrc, m_lWidth[i] ) ;
						pDest += lrect.Pitch ;
						pSrc += m_lWidth[i] ;
					}
				}
				
				hr = m_pTexture[i]->UnlockRect ( 0 ) ;
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

		HRESULT hr = m_pDevice->SetFVF ( D3DFVF_RENDER_VERTEX_macro ) ;
		hr = m_pDevice->SetRenderTarget( 0 , m_pSurface[SURFACE_U] ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetRenderTarget failed!\nhr=%x"), hr ) ;
		} 
		hr = m_pDevice->SetPixelShader ( NULL ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetPixelShader ( NULL ) failed!\nhr=%x"), hr ) ;
		} 

		hr = m_pDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT ) ;
		hr = m_pDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT ) ;
		hr = m_pDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT ) ;

		hr = m_pDevice->SetTexture ( 0, m_pTexture[TEXTURE_UV] ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetTexture [Y_TEX] failed!\nhr=%x"), hr ) ;
		} 
		
		hr = m_pDevice->SetStreamSource ( 0, m_pVertices[VERTEX_U], 0, sizeof(RENDERTARGET_VERTEX_st) ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetStreamSource failed!\nhr=%x"), hr ) ;
		}

		hr = m_pDevice->DrawPrimitive ( D3DPT_TRIANGLEFAN, 0 , 2 ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->DrawPrimitive FAILED!\nhr=%x"), hr ) ;
		}
////////////////////////////////////////////////////////////////////////////////////////////////////////

		hr = m_pDevice->SetRenderTarget( 0 , m_pSurface[SURFACE_V] ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetRenderTarget failed!\nhr=%x"), hr ) ;
		} 

		hr = m_pDevice->SetTexture ( 0, m_pTexture[TEXTURE_UV] ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetTexture [Y_TEX] failed!\nhr=%x"), hr ) ;
		} 
		
		hr = m_pDevice->SetStreamSource ( 0, m_pVertices[VERTEX_V], 0, sizeof(RENDERTARGET_VERTEX_st) ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetStreamSource failed!\nhr=%x"), hr ) ;
		}

		hr = m_pDevice->DrawPrimitive ( D3DPT_TRIANGLEFAN, 0 , 2 ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->DrawPrimitive FAILED!\nhr=%x"), hr ) ;
		}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		hr = m_pDevice->SetFVF ( D3DFVF_YUV_VERTEX_macro ) ;
		hr = m_pDevice->SetPixelShader ( m_pShader->m_pPixelShader ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetPixelShader failed!\nhr=%x"), hr ) ;
		} 

		hr = m_pDevice->SetRenderTarget( 0 , m_pSurface[SURFACE_FRM] ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetRenderTarget failed!\nhr=%x"), hr ) ;
		} 

		SetImage ( Transformation, pDstRect, pSrcRect ) ;

		hr = m_pShader->m_pPixelConstTable->SetFloat ( m_pDevice, m_pShader->m_transparentHandle, Transparent ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pShader->m_pPixelConstTable->SetFloat failed!\nhr=%x"), hr ) ;
		} 

		hr = m_pDevice->SetTexture ( m_pShader->m_YUVTextureDesc[CShader_YUV420::Y_TEX].RegisterIndex, m_pTexture[TEXTURE_Y] ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetTexture [Y_TEX] failed!\nhr=%x"), hr ) ;
		} 
		hr = m_pDevice->SetTexture ( m_pShader->m_YUVTextureDesc[CShader_YUV420::U_TEX].RegisterIndex, m_pTexture[TEXTURE_U] ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetTexture [U_TEX] failed!\nhr=%x"), hr ) ;
		}
		hr = m_pDevice->SetTexture ( m_pShader->m_YUVTextureDesc[CShader_YUV420::V_TEX].RegisterIndex, m_pTexture[TEXTURE_V] ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetTexture [V_TEX] failed!\nhr=%x"), hr ) ;
		}

		hr = m_pDevice->SetStreamSource ( 0, m_pVertices[VERTEX_Y], 0, sizeof(YUV_VERTEX_st) ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetStreamSource failed!\nhr=%x"), hr ) ;
		}

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
		for ( int i = 0 ; i < VERTEX_COUNT ; i++ )
		{
			SAFE_RELEASE ( m_pVertices[i] ) ;
		}
		for ( int i = 0 ; i < TEXTURE_COUNT ; i++ )
		{
			SAFE_RELEASE ( m_pTexture[i] ) ;
		}
		for ( int i = 0 ; i < SURFACE_COUNT ; i++ )
		{
			SAFE_RELEASE ( m_pSurface[i] ) ;
		}

		return true ;
	}

	virtual bool RestoreDeviceObjects ()
	{
		return CreateVT ( m_lWidth[PLANE_Y], m_lHeight[PLANE_Y], &m_DstRect ) ;
	}

	virtual bool Fill ( BYTE r, BYTE g, BYTE b )
	{
		if ( m_pTexture[TEXTURE_Y] == NULL )
		{
			return false ;
		}
		
		BYTE yuv[3] ;
		rgb2yuv ( r, g, b, &yuv[0], &yuv[1], &yuv[2] ) ;

		for ( int i = TEXTURE_Y ; i <= TEXTURE_UV ; i++ )
		{
			D3DLOCKED_RECT lrect ;
			HRESULT hr = m_pTexture[i]->LockRect ( 0, &lrect, NULL, D3DLOCK_DISCARD/*|D3DLOCK_NO_DIRTY_UPDATE*/ ) ;
			if ( SUCCEEDED(hr) )
			{
				BYTE *pDest = (BYTE*)lrect.pBits ;

				if ( i == TEXTURE_Y )
				{
					if ( lrect.Pitch == m_lWidth[i] )
					{
						memset ( pDest, yuv[0], m_lSize[i] ) ;
					}
					else
					{
						for ( int j = 0 ; j < m_lHeight[i] ; j++ )
						{
							memset ( pDest, yuv[0], m_lWidth[i] ) ;
							pDest += lrect.Pitch ;
						}
					}
				}
				else
				{
					for ( int j = 0 ; j < m_lHeight[i] ; j++ )
					{
						BYTE *p = pDest ;
						for ( int m = 0 ; m < m_lWidth[i] / 2 ; m++ )
						{
							CopyMemory ( p, &yuv[1], 2 ) ;
							p += 2 ;
						}
						pDest += lrect.Pitch ;
					}
				}
				
				hr = m_pTexture[i]->UnlockRect ( 0 ) ;
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
		}

		return true ;
	}

private:
	bool CreateVT ( long lWidth, long lHeight, RECT *rect )
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


			long uvWidth = lWidth / 2 ;
			long uvHeight = lHeight / 2 ;

			for ( int i = VERTEX_Y ; i < VERTEX_COUNT ; i++ )
			{
				DWORD length, fvf, usage ;
				if ( i == VERTEX_Y )
				{
					length = 4 * sizeof(YUV_VERTEX_st) ;
					fvf = D3DFVF_YUV_VERTEX_macro ;
					usage = D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC ;
				}
				else
				{
					length = 4 * sizeof(RENDERTARGET_VERTEX_st) ;
					fvf = D3DFVF_RENDER_VERTEX_macro ;
					usage = 0 ;
				}

				hr = m_pDevice->CreateVertexBuffer ( length, usage, fvf, D3DPOOL_DEFAULT, &m_pVertices[i], NULL ) ;
				if ( FAILED(hr) )
				{
					g_pDebug->Log_Error ( _T("m_pDevice->CreateVertexBuffer failed!\nhr=%x"), hr ) ;
					break ;
				}

				void *p ;
				
				hr = m_pVertices[i]->Lock ( 0, 0, (void**)&p, 0 ) ;
				if ( FAILED(hr) )
				{
					g_pDebug->Log_Error ( _T("m_pVertices->Lock failed!\nhr=%x"), hr ) ;
					break ;
				}

				float shift = -0.5f / 2 ;

				if ( i == VERTEX_Y )
				{
					YUV_VERTEX_st *pVB = (YUV_VERTEX_st*)p ;
					for ( int i = 0 ; i < 4 ; i++ )
					{
						pVB[i].pos = m_Ver_Point[i] ;
						pVB[i].color = m_dwColor ;
						pVB[i].tex1 = pVB[i].tex2 = pVB[i].tex3 = m_Tex_Point[i] ;
					}
				}
				else if ( i == VERTEX_U )
				{
					RENDERTARGET_VERTEX_st *pVB = (RENDERTARGET_VERTEX_st*)p ;
					pVB[0].color = pVB[1].color = pVB[2].color = pVB[3].color = D3DCOLOR_RGBA(255,255,255,255) ;

					pVB[0].pos = D3DXVECTOR4 ( shift,         shift,			0.0f, 1.0f ) ;
					pVB[1].pos = D3DXVECTOR4 ( uvWidth + shift,     shift,			0.0f, 1.0f ) ;
					pVB[2].pos = D3DXVECTOR4 ( uvWidth + shift,     uvHeight + shift,   0.0f, 1.0f ) ;
					pVB[3].pos = D3DXVECTOR4 ( shift,         uvHeight + shift,   0.0f, 1.0f ) ;

					pVB[0].tex1 = D3DXVECTOR2 ( 0.0f, 0.0f ) ;
					pVB[1].tex1 = D3DXVECTOR2 ( 1.0f, 0.0f ) ;
					pVB[2].tex1 = D3DXVECTOR2 ( 1.0f, 1.0f ) ;
					pVB[3].tex1 = D3DXVECTOR2 ( 0.0f, 1.0f ) ;
				}
				else
				{
					RENDERTARGET_VERTEX_st *pVB = (RENDERTARGET_VERTEX_st*)p ;
					pVB[0].color = pVB[1].color = pVB[2].color = pVB[3].color = D3DCOLOR_RGBA(255,255,255,255) ;

					pVB[0].pos = D3DXVECTOR4 ( shift,         shift,			0.0f, 1.0f ) ;
					pVB[1].pos = D3DXVECTOR4 ( uvWidth + shift,     shift,			0.0f, 1.0f ) ;
					pVB[2].pos = D3DXVECTOR4 ( uvWidth + shift,     uvHeight + shift,   0.0f, 1.0f ) ;
					pVB[3].pos = D3DXVECTOR4 ( shift,         uvHeight + shift,   0.0f, 1.0f ) ;
					
					float Texel = 1.0f / lWidth ;
					pVB[0].tex1 = D3DXVECTOR2 ( 0.0f + Texel, 0.0f ) ;
					pVB[1].tex1 = D3DXVECTOR2 ( 1.0f + Texel, 0.0f ) ;
					pVB[2].tex1 = D3DXVECTOR2 ( 1.0f + Texel, 1.0f ) ;
					pVB[3].tex1 = D3DXVECTOR2 ( 0.0f + Texel, 1.0f ) ;
				}

				hr = m_pVertices[i]->Unlock () ;
				if ( FAILED(hr) )
				{
					g_pDebug->Log_Error ( _T("m_pVertices->Unlock failed!\nhr=%x"), hr ) ;
					break ;
				}
			}

			for ( int i = TEXTURE_Y ; i < TEXTURE_COUNT ; i++ )
			{
				DWORD dwUsage = D3DUSAGE_DYNAMIC ;
				long Width = lWidth ;
				long Height = lHeight ;
				if ( i == TEXTURE_U || i == TEXTURE_V )
				{
					Width  /= 2 ;
					Height /= 2 ;
					dwUsage = D3DUSAGE_RENDERTARGET ;
				}
				if ( i == TEXTURE_UV )
				{
					Height /= 2 ;
				}

				hr = m_pDevice->CreateTexture ( Width, Height, 1, dwUsage, D3DFMT_L8, D3DPOOL_DEFAULT, &m_pTexture[i], NULL ) ;

				if ( FAILED(hr) )
				{
					g_pDebug->Log_Error ( _T("m_pDevice->CreateTexture failed!\nhr=%x"), hr ) ;
					break ;
				}
				
				if ( i == TEXTURE_U || i == TEXTURE_V )
				{
					int index ;
					if ( i == TEXTURE_U )
					{
						index = SURFACE_U ;
					}
					else
					{
						index = SURFACE_V ;
					}

					hr = m_pTexture[i]->GetSurfaceLevel ( 0, &m_pSurface[index] ) ;
					if ( FAILED(hr) )
					{
						g_pDebug->Log_Error ( _T("m_pTexture[i]->GetSurfaceLevel failed!\nhr=%x"), hr ) ;
						break ;
					}
				}
			}

			hr = m_pDevice->GetRenderTarget ( 0 , &m_pSurface[SURFACE_FRM] ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pDevice->GetRenderTarget failed!\nhr=%x"), hr ) ;
				break ;
			}

			m_lWidth[PLANE_Y] = lWidth ;
			m_lHeight[PLANE_Y] = lHeight ;
			m_lSize[PLANE_Y] = m_lWidth[PLANE_Y] * m_lHeight[PLANE_Y] ;

			m_lWidth[PLANE_UV] = lWidth ;
			m_lHeight[PLANE_UV] = lHeight / 2 ;
			m_lSize[PLANE_UV] = m_lWidth[PLANE_UV] * m_lHeight[PLANE_UV] ;

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
			pDstRect->right = m_lWidth[0] ;
			pDstRect->bottom = m_lHeight[0] ;
		}
		if ( pSrcRect == NULL )
		{
			pSrcRect = &s ;
			pSrcRect->left = pSrcRect->top = 0 ;
			pSrcRect->right = m_lWidth[0] ;
			pSrcRect->bottom = m_lHeight[0] ;
		}

		if ( !SetImageArgs ( 1.0f, Flip, pDstRect, pSrcRect ) )
		{
			return true ;
		}
				
		YUV_VERTEX_st *pVB ;

		HRESULT hr = m_pVertices[VERTEX_Y]->Lock ( 0, 0, (void**)&pVB, D3DLOCK_DISCARD ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pVertices->Lock failed!\nhr=%x"), hr ) ;
			return false ;
		}

		for ( int i = 0 ; i < 4 ; i++ )
		{
			pVB[i].pos = m_Ver_Point[i] ;
			pVB[i].color = m_dwColor ;
			pVB[i].tex1 = pVB[i].tex2 = pVB[i].tex3 = m_Tex_Point[i] ;
		}

		hr = m_pVertices[VERTEX_Y]->Unlock () ;

		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pVertices->Unlock failed!\nhr=%x"), hr ) ;
			return false ;
		}
	
		return true ;
	}

private:

	LPDIRECT3DVERTEXBUFFER9 m_pVertices[VERTEX_COUNT] ;
	LPDIRECT3DTEXTURE9	m_pTexture[TEXTURE_COUNT] ;
	LPDIRECT3DSURFACE9 m_pSurface[SURFACE_COUNT] ;

	long m_lWidth[PLANE_COUNT] ;
	long m_lHeight[PLANE_COUNT] ;
	long m_lSize[PLANE_COUNT] ;
	CShader_YUV420 *m_pShader ;
};