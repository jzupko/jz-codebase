using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;
using System.Text;

namespace jz
{
    public struct AnimationKeyFrame
    {
        public AnimationKeyFrame(float aTime, Matrix M)
        {
            Key = M;
            TimeAndPadding.X = aTime;
            TimeAndPadding.Y = 0.0f;
            TimeAndPadding.Z = 0.0f;
            TimeAndPadding.W = 0.0f;
        }

        public Matrix Key;
        public Vector4 TimeAndPadding; // align to 16-byte boundary.
    }
}
