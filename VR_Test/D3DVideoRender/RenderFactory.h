/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	ע�⣺�ڱ���������Ϣ�ͳ������ӵ�ǰ���£����������⸴�ơ��޸ġ��������ļ���

*/


#pragma once

#include "Display.h"
#include <map>

class CRenderFactory
{
public:
	typedef CDisplay *(*CreateCallback)( LPDIRECT3DDEVICE9 pDevice,	CPixelShader **pShader ) ;

public:

	bool RegisterRender ( ID3DVRInterface::COLOR_SPACE cs, CreateCallback Callback )
	{
		return m_Callback.insert ( CallbackMap::value_type(cs,Callback) ).second ;
	}
	CDisplay *CreateRender ( ID3DVRInterface::COLOR_SPACE cs, LPDIRECT3DDEVICE9 pDevice, CPixelShader **pShader )
	{
		CallbackMap::const_iterator it = m_Callback.find ( cs ) ;
		if ( it == m_Callback.end () )
		{
			return NULL ;
		}
		return it->second ( pDevice, pShader ) ;
	}

private:
	typedef std::map<ID3DVRInterface::COLOR_SPACE, CreateCallback> CallbackMap ;
	CallbackMap m_Callback ;
};
