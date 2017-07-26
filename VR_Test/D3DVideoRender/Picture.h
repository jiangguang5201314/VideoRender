/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/


#pragma once

class CPicture
{
	struct RENDER2FRAME_VERTEX_st
	{
		D3DXVECTOR4 pos ;
		DWORD color ;
		D3DXVECTOR2 tex1 ;
	} ;

	enum
	{
		D3DFVF_RENDER2FRAME_VERTEX_macro = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1
	} ;

public:

	CPicture ( HRESULT &hr, LPDIRECT3DDEVICE9 pDevice, const char *pSrcFile, RECT *DstRect )
		: m_pDevice(pDevice)
		, m_pVertices(NULL)
		, m_pTexture(NULL)
		, m_Flip(ID3DVRInterface::GS_NONE)
		, m_Transparent(1.0f)
		, m_dwColor(0xffffffff)
	{
		memset ( &m_DstRect, 0, sizeof(m_DstRect) ) ;
		memset ( &m_SrcRect, 0, sizeof(m_SrcRect) ) ;
		
		m_Tex_Point[0] = D3DXVECTOR2 ( 0.0f, 0.0f ) ;
		m_Tex_Point[1] = D3DXVECTOR2 ( 1.0f, 0.0f ) ;
		m_Tex_Point[2] = D3DXVECTOR2 ( 1.0f, 1.0f ) ;
		m_Tex_Point[3] = D3DXVECTOR2 ( 0.0f, 1.0f ) ;

		do
		{
			hr = D3DXGetImageInfoFromFile ( pSrcFile, &m_Info ) ;
			if ( hr != D3D_OK )
			{
				g_pDebug->Log_Error ( _T("D3DXGetImageInfoFromFile failed!\nhr=%x"), hr ) ;
				break ;
			}

			hr = D3DXCreateTextureFromFileEx ( pDevice, pSrcFile, m_Info.Width, m_Info.Height,
											   0, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_LINEAR,
											   D3DX_DEFAULT, 0xFF000000, &m_Info, NULL, &m_pTexture ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("D3DXCreateTextureFromFileEx failed!\nhr=%x"), hr ) ;
				break ;
			}

			m_Tex_dx = 1.0f / m_Info.Width ;
			m_Tex_dy = 1.0f / m_Info.Height ;

			hr = m_pDevice->CreateVertexBuffer ( 4 * sizeof(RENDER2FRAME_VERTEX_st), 0, D3DFVF_RENDER2FRAME_VERTEX_macro, D3DPOOL_MANAGED, &m_pVertices, NULL ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pDevice->CreateVertexBuffer failed!\nhr=%x"), hr ) ;
				break ;
			}

			if ( DstRect != NULL )
			{
				m_DstRect.left = DstRect->left ;
				m_DstRect.top = DstRect->top ;
				m_DstRect.right = DstRect->right ;
				m_DstRect.bottom = DstRect->bottom ;
			}
			else
			{
				m_DstRect.left = m_DstRect.top = 0 ;
				m_DstRect.right = m_Info.Width ;
				m_DstRect.bottom = m_Info.Height ;
			}

			m_Ver_Point[0] = D3DXVECTOR4 ( m_DstRect.left-0.5f,  m_DstRect.top-0.5f,	0.0f, 1.0f ) ;
			m_Ver_Point[1] = D3DXVECTOR4 ( m_DstRect.right-0.5f, m_DstRect.top-0.5f,	0.0f, 1.0f ) ;
			m_Ver_Point[2] = D3DXVECTOR4 ( m_DstRect.right-0.5f, m_DstRect.bottom-0.5f, 0.0f, 1.0f ) ;
			m_Ver_Point[3] = D3DXVECTOR4 ( m_DstRect.left-0.5f,  m_DstRect.bottom-0.5f, 0.0f, 1.0f ) ;

			RENDER2FRAME_VERTEX_st *pVB ;
			hr = m_pVertices->Lock ( 0, 0, (void**)&pVB, 0 ) ;
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

			hr = m_pVertices->Unlock () ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pVertices->Unlock failed!\nhr=%x"), hr ) ;
				break ;
			}

			hr = S_OK ;
			return ;

		} while ( false ) ;

		hr = E_FAIL ;
		Destroy () ;
	}

	~CPicture(void)
	{
		Destroy () ;
	}


	bool DrawPicture ( float Transparent, ID3DVRInterface::GEOMETRIC_TRANSFORMATION Transformation, RECT *pDstRect, RECT *pSrcRect )
	{
		if ( Transparent <= 0.001f )
		{
			return true ;
		}

		HRESULT hr = m_pDevice->SetFVF ( D3DFVF_RENDER2FRAME_VERTEX_macro ) ;
		hr = m_pDevice->SetPixelShader ( NULL ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetPixelShader failed!\nhr=%x"), hr ) ;
		}

		SetImage ( Transformation, Transparent, pDstRect, pSrcRect ) ;

		hr = m_pDevice->SetTexture ( 0, m_pTexture ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetTexture [Y_TEX] failed!\nhr=%x"), hr ) ;
		} 
		
		hr = m_pDevice->SetStreamSource ( 0, m_pVertices, 0, sizeof(RENDER2FRAME_VERTEX_st) ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->SetStreamSource failed!\nhr=%x"), hr ) ;
		}

		bool bAlpha = ( Transparent <= 0.999f ) ;
		if ( bAlpha )
		{
			m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
			m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
			m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

			m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE) ;
					// Set the source blend state.
			m_pDevice->SetRenderState(D3DRS_SRCBLEND, 
										D3DBLEND_SRCALPHA) ;

			// Set the destination blend state.
			m_pDevice->SetRenderState(D3DRS_DESTBLEND, 
										D3DBLEND_INVSRCALPHA) ;
		}

		hr = m_pDevice->DrawPrimitive ( D3DPT_TRIANGLEFAN, 0, 2 ) ;
		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pDevice->DrawPrimitive FAILED!\nhr=%x"), hr ) ;
		}

		if ( bAlpha )
		{
			m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
			m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

			m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE ) ;
		}

		return true ;
	}

private:

	bool SetImage ( ID3DVRInterface::GEOMETRIC_TRANSFORMATION Flip, float Transparent, RECT *pDstRect, RECT *pSrcRect )
	{
		RECT d, s ;
		if ( pDstRect == NULL )
		{
			pDstRect = &d ;
			pDstRect->left = pDstRect->top = 0 ;
			pDstRect->right = m_Info.Width ;
			pDstRect->bottom = m_Info.Height ;
		}
		if ( pSrcRect == NULL )
		{
			pSrcRect = &s ;
			pSrcRect->left = pSrcRect->top = 0 ;
			pSrcRect->right = m_Info.Width ;
			pSrcRect->bottom = m_Info.Height ;
		}

		bool bSet = false ;	
		if ( m_Transparent != Transparent )
		{
			m_Transparent = Transparent ;
			m_dwColor = ((DWORD)( 255 * Transparent ) << 24) | 0x00ffffff ;
			bSet = true ;
		}

		if ( memcmp ( &m_DstRect, pDstRect, sizeof(m_DstRect) ) != 0 )
		{
			memcpy ( &m_DstRect, pDstRect, sizeof(m_DstRect) ) ;
			m_Ver_Point[0] = D3DXVECTOR4 ( m_DstRect.left-0.5f,  m_DstRect.top-0.5f,	0.0f, 1.0f ) ;
			m_Ver_Point[1] = D3DXVECTOR4 ( m_DstRect.right-0.5f, m_DstRect.top-0.5f,	0.0f, 1.0f ) ;
			m_Ver_Point[2] = D3DXVECTOR4 ( m_DstRect.right-0.5f, m_DstRect.bottom-0.5f, 0.0f, 1.0f ) ;
			m_Ver_Point[3] = D3DXVECTOR4 ( m_DstRect.left-0.5f,  m_DstRect.bottom-0.5f, 0.0f, 1.0f ) ;
			bSet = true ;
		}

		if ( memcmp ( &m_SrcRect, pSrcRect, sizeof(m_SrcRect) ) != 0 || Flip != m_Flip )
		{
			memcpy ( &m_SrcRect, pSrcRect, sizeof(m_SrcRect) ) ;
			m_Tex_Point[0] = D3DXVECTOR2 ( m_Tex_dx * m_SrcRect.left, m_Tex_dy * m_SrcRect.top ) ;
			m_Tex_Point[1] = D3DXVECTOR2 ( m_Tex_dx * m_SrcRect.right, m_Tex_dy * m_SrcRect.top ) ;
			m_Tex_Point[2] = D3DXVECTOR2 ( m_Tex_dx * m_SrcRect.right, m_Tex_dy * m_SrcRect.bottom ) ;
			m_Tex_Point[3] = D3DXVECTOR2 ( m_Tex_dx * m_SrcRect.left, m_Tex_dy * m_SrcRect.bottom ) ;

			if ( Flip & ID3DVRInterface::GS_UPPER_DOWN_FLIP )
			{
				D3DXVECTOR2 temp = m_Tex_Point[0] ;
				m_Tex_Point[0] = m_Tex_Point[3] ;
				m_Tex_Point[3] = temp ;

				temp = m_Tex_Point[1] ;
				m_Tex_Point[1] = m_Tex_Point[2] ;
				m_Tex_Point[2] = temp ;
			}

			if ( Flip & ID3DVRInterface::GS_LEFT_RIGHT_FLIP )
			{
				D3DXVECTOR2 temp = m_Tex_Point[0] ;
				m_Tex_Point[0] = m_Tex_Point[1] ;
				m_Tex_Point[1] = temp ;

				temp = m_Tex_Point[2] ;
				m_Tex_Point[2] = m_Tex_Point[3] ;
				m_Tex_Point[3] = temp ;
			}
		
			m_Flip = Flip ;
			bSet = true ;
		}
		
		if ( !bSet )
		{
			return true ;
		}
		
		RENDER2FRAME_VERTEX_st *pVB ;
		HRESULT hr = m_pVertices->Lock ( 0, 0, (void**)&pVB, 0 ) ;
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
		
		hr = m_pVertices->Unlock () ;

		if ( FAILED(hr) )
		{
			g_pDebug->Log_Error ( _T("m_pVertices->Unlock failed!\nhr=%x"), hr ) ;
			return false ;
		}

		return true ;
	}

	void Destroy ()
	{
		SAFE_RELEASE ( m_pVertices ) ;
		SAFE_RELEASE ( m_pTexture ) ;
	}


private:
	LPDIRECT3DDEVICE9 m_pDevice ;
	LPDIRECT3DVERTEXBUFFER9 m_pVertices ;
	LPDIRECT3DTEXTURE9 m_pTexture ;
	D3DXIMAGE_INFO m_Info ;

	ID3DVRInterface::GEOMETRIC_TRANSFORMATION m_Flip ;
	float m_Transparent ;
	DWORD m_dwColor ;
	RECT m_DstRect, m_SrcRect ;

	D3DXVECTOR4 m_Ver_Point[4] ;
	D3DXVECTOR2 m_Tex_Point[4] ;

	float m_Tex_dx, m_Tex_dy ;
};
