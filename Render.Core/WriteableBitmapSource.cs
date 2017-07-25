using System;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows;
using Renderer.Core.Imaging;

namespace Renderer.Core
{
    public class WriteableBitmapSource : IRenderSource
    {
        #region 常量

        private const double DPI_X = 96.0;
        private const double DPI_Y = 96.0;

        #endregion

        #region 私有变量

        private WriteableBitmap imageSource;
        private Int32Rect imageSourceRect;

        // 视频格式信息
        //private FrameFormat frameFormat;
        private PixelAlignmentType pixelType;
        private ConverterResizer converter;

        // 帧格式为非YV12, NV12时，uv变量无效. 此时yStride即为图像宽，yHeight即为图像高度，ySize即为图像Buffer大小
        private int width;
        private int height;
        private int frameSize;

        #endregion

        #region IRenderSource

        public bool CheckFormat(FrameFormat format)
        {
            return ConvertToPixelFormat(format) != PixelAlignmentType.NotSupported;
        }

        public bool SetupSurface(int videoWidth, int videoHeight, FrameFormat format)
        {
            this.pixelType = ConvertToPixelFormat(format);
            if (pixelType == PixelAlignmentType.NotSupported)
            {
                return false;
            }

            this.width = videoWidth;
            this.height = videoHeight;
            switch (format)
            {
                case FrameFormat.YV12:
                case FrameFormat.NV12:
                    this.frameSize = this.width * this.height * 3 / 2;
                    break;

                case FrameFormat.YUY2:
                case FrameFormat.UYVY:
                case FrameFormat.RGB15: // rgb555
                case FrameFormat.RGB16: // rgb565
                    this.frameSize = this.width * this.height * 2; // 每个像素2字节
                    break;
                case FrameFormat.RGB24:
                    this.frameSize = this.width * this.height * 3; // 每个像素3字节

                    break;
                case FrameFormat.RGB32:
                case FrameFormat.ARGB32:
                    this.frameSize = this.width * this.height * 4; // 每个像素4字节
                    break;

                default:
                    return false;
            }

            this.imageSource = new WriteableBitmap(videoWidth, videoHeight, DPI_X, DPI_Y, System.Windows.Media.PixelFormats.Bgr32, null);
            this.imageSourceRect = new Int32Rect(0, 0, videoWidth, videoHeight);

            System.Drawing.Size size = new System.Drawing.Size(videoWidth, videoHeight);
            this.converter = new ConverterResizer(size, pixelType, size, PixelAlignmentType.BGRA);

            this.NotifyImageSourceChanged();

            return true;
        }

        public void Render(IntPtr buffer)
        {
            PlanarImage source = new PlanarImage(this.width, this.height, this.pixelType, buffer, this.frameSize);
            PlanarImage image = this.converter.DoTheWork(source);

            this.DisplayImage(image);
        }

        public void Render(IntPtr yBuffer, IntPtr uBuffer, IntPtr vBuffer)
        {
            PlanarImage source = null;

            switch (this.pixelType)
            {
                case PixelAlignmentType.YV12:
                case PixelAlignmentType.I420:
                    source = new PlanarImage(this.width, this.height, this.pixelType, new IntPtr[] { yBuffer, uBuffer, vBuffer });
                    break;

                case PixelAlignmentType.NV12:
                    source = new PlanarImage(this.width, this.height, this.pixelType, new IntPtr[] { yBuffer, uBuffer });
                    break;

                // 打包格式
                case PixelAlignmentType.YUY2:
                case PixelAlignmentType.UYVY:
                case PixelAlignmentType.BGRA:
                case PixelAlignmentType.ABGR:
                case PixelAlignmentType.RGB24:
                    source = new PlanarImage(this.width, this.height, this.pixelType, yBuffer, this.frameSize); 
                    break;

                default:
                    return;
            }

            PlanarImage image = this.converter.DoTheWork(source);
            this.DisplayImage(image);
        }

        public ImageSource ImageSource
        {
            get { return this.imageSource; }
        }

        public event EventHandler ImageSourceChanged;

        #endregion

        #region 私有函数

        private void DisplayImage(PlanarImage image)
        {
            if (!this.imageSource.Dispatcher.CheckAccess())
            {
                this.imageSource.Dispatcher.Invoke((Action)(() => this.DisplayImage(image)));
                return;
            }

            this.imageSource.Lock();
            Interop.Memcpy(this.imageSource.BackBuffer, image.Planes[0], image.PlaneSizes[0]);
            this.imageSource.AddDirtyRect(this.imageSourceRect);
            this.imageSource.Unlock();
        }

        private void NotifyImageSourceChanged()
        {
            if (this.ImageSourceChanged != null)
            {
                this.ImageSourceChanged(this, EventArgs.Empty);
            }
        }

        private static PixelAlignmentType ConvertToPixelFormat(FrameFormat format)
        {
            switch (format)
            {
                case FrameFormat.YV12:
                    return PixelAlignmentType.YV12;

                case FrameFormat.NV12:
                    return PixelAlignmentType.NV12;

                case FrameFormat.YUY2:
                    return PixelAlignmentType.YUY2;

                case FrameFormat.UYVY:
                    return PixelAlignmentType.UYVY;
              
                case FrameFormat.RGB32:
                    return PixelAlignmentType.BGRA;

                case FrameFormat.ARGB32:
                    return PixelAlignmentType.ABGR;

                case FrameFormat.RGB24:
                    return PixelAlignmentType.RGB24;

                case FrameFormat.RGB15:
                case FrameFormat.RGB16:
                default:
                    return PixelAlignmentType.NotSupported;
            }
        }

        #endregion

        #region IDisposable

        private bool isDisposed = false;

        public void Dispose()
        {
            this.Dispose(true);
            GC.SuppressFinalize(this);
        }

        private void Dispose(bool disposing)
        {
            if (!this.isDisposed)
            {
                this.isDisposed = true;

                if (disposing)
                {
                    this.SafeRelease(this.converter);
                }
            }
        }

        private void SafeRelease(IDisposable item)
        {
            try
            {
                if (item != null)
                {
                    item.Dispose();
                }
            }
            catch
            {
            }
        }


        #endregion
    }
}
