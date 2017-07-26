/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/


#pragma once
#include "Shader_Pixel.h"
#include "ShaderCode_RGB24.h"

class CShader_RGB24 :
	public CPixelShader
{
public:

	CShader_RGB24( HRESULT &hr, LPDIRECT3DDEVICE9 pDevice )
	{
		do
		{
			if ( !CreateShader ( pDevice, g_RGBps30_main, g_RGBps20_main ) )
			{
				break ;
			}

			m_TextureHandle = m_pPixelConstTable->GetConstantByName ( NULL, "ColorTextue" ) ;
			if ( m_TextureHandle == NULL )
			{
				g_pDebug->Log_Error ( _T("m_pPixelConstTable->GetConstantByName ColorTextue failed!\n") ) ;
				break ;
			}

			UINT count ;
			hr = m_pPixelConstTable->GetConstantDesc ( m_TextureHandle, &m_TextureDesc, &count ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pPixelConstTable->GetConstantDesc failed!\nhr=%x"), hr ) ;
				break ;
			}

			m_RGBdxHandle = m_pPixelConstTable->GetConstantByName ( NULL, "RGB_dx" ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pPixelConstTable->GetConstantByName RGB_dx failed!\nhr=%x"), hr ) ;
				break ;
			}
		
			hr = m_pPixelConstTable->SetDefaults ( pDevice ) ;

			hr = S_OK ;
			return ;

		} while ( false ) ;
		
		hr = E_FAIL ;
	}
public:

	~CShader_RGB24(void)
	{
	}

public:
	D3DXHANDLE m_TextureHandle ;
	D3DXHANDLE m_RGBdxHandle ;
	D3DXCONSTANT_DESC m_TextureDesc ;
};
