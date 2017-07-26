/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/


// D3DVideoRender.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#include "D3DManager.h"
#include "RenderFactory.h"
#include "mydebug.h"
#include "Display_YUV420.h"
#include "Display_YUV422.h"
#include "Display_NV12.h"
#include "Display_RGB24.h"
#include "Display_RGB32.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

HMODULE g_hModule = NULL ;

IDebug *g_pDebug ;
CRenderFactory g_RenderFactory ;

static CDummyDebug DummyDebug ;
static std::list<CD3DManager*> list_D3DManager ;

 
static void Register() ;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			g_hModule = hModule ;
			Register () ;
			break ;
		case DLL_THREAD_ATTACH:
			break ;
		case DLL_THREAD_DETACH:
			break ;
		case DLL_PROCESS_DETACH:
			break;
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif


ID3DVRInterface* D3D_VIDEO_RENDER::D3D_Video_Render_Create ()
{
	if ( g_pDebug == NULL || g_pDebug == &DummyDebug )
	{
		char path[1024+1] ;
		GetModuleFileName ( g_hModule, path, 1024 ) ;
		g_pDebug = new CDebugClass ( _T("D3D Video Render"), _T("D3D Video Render"), path ) ;
	}
	if ( g_pDebug == NULL )
	{
		g_pDebug = &DummyDebug ;
	}

	CD3DManager *p = new CD3DManager () ;
	if ( p != NULL )
	{
		list_D3DManager.push_back ( p ) ;
	}
	return p ;
}

void D3D_VIDEO_RENDER::D3D_Video_Render_Destroy ()
{
	std::list<CD3DManager*>::const_iterator it ;
	for ( it = list_D3DManager.begin() ; it != list_D3DManager.end() ; it++ )
	{
		delete *it ;
	}
	list_D3DManager.clear () ;
}

void Register()
{
	struct REGISTER
	{
		ID3DVRInterface::COLOR_SPACE cs ;
		CRenderFactory::CreateCallback pCallback ;
	} ;

	REGISTER Entries [] = {	{ID3DVRInterface::CS_YV12,  &CDisplay_YUV420::Create},
							{ID3DVRInterface::CS_I420,  &CDisplay_YUV420::Create},		
							{ID3DVRInterface::CS_NV12,  &CDisplay_NV12::Create},
							{ID3DVRInterface::CS_YUY2,  &CDisplay_YUV422::Create},
							{ID3DVRInterface::CS_UYVY,  &CDisplay_YUV422::Create},
							{ID3DVRInterface::CS_RGB24, &CDisplay_RGB24::Create},
							{ID3DVRInterface::CS_BGR24, &CDisplay_RGB24::Create},
							{ID3DVRInterface::CS_RGB32, &CDisplay_RGB32::Create},
							{ID3DVRInterface::CS_RGB16, &CDisplay_RGB32::Create},
							{ID3DVRInterface::CS_RGB15, &CDisplay_RGB32::Create}
						} ;

	for ( int i = 0 ; i < sizeof(Entries) / sizeof(REGISTER) ; i++ )
	{
		g_RenderFactory.RegisterRender ( Entries[i].cs, Entries[i].pCallback ) ;
	}
}