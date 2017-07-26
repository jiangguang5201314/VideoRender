#pragma once

#include "xvid.h"
#include "vfw.h"

class CXvidDecoder
{
private:
	UINT m_Width;
	UINT m_Height;
	void * dhandle;

	UINT VideoSize;
	UINT dstVideoSize;
	BYTE *DecBuf;

	HINSTANCE m_hdll;
	int (*xvid_global_func)(void *handle, int opt, void *param1, void *param2);
	int (*xvid_decore_func)(void *handle, int opt, void *param1, void *param2);

	xvid_gbl_init_t init;
	xvid_dec_create_t create;
	xvid_dec_frame_t frame;

	UINT m_dstw;
	UINT m_dstH;
	void initDecoder()
	{
		VideoSize=m_Width*m_Height*3/2;
		dhandle=NULL;

		m_hdll = LoadLibrary("xvidcore.dll");
		if (m_hdll==NULL)
			return;

		xvid_global_func =(int (__cdecl *)(void *,int,void *,void *))GetProcAddress(m_hdll, "xvid_global");
		xvid_decore_func = (int (__cdecl *)(void *, int, void *, void *))GetProcAddress(m_hdll, "xvid_decore");

		memset(&init, 0, sizeof(init));
		init.version = XVID_VERSION;
		xvid_global_func(0, XVID_GBL_INIT, &init, NULL);

		memset(&create, 0, sizeof(create));
		create.version = XVID_VERSION;
		create.width = m_Width;
		create.height = m_Height;

		xvid_decore_func(0, XVID_DEC_CREATE, &create, NULL);
		dhandle = create.handle;

		if (dhandle==NULL)
			return;

		memset(&frame, 0, sizeof(frame));
		frame.version = XVID_VERSION;
		frame.general = XVID_LOWDELAY;	/* force low_delay_default mode */

		frame.output.csp=XVID_CSP_YV12;
		frame.output.stride[0] = (8 * m_Width + 7) / 8;

		frame.general |= XVID_DEBLOCKY;
		frame.brightness = 0;
	}

public:
	CXvidDecoder()
	{
		m_Width=176;
		m_Height=144;
		initDecoder();
	}

	CXvidDecoder(UINT width,UINT height)
	{
		m_Width=width;
		m_Height=height;
		initDecoder();
	}
	virtual	~CXvidDecoder()
	{
		release();
	}
	
	DWORD Decode(LPVOID src,UINT srclen,LPVOID dst)
	{
		int  result;

		if (dhandle == NULL) 
			return (DWORD)-1;

		frame.bitstream = src;
		frame.output.plane[0] = dst;
		frame.length = srclen;

		result=xvid_decore_func(dhandle, XVID_DEC_DECODE, &frame, NULL);

		if (result>=0)
		{
			return ICERR_OK;
		}

		return (DWORD)result;
	}
	
	void release()
	{
		if (dhandle)
		xvid_decore_func(dhandle, XVID_DEC_DESTROY, NULL, NULL);
		dhandle = NULL;

		if (m_hdll)
			FreeLibrary(m_hdll);
		m_hdll=NULL;
	}
};