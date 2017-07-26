/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/


#pragma once

#include <crtdbg.h>
#include <d3dx9.h>
#include "CommonFunctions.h"

extern IDebug *g_pDebug ;

class CPixelShader
{
public:

	CPixelShader(void)
		: m_pPixelShader(NULL)
		, m_pPixelConstTable(NULL)
	{
	}
public:

	virtual ~CPixelShader(void)
	{
		SAFE_RELEASE ( m_pPixelShader ) ;
		SAFE_RELEASE ( m_pPixelConstTable ) ;
	}

protected:
	bool CreateShader ( LPDIRECT3DDEVICE9 pDevice, const DWORD *pCode3, const DWORD *pCode2 )
	{
		D3DCAPS9 caps ;
		HRESULT hr = S_OK ;

		do
		{
			pDevice->GetDeviceCaps ( &caps ) ;
			if ( caps.PixelShaderVersion < D3DPS_VERSION(2, 0) )
			{
				g_pDebug->Log_Error ( _T("caps.PixelShaderVersion < 2.0 !\nhr=%x"), hr ) ;
				break ;
			}

			const DWORD *pCode ;
			if ( caps.PixelShaderVersion >= D3DPS_VERSION(3, 0) )
			{
				pCode = pCode3 ;
			}
			else
			{
				pCode = pCode2 ;
			}

			hr = pDevice->CreatePixelShader ( pCode, &m_pPixelShader ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("m_pDevice->CreatePixelShader failed!\nhr=%x"), hr ) ;
				break ;
			}
			hr = D3DXGetShaderConstantTable ( pCode, &m_pPixelConstTable ) ;
			if ( FAILED(hr) )
			{
				g_pDebug->Log_Error ( _T("D3DXGetShaderConstantTable failed!\nhr=%x"), hr ) ;
				break ;
			}

			return true ;

		} while ( false ) ;

		SAFE_RELEASE ( m_pPixelShader ) ;
		SAFE_RELEASE ( m_pPixelConstTable ) ;

		return false ;
	}

public:
	LPDIRECT3DPIXELSHADER9 m_pPixelShader ;
	ID3DXConstantTable* m_pPixelConstTable ;
};
