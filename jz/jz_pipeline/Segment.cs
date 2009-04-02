using Microsoft.Xna.Framework;
using System;

namespace jare
{
    public struct Segment
    {
        public Vector3 P0;
        public Vector3 P1;

        /// <summary>
        /// Computes the points s and t of closest distance between this segment and segment b.
        /// </summary>
        /// <param name="b">Segment b.</param>
        /// <param name="s">Point of intersection on this segment.</param>
        /// <param name="t">Point of intersection on segment b.</param>
        /// <remarks>
        /// From Ericson, C. 2005. "Real-Time Collision Detection". Morgan Kaufmann.
        ///      ISBN: 1558607323
        ///      
        /// The point of shortest distance on this segment can be calculated with: (P0 + (P1 - P0) * s)
        /// The poitn of shortest distance on segment b can be calculated with: (b.P0 + (b.P1 - b.P0) * t)
        /// </remarks>
        public void ShortestDistance(Segment b, out float s, out float t)
        {
            Vector3 da = (P1 - P0);
            Vector3 db = (b.P1 - b.P0);
            Vector3 r = (P0 - b.P0);

            float a = da.LengthSquared();
            float e = db.LengthSquared();
            float f = Vector3.Dot(db, r);

            if (Utilities.AboutZero(a) && Utilities.AboutZero(e))
            {
                s = t = 0.0f;
            }

            if (Utilities.AboutZero(a))
            {
                s = 0.0f;
                t = (f / e); // t = ((b * s) + f) / e) with s = 0
                t = Utilities.Clamp(t, 0.0f, 1.0f);
            }
        }
    }
}
