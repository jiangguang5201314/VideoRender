using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Timers;
using Renderer.Core;
using System.Runtime.InteropServices;
using SharpDX.WPF;
using SharpDX.Direct3D9;
using SharpDX.Mathematics.Interop;

namespace SampleApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        FrameData yuvData;
        Timer timer; 
        int frameIndex;
        D2D1 render;
        public MainWindow()
        {
            InitializeComponent();
            render = new SharpDX.WPF.D2D1();
            render.Rendering += Render_Rendering;
            dxRender.Renderer = render;



            solidColorBrush = new SharpDX.Direct2D1.SolidColorBrush(render.RenderTarget2D, new RawColor4(22, 22, 11, 0xff));

            this.timer = new Timer();
            this.timer.Interval = 40;
            this.timer.Elapsed += new ElapsedEventHandler(timer_Elapsed);

           

            this.frameIndex = 0;

            try
            {
                this.yuvData = FrameData.LoadData("yv12.dat");

                if (!this.imageD3D.SetupSurface(RenderType.D3D,this.yuvData.FrameWidth, this.yuvData.FrameHeight, FrameFormat.YV12))
                
                {
                    MessageBox.Show("WriteableBitmapSource不支持该种帧格式：" + FrameFormat.YV12);
                }

                if (!this.imageWB.SetupSurface(RenderType.WriteBitmap,this.yuvData.FrameWidth, this.yuvData.FrameHeight, FrameFormat.YV12))
                {
                 
             
                    MessageBox.Show("本机显卡不支持该种帧格式：" + FrameFormat.YV12);
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show("加载数据文件失败"+ex.Message);
            }
        }
        RawColor4 back = new RawColor4(0, 0, 0, 0xff);
        SharpDX.Direct2D1.SolidColorBrush solidColorBrush;


        private void Render_Rendering(object sender, DrawEventArgs e)
        {
      
            render.RenderTarget2D.Clear(back);
            render.RenderTarget2D.DrawLine(new RawVector2(10, 10), new RawVector2(100, 100), solidColorBrush);
           
        }

        private void timer_Elapsed(object sender, ElapsedEventArgs e)
        {
            if (this.yuvData != null)
            {
                if (this.frameIndex >= this.yuvData.Frames)
                {
                    this.frameIndex %= this.yuvData.Frames;
                }

                try
                {
                    IntPtr ptr = Marshal.UnsafeAddrOfPinnedArrayElement(this.yuvData.FrameBuffer, this.frameIndex * this.yuvData.FrameSize);
                    this.imageD3D.Display(ptr);
                    this.imageWB.Display(ptr);
                }
                catch
                { 
                }

                this.frameIndex++;
            }
        }

        private void buttonStart_Click(object sender, RoutedEventArgs e)
        {
            this.timer.Start();
        }

        private void buttonStop_Click(object sender, RoutedEventArgs e)
        {
            this.timer.Stop();
        }
    }
}
