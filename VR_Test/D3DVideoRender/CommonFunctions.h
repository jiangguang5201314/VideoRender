/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	ע�⣺�ڱ���������Ϣ�ͳ������ӵ�ǰ���£����������⸴�ơ��޸ġ��������ļ���

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
