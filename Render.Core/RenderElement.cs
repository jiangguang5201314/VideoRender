﻿using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Runtime.InteropServices;

namespace Renderer.Core
{
    public class RenderElement : FrameworkElement
    {
        #region 私有变量

        private IRenderSource render;
        private Image image;

        private int srcWidth;
        private int srcHeight;
        private FrameFormat srcFormat;

        #endregion

        #region 构造函数

        public RenderElement()
        {
            this.image = new Image();
            this.image.Stretch = Stretch.Uniform; // We set this because our resizing isn't immediate
            this.AddVisualChild(this.image);
        }

        #endregion

        #region 公开接口

        public bool SetupSurface(RenderType renderType, int width, int height, FrameFormat format)
        {
            if (!this.Dispatcher.CheckAccess())
            {
                return (bool)this.Dispatcher.Invoke((Action)(() => this.SetupSurface(renderType, width, height, format)));

            }

            this.CreateRenderer(renderType);
            var ret = this.render.SetupSurface(width, height, format);
            this.image.Source = this.render.ImageSource;
            return ret;
        }

        public void Display(IntPtr bufferPtr)
        {
            this.render.Render(bufferPtr);
        }

        public void Display(IntPtr bufferPtr, int frameWidth, int frameHeight, FrameFormat format)
        {
            if (this.srcFormat != format || this.srcWidth != frameWidth || this.srcHeight != frameHeight)
            {
                this.render.SetupSurface(frameWidth, frameHeight, format); // 重建offscreen surface

                this.srcFormat = format;
                this.srcWidth = frameWidth;
                this.srcHeight = frameHeight;
            }

            this.render.Render(bufferPtr);
        }

        public void Display(IntPtr yPtr, IntPtr uPtr, IntPtr vPtr)
        {
            this.render.Render(yPtr, uPtr, vPtr);
        }

        public void Display(IntPtr yPtr, IntPtr uPtr, IntPtr vPtr, int frameWidth, int frameHeight, FrameFormat format)
        {
            if (this.srcFormat != format || this.srcWidth != frameWidth || this.srcHeight != frameHeight)
            {
                this.render.SetupSurface(frameWidth, frameHeight, format); // 重建offscreen surface

                this.srcFormat = format;
                this.srcWidth = frameWidth;
                this.srcHeight = frameHeight;
            }

            this.render.Render(yPtr, uPtr, vPtr);
        }

        #endregion

        #region Protected Override

        protected override int VisualChildrenCount
        {
            get { return 1; }
        }

        protected override Visual GetVisualChild(int index)
        {
            if (index != 0)
            {
                throw new ArgumentOutOfRangeException("index");
            }
            return this.image;
        }

        protected override Size ArrangeOverride(Size finalSize)
        {
            Size size = base.ArrangeOverride(finalSize);
            image.Arrange(new Rect(0, 0, size.Width, size.Height));
            return size;
        }

        protected override Size MeasureOverride(Size availableSize)
        {
            this.image.Measure(availableSize);
            return this.image.DesiredSize;
        }

        //protected override void OnRenderSizeChanged(SizeChangedInfo sizeInfo)
        //{
        //    base.OnRenderSizeChanged(sizeInfo);
        //}

        #endregion

        #region 私有函数

        private void CreateRenderer(RenderType renderType)
        {
            this.ReleaseRenderer();
            switch (renderType)
            {
                case RenderType.D3D:
                    this.render = new D3DImageSource();
                    break;
                case RenderType.WriteBitmap:
                    this.render = new WriteableBitmapSource();
                    break;
                default:
                    break;
            }

        }

        private void ReleaseRenderer()
        {
            if (this.render != null)
            {
                this.render.Dispose();
                this.render = null;
            }
        }

        [DllImport("user32.dll")]
        static extern IntPtr GetDesktopWindow();

        #endregion
    }
}
