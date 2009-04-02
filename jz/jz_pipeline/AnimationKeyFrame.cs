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
            Time = aTime;
            Key = M;
        }

        public float Time;
        public Matrix Key;
    }
}
