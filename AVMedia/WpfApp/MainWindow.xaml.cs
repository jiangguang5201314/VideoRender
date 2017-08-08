using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace WpfApp
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        private int imageDataSize;
        private Int32Rect imageRect;
        private System.Drawing.Rectangle imageSize;
        AVMedia.FFmpegSource srouce;
        WriteableBitmap writeBitmap;
        public MainWindow()
        {
            InitializeComponent();
              srouce = new AVMedia.FFmpegSource();
            srouce.NewFrame += Srouce_NewFrame;
            srouce.Start();
        }
        bool capture = false;
        private void Srouce_NewFrame(object sender, Accord.Video.NewFrameEventArgs eventArgs)
        {
            var frame = eventArgs.Frame.Clone() as Bitmap;
            if (this.Dispatcher!=null)
            {
                Dispatcher.BeginInvoke(new Action(() =>
                {
                   
                    if (writeBitmap == null)
                    {
                        writeBitmap = new WriteableBitmap(frame.Width, frame.Height, 96, 96, PixelFormats.Pbgra32, null);
                        image. Source = writeBitmap;
                        imageSize = new System.Drawing.Rectangle(0, 0, frame.Width, frame.Height);
                        imageRect = new Int32Rect(0, 0, frame.Width, frame.Height);
                        imageDataSize = frame.Height * writeBitmap.BackBufferStride;

                    }
                  
                    if (capture)
                    {
                        frame.Save(DateTime.Now.ToString("yyyyMMddHHmmss") + ".png", ImageFormat.Png);
                        capture = false;
                    }
                    BitmapData bData = frame.LockBits(imageSize, ImageLockMode.ReadWrite, frame.PixelFormat);
                    writeBitmap.WritePixels(imageRect, bData.Scan0, imageDataSize, writeBitmap.BackBufferStride);
                    frame.UnlockBits(bData);
                    frame.Dispose();
                }));
            }
        }
    }
}
