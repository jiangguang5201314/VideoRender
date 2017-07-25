using System.Runtime.InteropServices;
using SharpDX;

namespace Renderer.Core
{
    [StructLayout(LayoutKind.Sequential)]
    struct VERTEX
    {
        public Vector3 pos;        // vertex untransformed position
        public uint color;         // diffuse color
        public Vector2 texPos;     // texture relative coordinates
    };
}
