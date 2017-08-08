
namespace AVMedia
{
    using Accord.Video;
    using FFmpeg.AutoGen;
    using System;
    using System.Drawing;
    using System.Runtime.InteropServices;
    using System.Threading;

    
    /// <summary>
    /// FFmpeg Source 封装
    /// </summary>
    public   class FFmpegSource : IVideoSource
    {

        // frame interval in milliseconds
        private string url = "rtsp://admin:abc12345@192.168.18.208:554/";
        private int framesReceived;
        private Thread thread = null;
        private ManualResetEvent stopEvent = null;
        private Size displaySize;
        static FFmpegSource()
        {
            ffmpeg.av_register_all();
            ffmpeg.avcodec_register_all();
            ffmpeg.avformat_network_init();
            ffmpeg.avdevice_register_all();
            ffmpeg.avfilter_register_all();
        }

        /// <summary>
        /// New frame event.
        /// </summary>
        /// 
        /// <remarks><para>Notifies clients about new available frame from video source.</para>
        /// 
        /// <para><note>Since video source may have multiple clients, each client is responsible for
        /// making a copy (cloning) of the passed video frame, because the video source disposes its
        /// own original copy after notifying of clients.</note></para>
        /// </remarks>
        /// 
        public event NewFrameEventHandler NewFrame;

        /// <summary>
        /// Video source error event.
        /// </summary>
        /// 
        /// <remarks>This event is used to notify clients about any type of errors occurred in
        /// video source object, for example internal exceptions.</remarks>
        /// 
        public event VideoSourceErrorEventHandler VideoSourceError;

        /// <summary>
        /// Video playing finished event.
        /// </summary>
        /// 
        /// <remarks><para>This event is used to notify clients that the video playing has finished.</para>
        /// </remarks>
        /// 
        public event PlayingFinishedEventHandler PlayingFinished;

        /// <summary>
        /// Video source.
        /// </summary>
        /// 
        public virtual string Source
        {
            get { return url; }
        }




        /// <summary>
        /// Received frames count.
        /// </summary>
        /// 
        /// <remarks>Number of frames the video source provided from the moment of the last
        /// access to the property.
        /// </remarks>
        /// 
        public int FramesReceived
        {
            get
            {
                int frames = framesReceived;
                framesReceived = 0;
                return frames;
            }
        }

        /// <summary>
        /// Received bytes count.
        /// </summary>
        /// 
        /// <remarks><para><note>The property is not implemented for this video source and always returns 0.</note></para>
        /// </remarks>
        /// 
        public long BytesReceived
        {
            get { return 0; }
        }

        /// <summary>
        /// State of the video source.
        /// </summary>
        /// 
        /// <remarks>Current state of video source object - running or not.</remarks>
        /// 
        public bool IsRunning
        {
            get
            {
                if (thread != null)
                {
                    // check thread status
                    if (thread.Join(0) == false)
                        return true;

                    // the thread is not running, free resources
                    Free();
                }
                return false;
            }
        }
        
        public FFmpegSource(string uri, Size size)
        {
            displaySize = size;
            this.url = uri;
        }
        /// <summary>
        /// Start video source.
        /// </summary>
        /// 
        /// <remarks>Starts video source and return execution to caller. Video source
        /// object creates background thread and notifies about new frames with the
        /// help of <see cref="NewFrame"/> event.</remarks>
        /// 
        /// <exception cref="ArgumentException">Video source is not specified.</exception>
        /// 
        public void Start()
        {
            if (!IsRunning)
            {
                framesReceived = 0;

                // create events
                stopEvent = new ManualResetEvent(false);

                // create and start new thread
                thread = new Thread(new ThreadStart(WorkerThread));
                thread.Name = Source; // mainly for debugging
                thread.Start();
            }
        }

        /// <summary>
        /// Signal video source to stop its work.
        /// </summary>
        /// 
        /// <remarks>Signals video source to stop its background thread, stop to
        /// provide new frames and free resources.</remarks>
        /// 
        public void SignalToStop()
        {
            // stop thread
            if (thread != null)
            {
                // signal to stop
                stopEvent.Set();
            }
        }

        /// <summary>
        /// Wait for video source has stopped.
        /// </summary>
        /// 
        /// <remarks>Waits for source stopping after it was signalled to stop using
        /// <see cref="SignalToStop"/> method.</remarks>
        /// 
        public void WaitForStop()
        {
            if (thread != null)
            {
                // wait for thread stop
                thread.Join();

                Free();
            }
        }

        /// <summary>
        /// Stop video source.
        /// </summary>
        /// 
        /// <remarks><para>Stops video source aborting its thread.</para>
        /// 
        /// <para><note>Since the method aborts background thread, its usage is highly not preferred
        /// and should be done only if there are no other options. The correct way of stopping camera
        /// is <see cref="SignalToStop">signaling it stop</see> and then
        /// <see cref="WaitForStop">waiting</see> for background thread's completion.</note></para>
        /// </remarks>
        /// 
        public void Stop()
        {
            if (this.IsRunning)
            {
                stopEvent.Set();
                thread.Abort();
                WaitForStop();
            }
        }

        /// <summary>
        /// Free resource.
        /// </summary>
        /// 
        private void Free()
        {
            thread = null;

            // release events
            stopEvent.Close();
            stopEvent = null;
        }

        // Worker thread
        private unsafe void WorkerThread()
        {



            Console.WriteLine($"FFmpeg version info: {ffmpeg.av_version_info()}");

            // setup logging
            ffmpeg.av_log_set_level(ffmpeg.AV_LOG_VERBOSE);
            av_log_set_callback_callback logCallback = (p0, level, format, vl) =>
            {
                if (level > ffmpeg.av_log_get_level()) return;

                var lineSize = 1024;
                var lineBuffer = stackalloc byte[lineSize];
                var printPrefix = 1;
                ffmpeg.av_log_format_line(p0, level, format, vl, lineBuffer, lineSize, &printPrefix);
                var line = Marshal.PtrToStringAnsi((IntPtr)lineBuffer);
                Console.Write(line);
            };
            ffmpeg.av_log_set_callback(logCallback);

            // decode N frames from url or path

            //string url = @"../../sample_mpeg4.mp4";

            var pFormatContext = ffmpeg.avformat_alloc_context();

            if (ffmpeg.avformat_open_input(&pFormatContext, url, null, null) != 0)
                throw new ApplicationException(@"Could not open file.");

            if (ffmpeg.avformat_find_stream_info(pFormatContext, null) != 0)
                throw new ApplicationException(@"Could not find stream info");

            AVStream* pStream = null;
            for (var i = 0; i < pFormatContext->nb_streams; i++)
                if (pFormatContext->streams[i]->codec->codec_type == AVMediaType.AVMEDIA_TYPE_VIDEO)
                {
                    pStream = pFormatContext->streams[i];
                    break;
                }
            if (pStream == null)
                throw new ApplicationException(@"Could not found video stream.");


            var codecContext = *pStream->codec;

            Console.WriteLine($"codec name: {ffmpeg.avcodec_get_name(codecContext.codec_id)}");

            var width = codecContext.width;
            var height = codecContext.height;
            var sourcePixFmt = codecContext.pix_fmt;
            var codecId = codecContext.codec_id;


            var pCodec = ffmpeg.avcodec_find_decoder(codecId);
            if (pCodec == null)
                throw new ApplicationException(@"Unsupported codec.");

            var pCodecContext = &codecContext;

            if ((pCodec->capabilities & ffmpeg.AV_CODEC_CAP_TRUNCATED) == ffmpeg.AV_CODEC_CAP_TRUNCATED)
                pCodecContext->flags |= ffmpeg.AV_CODEC_FLAG_TRUNCATED;

            if (ffmpeg.avcodec_open2(pCodecContext, pCodec, null) < 0)
                throw new ApplicationException(@"Could not open codec.");

            var pDecodedFrame = ffmpeg.av_frame_alloc();

            var packet = new AVPacket();
            var pPacket = &packet;
            ffmpeg.av_init_packet(pPacket);
            var filter = new FFmpegFilter(pStream, displaySize);
            var frameNumber = 0;
            filter.AddLogo("logo.png");
            filter.AddText("麦迪科技", 10, 10);
            filter.Build();
          
            while (!stopEvent.WaitOne(0, false))
            {
                try
                {
                    if (ffmpeg.av_read_frame(pFormatContext, pPacket) < 0)
                        throw new ApplicationException(@"Could not read frame.");

                    if (pPacket->stream_index != pStream->index)
                        continue;

                    if (ffmpeg.avcodec_send_packet(pCodecContext, pPacket) < 0)
                        throw new ApplicationException($@"Error while sending packet {frameNumber}.");

                    if (ffmpeg.avcodec_receive_frame(pCodecContext, pDecodedFrame) < 0)
                    {
                        Console.WriteLine($@"frame: {frameNumber} fail");
                    }
                    else
                    {
                        Console.WriteLine($@"frame: {frameNumber}");
                        var bitmap = filter.ProcessFrame(pDecodedFrame);
                        // notify client
                        NewFrame?.Invoke(this, new NewFrameEventArgs(bitmap));

                    }
                    frameNumber++;
                    framesReceived++;
                }
                catch (ThreadAbortException)
                {
                    break;
                }
                catch (Exception exception)
                {
                    // provide information to clients
                    if (VideoSourceError != null)
                    {
                        VideoSourceError(this, new VideoSourceErrorEventArgs(exception.Message));
                    }
                    // wait for a while before the next try
                    Thread.Sleep(250);
                }
                finally
                {
                    ffmpeg.av_packet_unref(pPacket);
                    ffmpeg.av_frame_unref(pDecodedFrame);
                }
                // need to stop ?
                if (stopEvent.WaitOne(0, false))
                    break;
            } 
            ffmpeg.av_free(pDecodedFrame);
            ffmpeg.avcodec_close(pCodecContext);
            ffmpeg.avformat_close_input(&pFormatContext);
            if (PlayingFinished != null)
            {
                PlayingFinished(this, ReasonToFinishPlaying.StoppedByUser);
            }
        }
    }
}
