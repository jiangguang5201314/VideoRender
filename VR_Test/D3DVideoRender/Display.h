/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/


#pragma once

#include <crtdbg.h>
#include <d3dx9.h>
#include "Shader_Pixel.h"

class CDisplay
{	
public:
	virtual bool SetImageBuffer ( long lWidth, long lHeight, ID3DVRInterface::COLOR_SPACE cs, RECT *rect ) = 0 ;
	virtual bool UpdateImage ( BYTE *pData ) = 0 ;
	virtual bool DrawImage ( float Transparent, ID3DVRInterface::GEOMETRIC_TRANSFORMATION Transformation, RECT *pDstRect, RECT *pSrcRect ) = 0 ;
	virtual bool InvalidateDeviceObjects () = 0 ;
	virtual bool RestoreDeviceObjects () = 0 ;
	virtual bool Fill ( BYTE r, BYTE g, BYTE b ) = 0 ;

public:

	CDisplay(void)
		: m_pDevice(NULL)
		, m_ColorSpace(ID3DVRInterface::CS_UNKNOWN)
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
	}

	virtual ~CDisplay(void)
	{
	}

protected:
	void rgb2yuv ( BYTE r, BYTE g, BYTE b, BYTE *y, BYTE *u, BYTE *v )
	{
		*y = ( (  66 * r + 129 * g +  25 * b + 128) >> 8) +  16 ; // Y
		*v = ( ( 112 * r -  94 * g -  18 * b + 128) >> 8) + 128 ; // V
		*u = ( ( -38 * r -  74 * g + 112 * b + 128) >> 8) + 128 ; // U
	}

	inline bool SetImageArgs ( float Transparent, ID3DVRInterface::GEOMETRIC_TRANSFORMATION Flip, RECT *pDstRect, RECT *pSrcRect )
	{
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
		
		return bSet ;
	}

	inline void CalculateTextureCoords ()
	{
		m_Tex_Point[0] = D3DXVECTOR2 ( m_Tex_dx * m_SrcRect.left, m_Tex_dy * m_SrcRect.top ) ;
		m_Tex_Point[1] = D3DXVECTOR2 ( m_Tex_dx * m_SrcRect.right, m_Tex_dy * m_SrcRect.top ) ;
		m_Tex_Point[2] = D3DXVECTOR2 ( m_Tex_dx * m_SrcRect.right, m_Tex_dy * m_SrcRect.bottom ) ;
		m_Tex_Point[3] = D3DXVECTOR2 ( m_Tex_dx * m_SrcRect.left, m_Tex_dy * m_SrcRect.bottom ) ;

		if ( m_Flip & ID3DVRInterface::GS_UPPER_DOWN_FLIP )
		{
			D3DXVECTOR2 temp = m_Tex_Point[0] ;
			m_Tex_Point[0] = m_Tex_Point[3] ;
			m_Tex_Point[3] = temp ;

			temp = m_Tex_Point[1] ;
			m_Tex_Point[1] = m_Tex_Point[2] ;
			m_Tex_Point[2] = temp ;
		}

		if ( m_Flip & ID3DVRInterface::GS_LEFT_RIGHT_FLIP )
		{
			D3DXVECTOR2 temp = m_Tex_Point[0] ;
			m_Tex_Point[0] = m_Tex_Point[1] ;
			m_Tex_Point[1] = temp ;

			temp = m_Tex_Point[2] ;
			m_Tex_Point[2] = m_Tex_Point[3] ;
			m_Tex_Point[3] = temp ;
		}
	}

protected:
	LPDIRECT3DDEVICE9 m_pDevice ;
	RECT m_DstRect, m_SrcRect ;
	ID3DVRInterface::GEOMETRIC_TRANSFORMATION m_Flip ;
	ID3DVRInterface::COLOR_SPACE m_ColorSpace ;

	float m_Tex_dx, m_Tex_dy ;

	D3DXVECTOR2 m_Tex_Point[4] ;
	D3DXVECTOR4 m_Ver_Point[4] ;

	float m_Transparent ;
	DWORD m_dwColor ;
};
