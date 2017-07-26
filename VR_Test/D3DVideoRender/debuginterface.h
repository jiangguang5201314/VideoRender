/*
	Author: dengzikun

	http://

	ע�⣺�ڱ���������Ϣ�ͳ������ӵ�ǰ���£����������⸴�ơ��޸ġ��������ļ���

*/


#pragma once

class IDebug
{
public:
	virtual void Log_Error( const TCHAR *pszFormat, ... ) = 0 ;
	virtual void Log_Warning( const TCHAR *pszFormat, ... ) = 0 ;
	virtual void Log_Info( const TCHAR *pszFormat, ... ) = 0 ;
	virtual void Log_LastErrorCode ( DWORD dwErrorCode, const TCHAR *pContext ) = 0 ;
	virtual void SetDebug ( bool Debug ) = 0 ;

public:
	virtual ~IDebug(){}
} ;


