
using FFmpeg.AutoGen;
using System;
using System.Runtime.InteropServices;

namespace AVMedia
{
    public unsafe class FFmpegFilter
    {
        int filterLen;
        AVFilterContext* lastFilter = null;
        AVFilterContext* buffersinkCtx = null;
        AVFilterContext* buffersrcCtx = null;
        AVFilterGraph* filterGraph = null;
        AVFrame* avFrame = null;

        int convertedFrameBufferSize;
        IntPtr convertedFrameBufferPtr;
        public FFmpegFilter(AVStream* stream, System.Drawing.Size vsize)
        {
            var p = stream->codecpar;
            var frate = stream->r_frame_rate;
            filterLen = 0;
            filterGraph = ffmpeg.avfilter_graph_alloc();
            buffersrcCtx = AddFilter("buffer", "in", string.Format("video_size={0}x{1}:pix_fmt={2}:time_base={3}/{4}",
                p->width, p->height, p->format, frate.num, frate.den));
            buffersinkCtx = AddFilter("buffersink", "out", string.Empty);
            avFrame = ffmpeg.av_frame_alloc();
            avFrame->width = vsize.Width;
            avFrame->height = vsize.Height;
            AVFilterContext* srcScale = AddFilter("scale", string.Empty, string.Format("w={0}:h={1}", avFrame->width, avFrame->height));
            ffmpeg.avfilter_link(buffersrcCtx, 0, srcScale, 0);
            lastFilter = srcScale;

            convertedFrameBufferSize = ffmpeg.av_image_get_buffer_size(AVPixelFormat.AV_PIX_FMT_BGRA, avFrame->width, avFrame->height, 1);
            convertedFrameBufferPtr = Marshal.AllocHGlobal(convertedFrameBufferSize);
        }
        public void AddLogo(string path)
        {

            var logoCtx = AddFilter("movie", null, string.Format("filename={0}", path)); 
            int ow = avFrame->width / 5;
            if (ow < 10)
            {
                ow = 10;
            };
            var logoScale = AddFilter("scale", null, string.Format("w={0}:h=ow*ih/iw", ow));

            ffmpeg.avfilter_link(logoCtx, 0, logoScale, 0);
            var  overCtx = AddFilter("overlay", null, "main_w-overlay_w-10:main_h-overlay_h-10");
            ffmpeg.avfilter_link(lastFilter, 0, overCtx, 0); 
            ffmpeg.avfilter_link(logoScale, 0, overCtx, 1);
            lastFilter = overCtx;
        }
        public void AddText(string text, int x, int y)
        {
            int fsize = avFrame->width / 20;
            if (fsize < 18)
            {
                fsize = 18;
            }
            int bordersize = fsize / 20;
            if (bordersize < 1)
            {
                bordersize = 1;
            }
            AVFilterContext* drawCtx = AddFilter("drawtext", null, string.Format("borderw={0}:bordercolor=black:fontfile=simsun.ttc:fontcolor=white:fontsize={1}:x={2}:y={3}:text={4}", bordersize, fsize, x, y, text));
            ffmpeg.avfilter_link(lastFilter, 0, drawCtx, 0);
            lastFilter = drawCtx;
        }

        public void Build()
        {

            AVFilterContext* formatCtx = AddFilter("format", string.Empty, string.Format("pix_fmts={0}", (int)AVPixelFormat.AV_PIX_FMT_BGRA));
            ffmpeg.avfilter_link(lastFilter, 0, formatCtx, 0);
            ffmpeg.avfilter_link(formatCtx, 0, buffersinkCtx, 0);
            if (ffmpeg.avfilter_graph_config(filterGraph, null) < 0)
            {
                throw new Exception("avfilter_graph_config\n");
            }
        }

        public System.Drawing.Bitmap ProcessFrame(AVFrame* frame)
        {
            ffmpeg.av_frame_unref(avFrame);
            ffmpeg.av_buffersrc_add_frame(buffersrcCtx, frame);
            if (ffmpeg.av_buffersink_get_frame(buffersinkCtx, avFrame) >= 0)
            {
                SystemTools.CopyUnmanagedMemory((byte*)convertedFrameBufferPtr, avFrame->data[0], convertedFrameBufferSize);
                var bitmap = new System.Drawing.Bitmap(avFrame->width, avFrame->height, avFrame->linesize[0],
                    System.Drawing.Imaging.PixelFormat.Format32bppArgb, convertedFrameBufferPtr);

                return bitmap;
            }
            return null;
        }
        AVFilterContext* AddFilter(string filterName, string instanceName, string instanceParam)
        {
            AVFilterContext* newFilter = null;
            if (string.IsNullOrEmpty(instanceName))
            {
                filterLen++;
                instanceName = string.Format("filterinstance{0}", filterLen);
            }

            if (ffmpeg.avfilter_graph_create_filter(&newFilter, ffmpeg.avfilter_get_by_name(filterName), instanceName, instanceParam, null, filterGraph) < 0)
            {
                throw new Exception("Cannot create buffer source");
            }
            return newFilter;
        }
    }
}
