/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/


#pragma once
#include "shader_pixel.h"
#include "ShaderCode_YUV420.h"

class CShader_YUV420 :
	public CPixelShader
{
public:
	enum
	{
		Y_TEX = 0,
		U_TEX = 1,
		V_TEX = 2,
		COUNT_YUV_TEX = 3,
	} ;

public:

	CShader_YUV420(HRESULT &hr, LPDIRECT3DDEVICE9 pDevice)
	{
		do
		{
			if ( !CreateShader ( pDevice, g_YUV420ps30_main, g_YUV420ps20_main ) )
			{
				break ;
			}

			m_YUVTextureHandle[Y_TEX] = m_pPixelConstTable->GetConstantByName ( NULL, "YTextue" ) ;
			if ( m_YUVTextureHandle[Y_TEX] == NULL )
			{
				g_pDebug->Log_Error ( _T("m_pPixelConstTable->GetConstantByName YTextue failed!\nhr=%x"), hr ) ;
				break ;
			}

			m_YUVTextureHandle[U_TEX] = m_pPixelConstTable->GetConstantByName ( NULL, "UTextue" ) ;
			if ( m_YUVTextureHandle[U_TEX] == NULL )
			{
				g_pDebug->Log_Error ( _T("m_pPixelConstTable->GetConstantByName UTextue failed!\nhr=%x"), hr ) ;
				break ;
			}

			m_YUVTextureHandle[V_TEX] = m_pPixelConstTable->GetConstantByName ( NULL, "VTextue" ) ;
			if ( m_YUVTextureHandle[V_TEX] == NULL )
			{
				g_pDebug->Log_Error ( _T("m_pPixelConstTable->GetConstantByName VTextue failed!\nhr=%x"), hr ) ;
				break ;
			}

			UINT count ;
			hr = m_pPixelConstTable->GetConstantDesc ( m_YUVTextureHandle[Y_TEX], &m_YUVTextureDesc[Y_TEX], &count ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pPixelConstTable->GetConstantDesc [Y_TEX] failed!\nhr=%x"), hr ) ;
				break ;
			}
			hr = m_pPixelConstTable->GetConstantDesc ( m_YUVTextureHandle[U_TEX], &m_YUVTextureDesc[U_TEX], &count ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pPixelConstTable->GetConstantDesc [U_TEX] failed!\nhr=%x"), hr ) ;
				break ;
			}
			hr = m_pPixelConstTable->GetConstantDesc ( m_YUVTextureHandle[V_TEX], &m_YUVTextureDesc[V_TEX], &count ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pPixelConstTable->GetConstantDesc [V_TEX] failed!\nhr=%x"), hr ) ;
				break ;
			}
		
			m_transparentHandle = m_pPixelConstTable->GetConstantByName ( NULL, "transparent" ) ;
			if ( m_transparentHandle == NULL )
			{
				g_pDebug->Log_Error ( _T("m_pPixelConstTable->GetConstantByName transparent failed!\nhr=%x"), hr ) ;
				break ;
			}

			hr = m_pPixelConstTable->SetDefaults ( pDevice ) ;

			hr = S_OK ;
			return ;

		} while ( false ) ;
		
		hr = E_FAIL ;
	}
public:

	~CShader_YUV420(void)
	{
	}

public:
	D3DXHANDLE m_YUVTextureHandle[COUNT_YUV_TEX] ;
	D3DXCONSTANT_DESC m_YUVTextureDesc[COUNT_YUV_TEX] ;

	D3DXHANDLE m_transparentHandle ;
};
