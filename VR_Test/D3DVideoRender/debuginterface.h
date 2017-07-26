/*
	Author: dengzikun

	http://

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

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


