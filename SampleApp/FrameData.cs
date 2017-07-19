using System;
using System.IO;

namespace SampleApp
{
    /// <summary>
    /// 帧类型定义
    /// </summary>
    public enum VideoType
    {
        Unknown = 0,
        AUDIO16 = 1,
        RGB32 = 2,
        YV12 = 3,
        UYVY = 4,
        YUV420 = 5,
        YUY2 = 6,
        AUDIO8 = 7,

        RGB24 = 9,
        GRAY = 10,

        RGB555 = 11,
        RGB565 = 12,
        YUV422 = 13,
        YUV444 = 14
    }

    /// <summary>
    /// 缓冲视频数据
    /// </summary>
    public class FrameData
    {
        /// <summary>
        /// 缓冲取到的视频帧
        /// </summary>
        public byte[] FrameBuffer
        {
            get;
            set;
        }

        /// <summary>
        /// 每帧大小
        /// </summary>
        public int FrameSize
        {
            get;
            set;
        }

        /// <summary>
        /// 总帧数
        /// </summary>
        public int Frames
        {
            get;
            set;
        }

        /// <summary>
        /// 每帧宽度
        /// </summary>
        public int FrameWidth
        {
            get;
            set;
        }

        /// <summary>
        /// 每帧高度
        /// </summary>
        public int FrameHeight
        {
            get;
            set;
        }

        /// <summary>
        /// 帧类型
        /// </summary>
        public VideoType Type
        {
            get;
            set;
        }


        #region Load Save保存接口

        /// <summary>
        /// 保存VideoData数据. 格式为Frame头+FrameData。
        /// 
        /// 其中Frame头固定为64个int。
        /// 格式为：[VideoType][Frames][FrameSize][FrameWidth][FrameHeight][VideoBufferSize][保留字节]
        /// </summary>
        /// <param name="path"></param>
        public static void SaveData(string filePath, FrameData videoData)
        {
            if (videoData != null)
            {
                using (FileStream fs = new FileStream(filePath, FileMode.Create))
                {
                    using (BinaryWriter writer = new BinaryWriter(fs))
                    {
                        writer.Write(videoData.Frames);         // number of frames
                        writer.Write(videoData.FrameWidth);     // frame width
                        writer.Write(videoData.FrameHeight);    // frame height
                        writer.Write((int)videoData.Type);      // video type
                        writer.Write(videoData.FrameSize);      // frame size
                        for (int i = 0; i < 59; i++)
                        {
                            writer.Write((int)0);
                        }

                        if (videoData.FrameBuffer != null)
                        {
                            // 由于video buffer的size是重用的，因此buffer的size可能会大于实际数据的大小
                            // 这里使用实际的数据大小来进行保存
                            int dataSize = videoData.FrameSize * videoData.Frames;
                            writer.Write(videoData.FrameBuffer, 0, dataSize);
                        }
                    }
                }
            }
        }

        public static FrameData LoadData(string filePath)
        {
            try
            {
                using (FileStream fs = new FileStream(filePath, FileMode.Open))
                {
                    using (BinaryReader reader = new BinaryReader(fs))
                    {
                        FrameData videoData = new FrameData();

                        videoData.Frames = reader.ReadInt32();
                        videoData.FrameWidth = reader.ReadInt32();
                        videoData.FrameHeight = reader.ReadInt32();
                        videoData.Type = (VideoType)reader.ReadInt32();
                        videoData.FrameSize = reader.ReadInt32();
                        for (int i = 0; i < 59; i++)
                        {
                            reader.ReadInt32();
                        }

                        int bufferLength = videoData.Frames * videoData.FrameSize;
                        if (bufferLength > 0)
                        {
                            videoData.FrameBuffer = reader.ReadBytes(bufferLength);
                        }

                        return videoData;
                    }
                }
            }
            catch
            {
                return null;
            }
        }

        #endregion
    }
}
