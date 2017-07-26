/*
	Author: dengzikun

	http://

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/



#pragma once
#include "debuginterface.h"
#include "LogMessage.h"
#include "tchar.h"
#include <stdio.h>

class CDebugClass : public IDebug 
{
	CDebugClass ( const CDebugClass &refDebug ) ;
	CDebugClass &operator= ( const CDebugClass &refDebug ) ;

public:
	void Log_Error( const TCHAR *pszFormat, ... )
	{
		TCHAR buf[1024] ;
		va_list arglist ;

		try
		{
			_tcscpy_s ( buf, 1023, _T("\n") ) ;
			va_start(arglist, pszFormat) ;
			_vstprintf_s(&buf[_tcslen(buf)], 1023-_tcslen(buf), pszFormat, arglist) ;
			va_end(arglist) ;

			if ( !m_Debug )
			{
				Log_Event ( SYSTEM_STATUS_ERROR, EVENTLOG_ERROR_TYPE, buf ) ;
			}
			else
			{
				_tprintf ( buf ) ;
			}
		}
		catch ( ... ){}
	}

	void Log_Warning( const TCHAR *pszFormat, ... )
	{
		TCHAR buf[1024] ;
		va_list arglist ;

		try
		{
			_tcscpy_s ( buf, 1023, _T("\n") ) ;
			va_start(arglist, pszFormat) ;
			_vstprintf_s(&buf[_tcslen(buf)], 1023-_tcslen(buf), pszFormat, arglist) ;
			va_end(arglist) ;
			if ( !m_Debug )
			{
				Log_Event ( SYSTEM_STATUS_WARNING, EVENTLOG_WARNING_TYPE, buf ) ;
			}
			else
			{
				_tprintf ( buf ) ;
			}
		}
		catch ( ... ){}
	}

	void Log_Info( const TCHAR *pszFormat, ... )
	{
		TCHAR buf[1024] ;
		va_list arglist ;

		try
		{
			_tcscpy_s ( buf, 1023, _T("\n") ) ;
			va_start(arglist, pszFormat) ;
			_vstprintf_s(&buf[_tcslen(buf)], 1023-_tcslen(buf), pszFormat, arglist) ;
			va_end(arglist) ;
			if ( !m_Debug )
			{
				Log_Event ( SYSTEM_STATUS_OK, EVENTLOG_INFORMATION_TYPE, buf ) ;
			}
			else
			{
				_tprintf ( buf ) ;
			}
		}
		catch ( ... ){}
	}

	void Log_LastErrorCode ( DWORD dwErrorCode, const TCHAR *pContext )
	{
		LPTSTR lpMsgBuf ;

		try
		{
			DWORD dwLen = FormatMessage ( FORMAT_MESSAGE_ALLOCATE_BUFFER | 
											FORMAT_MESSAGE_FROM_SYSTEM,
											NULL,
											dwErrorCode,
											MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
											(LPTSTR) &lpMsgBuf,
											0, NULL );

			if ( dwLen > 0 )
			{
				if ( !m_Debug )
				{
					Log_Event (  SYSTEM_STATUS_ERROR, EVENTLOG_ERROR_TYPE, _T("%s: %sLastErrorCode: %d"), pContext, lpMsgBuf, dwErrorCode ) ;
				}
				else
				{
					_tprintf ( _T("\n%s:%sLastErrorCode: %d"), pContext, lpMsgBuf, dwErrorCode ) ;
				}

				LocalFree ( lpMsgBuf ) ;
			}
		}
		catch ( ... ){}
	}
	
	virtual void SetDebug ( bool Debug )
	{
		m_Debug = Debug ;
	}

	CDebugClass ( TCHAR *LogName, TCHAR *SourceName, TCHAR *MessageDllName )
		: m_Debug(false)
		, m_bInit(false)
	{
		try
		{
			TCHAR ProgramName[1024] ;
			if ( MessageDllName == NULL )
			{	
				if ( !GetModuleFileName ( NULL, ProgramName, 1024 - 1 ) )
				{
					_tcscpy_s ( ProgramName, _T("ProgramName") ) ;
				}
			}
			else
			{
				_tcscpy_s ( ProgramName, MessageDllName ) ;
			}

			if ( CustomLog ( LogName, SourceName, ProgramName ) )
			{
				m_bInit = true ;
			}
		}
		catch ( ... )
		{
		}
	}

private:

	void Log_Event ( DWORD dwEventId, WORD wType, const TCHAR *pszFormat, ... )
	{
		TCHAR buf[1024] ;
		_tcscpy_s ( buf, 1023, _T("\n") ) ;
		va_list arglist ;
		va_start(arglist, pszFormat) ;
		_vstprintf_s(&buf[_tcslen(buf)], 1023-_tcslen(buf), pszFormat, arglist) ;
		va_end(arglist) ;
		_tcscat_s(&buf[_tcslen(buf)], 1022-_tcslen(buf), _T("")) ;
			
		Log_Event ( dwEventId, wType, buf ) ;
	}

	void Log_Event ( DWORD dwEventId, WORD wType, TCHAR *pStr )
	{
		TCHAR* pBuf[1] ;
		pBuf[0] = pStr ;
				
		if ( !m_bInit )
		{
			_tcscpy_s ( m_SourceName, _T("TEST_FAIL") ) ;
			dwEventId = 0 ;
		}

		HANDLE hEventSource = RegisterEventSource ( NULL, m_SourceName ) ;
		if ( hEventSource != NULL )
		{
			ReportEvent ( hEventSource, // handle of event source
				wType,  // event type
				0,                    // event category
				dwEventId,                    // event ID
				NULL,                 // current user's SID
				1,                    // strings in lpszStrings
				0,                    // no bytes of raw data
				(LPCTSTR*)pBuf,  // array of error strings
				NULL ) ;              // no raw data

			DeregisterEventSource ( hEventSource ) ;
		}
	}

	bool CustomLog( TCHAR *logName, TCHAR *sourceName, TCHAR *MessageDllName )
	{
		// This number of categories for the event source.
		DWORD dwCategoryNum = 1;

		HKEY hk; 
		DWORD dwData, dwDisp; 
		TCHAR szBuf[MAX_PATH]; 
		size_t cchSize = MAX_PATH;

		// Create the event source as a subkey of the log.

		HRESULT hr = _stprintf_s(szBuf,  //
			_T("SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s\\%s"),
			logName, sourceName); 

		if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, szBuf, 
			0, NULL, REG_OPTION_NON_VOLATILE,
			KEY_WRITE, NULL, &hk, &dwDisp)) 
		{
			return false ;
		}

		// Set the name of the message file. 

		if (RegSetValueEx(hk,             // subkey handle 
			_T("EventMessageFile"),        // value name 
			0,                         // must be zero 
			REG_EXPAND_SZ,             // value type 
			(LPBYTE) MessageDllName,          // pointer to value data 
			(DWORD) (lstrlen(MessageDllName)+1)*sizeof(TCHAR))) // data size
		{
			RegCloseKey(hk); 
			return false ;
		}

		// Set the supported event types. 

		dwData = EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | 
			EVENTLOG_INFORMATION_TYPE; 

		if (RegSetValueEx(hk,      // subkey handle 
			_T("TypesSupported"),  // value name 
			0,                 // must be zero 
			REG_DWORD,         // value type 
			(LPBYTE) &dwData,  // pointer to value data 
			sizeof(DWORD)))    // length of value data 
		{
			RegCloseKey(hk); 
			return false ;
		}

		// Set the category message file and number of categories.

		if (RegSetValueEx(hk,              // subkey handle 
			_T("CategoryMessageFile"),     // value name 
			0,                         // must be zero 
			REG_EXPAND_SZ,             // value type 
			(LPBYTE) MessageDllName,          // pointer to value data 
			(DWORD) (lstrlen(MessageDllName)+1)*sizeof(TCHAR))) // data size
		{
			RegCloseKey(hk); 
			return false ;
		}

		if (RegSetValueEx(hk,            // subkey handle 
			_T("CategoryCount"),         // value name 
			0,                       // must be zero 
			REG_DWORD,               // value type 
			(LPBYTE) &dwCategoryNum, // pointer to value data 
			sizeof(DWORD)))          // length of value data 
		{
			RegCloseKey(hk);
			return false ;
		}

		RegCloseKey(hk);

		_tcscpy_s ( m_SourceName, sourceName ) ;


		return true ;
	}

private:
	bool m_Debug ;
	bool m_bInit ;
	TCHAR m_SourceName[256] ;
} ;


class CDummyDebug : public IDebug
{
	public:
		virtual void Log_Error( const TCHAR *pszFormat, ... ){}
		virtual void Log_Warning( const TCHAR *pszFormat, ... ){}
		virtual void Log_Info( const TCHAR *pszFormat, ... ){}
		virtual void Log_LastErrorCode ( DWORD dwErrorCode, const TCHAR *pContext ){}
		virtual void SetDebug ( bool Debug ){}
} ;