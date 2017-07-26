/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/


#pragma once
#include "Shader_Pixel.h"
#include "ShaderCode_YUV422.h"

class CShader_YUV422 :
	public CPixelShader
{
public:

	CShader_YUV422( HRESULT &hr, LPDIRECT3DDEVICE9 pDevice )
	{
		do
		{
			if ( !CreateShader ( pDevice, g_YUV422ps30_main, g_YUV422ps20_main ) )
			{
				break ;
			}

			m_TextureHandle[0] = m_pPixelConstTable->GetConstantByName ( NULL, "YUVTextue" ) ;
			if ( m_TextureHandle[0] == NULL )
			{
				g_pDebug->Log_Error ( _T("m_pPixelConstTable->GetConstantByName YUVTextue failed!\n") ) ;
				break ;
			}
			m_TextureHandle[1] = m_pPixelConstTable->GetConstantByName ( NULL, "ParityTextue" ) ;
			if ( m_TextureHandle[1] == NULL )
			{
				g_pDebug->Log_Error ( _T("m_pPixelConstTable->GetConstantByName ParityTextue failed!\n") ) ;
				break ;
			}

			UINT count ;
			hr = m_pPixelConstTable->GetConstantDesc ( m_TextureHandle[0], &m_TextureDesc[0], &count ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pPixelConstTable->GetConstantDesc failed!\nhr=%x"), hr ) ;
				break ;
			}
			hr = m_pPixelConstTable->GetConstantDesc ( m_TextureHandle[1], &m_TextureDesc[1], &count ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pPixelConstTable->GetConstantDesc failed!\nhr=%x"), hr ) ;
				break ;
			}

			m_YUVdxHandle = m_pPixelConstTable->GetConstantByName ( NULL, "YUV_dx" ) ;
			if ( m_YUVdxHandle == NULL )
			{
				g_pDebug->Log_Error ( _T("m_pPixelConstTable->GetConstantByName YUV_dx failed!\nhr=%x"), hr ) ;
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

	~CShader_YUV422(void)
	{
	}

public:
	D3DXHANDLE m_TextureHandle[2] ;
	D3DXHANDLE m_YUVdxHandle ;
	D3DXCONSTANT_DESC m_TextureDesc[2] ;

	D3DXHANDLE m_transparentHandle ;
};
