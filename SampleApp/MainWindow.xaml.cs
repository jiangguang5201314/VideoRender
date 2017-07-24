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
using Accord.Video.DirectShow;

namespace SampleApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        WriteableBitmap rw;
         VideoCaptureDevice device;
        public MainWindow()
        {

            InitializeComponent();
            var frm = new VideoCaptureDeviceForm();
            frm.ShowDialog();
            device = frm.VideoDevice;
            device.NewFrame += Device_NewFrame;
            rec = new Int32Rect(0, 0, device.VideoResolution.FrameSize.Width, device.VideoResolution.FrameSize.Height);
            rcsrc = new System.Drawing.Rectangle(0, 0, rec.Width, rec.Height);
            


        }

         Int32Rect rec;
        System.Drawing.Rectangle rcsrc;
        private void Device_NewFrame(object sender, Accord.Video.NewFrameEventArgs eventArgs)
        {
            this.Dispatcher.Invoke(new Action(() =>
            {
                var ldata = eventArgs.Frame.LockBits(rcsrc, System.Drawing.Imaging.ImageLockMode.ReadOnly, eventArgs.Frame.PixelFormat);

                if (rw == null)
                {
                    rw = new WriteableBitmap(device.VideoResolution.FrameSize.Width, device.VideoResolution.FrameSize.Height, 96, 96, System.Windows.Media.PixelFormats.Bgr24, null);
                //    imageD3D.SetupSurface(RenderType.D3D, eventArgs.Frame.Width, eventArgs.Frame.Height, FrameFormat.RGB32);
                    imageWB.SetupSurface(RenderType.WriteBitmap, eventArgs.Frame.Width, eventArgs.Frame.Height, FrameFormat.RGB24);
                    capture.Source = rw;
                }
                
                rw.WritePixels(rec, ldata.Scan0, ldata.Stride * ldata.Height, rw.BackBufferStride);
           //     imageD3D.Display(ldata.Scan0);
                imageWB.Display(ldata.Scan0);
                eventArgs.Frame.UnlockBits(ldata);

            })); 
        }

        private void buttonStart_Click(object sender, RoutedEventArgs e)
        {
            device.Start();
        }

        private void buttonStop_Click(object sender, RoutedEventArgs e)
        {
            device.SignalToStop();

        }

        
    }
}
