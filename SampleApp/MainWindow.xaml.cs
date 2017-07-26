using System;
using System.Windows;
using System.Windows.Media.Imaging;
using Renderer.Core;
using Accord.Video.DirectShow;
using System.Windows.Media;
using System.Threading;
using System.Windows.Controls;

namespace SampleApp
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        RenderElement imageD3D;
        RenderElement imageD3D1;

        void createChildInNewThread(WrapPanel container,ref RenderElement render)
      
        {
            
               HostVisual hostVisual = new HostVisual();

            UIElement content = new VisualHost(hostVisual);
            container.Children.Add (content);

            Thread thread = new Thread(new ThreadStart(() =>
            {
                VisualTarget visualTarget = new VisualTarget(hostVisual);

                var control = new RenderElement();
                if (imageD3D == null)
                {
                    imageD3D = control;
                }
                else
                {
                    imageD3D1 = control;
                }
              
                control.Arrange(new Rect(new Point(), content.RenderSize));
                control.SetupSurface(RenderType.D3D, rec.Width, rec.Height, FrameFormat.RGB32);
                visualTarget.RootVisual = control;
                device.Start();
                System.Windows.Threading.Dispatcher.Run();
              

            }));

            thread.SetApartmentState(ApartmentState.STA);
            thread.IsBackground = true;
            thread.Start();
        }
        VideoCaptureDevice device;
        public MainWindow()
        {

            InitializeComponent();
            var frm = new VideoCaptureDeviceForm();
            frm.ShowDialog();


            device = new Accord.Video.DirectShow.VideoCaptureDevice(frm.VideoDevice.Source, System.Drawing.Imaging.PixelFormat.Format32bppRgb);
            device.VideoResolution = frm.VideoDevice.VideoResolution;
            device.NewFrame += Device_NewFrame;
         
            rec = new Int32Rect(0, 0, device.VideoResolution.FrameSize.Width, device.VideoResolution.FrameSize.Height);
            rcsrc = new System.Drawing.Rectangle(0, 0, rec.Width, rec.Height);

           
            //    imageWB.SetupSurface(RenderType.WriteBitmap, rec.Width, rec.Height, FrameFormat.RGB32);
        }

        Int32Rect rec;
        System.Drawing.Rectangle rcsrc;
        private void Device_NewFrame(object sender, Accord.Video.NewFrameEventArgs eventArgs)
        {
            var frame = eventArgs.Frame.Clone() as System.Drawing.Bitmap;
            imageD3D.Dispatcher.BeginInvoke(new Action(() =>
            {
                var ldata = frame.LockBits(rcsrc, System.Drawing.Imaging.ImageLockMode.ReadOnly, frame.PixelFormat);
                imageD3D.Display(ldata.Scan0);
                //  imageWB.Display(ldata.Scan0);
                frame.UnlockBits(ldata);
                frame.Dispose();
            }));
            //this.Dispatcher.BeginInvoke(new Action(() =>
            //{
            //    var ldata = frame.LockBits(rcsrc, System.Drawing.Imaging.ImageLockMode.ReadOnly, frame.PixelFormat); 
            //    imageD3D.Display(ldata.Scan0);
            //    //  imageWB.Display(ldata.Scan0);
            //    frame.UnlockBits(ldata);
            //    frame.Dispose();
            //}));
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
            var w = new SampleApp.Window1(imageD3D.SourceImage);

            w.Show();

        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            createChildInNewThread(p1, ref imageD3D);
            createChildInNewThread(p2, ref  imageD3D1);
           
        }
    }


    public class VisualHost : FrameworkElement
    {
        Visual child;

        public VisualHost(Visual child)
        {
            if (child == null)
                throw new ArgumentException("child");

            this.child = child;
            AddVisualChild(child);
        }

        protected override Visual GetVisualChild(int index)
        {
            return (index == 0) ? child : null;
        }

        protected override int VisualChildrenCount
        {
            get { return 1; }
        }
    }

}
