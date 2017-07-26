/*
	Author: dengzikun

	http://hi.csdn.net/dengzikun

	ע�⣺�ڱ���������Ϣ�ͳ������ӵ�ǰ���£����������⸴�ơ��޸ġ��������ļ���

*/

#pragma once

// NOTE
//
// 1. ���к�������ֻ֧�ֵ��̣߳���֧�ֶ��̡߳�
// 2. ����D3D9.0c SDK�ĵ�˵������ģ�����к����������Ƶ��ʾ���ڵ���Ϣ�����߳��е��á�
// 3. һ������ֻ�ܴ���һ����Ⱦģ��ʵ����һ����Ⱦģ��ʵ���ɴ����������(�����ʹ���һһ��Ӧ)��
//	  ��һ�������п���ͬʱ��Ⱦ���ͼ��

// 4. ʹ��Ĭ���Կ�����֧�ֶ��Կ���
// 5. ��֧��D3Dȫ��ģʽ��
// 6. windows XP SP2��VC++ 2005 �� D3D9.0c SDK ���룬NVIDIA GeForce 9800 GT�Կ��ϲ���ͨ����



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
		GS_UPPER_DOWN_FLIP = 1,    // ͼ�����¾���
		GS_LEFT_RIGHT_FLIP = 2     // ͼ�����Ҿ���
	} ;

	enum IMAGE_FILEFORMAT
	{
		IFF_BMP = 0,
		IFF_JPG = 1,
		IFF_PNG = 2,
		IFF_DIB = 3
	} ;

	typedef void *HCANVAS ; // �������
	typedef void *HIMAGE ;  // ��Ƶͼ����
	typedef void *HPICTURE ;  // ��̬ͼ����
	typedef void *HTEXT ;   // �ı����

public:

	// CreateCanvas�������λ�����
	// ���������
	// hMainWnd:			��Ƶ��Ⱦ�����ھ��(���л�������һ�������ھ��)��
	// lWidth:				������ȣ���λ�����ء�
	// lHeight:				�����߶ȣ���λ�����ء�
	// dwBackBufferCount:   �����󱸻�����������
	
	// ����������ޡ�
	// ����ֵ���������HCANVAS��������NULL����������ʧ�ܡ�
	// ��һ�ε���CreateCanvas�����Ļ���Ϊȱʡ���������ܵ���DestroyCanvas����ȱʡ������
	//
	// NOTE: ����ȱʡ��������ܴ���������Դ��
	virtual HCANVAS CreateCanvas ( HWND hMainWnd, long lWidth, long lHeight, DWORD dwBackBufferCount = 2 ) = 0 ;

	// DestroyCanvas���ٻ�����
	// ���������
	// hCanvas: �������(��������ȱʡ����)��
	// ����������ޡ�
	virtual bool DestroyCanvas ( HCANVAS hCanvas ) = 0 ;

	// SaveCanvas���滭��Ϊͼ���ļ���
	// ���������
	// hCanvas: �������, ��ΪNULL�򱣴����档
	// FileFormat: ͼ���ļ���ʽ(bmp, jpg, png, dib)��
	// pSrcRect: ͼ�񱣴�����NULLΪ����ͼ������
	// ����������ޡ�
	virtual bool SaveCanvas ( HCANVAS hCanvas, const char *pFileName, IMAGE_FILEFORMAT FileFormat, RECT *pSrcRect ) = 0 ;

	// CreateImage������Ƶͼ��
	// ���������
	// lWidth:  ��Ƶͼ���ȣ���λ�����ء�
	// lHeight: ��Ƶͼ��߶ȣ���λ�����ء�
	// ColorSpace: ��Ƶͼ����ɫ�ռ䡣
	// DstRect:	��Ƶͼ����ʾ����(�ڻ����е���ʾ����)��

	// ����������ޡ�
	// ����ֵ����Ƶͼ����HIMAGE��������NULL����������ʧ�ܡ�
	virtual HIMAGE CreateImage ( long lWidth, long lHeight, COLOR_SPACE ColorSpace, RECT *DstRect ) = 0 ;

	// DestroyImage������Ƶͼ��
	// ���������
	// hImage: ��Ƶͼ������
	virtual bool DestroyImage ( HIMAGE hImage ) = 0 ;


	// CreatePicture������̬ͼ��
	// ���������
	// pSrcFile: ��̬ͼ���ļ�����
	// DstRect:	 ��̬ͼ����ʾ����(�ڻ����е���ʾ����)��

	// ����������ޡ�
	// ����ֵ����̬ͼ����HPICTURE��������NULL����������ʧ�ܡ�
	virtual HPICTURE CreatePicture ( const char *pSrcFile, RECT *DstRect ) = 0 ;

	// DestroyImage���پ�̬ͼ��
	// ���������
	// hImage: ��̬ͼ������
	virtual bool DestroyPicture ( HPICTURE hPicture ) = 0 ;

	// CreateOverlayText���������ı�, ֧�����ġ�
	// ���������
	// strText: �����ı����ݡ�
	// logfont: �����ı����塣

	// �����������
	// ����ֵ: �ı����HTEXT��������NULL����������ʧ�ܡ�
	virtual HTEXT CreateOverlayText ( const char *strText, LOGFONT *logfont ) = 0 ;

	// DestroyOverlayText���ٵ����ı���
	// ���������
	// hText: �����ı������
	virtual bool DestroyOverlayText ( HTEXT hText ) = 0 ;

	// ���������
	// hText: �����ı������

	// ���������
	// lWidth:  �����ı���ʵ�ʿ�ȣ���λ�����ء�
	// lHeight: �����ı���ʵ�ʸ߶ȣ���λ�����ء�
	virtual bool GetOverlayTextWH ( HTEXT hText, long *lWidth, long *lHeight ) = 0 ;

	// CreateDynamicFont�������ݶ�̬�仯�����֡�Ӣ���ַ��ı�����Ҫ������ʾ�仯�����֣���֧�����ġ�
	// ���������
	// strFongName: �������ơ�
	// dwHeight: ����߶ȡ�
	virtual bool CreateDynamicFont ( const char* strFontName, DWORD dwHeight ) = 0 ;

	// DestroyDynamicFont���ٶ�̬�ı���
	virtual bool DestroyDynamicFont ( void ) = 0 ;

	// ColorFill�����Ƶͼ��Ϊָ����ɫ��
	// ���������
	// hImage: ��Ƶͼ������
	// r g b  : RGB COLOR��
	virtual bool ColorFill ( HIMAGE hImage, BYTE r, BYTE g, BYTE b ) = 0 ;

	// HandleDeviceLost����D3D�豸��ʧ
	virtual bool HandleDeviceLost( void ) = 0 ;

	// UpdateImage������Ƶͼ��
	// ���������
	// hImage: ��Ƶͼ������
	// pData: �������ݻ�����ָ�롣
	virtual bool UpdateImage ( HIMAGE hImage, BYTE *pData ) = 0 ;

	// BeginRender��ʼD3D��Ⱦ����Ⱦ����hCanvas��
	// ���������
	// hCanvas�������������ΪNULL����ʹ��ȱʡ����(��һ�ε���CreateCanvas�����Ļ���)��
	// bClearBackBuffer: �Ƿ���ջ���(����ջ��������������)��
	// dwBKColor: ��ջ���Ϊָ����ɫ��[0Xargb]
	virtual bool BeginRender ( HCANVAS hCanvas = NULL, bool bClearBackBuffer = false, DWORD dwBKColor = 0xff0000ff ) = 0 ;

	// DrawImage��Ⱦ��Ƶͼ��
	// ���������
	// hImage: ��Ƶͼ������
	// Transparent: ͸����(0.0: ��ȫ͸����1.0: ��ȫ��͸��)
	// Transformation:  ��Ƶͼ�񼸺α任(GEOMETRIC_TRANSFORMATION��һ����������)��
	// pDstRect:   ��Ƶͼ���ڻ����е���ʾ����ȱʡΪNULL����������������ʾͼ��
	// pSrcRect:   ѡȡ��Ƶͼ���һ����������ʾ��ȱʡΪNULL��ѡȡ����ͼ������ʾ��
	virtual bool DrawImage ( HIMAGE hImage, float Transparent = 1.0f, GEOMETRIC_TRANSFORMATION Transformation = GS_NONE, RECT *pDstRect = NULL, RECT *pSrcRect = NULL ) = 0 ;


	// DrawPicture��Ⱦ��̬ͼ��
	// ���������
	// hPicture: ��̬ͼ������
	// Transparent: ͸����(0.0: ��ȫ͸����1.0: ��ȫ��͸��)
	// Transformation:  ��̬ͼ�񼸺α任(GEOMETRIC_TRANSFORMATION��һ����������)��
	// pDstRect:   ��̬ͼ���ڻ����е���ʾ����ȱʡΪNULL����������������ʾͼ��
	// pSrcRect:   ѡȡ��̬ͼ���һ����������ʾ��ȱʡΪNULL��ѡȡ����ͼ������ʾ��
	virtual bool DrawPicture ( HPICTURE hPicture, float Transparent = 1.0f, GEOMETRIC_TRANSFORMATION Transformation = GS_NONE, RECT *pDstRect = NULL, RECT *pSrcRect = NULL ) = 0 ;


	// DrawText��Ⱦ�����ı���
	// ���������
	// hText: �ı������
	// dwColor: �ı���ɫ��[0Xargb]
	// DstRect: �ı���ʾ����(�ڻ����е���ʾ����)��
	virtual bool DrawOverlayText ( HTEXT hText, DWORD dwColor, RECT *DstRect ) = 0 ;

	// DrawDynamicText��Ⱦ��̬�ı���
	// ���������
	// x,y: ��̬�ı���Ⱦ����(���Ͻ�)��
	// dwColor: ��̬�ı���ɫ��[0Xargb]
	// strText: ��̬�ı���
	virtual bool DrawDynamicText ( long x, long y, DWORD dwColor, const char* strText ) = 0 ;
	
	// EndRender����D3D��Ⱦ����������Ⱦ������hDestWindow��
	// ��hDestWindowΪNULL������Ⱦ��CreateCanvas�е�������hMainWnd��
	virtual bool EndRender ( HWND hDestWindow = NULL ) = 0 ;
} ;


#if defined(__cplusplus)
extern "C" 
{
#endif

namespace D3D_VIDEO_RENDER
{
	/*****************************************************************************/
	//	���ܣ�	������Ƶ��Ⱦģ�顣
	//
	//	����ԭ�ͣ�
	//
	/**/	ID3DVRInterface* D3D_Video_Render_Create () ;
	//
	//	������
	//
	//		���룺void��
	//
	//		������ޡ�
	//              
	//	����ֵ��ָ����Ƶ��Ⱦģ��ӿڵ�ָ��,��ΪNULL��ʾʧ�ܡ�
	//
	//	��ע��һ������ֻ�ܴ���һ����Ⱦģ��ʵ����
	/*****************************************************************************/

	/*****************************************************************************/
	//	���ܣ�	������Ƶ��Ⱦģ�顣
	//
	//	����ԭ�ͣ�
	//
	/**/	void D3D_Video_Render_Destroy () ;
	//
	//	������
	//
	//		���룺�ޡ�
	//
	//		������ޡ�
	//              
	//	����ֵ���ޡ�
	//
	//	��ע��
	/*****************************************************************************/
}

#if defined(__cplusplus)
}
#endif