using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Timers;
using Renderer.Core;
using System.Runtime.InteropServices;
using SharpDX.WPF;
using SharpDX.Direct3D9;
using SharpDX.Mathematics.Interop;
using SharpDX.DirectWrite;

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
        Sprite sprite;
        D3D9 render;
        SharpDX.Direct3D9.Font font;
        string displayText = "201111";
        RawRectangle fontDimension;
        public MainWindow()
        {
            InitializeComponent();
            render = new SharpDX.WPF.D3D9();
            render.Rendering += Render_Rendering;
            dxRender.Renderer = render;
            sprite = new Sprite(render.Device);
            // Initialize the Font
            FontDescription fontDescription = new FontDescription()
            {
                Height = 72,
                Italic = false,
                CharacterSet = FontCharacterSet.Ansi,
                FaceName = "Arial",
                MipLevels = 0,
                OutputPrecision = FontPrecision.TrueType,
                PitchAndFamily = FontPitchAndFamily.Default,
                Quality = FontQuality.ClearType,
                Weight =SharpDX.Direct3D9. FontWeight.Bold
            };

              font = new SharpDX.Direct3D9.Font(render.Device, fontDescription);
          
            //  solidColorBrush = new SharpDX.Direct2D1.SolidColorBrush(render.RenderTarget2D, new RawColor4(22, 22, 11, 0xff));

            this.timer = new Timer();
            this.timer.Interval = 40;
            this.timer.Elapsed += new ElapsedEventHandler(timer_Elapsed);

           

            this.frameIndex = 0;

            try
            {
                this.yuvData = FrameData.LoadData("yv12.dat");
                  fontDimension = font.MeasureText(null, displayText, new SharpDX.Rectangle(0, 0, yuvData.FrameWidth, yuvData.FrameHeight), FontDrawFlags.Center | FontDrawFlags.VerticalCenter);
           
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
      
        private void Render_Rendering(object sender, DrawEventArgs e)
        {
            //render.RenderTarget.LockRectangle()
            //render.RenderTarget2D.Clear(back);
            //render.RenderTarget2D.DrawLine(new RawVector2(10, 10), new RawVector2(100, 100), solidColorBrush);
            var device = render.Device;
            device.Clear(ClearFlags.Target, SharpDX.Color.Black, 1.0f, 0);
            device.BeginScene(); 
            fontDimension.Left =10;
            fontDimension.Top =10;
            fontDimension.Bottom =300;
            fontDimension.Right = 300;//+= (int)xDir;
            // Draw the text
         
            font.DrawText(null, displayText,10,10, SharpDX.Color.White);
        
            device.EndScene();
            device.Present();
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
