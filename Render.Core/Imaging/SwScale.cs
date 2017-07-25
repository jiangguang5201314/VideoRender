using System;
using System.Runtime.InteropServices;
using System.Security;
using System.Reflection;
using System.IO;
using System.ComponentModel;

namespace Renderer.Core
{
    [SuppressUnmanagedCodeSecurity]
    internal static class SwScale
    {


        const string libraryName = "swscale-4.dll";
        
        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int swscale_version();

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern unsafe int* sws_getCoefficients(ColorSpace colorspace);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int sws_isSupportedInput(SwsPixelFormat pix_fmt);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int sws_isSupportedOutput(SwsPixelFormat pix_fmt);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr sws_alloc_context();

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern int sws_init_context(IntPtr sws_context, IntPtr srcFilter, IntPtr dstFilter);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern void sws_freeContext(IntPtr swsContext);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr sws_getContext(int srcW, int srcH, SwsPixelFormat srcFormat,
                                            int dstW, int dstH, SwsPixelFormat dstFormat,
                                            SwScale.ConvertionFlags flags, IntPtr srcFilter, IntPtr dstFilter, IntPtr param);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr sws_getCachedContext(IntPtr context, int srcW, int srcH, SwsPixelFormat srcFormat,
                                                  int dstW, int dstH, SwsPixelFormat dstFormat,
                                                  SwScale.ConvertionFlags flags, IntPtr srcFilter, IntPtr dstFilter, IntPtr param);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static unsafe extern int sws_scale(IntPtr context, byte** srcSlice, int[] srcStride,
                                           int srcSliceY, int srcSliceH, byte** dst, int[] dstStride);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static unsafe extern int sws_setColorspaceDetails(IntPtr c, int* inv_table,
                             int srcRange, int* table, int dstRange,
                             int brightness, int contrast, int saturation);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static unsafe extern int sws_getColorspaceDetails(IntPtr c, out int* inv_table,
                             out int srcRange, out int* table, out int dstRange,
                             out int brightness, out int contrast, out int saturation);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static unsafe extern SwsVector sws_allocVec(int length);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static unsafe extern SwsVector sws_getGaussianVec(double variance, double quality);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static unsafe extern SwsVector sws_getConstVec(double c, int length);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static unsafe extern SwsVector sws_getIdentityVec();

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static unsafe extern void sws_scaleVec(SwsVector a, double scalar);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static unsafe extern void sws_normalizeVec(SwsVector a, double height);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static unsafe extern void sws_convVec(SwsVector a, SwsVector b);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static unsafe extern void sws_addVec(SwsVector a, SwsVector b);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static unsafe extern void sws_subVec(SwsVector a, SwsVector b);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static unsafe extern void sws_shiftVec(SwsVector a, int shift);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static unsafe extern SwsVector sws_cloneVec(SwsVector* a);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static unsafe extern void sws_freeVec(SwsVector a);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static unsafe extern SwsFilter sws_getDefaultFilter(float lumaGBlur, float chromaGBlur,
                                        float lumaSharpen, float chromaSharpen,
                                        float chromaHShift, float chromaVShift,
                                        int verbose);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static unsafe extern void sws_freeFilter(SwsFilter filter);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static unsafe extern void sws_convertPalette8ToPacked32(byte* src, byte* dst, int num_pixels, byte* palette);

        [DllImport(libraryName, CallingConvention = CallingConvention.Cdecl)]
        public static unsafe extern void sws_convertPalette8ToPacked24(byte* src, byte* dst, int num_pixels, byte* palette);

        [StructLayout(LayoutKind.Sequential)]
        public struct SwsVector
        {
            IntPtr coeff;              ///< pointer to the list of coefficients
            int length;                 ///< number of coefficients in the vector
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct SwsFilter
        {
            SwsVector lumH;
            SwsVector lumV;
            SwsVector chrH;
            SwsVector chrV;
        }

        [Flags]
        public enum ConvertionFlags : int
        {
            SWS_FAST_BILINEAR = 1,
            SWS_BILINEAR = 2,
            SWS_BICUBIC = 4,
            SWS_X = 8,
            SWS_POINT = 0x10,
            SWS_AREA = 0x20,
            SWS_BICUBLIN = 0x40,
            SWS_GAUSS = 0x80,
            SWS_SINC = 0x100,
            SWS_LANCZOS = 0x200,
            SWS_SPLINE = 0x400
        }

        public enum SwsPixelFormat
        {
            PIX_FMT_NONE = -1,
            /// <summary>
            /// planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
            /// </summary>
            PIX_FMT_YUV420P,
            /// <summary>
            /// packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
            /// </summary>
            PIX_FMT_YUYV422,
            /// <summary>
            /// packed RGB 8:8:8, 24bpp, RGBRGB...
            /// </summary>
            PIX_FMT_RGB24, 
            /// <summary>
            /// packed RGB 8:8:8, 24bpp, BGRBGR...
            /// </summary>
            PIX_FMT_BGR24,     
            /// <summary>
            /// planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
            /// </summary>
            PIX_FMT_YUV422P,
            /// <summary>
            /// planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)
            /// </summary>
            PIX_FMT_YUV444P,   
            /// <summary>
            /// planar YUV 4:1:0,  9bpp, (1 Cr & Cb sample per 4x4 Y samples)
            /// </summary>
            PIX_FMT_YUV410P,   
            /// <summary>
            /// planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples)
            /// </summary>
            PIX_FMT_YUV411P,  
            /// <summary>
            /// Y, 8bpp
            /// </summary>
            PIX_FMT_GRAY8,     
            /// <summary>
            /// Y, 1bpp, 0 is white, 1 is black, in each byte pixels are ordered from the msb to the lsb
            /// </summary>
            PIX_FMT_MONOWHITE, 
            /// <summary>
            /// Y, 1bpp, 0 is black, 1 is white, in each byte pixels are ordered from the msb to the lsb
            /// </summary>
            PIX_FMT_MONOBLACK,         
            PIX_FMT_PAL8,      ///< 8 bit with PIX_FMT_RGB32 palette
            PIX_FMT_YUVJ420P,  ///< planar YUV 4:2:0, 12bpp, full scale (JPEG), deprecated in favor of PIX_FMT_YUV420P and setting color_range
            PIX_FMT_YUVJ422P,  ///< planar YUV 4:2:2, 16bpp, full scale (JPEG), deprecated in favor of PIX_FMT_YUV422P and setting color_range
            PIX_FMT_YUVJ444P,  ///< planar YUV 4:4:4, 24bpp, full scale (JPEG), deprecated in favor of PIX_FMT_YUV444P and setting color_range
            PIX_FMT_XVMC_MPEG2_MC,///< XVideo Motion Acceleration via common packet passing
            PIX_FMT_XVMC_MPEG2_IDCT,
            /// <summary>
            /// packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1
            /// </summary>
            PIX_FMT_UYVY422,  
            /// <summary>
            /// packed YUV 4:1:1, 12bpp, Cb Y0 Y1 Cr Y2 Y3
            /// </summary>
            PIX_FMT_UYYVYY411, 
            /// <summary>
            /// packed RGB 3:3:2,  8bpp, (msb)2B 3G 3R(lsb)
            /// </summary>
            PIX_FMT_BGR8,     
            /// <summary>
            /// packed RGB 1:2:1 bitstream,  4bpp, (msb)1B 2G 1R(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits
            /// </summary>
            PIX_FMT_BGR4,      
            /// <summary>
            /// packed RGB 1:2:1,  8bpp, (msb)1B 2G 1R(lsb)
            /// </summary>
            PIX_FMT_BGR4_BYTE, 
            /// <summary>
            /// packed RGB 3:3:2,  8bpp, (msb)2R 3G 3B(lsb)
            /// </summary>
            PIX_FMT_RGB8,     
            /// <summary>
            /// packed RGB 1:2:1 bitstream,  4bpp, (msb)1R 2G 1B(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits
            /// </summary>
            PIX_FMT_RGB4,      
            /// <summary>
            /// packed RGB 1:2:1,  8bpp, (msb)1R 2G 1B(lsb)
            /// </summary>
            PIX_FMT_RGB4_BYTE, 
            /// <summary>
            /// planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
            /// </summary>
            PIX_FMT_NV12,      
            /// <summary>
            /// as above, but U and V bytes are swapped
            /// </summary>
            PIX_FMT_NV21,     
            /// <summary>
            /// packed ARGB 8:8:8:8, 32bpp, ARGBARGB...
            /// </summary>
            PIX_FMT_ARGB,     
            /// <summary>
            /// packed RGBA 8:8:8:8, 32bpp, RGBARGBA...
            /// </summary>
            PIX_FMT_RGBA,     
            /// <summary>
            /// packed ABGR 8:8:8:8, 32bpp, ABGRABGR...
            /// </summary>
            PIX_FMT_ABGR,      
            /// <summary>
            /// packed BGRA 8:8:8:8, 32bpp, BGRABGRA...
            /// </summary>
            PIX_FMT_BGRA,

            PIX_FMT_GRAY16BE,  ///<        Y        , 16bpp, big-endian
            PIX_FMT_GRAY16LE,  ///<        Y        , 16bpp, little-endian
            PIX_FMT_YUV440P,   ///< planar YUV 4:4:0 (1 Cr & Cb sample per 1x2 Y samples)
            PIX_FMT_YUVJ440P,  ///< planar YUV 4:4:0 full scale (JPEG), deprecated in favor of PIX_FMT_YUV440P and setting color_range
            PIX_FMT_YUVA420P,  ///< planar YUV 4:2:0, 20bpp, (1 Cr & Cb sample per 2x2 Y & A samples)
            PIX_FMT_VDPAU_H264,///< H.264 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
            PIX_FMT_VDPAU_MPEG1,///< MPEG-1 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
            PIX_FMT_VDPAU_MPEG2,///< MPEG-2 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
            PIX_FMT_VDPAU_WMV3,///< WMV3 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
            PIX_FMT_VDPAU_VC1, ///< VC-1 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
            PIX_FMT_RGB48BE,   ///< packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as big-endian
            PIX_FMT_RGB48LE,   ///< packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as little-endian

            PIX_FMT_RGB565BE,  ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), big-endian
            PIX_FMT_RGB565LE,  ///< packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), little-endian
            PIX_FMT_RGB555BE,  ///< packed RGB 5:5:5, 16bpp, (msb)1A 5R 5G 5B(lsb), big-endian, most significant bit to 0
            PIX_FMT_RGB555LE,  ///< packed RGB 5:5:5, 16bpp, (msb)1A 5R 5G 5B(lsb), little-endian, most significant bit to 0

            PIX_FMT_BGR565BE,  ///< packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), big-endian
            PIX_FMT_BGR565LE,  ///< packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), little-endian
            PIX_FMT_BGR555BE,  ///< packed BGR 5:5:5, 16bpp, (msb)1A 5B 5G 5R(lsb), big-endian, most significant bit to 1
            PIX_FMT_BGR555LE,  ///< packed BGR 5:5:5, 16bpp, (msb)1A 5B 5G 5R(lsb), little-endian, most significant bit to 1

            PIX_FMT_VAAPI_MOCO, ///< HW acceleration through VA API at motion compensation entry-point, Picture.data[3] contains a vaapi_render_state struct which contains macroblocks as well as various fields extracted from headers
            PIX_FMT_VAAPI_IDCT, ///< HW acceleration through VA API at IDCT entry-point, Picture.data[3] contains a vaapi_render_state struct which contains fields extracted from headers
            PIX_FMT_VAAPI_VLD,  ///< HW decoding through VA API, Picture.data[3] contains a vaapi_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers

            PIX_FMT_YUV420P16LE,  ///< planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
            PIX_FMT_YUV420P16BE,  ///< planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
            PIX_FMT_YUV422P16LE,  ///< planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
            PIX_FMT_YUV422P16BE,  ///< planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
            PIX_FMT_YUV444P16LE,  ///< planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
            PIX_FMT_YUV444P16BE,  ///< planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
            PIX_FMT_VDPAU_MPEG4,  ///< MPEG4 HW decoding with VDPAU, data[0] contains a vdpau_render_state struct which contains the bitstream of the slices as well as various fields extracted from headers
            PIX_FMT_DXVA2_VLD,    ///< HW decoding through DXVA2, Picture.data[3] contains a LPDIRECT3DSURFACE9 pointer

            PIX_FMT_RGB444LE,  ///< packed RGB 4:4:4, 16bpp, (msb)4A 4R 4G 4B(lsb), little-endian, most significant bits to 0
            PIX_FMT_RGB444BE,  ///< packed RGB 4:4:4, 16bpp, (msb)4A 4R 4G 4B(lsb), big-endian, most significant bits to 0
            PIX_FMT_BGR444LE,  ///< packed BGR 4:4:4, 16bpp, (msb)4A 4B 4G 4R(lsb), little-endian, most significant bits to 1
            PIX_FMT_BGR444BE,  ///< packed BGR 4:4:4, 16bpp, (msb)4A 4B 4G 4R(lsb), big-endian, most significant bits to 1
            PIX_FMT_GRAY8A,    ///< 8bit gray, 8bit alpha
            PIX_FMT_BGR48BE,   ///< packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as big-endian
            PIX_FMT_BGR48LE,   ///< packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as little-endian

            //the following 10 formats have the disadvantage of needing 1 format for each bit depth, thus
            //If you want to support multiple bit depths, then using PIX_FMT_YUV420P16* with the bpp stored seperately
            //is better
            PIX_FMT_YUV420P9BE, ///< planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
            PIX_FMT_YUV420P9LE, ///< planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
            PIX_FMT_YUV420P10BE,///< planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
            PIX_FMT_YUV420P10LE,///< planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
            PIX_FMT_YUV422P10BE,///< planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
            PIX_FMT_YUV422P10LE,///< planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
            PIX_FMT_YUV444P9BE, ///< planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
            PIX_FMT_YUV444P9LE, ///< planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
            PIX_FMT_YUV444P10BE,///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
            PIX_FMT_YUV444P10LE,///< planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
            PIX_FMT_NB,        ///< number of pixel formats, DO NOT USE THIS if you want to link with shared libav* because the number of formats might differ between versions
        }
        static SwScale() {

            var currentAssembly = Assembly.GetEntryAssembly();
            var currentDirectory = new FileInfo(currentAssembly.Location).DirectoryName;
            if (currentDirectory == null)
                return;
            string dllPath;
            if (AssemblyName.GetAssemblyName(currentAssembly.Location).ProcessorArchitecture == ProcessorArchitecture.X86)
            {
                dllPath = Path.Combine(currentDirectory, @"x86\", libraryName);
            }
            else
            {
                dllPath = Path.Combine(currentDirectory, @"x64\", libraryName);
            }
            Win32Api.LoadLibrary(dllPath);
        }
    }

    public static class Win32Api
    {
        const int DONT_RESOLVE_DLL_REFERENCES = 0x00000001;
        const int LOAD_LIBRARY_AS_DATAFILE = 0x00000002;
        const int LOAD_PACKAGED_LIBRARY = 0x00000004;
        const int LOAD_WITH_ALTERED_SEARCH_PATH = 0x00000008;
        const int LOAD_IGNORE_CODE_AUTHZ_LEVEL = 0x00000010;
        const int LOAD_LIBRARY_AS_IMAGE_RESOURCE = 0x00000020;
        const int LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE = 0x00000040;
        const int LOAD_LIBRARY_REQUIRE_SIGNED_TARGET = 0x00000080;
        const int LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR = 0x00000100;
        const int LOAD_LIBRARY_SEARCH_APPLICATION_DIR = 0x00000200;
        const int LOAD_LIBRARY_SEARCH_USER_DIRS = 0x00000400;
        const int LOAD_LIBRARY_SEARCH_SYSTEM32 = 0x00000800;
        const int LOAD_LIBRARY_SEARCH_DEFAULT_DIRS = 0x00001000;

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool SetDllDirectory(string lpPathName);
        [DllImport("kernel32", SetLastError = true, CharSet = CharSet.Ansi, EntryPoint = "LoadLibrary")]
        private static extern IntPtr LoadLibraryStatic(string lpFileName);
        [DllImport("kernel32", SetLastError = true, CharSet = CharSet.Ansi, EntryPoint = "LoadLibraryEx")]
        private static extern IntPtr LoadLibraryEx(string lpFileName, IntPtr ptr, int flag);
        /// <summary>
        ///     进程调用 LoadLibrary 以显式链接到 DLL,如果函数执行成功,它会将指定的 DLL 映射到调用进程的地址空间中并返回该 DLL 的句柄,此句柄可以与其他函数(如 GetProcAddress 和
        ///     FreeLibrary)一起在显式链接中使用
        ///     LoadLibrary 将尝试使用用于隐式链接的相同搜索序列来查找 DLL.如果系统无法找到所需的 DLL 或者入口点函数返回 FALSE.则 LoadLibrary 将抛出异常.如果对 LoadLibrary 的调用所指定的
        ///     DLL 模块已映射到调用进程的地址空间中,则该函数将返回该 DLL 的句柄并递增模块的引用数
        /// </summary>
        /// <param name="lpFileName">DLL 模块地址</param>
        /// <returns>返回 DLL 模块句柄,如果出错将抛出异常</returns>
        public static IntPtr LoadLibrary(string lpFileName)
        {
            if (!System.IO.File.Exists(lpFileName))
            {
                throw new Exception(String.Format("模块文件不存在:{0}", lpFileName));
            }
            var result = LoadLibraryEx(lpFileName, IntPtr.Zero, LOAD_WITH_ALTERED_SEARCH_PATH);
            if (result != IntPtr.Zero) return result;
            var error = GetLastError();
            if (error == 0)
            {
                throw new Win32Exception("无法载入指定的模块,未知错误.");
            }
            throw new Win32Exception(error, "无法载入指定的模块.");
        }

        [DllImport("kernel32", CharSet = CharSet.Ansi, ExactSpelling = true, SetLastError = true,
            EntryPoint = "GetProcAddress")]
        private static extern IntPtr GetProcAddressStatic(IntPtr hModule, string lpProcName);

        /// <summary>
        ///     显式链接到 DLL 的进程调用 GetProcAddress 来获取 DLL 导出函数的地址,由于是通过指针调用 DLL 函数并且没有编译时类型检查,需确保函数的参数是正确的,以便不会超出在堆栈上分配的内存和不会导致访问冲突
        /// </summary>
        /// <param name="hModule">DLL 模块句柄</param>
        /// <param name="lpProcName">调用的函数名</param>
        /// <returns>返回函数地址</returns>
        public static IntPtr GetProcAddress(IntPtr hModule, string lpProcName)
        {
            IntPtr result = GetProcAddressStatic(hModule, lpProcName);
            if (result == IntPtr.Zero)
            {
                int error = GetLastError();
                if (error == 0)
                {
                    throw new Win32Exception("无法获取函数地址,未知错误.");
                }
                throw new Win32Exception(error, "无法获取函数地址.");
            }
            return result;
        }

        /// <summary>
        ///     不再需要 DLL 模块时,显式链接到 DLL 的进程调用 FreeLibrary 函数.此函数递减模块的引用数,如果引用数为零,此函数便从进程的地址空间中取消模块的映射
        /// </summary>
        /// <param name="hModule">DLL 模块句柄</param>
        /// <returns>如果成功会返回 true ,否则会返回 false,请通过 GetLastError 获取更多信息</returns>
        [DllImport("kernel32", SetLastError = true)]
        public static extern bool FreeLibrary(IntPtr hModule);

        /// <summary>
        ///     创建一个新的文件映射内核对象
        /// </summary>
        /// <param name="hFile">指定欲在其中创建映射的一个文件句柄,为0xFFFFFFFF则表示创建一个内存文件映射</param>
        /// <param name="lpAttributes">它指明返回的句柄是否可以被子进程所继承,使用 NULL 表示使用默认安全设置</param>
        /// <param name="flProtect">指定文件映射对象的页面保护</param>
        /// <param name="dwMaximumSizeHigh">表示映射文件大小的高32位</param>
        /// <param name="dwMaximumSizeLow">表示映射文件大小的低32位</param>
        /// <param name="lpName">指定文件映射对象的名字,如果为 NULL 则会创建一个无名称的文件映射对象</param>
        /// <returns>返回文件映射对象指针,如果错误将返回 NULL,请通过 GetLastError 获取更多信息</returns>
        [DllImport("kernel32", SetLastError = true)]
        public static extern IntPtr CreateFileMapping(IntPtr hFile, IntPtr lpAttributes, PageAccess flProtect,
            int dwMaximumSizeHigh, int dwMaximumSizeLow, string lpName);

        /// <summary>
        ///     将一个文件映射对象映射到当前应用程序的地址空间
        /// </summary>
        /// <param name="hFileMappingObject">文件映射对象的句柄</param>
        /// <param name="dwDesiredAccess">映射对象的文件数据的访问方式,而且同样要与 CreateFileMapping 函数所设置的保护属性相匹配</param>
        /// <param name="dwFileOffsetHigh">表示文件映射起始偏移的高32位</param>
        /// <param name="dwFileOffsetLow">表示文件映射起始偏移的低32位</param>
        /// <param name="dwNumberOfBytesToMap">指定映射文件的字节数</param>
        /// <returns>返回文件映射在内存中的起始地址,如果错误将返回 NULL,请通过 GetLastError 获取更多信息</returns>
        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern IntPtr MapViewOfFile(IntPtr hFileMappingObject, FileMapAccess dwDesiredAccess,
            uint dwFileOffsetHigh, uint dwFileOffsetLow, uint dwNumberOfBytesToMap);

        [DllImport("kernel32", SetLastError = true)]
        public static extern bool UnmapViewOfFile(IntPtr lpBaseAddress);

        /// <summary>
        ///     关闭一个内核对象.其中包括文件,文件映射,进程,线程,安全和同步对象等
        /// </summary>
        /// <param name="handle">欲关闭的一个对象的句柄</param>
        /// <returns>如果成功会返回 true ,否则会返回 false,请通过 GetLastError 获取更多信息</returns>
        [DllImport("kernel32", SetLastError = true)]
        public static extern bool CloseHandle(IntPtr handle);

        [DllImport("kernel32")]
        public static extern int GetLastError();
    }

    public enum PageAccess
    {
        NoAccess = 0x01,
        ReadOnly = 0x02,
        ReadWrite = 0x04,
        WriteCopy = 0x08,
        Execute = 0x10,
        ExecuteRead = 0x20,
        ExecuteReadWrite = 0x40,
        ExecuteWriteCopy = 0x80,
        Guard = 0x100,
        NoCache = 0x200,
        WriteCombine = 0x400
    }

    public enum FileMapAccess : uint
    {
        Write = 0x00000002,
        Read = 0x00000004,
        AllAccess = 0x000f001f,
        Copy = 0x00000001,
        Execute = 0x00000020
    }
    public enum ColorSpace : int
    {
        ITU709 = 1,
        FCC = 4,
        ITU601 = 5,
        ITU624 = 5,
        SMPTE170M = 5,
        SMPTE240M = 7,
        DEFAULT = 5,
    }
}
