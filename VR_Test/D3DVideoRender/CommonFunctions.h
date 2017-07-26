/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/

#pragma once

template<class T> void SAFE_RELEASE( T &t )
{
	if ( t != NULL )
	{
		t->Release () ;
		t = NULL ;
	}
}

template<class T> void SAFE_DELETE( T &t )
{
	delete t ;
	t = NULL ;
}

template<class T> void SAFE_DELETEARRAY( T &t )
{
	delete [] t ;
	t = NULL ;
}

template<class T> void Init_Array( T *t, int count, T value )
{
	for ( int i = 0 ; i < count ; i++ )
	{
		t[i] = value ;
	}
}

extern "C" void __cdecl memcpyMMX(void *Dest, void *Src, size_t nBytes) ;
