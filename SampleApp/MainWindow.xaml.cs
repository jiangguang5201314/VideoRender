using System;
using System.Windows;
using System.Windows.Media.Imaging;
using Renderer.Core;
using Accord.Video.DirectShow;

namespace SampleApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

    
        VideoCaptureDevice device;
        public MainWindow()
        {

            InitializeComponent();
            var frm = new VideoCaptureDeviceForm();
            frm.ShowDialog();


            device = new Accord.Video.DirectShow.VideoCaptureDevice(frm.VideoDevice.Source, System.Drawing.Imaging.PixelFormat.Format32bppRgb);
            device.VideoResolution = frm.VideoDevice.VideoResolution;
            device.NewFrame += Device_NewFrame;
            device.Start();
            rec = new Int32Rect(0, 0, device.VideoResolution.FrameSize.Width, device.VideoResolution.FrameSize.Height);
            rcsrc = new System.Drawing.Rectangle(0, 0, rec.Width, rec.Height);

            imageD3D.SetupSurface(RenderType.D3D, rec.Width, rec.Height, FrameFormat.RGB32);
            imageWB.SetupSurface(RenderType.WriteBitmap, rec.Width, rec.Height, FrameFormat.RGB32);
        }

        Int32Rect rec;
        System.Drawing.Rectangle rcsrc;
        private void Device_NewFrame(object sender, Accord.Video.NewFrameEventArgs eventArgs)
        {
            this.Dispatcher.Invoke(new Action(() =>
            {
                var ldata = eventArgs.Frame.LockBits(rcsrc, System.Drawing.Imaging.ImageLockMode.ReadOnly, eventArgs.Frame.PixelFormat);

              
               
              //  imageD3D.Display(ldata.Scan0);
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

        private void buttonCopy_Click(object sender, RoutedEventArgs e)
        {
             var w = new SampleApp.Window1(imageWB.SourceImage);
             
            w.Show();

        }
    }
    
}
