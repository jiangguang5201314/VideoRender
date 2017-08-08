using Accord.Video;
using System;
using System.Drawing.Imaging;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Imaging;
namespace AVMedia
{
    public class VideoSourceElement : Image
    {

        IVideoSource videoSource;
        WriteableBitmap writeBitmap;
        System.Drawing.Rectangle imageSize;
        Int32Rect imageRect;
        int imageDataSize;
        public void Start()
        {
            writeBitmap = null;
            videoSource.Start();

        }

        public IVideoSource VideoSource
        {
            get { return videoSource; }
            set
            {
                this.Dispatcher.Invoke(new Action(() =>
                {
                    // detach events
                    if (videoSource != null)
                    {
                        videoSource.NewFrame -= new NewFrameEventHandler(videoSource_NewFrame);
                        videoSource.VideoSourceError -= new VideoSourceErrorEventHandler(videoSource_VideoSourceError);
                        videoSource.PlayingFinished -= new PlayingFinishedEventHandler(videoSource_PlayingFinished);
                    }
                    videoSource = value;
                    // atach events
                    if (videoSource != null)
                    {
                        videoSource.NewFrame += new NewFrameEventHandler(videoSource_NewFrame);
                        videoSource.VideoSourceError += new VideoSourceErrorEventHandler(videoSource_VideoSourceError);
                        videoSource.PlayingFinished += new PlayingFinishedEventHandler(videoSource_PlayingFinished);
                    }
                }));
            }
        }

        private void videoSource_PlayingFinished(object sender, ReasonToFinishPlaying reason)
        {
            // throw new NotImplementedException();
        }

        private void videoSource_VideoSourceError(object sender, VideoSourceErrorEventArgs eventArgs)
        {
            // throw new NotImplementedException();
        }


        private void videoSource_NewFrame(object sender, NewFrameEventArgs e)
        {
            if (Application.Current == null)
            {
                videoSource.Stop();
                return;
            }
            if (Dispatcher != null)
            {
                Dispatcher.Invoke(new Action(() =>
            {
                var frame = e.Frame;
                if (writeBitmap == null)
                { 
                    writeBitmap = new WriteableBitmap(frame.Width, frame.Height, 96, 96, PixelFormats.Pbgra32, null);
                    Source = writeBitmap;
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
        bool capture = false;
        public void Save()
        {
            capture = true;
        }
    }
}
