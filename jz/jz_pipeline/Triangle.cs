using Microsoft.Xna.Framework;
using System;
using System.Collections.Generic;

namespace jz
{

    public interface IConvex
    {
        Vector3 GetLocalSupport(Vector3 aNormal);
        Vector3 GetWorldSupport(Vector3 aNormal);
        Vector3 WorldTranslation { get; }
    }

    public struct Triangle : IConvex
    {
        #region Overrides
        public Vector3 GetLocalSupport(Vector3 aNormal)
        {
            if (Vector3.Dot(P2 - P0, aNormal) > 0.0f)
            {
                if (Vector3.Dot(P2 - P1, aNormal) > 0.0f) { return P2; }
                else { return P1; }
            }
            else
            {
                if (Vector3.Dot(P1 - P0, aNormal) > 0.0f) { return P1; }
                else { return P0; }
            }
        }

        public Vector3 GetWorldSupport(Vector3 aNormal)
        {
            return GetLocalSupport(aNormal);
        }

        public Vector3 WorldTranslation
        {
            get
            {
                const float kFactor = (float)(1.0 / 3.0);
                return (kFactor * (P0 + P1 + P2));
            }
        }

        public bool bConsiderInKdTreeBuild { get { return (!IsDegenerate); } }
        public BoundingBox AABB { get { return Box; } }
        public int FaceCount { get { return 1; } }
        #endregion

        public readonly BoundingBox Box;
        public readonly Plane Plane;

        public readonly Vector3 P0;
        public readonly Vector3 P1;
        public readonly Vector3 P2;

        public Triangle(Vector3 p0, Vector3 p1, Vector3 p2)
        {
            P0 = p0;
            P1 = p1;
            P2 = p2;

            // Note: although XNA defaults to clockwise winding, the Plane constructor is setup like a counter-clockwise
            // wound environment. I.e. normal is calculated with Vector3.Normalize(Vector3.Cross(point2 - point1, point3 - point1)),
            // where point1, point2, point3 are arguments to the Plane() constructor.
            #region Plane calculation
#if JZ_DEFAULT_CLOCKWISE_WINDING
            Plane = new Plane(P0, P2, P1);
#elif JZ_DEFAULT_COUNTER_CLOCKWISE_WINDING
            Plane = new Plane(P0, P1, P2);
#endif
            #endregion

            #region AABB calculation
            Box.Min = Vector3.Min(P0, Vector3.Min(P1, P2));
            Box.Max = Vector3.Max(P0, Vector3.Max(P1, P2));
            #endregion
        }

        public float Area { get { return (0.5f * Vector3.Cross((P2 - P0), (P1 - P0)).Length()); } }
        public Vector3 Center { get { return ((P0 + P1 + P2) * (float)(1.0 / 3.0)); } }

        public Vector3 Min { get { return Vector3.Min(Vector3.Min(P0, P1), P2); } }
        public Vector3 Max { get { return Vector3.Max(Vector3.Max(P0, P1), P2); } }

        public bool IsDegenerate
        {
            get
            {
                bool bReturn = Utilities.AboutZero(Area);

                return bReturn;
            }
        }

        public bool Intersects(Vector3 p)
        {
            Vector3 u = (new Triangle(p, P0, P1)).Plane.Normal;
            Vector3 v = (new Triangle(p, P1, P2)).Plane.Normal;

            if (Vector3.Dot(u, v) < 0.0f) { return false; }

            Vector3 w = (new Triangle(p, P2, P0)).Plane.Normal;

            if (Vector3.Dot(u, w) < 0.0f) { return false; }

            return true;
        }
    }
}
