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

namespace SampleApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        FrameData yuvData;
        Timer timer;
        WriteableBitmapSource wbSource;
        D3DImageSource d3dSource;
        int frameIndex;

        public MainWindow()
        {
            InitializeComponent();

            this.timer = new Timer();
            this.timer.Interval = 40;
            this.timer.Elapsed += new ElapsedEventHandler(timer_Elapsed);

            this.d3dSource = new D3DImageSource();
            this.wbSource = new WriteableBitmapSource();

            this.frameIndex = 0;

            try
            {
                this.yuvData = FrameData.LoadData("yv12.dat");

                if (this.wbSource.SetupSurface(this.yuvData.FrameWidth, this.yuvData.FrameHeight, FrameFormat.YV12))
                {
                    this.imageWB.Source = this.wbSource.ImageSource;
                }
                else
                {
                    MessageBox.Show("WriteableBitmapSource不支持该种帧格式：" + FrameFormat.YV12);
                }

                if (this.d3dSource.SetupSurface(this.yuvData.FrameWidth, this.yuvData.FrameHeight, FrameFormat.YV12))
                {
                    this.imageD3D.Source = this.d3dSource.ImageSource;
                }
                else
                {
                    MessageBox.Show("本机显卡不支持该种帧格式：" + FrameFormat.YV12);
                }
            }
            catch(Exception ex)
            {
                MessageBox.Show("加载数据文件失败"+ex.Message);
            }
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
                    this.wbSource.Render(ptr);
                    this.d3dSource.Render(ptr);
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
