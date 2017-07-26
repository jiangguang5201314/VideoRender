/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	注意：在保留作者信息和出处链接的前提下，您可以任意复制、修改、传播本文件。

*/

#pragma once

// NOTE
//
// 1. 所有函数调用只支持单线程，不支持多线程。
// 2. 根据D3D9.0c SDK文档说明，本模块所有函数最好在视频显示窗口的消息处理线程中调用。
// 3. 一个进程只能创建一个渲染模块实例，一个渲染模块实例可创建多个画布(画布和窗口一一对应)，
//	  在一个画布中可以同时渲染多个图像。

// 4. 使用默认显卡，不支持多显卡。
// 5. 不支持D3D全屏模式。
// 6. windows XP SP2，VC++ 2005 ＋ D3D9.0c SDK 编译，NVIDIA GeForce 9800 GT显卡上测试通过。



class ID3DVRInterface
{

public:
	enum COLOR_SPACE
	{
		CS_YV12		= 0, // YVU420
		CS_I420		= 1, // YUV420
		CS_YUY2		= 2, // YUYV
		CS_UYVY		= 3, // UYVY 
		CS_NV12		= 4, // NV12
		CS_RGB24	= 5, // rgb rgb ...
		CS_BGR24	= 6, // bgr bgr ...
		CS_RGB16	= 7, // R5G6B5
		CS_RGB15	= 8, // X1R5G5B5
		CS_RGB32	= 9, // bgra bgra ...
		CS_UNKNOWN  = 0xffffffff
	} ;

	enum GEOMETRIC_TRANSFORMATION
	{
		GS_NONE = 0,
		GS_UPPER_DOWN_FLIP = 1,    // 图像上下镜像
		GS_LEFT_RIGHT_FLIP = 2     // 图像左右镜像
	} ;

	enum IMAGE_FILEFORMAT
	{
		IFF_BMP = 0,
		IFF_JPG = 1,
		IFF_PNG = 2,
		IFF_DIB = 3
	} ;

	typedef void *HCANVAS ; // 画布句柄
	typedef void *HIMAGE ;  // 视频图像句柄
	typedef void *HPICTURE ;  // 静态图像句柄
	typedef void *HTEXT ;   // 文本句柄

public:

	// CreateCanvas创建矩形画布。
	// 输入参数：
	// hMainWnd:			视频渲染主窗口句柄(所有画布共用一个主窗口句柄)。
	// lWidth:				画布宽度，单位：像素。
	// lHeight:				画布高度，单位：像素。
	// dwBackBufferCount:   画布后备缓冲区个数。
	
	// 输出参数：无。
	// 返回值：画布句柄HCANVAS。若返回NULL，则函数调用失败。
	// 第一次调用CreateCanvas创建的画布为缺省画布，不能调用DestroyCanvas销毁缺省画布。
	//
	// NOTE: 创建缺省画布后才能创建其他资源。
	virtual HCANVAS CreateCanvas ( HWND hMainWnd, long lWidth, long lHeight, DWORD dwBackBufferCount = 2 ) = 0 ;

	// DestroyCanvas销毁画布。
	// 输入参数：
	// hCanvas: 画布句柄(不能销毁缺省画布)。
	// 输出参数：无。
	virtual bool DestroyCanvas ( HCANVAS hCanvas ) = 0 ;

	// SaveCanvas保存画布为图像文件。
	// 输入参数：
	// hCanvas: 画布句柄, 若为NULL则保存桌面。
	// FileFormat: 图像文件格式(bmp, jpg, png, dib)。
	// pSrcRect: 图像保存区域，NULL为整个图像区域。
	// 输出参数：无。
	virtual bool SaveCanvas ( HCANVAS hCanvas, const char *pFileName, IMAGE_FILEFORMAT FileFormat, RECT *pSrcRect ) = 0 ;

	// CreateImage创建视频图像。
	// 输入参数：
	// lWidth:  视频图像宽度，单位：像素。
	// lHeight: 视频图像高度，单位：像素。
	// ColorSpace: 视频图像颜色空间。
	// DstRect:	视频图像显示区域(在画布中的显示区域)。

	// 输出参数：无。
	// 返回值：视频图像句柄HIMAGE。若返回NULL，则函数调用失败。
	virtual HIMAGE CreateImage ( long lWidth, long lHeight, COLOR_SPACE ColorSpace, RECT *DstRect ) = 0 ;

	// DestroyImage销毁视频图像。
	// 输入参数：
	// hImage: 视频图像句柄。
	virtual bool DestroyImage ( HIMAGE hImage ) = 0 ;


	// CreatePicture创建静态图像。
	// 输入参数：
	// pSrcFile: 静态图像文件名。
	// DstRect:	 静态图像显示区域(在画布中的显示区域)。

	// 输出参数：无。
	// 返回值：静态图像句柄HPICTURE。若返回NULL，则函数调用失败。
	virtual HPICTURE CreatePicture ( const char *pSrcFile, RECT *DstRect ) = 0 ;

	// DestroyImage销毁静态图像。
	// 输入参数：
	// hImage: 静态图像句柄。
	virtual bool DestroyPicture ( HPICTURE hPicture ) = 0 ;

	// CreateOverlayText创建叠加文本, 支持中文。
	// 输入参数：
	// strText: 叠加文本内容。
	// logfont: 叠加文本字体。

	// 输出参数：无
	// 返回值: 文本句柄HTEXT。若返回NULL，则函数调用失败。
	virtual HTEXT CreateOverlayText ( const char *strText, LOGFONT *logfont ) = 0 ;

	// DestroyOverlayText销毁叠加文本。
	// 输入参数：
	// hText: 叠加文本句柄。
	virtual bool DestroyOverlayText ( HTEXT hText ) = 0 ;

	// 输入参数：
	// hText: 叠加文本句柄。

	// 输出参数：
	// lWidth:  叠加文本的实际宽度，单位：像素。
	// lHeight: 叠加文本的实际高度，单位：像素。
	virtual bool GetOverlayTextWH ( HTEXT hText, long *lWidth, long *lHeight ) = 0 ;

	// CreateDynamicFont创建内容动态变化的数字、英文字符文本，主要用来显示变化的数字，不支持中文。
	// 输入参数：
	// strFongName: 字体名称。
	// dwHeight: 字体高度。
	virtual bool CreateDynamicFont ( const char* strFontName, DWORD dwHeight ) = 0 ;

	// DestroyDynamicFont销毁动态文本。
	virtual bool DestroyDynamicFont ( void ) = 0 ;

	// ColorFill填充视频图像为指定颜色。
	// 输入参数：
	// hImage: 视频图像句柄。
	// r g b  : RGB COLOR。
	virtual bool ColorFill ( HIMAGE hImage, BYTE r, BYTE g, BYTE b ) = 0 ;

	// HandleDeviceLost处理D3D设备丢失
	virtual bool HandleDeviceLost( void ) = 0 ;

	// UpdateImage更新视频图像。
	// 输入参数：
	// hImage: 视频图像句柄。
	// pData: 更新数据缓冲区指针。
	virtual bool UpdateImage ( HIMAGE hImage, BYTE *pData ) = 0 ;

	// BeginRender开始D3D渲染，渲染画布hCanvas。
	// 输入参数：
	// hCanvas：画布句柄，若为NULL，则使用缺省画布(第一次调用CreateCanvas创建的画布)。
	// bClearBackBuffer: 是否清空画布(不清空画布可以提高性能)。
	// dwBKColor: 清空画布为指定颜色。[0Xargb]
	virtual bool BeginRender ( HCANVAS hCanvas = NULL, bool bClearBackBuffer = false, DWORD dwBKColor = 0xff0000ff ) = 0 ;

	// DrawImage渲染视频图像。
	// 输入参数：
	// hImage: 视频图像句柄。
	// Transparent: 透明度(0.0: 完全透明，1.0: 完全不透明)
	// Transformation:  视频图像几何变换(GEOMETRIC_TRANSFORMATION中一项或多项的组合)。
	// pDstRect:   视频图像在画布中的显示区域。缺省为NULL，在整个画布中显示图像。
	// pSrcRect:   选取视频图像的一个区域做显示。缺省为NULL，选取整个图像做显示。
	virtual bool DrawImage ( HIMAGE hImage, float Transparent = 1.0f, GEOMETRIC_TRANSFORMATION Transformation = GS_NONE, RECT *pDstRect = NULL, RECT *pSrcRect = NULL ) = 0 ;


	// DrawPicture渲染静态图像。
	// 输入参数：
	// hPicture: 静态图像句柄。
	// Transparent: 透明度(0.0: 完全透明，1.0: 完全不透明)
	// Transformation:  静态图像几何变换(GEOMETRIC_TRANSFORMATION中一项或多项的组合)。
	// pDstRect:   静态图像在画布中的显示区域。缺省为NULL，在整个画布中显示图像。
	// pSrcRect:   选取静态图像的一个区域做显示。缺省为NULL，选取整个图像做显示。
	virtual bool DrawPicture ( HPICTURE hPicture, float Transparent = 1.0f, GEOMETRIC_TRANSFORMATION Transformation = GS_NONE, RECT *pDstRect = NULL, RECT *pSrcRect = NULL ) = 0 ;


	// DrawText渲染叠加文本。
	// 输入参数：
	// hText: 文本句柄。
	// dwColor: 文本颜色。[0Xargb]
	// DstRect: 文本显示区域(在画布中的显示区域)。
	virtual bool DrawOverlayText ( HTEXT hText, DWORD dwColor, RECT *DstRect ) = 0 ;

	// DrawDynamicText渲染动态文本。
	// 输入参数：
	// x,y: 动态文本渲染坐标(左上角)。
	// dwColor: 动态文本颜色。[0Xargb]
	// strText: 动态文本。
	virtual bool DrawDynamicText ( long x, long y, DWORD dwColor, const char* strText ) = 0 ;
	
	// EndRender结束D3D渲染，将画布渲染到窗口hDestWindow。
	// 若hDestWindow为NULL，则渲染到CreateCanvas中的主窗口hMainWnd。
	virtual bool EndRender ( HWND hDestWindow = NULL ) = 0 ;
} ;


#if defined(__cplusplus)
extern "C" 
{
#endif

namespace D3D_VIDEO_RENDER
{
	/*****************************************************************************/
	//	功能：	创建视频渲染模块。
	//
	//	函数原型：
	//
	/**/	ID3DVRInterface* D3D_Video_Render_Create () ;
	//
	//	参数：
	//
	//		输入：void。
	//
	//		输出：无。
	//              
	//	返回值：指向视频渲染模块接口的指针,若为NULL表示失败。
	//
	//	备注：一个进程只能创建一个渲染模块实例。
	/*****************************************************************************/

	/*****************************************************************************/
	//	功能：	销毁视频渲染模块。
	//
	//	函数原型：
	//
	/**/	void D3D_Video_Render_Destroy () ;
	//
	//	参数：
	//
	//		输入：无。
	//
	//		输出：无。
	//              
	//	返回值：无。
	//
	//	备注：
	/*****************************************************************************/
}

#if defined(__cplusplus)
}
#endif