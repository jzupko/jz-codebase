using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace jz
{

    public enum Axis
    {
        X = 0,
        Y = 1,
        Z = 2,
        W = 3
    }

    public enum ImageOrigin
    {
        BottomLeft,
        TopLeft
    }

    public enum ParameterType
    {
        kSingle = 1,
        kMatrix = 2,
        kVector2 = 3,
        kVector3 = 4,
        kVector4 = 5,
        kTexture = 6
    }

    public enum SceneNodeType
    {
        None,
        AnimatedMeshPart,
        Camera,
        DirectionalLight,
        Joint,
        Light,
        MeshPart,
        Node,
        Physics,
        PointLight,
        SpotLight
    }

    public enum Winding
    {
        Clockwise,
        CounterClockwise
    }

    public static class Utilities
    {
        #region Constants
        public const CullMode kBackFaceCulling = CullMode.CullCounterClockwiseFace;
        public const CullMode kFrontFaceCulling = CullMode.CullClockwiseFace;
        public const ImageOrigin kImageOrigin = ImageOrigin.TopLeft;
        public const Winding kWinding = Winding.Clockwise;

        public const int kFindEigenspaceIterations = 32;
        public static Matrix kIdentity = Matrix.Identity;
        public static BoundingBox kInvertedMaxBox = new BoundingBox(kMaxVector3, kMinVector3);
        public const float kJacobiToleranceFloat = 1e-8f;
        public const double kJacobiToleranceDouble = 1e-10;
        public const double kLooseToleranceDouble = 1e-5f;
        public const float kLooseToleranceFloat = 1e-3f;
        public static BoundingBox kMaxBox = new BoundingBox(kMinVector3, kMaxVector3);
        public static BoundingSphere kMaxSphere = new BoundingSphere(Vector3.Zero, float.MaxValue);
        public static readonly Vector2 kMaxVector2 = new Vector2(float.MaxValue, float.MaxValue);
        public static readonly Vector3 kMaxVector3 = new Vector3(float.MaxValue, float.MaxValue, float.MaxValue);
        public static readonly Vector2 kMinVector2 = new Vector2(float.MinValue, float.MinValue);
        public static readonly Vector3 kMinVector3 = new Vector3(float.MinValue, float.MinValue, float.MinValue);
        public const string kMediaRoot = "Content\\";
        public const string kNotImplemented = "Execution has reached code that has not yet been implemented.";
        public const string kShouldNotBeHere = "Execution has reached a code branch that should not be possible. Please send the details of this exception to a developer.";
        public const string kSingleton = "This class can only have one instance at a time.";
        public static BoundingBox kZeroBox = default(BoundingBox);
        public static BoundingSphere kZeroSphere = default(BoundingSphere);
        public const double kZeroToleranceDouble = 1e-8;
        public const float kZeroToleranceFloat = 1e-6f;
        public static Vector3 kZeroVector3 = Vector3.Zero;
        #endregion

        #region AboutEqual
        public static bool AboutEqual(float a, float b)
        {
            return (Math.Abs(a - b) < kZeroToleranceFloat);
        }

        public static bool AboutEqual(float a, float b, float aTolerance)
        {
            return (Math.Abs(a - b) < aTolerance);
        }

        public static bool AboutEqual(double a, double b)
        {
            return (Math.Abs(a - b) < kZeroToleranceDouble);
        }

        public static bool AboutEqual(double a, double b, double aTolerance)
        {
            return (Math.Abs(a - b) < aTolerance);
        }

        public static bool AboutEqual(ref Vector2 u, ref Vector2 v)
        {
            return AboutEqual(u.X, v.X) &&
                   AboutEqual(u.Y, v.Y);
        }

        public static bool AboutEqual(Vector2 u, Vector2 v)
        {
            return AboutEqual(u.X, v.X) &&
                   AboutEqual(u.Y, v.Y);
        }

        public static bool AboutEqual(Vector2 u, Vector2 v, float aTolerance)
        {
            return AboutEqual(u.X, v.X, aTolerance) &&
                   AboutEqual(u.Y, v.Y, aTolerance);
        }

        public static bool AboutEqual(ref Vector3 u, ref Vector3 v)
        {
            return AboutEqual(u.X, v.X) &&
                   AboutEqual(u.Y, v.Y) &&
                   AboutEqual(u.Z, v.Z);
        }

        public static bool AboutEqual(Vector3 u, Vector3 v)
        {
            return AboutEqual(u.X, v.X) &&
                   AboutEqual(u.Y, v.Y) &&
                   AboutEqual(u.Z, v.Z);
        }

        public static bool AboutEqual(Vector3 u, Vector3 v, float aTolerance)
        {
            return AboutEqual(u.X, v.X, aTolerance) &&
                   AboutEqual(u.Y, v.Y, aTolerance) &&
                   AboutEqual(u.Z, v.Z, aTolerance);
        }

        public static bool AboutEqual(BoundingBox a, BoundingBox b)
        {
            return AboutEqual(ref a.Max, ref b.Max) && AboutEqual(ref a.Min, ref b.Min);
        }

        public static bool AboutEqual(BoundingBox a, BoundingBox b, float aTolerance)
        {
            return AboutEqual(ref a.Max, ref b.Max, aTolerance) && AboutEqual(ref a.Min, ref b.Min, aTolerance);
        }

        public static bool AboutEqual(BoundingSphere a, BoundingSphere b)
        {
            return AboutEqual(ref a.Center, ref b.Center) && AboutEqual(a.Radius, b.Radius);
        }

        public static bool AboutEqual(BoundingSphere a, BoundingSphere b, float aTolerance)
        {
            return AboutEqual(ref a.Center, ref b.Center, aTolerance) && AboutEqual(a.Radius, b.Radius, aTolerance);
        }

        public static bool AboutEqual(ref BoundingSphere a, ref BoundingSphere b)
        {
            return AboutEqual(ref a.Center, ref b.Center) && AboutEqual(a.Radius, b.Radius);
        }

        public static bool AboutEqual(ref BoundingSphere a, ref BoundingSphere b, float aTolerance)
        {
            return AboutEqual(ref a.Center, ref b.Center, aTolerance) && AboutEqual(a.Radius, b.Radius, aTolerance);
        }

        public static bool AboutEqual(ref Quaternion a, ref Quaternion b)
        {
            return AboutEqual(a.X, b.X) && AboutEqual(a.Y, b.Y) && AboutEqual(a.Z, b.Z) && AboutEqual(a.W, b.W);
        }

        public static bool AboutEqual(ref Quaternion a, ref Quaternion b, float aTolerance)
        {
            return AboutEqual(a.X, b.X, aTolerance) && AboutEqual(a.Y, b.Y, aTolerance) && AboutEqual(a.Z, b.Z, aTolerance) && AboutEqual(a.W, b.W, aTolerance);
        }

        public static bool AboutEqual(Quaternion a, Quaternion b)
        {
            return AboutEqual(a.X, b.X) && AboutEqual(a.Y, b.Y) && AboutEqual(a.Z, b.Z) && AboutEqual(a.W, b.W);
        }

        public static bool AboutEqual(Quaternion a, Quaternion b, float aTolerance)
        {
            return AboutEqual(a.X, b.X, aTolerance) && AboutEqual(a.Y, b.Y, aTolerance) && AboutEqual(a.Z, b.Z, aTolerance) && AboutEqual(a.W, b.W, aTolerance);
        }

        public static bool AboutEqual(ref Vector3 u, ref Vector3 v, float aTolerance)
        {
            return AboutEqual(u.X, v.X, aTolerance) &&
                   AboutEqual(u.Y, v.Y, aTolerance) &&
                   AboutEqual(u.Z, v.Z, aTolerance);
        }

        public static bool AboutEqual(ref Matrix m, ref Matrix n)
        {
            return AboutEqual(m.M11, n.M11) && AboutEqual(m.M12, n.M12) && AboutEqual(m.M13, n.M13) && AboutEqual(m.M14, n.M14) &&
                   AboutEqual(m.M21, n.M21) && AboutEqual(m.M22, n.M22) && AboutEqual(m.M23, n.M23) && AboutEqual(m.M24, n.M24) &&
                   AboutEqual(m.M31, n.M31) && AboutEqual(m.M32, n.M32) && AboutEqual(m.M33, n.M33) && AboutEqual(m.M34, n.M34) &&
                   AboutEqual(m.M41, n.M41) && AboutEqual(m.M42, n.M42) && AboutEqual(m.M43, n.M43) && AboutEqual(m.M44, n.M44);
        }

        public static bool AboutEqual(ref Matrix m, ref Matrix n, float aTolerance)
        {
            return AboutEqual(m.M11, n.M11, aTolerance) && AboutEqual(m.M12, n.M12, aTolerance) && AboutEqual(m.M13, n.M13, aTolerance) && AboutEqual(m.M14, n.M14, aTolerance) &&
                   AboutEqual(m.M21, n.M21, aTolerance) && AboutEqual(m.M22, n.M22, aTolerance) && AboutEqual(m.M23, n.M23, aTolerance) && AboutEqual(m.M24, n.M24, aTolerance) &&
                   AboutEqual(m.M31, n.M31, aTolerance) && AboutEqual(m.M32, n.M32, aTolerance) && AboutEqual(m.M33, n.M33, aTolerance) && AboutEqual(m.M34, n.M34, aTolerance) &&
                   AboutEqual(m.M41, n.M41, aTolerance) && AboutEqual(m.M42, n.M42, aTolerance) && AboutEqual(m.M43, n.M43, aTolerance) && AboutEqual(m.M44, n.M44, aTolerance);
        }

        public static bool AboutEqual(Matrix m, Matrix n)
        {
            return AboutEqual(m.M11, n.M11) && AboutEqual(m.M12, n.M12) && AboutEqual(m.M13, n.M13) && AboutEqual(m.M14, n.M14) &&
                   AboutEqual(m.M21, n.M21) && AboutEqual(m.M22, n.M22) && AboutEqual(m.M23, n.M23) && AboutEqual(m.M24, n.M24) &&
                   AboutEqual(m.M31, n.M31) && AboutEqual(m.M32, n.M32) && AboutEqual(m.M33, n.M33) && AboutEqual(m.M34, n.M34) &&
                   AboutEqual(m.M41, n.M41) && AboutEqual(m.M42, n.M42) && AboutEqual(m.M43, n.M43) && AboutEqual(m.M44, n.M44);
        }

        public static bool AboutEqual(Matrix m, Matrix n, float aTolerance)
        {
            return AboutEqual(m.M11, n.M11, aTolerance) && AboutEqual(m.M12, n.M12, aTolerance) && AboutEqual(m.M13, n.M13, aTolerance) && AboutEqual(m.M14, n.M14, aTolerance) &&
                   AboutEqual(m.M21, n.M21, aTolerance) && AboutEqual(m.M22, n.M22, aTolerance) && AboutEqual(m.M23, n.M23, aTolerance) && AboutEqual(m.M24, n.M24, aTolerance) &&
                   AboutEqual(m.M31, n.M31, aTolerance) && AboutEqual(m.M32, n.M32, aTolerance) && AboutEqual(m.M33, n.M33, aTolerance) && AboutEqual(m.M34, n.M34, aTolerance) &&
                   AboutEqual(m.M41, n.M41, aTolerance) && AboutEqual(m.M42, n.M42, aTolerance) && AboutEqual(m.M43, n.M43, aTolerance) && AboutEqual(m.M44, n.M44, aTolerance);
        }
        #endregion

        #region AboutZero
        public static bool AboutZero(float a)
        {
            return (Math.Abs(a) < kZeroToleranceFloat);
        }

        public static bool AboutZero(float a, float aTolerance)
        {
            return (Math.Abs(a) < aTolerance);
        }

        public static bool AboutZero(double a)
        {
            return (Math.Abs(a) < kZeroToleranceDouble);
        }

        public static bool AboutZero(double a, double aTolerance)
        {
            return (Math.Abs(a) < aTolerance);
        }

        public static bool AboutZero(Vector3 u)
        {
            return AboutZero(u.X) &&
                   AboutZero(u.Y) &&
                   AboutZero(u.Z);
        }

        public static bool AboutZero(Vector3 u, float aTolerance)
        {
            return AboutZero(u.X, aTolerance) &&
                   AboutZero(u.Y, aTolerance) &&
                   AboutZero(u.Z, aTolerance);
        }

        public static bool AboutZero(ref Vector3 u)
        {
            return AboutZero(u.X) &&
                   AboutZero(u.Y) &&
                   AboutZero(u.Z);
        }

        public static bool AboutZero(ref Vector3 u, float aTolerance)
        {
            return AboutZero(u.X, aTolerance) &&
                   AboutZero(u.Y, aTolerance) &&
                   AboutZero(u.Z, aTolerance);
        }
        #endregion

        #region GreaterThan
        public static bool GreaterThan(float a, float b)
        {
            return (a >= (b + kZeroToleranceFloat));
        }

        public static bool GreaterThan(float a, float b, float aTolerance)
        {
            return (a >= (b + aTolerance));
        }

        public static bool GreaterThan(double a, double b)
        {
            return (a >= (b + kZeroToleranceDouble));
        }

        public static bool GreaterThan(double a, double b, double aTolerance)
        {
            return (a >= (b + aTolerance));
        }

        public static bool GreaterThan(Vector3 u, Vector3 v)
        {
            return (GreaterThan(u.X, v.X) && 
                    GreaterThan(u.Y, v.Y) && 
                    GreaterThan(u.Z, v.Z));
        }

        public static bool GreaterThan(Vector3 u, Vector3 v, float aTolerance)
        {
            return (GreaterThan(u.X, v.X, aTolerance) &&
                    GreaterThan(u.Y, v.Y, aTolerance) &&
                    GreaterThan(u.Z, v.Z, aTolerance));
        }
        #endregion

        #region LessThan
        public static bool LessThan(float a, float b)
        {
            return (a <= (b - kZeroToleranceFloat));
        }

        public static bool LessThan(float a, float b, float aTolerance)
        {
            return (a <= (b - aTolerance));
        }

        public static bool LessThan(double a, double b)
        {
            return (a <= (b - kZeroToleranceDouble));
        }

        public static bool LessThan(double a, double b, double aTolerance)
        {
            return (a <= (b - aTolerance));
        }
        
        public static bool LessThan(Vector3 u, Vector3 v)
        {
            return (LessThan(u.X, v.X) && LessThan(u.Y, v.Y) && LessThan(u.Z, v.Z));
        }

        public static bool LessThan(Vector3 u, Vector3 v, float aTolerance)
        {
            return (LessThan(u.X, v.X, aTolerance) &&
                    LessThan(u.Y, v.Y, aTolerance) &&
                    LessThan(u.Z, v.Z, aTolerance));
        }
        #endregion

        #region Min/Max
        public static T Min<T>(T a, T b) where T : IComparable<T>
        {
            if (a.CompareTo(b) < 0) { return a; }
            else { return b; }
        }

        public static T Min<T>(T a, T b, T c) where T : IComparable<T>
        {
            return Min(a, Min(b, c));
        }

        public static T Min<T>(T a, T b, T c, T d) where T : IComparable<T>
        {
            return Min(a, b, Min(c, d));
        }

        public static float Min(ref Vector3 v)
        {
            return Min(v.X, v.Y, v.Z);
        }

        public static float Min(ref Vector4 v)
        {
            return Min(v.X, v.Y, v.Z, v.W);
        }

        public static T Max<T>(T a, T b) where T : IComparable<T>
        {
            if (a.CompareTo(b) > 0) { return a; }
            else { return b; }
        }

        public static T Max<T>(T a, T b, T c) where T : IComparable<T>
        {
            return Max(a, Max(b, c));
        }

        public static T Max<T>(T a, T b, T c, T d) where T : IComparable<T>
        {
            return Max(a, b, Max(c, d));
        }

        public static float Max(ref Vector3 v)
        {
            return Max(v.X, v.Y, v.Z);
        }

        public static float Max(Vector3 v)
        {
            return Max(v.X, v.Y, v.Z);
        }

        public static float Max(ref Vector4 v)
        {
            return Max(v.X, v.Y, v.Z, v.W);
        }

        public static float Max(Vector4 v)
        {
            return Max(v.X, v.Y, v.Z, v.W);
        }
        #endregion

        #region Geometry
        private static void FindEigenspaceHelper(ref Matrix3 R, ref float m1, ref float m2, ref float m3, ref float m4, ref float m5, int i1, int i2)
        {
            if (!AboutZero(m2))
            {
                float u = (0.5f * (m4 - m1)) / m2;
                float u2 = u * u;
                float u2p1 = u2 + 1.0f;
                float t = !AboutEqual(u2p1, u2) ? (LessThan(u, 0.0f) ? -1.0f : 1.0f) * ((float)(Math.Sqrt(u2p1)) - Math.Abs(u)) : 0.5f / u;
                float c = 1.0f / ((float)(Math.Sqrt((t * t) + 1.0f)));
                float s = c * t;

                m1 -= t * m2;
                m4 += t * m2;
                m2 = 0.0f;

                float t1 = (c * m3) - (s * m5);
                m5 = (s * m3) + (c * m5);
                m3 = t1;

                for (int i = 0; i < 3; i++)
                {
                    float t2 = (c * R[i, i1]) - (s * R[i, i2]);
                    R[i, i2] = (s * R[i, i1]) + (c * R[i, i2]);
                    R[i, i1] = t2;
                }
            }
        }

        // From: http://www.terathon.com/code/linear.php
        public static void FindEigenspace(ref Matrix3 M, out Vector3 arValues, out Matrix3 arVectors)
        {
            float m11 = M.M11; float m12 = M.M12; float m13 = M.M13;
            float m22 = M.M22; float m23 = M.M23;
            float m33 = M.M33;

            Matrix3 R = Matrix3.Identity;

            for (int i = 0; i < kFindEigenspaceIterations; i++)
            {
                if (AboutZero(Math.Abs(m12), kJacobiToleranceFloat) &&
                    AboutZero(Math.Abs(m13), kJacobiToleranceFloat) &&
                    AboutZero(Math.Abs(m23), kJacobiToleranceFloat))
                {
                    break;
                }

                FindEigenspaceHelper(ref R, ref m11, ref m12, ref m13, ref m22, ref m23, 0, 1);
                FindEigenspaceHelper(ref R, ref m11, ref m13, ref m12, ref m33, ref m23, 0, 2);
                FindEigenspaceHelper(ref R, ref m22, ref m23, ref m12, ref m33, ref m13, 1, 2);
            }

            arValues.X = m11;
            arValues.Y = m22;
            arValues.Z = m33;

            arVectors = R;
        }

        /// <summary>
        /// Calculates the three primary axes of a container of points using
        /// Principal components analysis (PCA). See 
        /// http://en.wikipedia.org/wiki/Principal_components_analysis for a summary.
        /// These axes represent the orthogonal three axes of most change of the points.
        /// These will roughly correspond to the "natural" axes of a geometric figure.
        /// </summary>
        /// <param name="aPoints">A container of position vertices.</param>
        /// <param name="arAxisR">Principal R axis. Axis of most change.</param>
        /// <param name="arAxisS">Principal S axis. Axis of second most change.</param>
        /// <param name="arAxisT">Principal T axis. Axis of third most change.</param>
        /// <returns>The number of points in the container aPoints.</returns>
        public static int CalculatePrincipalComponentAxes(IEnumerable<Vector3> aPoints, out Vector3 arAxisR, out Vector3 arAxisS, out Vector3 arAxisT)
        {
            int count = 0;
            Vector3 mean = Vector3.Zero;

            foreach (Vector3 u in aPoints)
            {
                mean += u;
                count++;
            }

            float invCount = (count > 0) ? ((float)(1.0 / ((double)count))) : 0.0f;
            mean *= invCount;

            float m11 = 0.0f;
            float m22 = 0.0f;
            float m33 = 0.0f;
            float m12 = 0.0f;
            float m13 = 0.0f;
            float m23 = 0.0f;

            foreach (Vector3 u in aPoints)
            {
                Vector3 v = u - mean;
                m11 += v.X * v.X;
                m22 += v.Y * v.Y;
                m33 += v.Z * v.Z;
                m12 += v.X * v.Y;
                m13 += v.X * v.Z;
                m23 += v.Y * v.Z;
            }

            m11 *= invCount;
            m22 *= invCount;
            m33 *= invCount;
            m12 *= invCount;
            m13 *= invCount;
            m23 *= invCount;

            Matrix3 m = new Matrix3(m11, m12, m13, m12, m22, m23, m13, m23, m33);
            Vector3 eigenValues;
            Matrix3 eigenVectors;
            Utilities.FindEigenspace(ref m, out eigenValues, out eigenVectors);

            m11 = Math.Abs(eigenValues.X);
            m22 = Math.Abs(eigenValues.Y);
            m33 = Math.Abs(eigenValues.Z);

            if (Utilities.GreaterThan(m11, m22) && Utilities.GreaterThan(m11, m33))
            {
                arAxisR = new Vector3(eigenVectors.M11, eigenVectors.M21, eigenVectors.M31);

                if (Utilities.GreaterThan(m22, m33))
                {
                    arAxisS = new Vector3(eigenVectors.M12, eigenVectors.M22, eigenVectors.M32);
                    arAxisT = new Vector3(eigenVectors.M13, eigenVectors.M23, eigenVectors.M33);
                }
                else
                {
                    arAxisS = new Vector3(eigenVectors.M13, eigenVectors.M23, eigenVectors.M33);
                    arAxisT = new Vector3(eigenVectors.M12, eigenVectors.M22, eigenVectors.M32);
                }
            }
            else if (Utilities.GreaterThan(m22, m33))
            {
                arAxisR = new Vector3(eigenVectors.M12, eigenVectors.M22, eigenVectors.M32);

                if (Utilities.GreaterThan(m11, m33))
                {
                    arAxisS = new Vector3(eigenVectors.M11, eigenVectors.M21, eigenVectors.M31);
                    arAxisT = new Vector3(eigenVectors.M13, eigenVectors.M23, eigenVectors.M33);
                }
                else
                {
                    arAxisS = new Vector3(eigenVectors.M13, eigenVectors.M23, eigenVectors.M33);
                    arAxisT = new Vector3(eigenVectors.M11, eigenVectors.M21, eigenVectors.M31);
                }
            }
            else
            {
                arAxisR = new Vector3(eigenVectors.M13, eigenVectors.M23, eigenVectors.M33);

                if (Utilities.GreaterThan(m11, m22))
                {
                    arAxisS = new Vector3(eigenVectors.M11, eigenVectors.M21, eigenVectors.M31);
                    arAxisT = new Vector3(eigenVectors.M12, eigenVectors.M22, eigenVectors.M32);
                }
                else
                {
                    arAxisS = new Vector3(eigenVectors.M12, eigenVectors.M22, eigenVectors.M32);
                    arAxisT = new Vector3(eigenVectors.M11, eigenVectors.M21, eigenVectors.M31);
                }
            }

            arAxisR.Normalize();
            arAxisS.Normalize();
            arAxisT.Normalize();

            Matrix reflection = new Matrix(arAxisR.X, arAxisR.Y, arAxisR.Z, 0.0f,
                                  arAxisS.X, arAxisS.Y, arAxisS.Z, 0.0f,
                                  arAxisT.X, arAxisT.Y, arAxisT.Z, 0.0f,
                                  0, 0, 0, 1);

            if (Utilities.IsReflection(ref reflection)) { arAxisT = -arAxisT; }

            return count;
        }

        /// <summary>
        /// Given a container a points and three "natural" axes of the points, this function
        /// calculates the center of the points.
        /// </summary>
        /// <param name="aPoints">A container of points.</param>
        /// <param name="aAxisR">The axis of most change.</param>
        /// <param name="aAxisS">The second most axis of change.</param>
        /// <param name="aAxisT">The third most axis of change.</param>
        /// <param name="arCenter">The output center of the points.</param>
        public static void CalculateCenter(IEnumerable<Vector3> aPoints, ref Vector3 aAxisR, ref Vector3 aAxisS, ref Vector3 aAxisT, out Vector3 arCenter)
        {
            Vector3 min = kMaxVector3;
            Vector3 max = kMinVector3;

            foreach (Vector3 v in aPoints)
            {
                Vector3 a;
                a.X = Vector3.Dot(v, aAxisR);
                a.Y = Vector3.Dot(v, aAxisS);
                a.Z = Vector3.Dot(v, aAxisT);

                min = Vector3.Min(min, a);
                max = Vector3.Max(max, a);
            }

            Vector3 abc = 0.5f * (max + min);

            arCenter = (abc.X * aAxisR) + (abc.Y * aAxisS) + (abc.Z * aAxisT);
        }

        /// <summary>
        /// Given a three orthonormal axes and a collection of points, this function will
        /// return the center and half extents of the axes R, S, T that form a tightest fitting 
        /// OBB around the collection of points.
        /// </summary>
        /// <param name="aPoints">The collection of points.</param>
        /// <param name="arCenter">Origin of the coordinate frame formed by R, S, T.</param>
        /// <param name="aAxisR">Axis of most change.</param>
        /// <param name="aAxisS">Axis of second most change.</param>
        /// <param name="aAxisT">Axis of third most change.</param>
        /// <param name="arHalfExtents">Output half extents of R, S, T stored as X, Y, Z.</param>
        public static void CalculateCenterAndHalfExtents(IEnumerable<Vector3> aPoints, ref Vector3 aAxisR, ref Vector3 aAxisS, ref Vector3 aAxisT, out Vector3 arCenter, out Vector3 arHalfExtents)
        {
            Vector3 min = kMaxVector3;
            Vector3 max = kMinVector3;

            foreach (Vector3 v in aPoints)
            {
                Vector3 a;
                a.X = Vector3.Dot(v, aAxisR);
                a.Y = Vector3.Dot(v, aAxisS);
                a.Z = Vector3.Dot(v, aAxisT);

                min = Vector3.Min(min, a);
                max = Vector3.Max(max, a);
            }

            Vector3 abc = 0.5f * (max + min);

            arCenter = (abc.X * aAxisR) + (abc.Y * aAxisS) + (abc.Z * aAxisT);
            arHalfExtents = 0.5f * (max - min);
        }

        /// <summary>
        /// Gvien an origin and three orthonormal axes, this function will return a matrix
        /// that will transform a point into the local coordinate frame formed by the
        /// origin and axes. This function chooses the axes X, Y, Z for R, S, T such that 
        /// the overall orientation change is minimal.
        /// </summary>
        /// <param name="aCenter">Origin of the new coordinate frame.</param>
        /// <param name="aAxisR">Axis of most change.</param>
        /// <param name="aAxisS">Axis of second most change.</param>
        /// <param name="aAxisT">Axis of third most change.</param>
        /// <param name="arOut">Output transform.</param>
        public static void CalculateMatrix(ref Vector3 aCenter, ref Vector3 aAxisR, ref Vector3 aAxisS, ref Vector3 aAxisT, out Matrix arOut)
        {
            arOut = new Matrix(
                aAxisR.X, aAxisR.Y, aAxisR.Z, 0.0f,
                aAxisS.X, aAxisS.Y, aAxisS.Z, 0.0f,
                aAxisT.X, aAxisT.Y, aAxisT.Z, 0.0f,
                aCenter.X, aCenter.Y, aCenter.Z, 1.0f);
        }

        public static BoundingBox CreateBoxFromPoints(IEnumerable<Vector3> e)
        {
            Vector3 min = Utilities.kMaxVector3;
            Vector3 max = Utilities.kMinVector3;

            foreach (Vector3 v in e)
            {
                min = Vector3.Min(min, v);
                max = Vector3.Max(max, v);
            }

            BoundingBox ret = (new BoundingBox(min, max));

            return ret;
        }

        public static BoundingSphere CreateSphereFromPoints(IEnumerable<Vector3> e)
        {
            Vector3 r, s, t;
            Vector3 halfExtents;

            BoundingSphere bs;
            CalculatePrincipalComponentAxes(e, out r, out s, out t);
            CalculateCenterAndHalfExtents(e, ref r, ref s, ref t, out bs.Center, out halfExtents);
            bs.Radius = halfExtents.Length();

            foreach (Vector3 v in e)
            {
                Vector3 a = (v - bs.Center);
                float n = a.LengthSquared();

                if (Utilities.GreaterThan(n, (bs.Radius * bs.Radius), Utilities.kLooseToleranceFloat))
                {
                    Vector3 b = (bs.Center - (a * (1.0f / ((float)Math.Sqrt(n)))) * bs.Radius);

                    bs.Center = 0.5f * (b + v);
                    bs.Radius = Vector3.Distance(b, bs.Center);
                }
            }

            return bs;
        }
        #endregion

        public static T Clamp<T>(T a, T aMin, T aMax) where T : IComparable<T>
        {
            if (a.CompareTo(aMax) > 0) { return aMax; }
            else if (a.CompareTo(aMin) < 0) { return aMin; }
            else { return a; }
        }

        public static byte[] Convert(float[] aIn)
        {
            if (aIn != null)
            {
                int inCount = aIn.Length;
                int outCount = (inCount * sizeof(float));
                byte[] ret = new byte[outCount];

                for (int input = 0, output = 0; input < inCount; input++, output += sizeof(float))
                {
                    byte[] buf = BitConverter.GetBytes(aIn[input]);
                    Array.Copy(buf, 0, ret, output, sizeof(float));
                }

                return ret;
            }
            else
            {
                return null;
            }
        }

        public static CompilerMacro CreateMacro(string aName, string aDefinition)
        {
            CompilerMacro ret = new CompilerMacro();
            ret.Name = aName;
            ret.Definition = aDefinition;

            return ret;
        }

        public static float Cross(Vector2 u, Vector2 v)
        {
            float ret = ((u.X * v.Y) - (u.Y * v.X));

            return ret;
        }
        
        public static string ExtractBaseFilename(string aFilename)
        {
            string ret = Utilities.RemoveExtension(aFilename);
            ret = ret.Trim();

            return ret;
        }

        public static string ExtractRelativeFilename(string aInDirectory, string aFilename)
        {
            string ret = aFilename;
            int index = aFilename.LastIndexOf(aInDirectory);
            if (index >= 0) { ret = aFilename.Substring(index + aInDirectory.Length); }
            else { ret = Path.GetFileName(aFilename); }

            ret = ret.TrimStart(Path.DirectorySeparatorChar);
            ret = ret.Trim();

            return ret;
        }

        public static void FlipTextureV(VertexElement[] aVertexDeclaration, byte[] aVertices)
        {
            int vertexLength = (aVertices.Length);
            int vertexStrideInBytes = (aVertexDeclaration[aVertexDeclaration.Length - 1].Offset + GetSizeInBytes(aVertexDeclaration[aVertexDeclaration.Length - 1].VertexElementFormat));

            for (int i = 0; i < aVertexDeclaration.Length; i++)
            {
                if (aVertexDeclaration[i].VertexElementUsage == VertexElementUsage.TextureCoordinate)
                {
                    for (int j = 0; j < vertexLength; j += vertexStrideInBytes)
                    {
                        int index = (j + aVertexDeclaration[i].Offset + sizeof(float));

                        float v = BitConverter.ToSingle(aVertices, index);
                        v = (1.0f - v);
                        Array.Copy(BitConverter.GetBytes(v), 0, aVertices, index, sizeof(float));
                    }
                }
            }
        }

        public static void FlipWindingOrder(PrimitiveType aType, UInt16[] arIndices)
        {
            int count = arIndices.Length;
            if (aType == PrimitiveType.TriangleList)
            {
                for (int i = 1; i + 1 < count; i += 3)
                {
                    UInt16 t = arIndices[i];
                    arIndices[i] = arIndices[i + 1];
                    arIndices[i + 1] = t;
                }
            }
            else if (aType == PrimitiveType.TriangleFan)
            {
                if (count > 1)
                {
                    Array.Reverse(arIndices, 1, count - 1);
                }
            }
            else if (aType == PrimitiveType.TriangleStrip)
            {
                if (count > 2)
                {
                    UInt16 t = arIndices[1];
                    arIndices[1] = arIndices[2];
                    arIndices[2] = t;
                }
            }
        }

        public static string FromUriFileToPath(string aUriBase, string aUriFile)
        {
            string ret = Uri.UnescapeDataString(aUriFile);
            Uri uri = new Uri(new Uri(aUriBase), ret);
            ret = uri.AbsolutePath;
            ret = Uri.UnescapeDataString(ret);
            ret = Path.GetFullPath(ret);

            return ret;
        }

        public static Vector3 GetCenter(BoundingBox aAABB)
        {
            return 0.5f * (aAABB.Max + aAABB.Min);
        }

        public static float GetElement(Vector2 v, int i)
        {
            switch (i)
            {
                case 0: return v.X;
                case 1: return v.Y;
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        public static float GetElement(Vector3 v, int i)
        {
            switch (i)
            {
                case 0: return v.X;
                case 1: return v.Y;
                case 2: return v.Z;
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        public static float GetElement(Vector4 v, int i)
        {
            switch (i)
            {
                case 0: return v.X;
                case 1: return v.Y;
                case 2: return v.Z;
                case 3: return v.W;
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        public static Vector3 GetExtents(BoundingBox aAABB)
        {
            return (aAABB.Max - aAABB.Min);
        }

        public static Vector3 GetHalfExtents(BoundingBox aAABB)
        {
            return 0.5f * GetExtents(aAABB);
        }

        public static float GetInverseSurfaceArea(BoundingBox aBox)
        {
            Vector3 whd = (aBox.Max - aBox.Min);
            float invSa = 0.5f / ((whd.Z * whd.X) + (whd.Z * whd.Y) + (whd.X * whd.Y));

            return invSa;
        }

        public static int GetSizeInBytes(VertexElementFormat aFormat)
        {
            switch (aFormat)
            {
                case VertexElementFormat.Byte4: return 4;
                case VertexElementFormat.Color: return 4;
                case VertexElementFormat.HalfVector2: return 4;
                case VertexElementFormat.HalfVector4: return 8;
                case VertexElementFormat.Normalized101010: return 4;
                case VertexElementFormat.NormalizedShort2: return 4;
                case VertexElementFormat.NormalizedShort4: return 8;
                case VertexElementFormat.Rg32: return 4;
                case VertexElementFormat.Rgba32: return 4;
                case VertexElementFormat.Rgba64: return 8;
                case VertexElementFormat.Short2: return 4;
                case VertexElementFormat.Short4: return 8;
                case VertexElementFormat.Single: return 4;
                case VertexElementFormat.UInt101010: return 4;
                case VertexElementFormat.Vector2: return 8;
                case VertexElementFormat.Vector3: return 12;
                case VertexElementFormat.Vector4: return 16;
                default:
                    throw new Exception(Utilities.kShouldNotBeHere);
            }
        }

        public static float GetSurfaceArea(BoundingBox aBox)
        {
            Vector3 whd = aBox.Max - aBox.Min;
            float sa = 2.0f * ((whd.Z * whd.X) + (whd.Z * whd.Y) + (whd.X * whd.Y));

            return sa;
        }

        public static bool IsNaN(BoundingBox bb)
        {
            return (IsNaN(bb.Min) || IsNaN(bb.Max));
        }

        public static bool IsNaN(BoundingSphere bs)
        {
            return (IsNaN(bs.Center) || float.IsNaN(bs.Radius));
        }

        public static bool IsNaN(Matrix m)
        {
            return (float.IsNaN(m.M11) || float.IsNaN(m.M12) || float.IsNaN(m.M13) || float.IsNaN(m.M14) ||
                    float.IsNaN(m.M21) || float.IsNaN(m.M22) || float.IsNaN(m.M23) || float.IsNaN(m.M24) ||
                    float.IsNaN(m.M31) || float.IsNaN(m.M32) || float.IsNaN(m.M33) || float.IsNaN(m.M34) ||
                    float.IsNaN(m.M41) || float.IsNaN(m.M42) || float.IsNaN(m.M43) || float.IsNaN(m.M44));
        }

        public static bool IsNaN(Quaternion q)
        {
            return (float.IsNaN(q.X) || float.IsNaN(q.Y) || float.IsNaN(q.Z) || float.IsNaN(q.W));
        }

        public static bool IsNaN(Vector2 u)
        {
            return (float.IsNaN(u.X) || float.IsNaN(u.Y));
        }

        public static bool IsNaN(Vector3 u)
        {
            return (float.IsNaN(u.X) || float.IsNaN(u.Y) || float.IsNaN(u.Z));
        }

        public static bool IsNaN(Vector4 u)
        {
            return (float.IsNaN(u.X) || float.IsNaN(u.Y) || float.IsNaN(u.Z) || float.IsNaN(u.W));
        }

        public static bool IsReflection(ref Matrix m)
        {
            bool bReturn = LessThan(UpperLeft3x3Determinant(ref m), 0.0f);
            return bReturn;
        }

        public static float UpperLeft3x3Determinant(ref Matrix m)
        {
            float ret = ((m.M11 * m.M22 * m.M33) + (m.M12 * m.M23 * m.M31) + (m.M13 * m.M21 * m.M32)) -
                        ((m.M31 * m.M22 * m.M13) + (m.M32 * m.M23 * m.M11) + (m.M33 * m.M21 * m.M12));

            return ret;
        }

        public static float Luminance(Vector3 v)
        {
            float ret = (0.2126f * v.X) + (0.7152f * v.Y) + (0.0722f * v.Z);

            return ret;
        }

        public static float Luminance(Vector4 v)
        {
            return Luminance(new Vector3(v.X, v.Y, v.Z));
        }

        public static int PrimitiveCount(PrimitiveType aType, int aIndexBufferSize)
        {
            if (aIndexBufferSize <= 0)
            {
                return 0;
            }
            else
            {
                switch (aType)
                {
                    case PrimitiveType.LineList: return aIndexBufferSize / 2;
                    case PrimitiveType.LineStrip: return aIndexBufferSize - 1;
                    case PrimitiveType.PointList: return aIndexBufferSize;
                    case PrimitiveType.TriangleFan: return aIndexBufferSize - 2;
                    case PrimitiveType.TriangleList: return aIndexBufferSize / 3;
                    case PrimitiveType.TriangleStrip: return aIndexBufferSize - 2;
                    default:
                        throw new Exception(Utilities.kShouldNotBeHere);
                }
            }
        }

        public static void Remove<T>(int[] aToRemove, ref T[] a)
        {
            if (aToRemove != null && aToRemove.Length > 0)
            {
                Array.Sort(aToRemove);

                int removeIndex = 0;
                int outIndex = 0;
                int count = a.Length;
                for (int i = 0; i < count; i++)
                {
                    if (i == aToRemove[removeIndex])
                    {
                        removeIndex++;
                    }
                    else
                    {
                        a[outIndex++] = a[i];
                    }
                }

                Array.Resize(ref a, outIndex);
            }
        }

        public static string RemoveExtension(string aPath)
        {
            string directory = System.IO.Path.GetDirectoryName(aPath);
            string ret = string.Empty;

            if (directory != string.Empty)
            {
                ret = directory + System.IO.Path.DirectorySeparatorChar + System.IO.Path.GetFileNameWithoutExtension(aPath);
            }
            else
            {
                ret = System.IO.Path.GetFileNameWithoutExtension(aPath);
            }

            return ret;
        }

        public static Vector2 SafeNormalize(Vector2 v)
        {
            float len = v.Length();

            if (!Utilities.AboutZero(len)) { return (v / len); }
            else { return v; }
        }

        public static Vector3 SafeNormalize(Vector3 v)
        {
            float len = v.Length();

            if (!Utilities.AboutZero(len)) { return (v / len); }
            else { return v; }
        }

        public static void SetElement(ref Vector2 v, int i, float s)
        {
            switch (i)
            {
                case 0: v.X = s; break;
                case 1: v.Y = s; break;
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        public static void SetElement(ref Vector3 v, int i, float s)
        {
            switch (i)
            {
                case 0: v.X = s; break;
                case 1: v.Y = s; break;
                case 2: v.Z = s; break;
                default:
                    throw new ArgumentOutOfRangeException();
            }
        }

        public static void Swap<T>(ref T a, ref T b)
        {
            T tmp;
            tmp = a;
            a = b;
            b = tmp;
        }

        #region Tokenize
        public delegate T TokenizeConvertFunction<T>(string aToken);
        public static void Tokenize<T>(string aInput, T[] aArray, TokenizeConvertFunction<T> aFunc)
        {
            string input = aInput.Trim();
            int numFound = 0;
            int i = 0;
            int length = input.Length;
            int expectedCount = aArray.Length;

            while (i < length && numFound < expectedCount)
            {
                string token = "";

                while (i < length && !char.IsWhiteSpace(input[i]))
                {
                    token += input[i];
                    i++;
                }

                aArray[numFound] = aFunc(token);
                numFound++;

                while (i < length && char.IsWhiteSpace(input[i]))
                {
                    i++;
                }
            }

            if (i != length || numFound != expectedCount)
            {
                throw new Exception("Token count was not expected number.");
            }
        }

        public static void Tokenize(string aInput, string[] aArray)
        {
            Tokenize<string>(aInput, aArray, delegate(String aIn) { return aIn; });
        }

        public static T[] Tokenize<T>(string aInput, TokenizeConvertFunction<T> aFunc)
        {
            List<T> buf = new List<T>();

            string input = aInput.Trim();
            int i = 0;
            int length = input.Length;

            while (i < length)
            {
                string token = "";

                while (i < length && !char.IsWhiteSpace(input[i]))
                {
                    token += input[i];
                    i++;
                }

                buf.Add(aFunc(token));

                while (i < length && char.IsWhiteSpace(input[i]))
                {
                    i++;
                }
            }

            if (i != length)
            {
                throw new Exception("Tokenizing ended before string end.");
            }

            return buf.ToArray();
        }

        public static string[] Tokenize(string aInput)
        {
            return Tokenize<string>(aInput, delegate(String aIn) { return aIn; });
        }
        #endregion

        public static BoundingBox Transform(BoundingBox aabb, Matrix m)
        {
            Vector3[] corners = aabb.GetCorners();
            Vector3.Transform(corners, ref m, corners);

            BoundingBox ret = BoundingBox.CreateFromPoints(corners);

            return ret;
        }

        public static BoundingSphere Transform(BoundingSphere s, Matrix m)
        {
            Vector3 x = Vector3.TransformNormal(Vector3.UnitX, m);
            Vector3 y = Vector3.TransformNormal(Vector3.UnitY, m);
            Vector3 z = Vector3.TransformNormal(Vector3.UnitZ, m);

            BoundingSphere ret;
            ret.Center = Vector3.Transform(s.Center, m);
            ret.Radius = s.Radius * Max(x.Length(), y.Length(), z.Length());

            return ret;
        }

        /// <summary>
        /// Generates indices and vertices for a sphere mesh.
        /// </summary>
        /// <param name="r">Radius of the sphere.</param>
        /// <param name="n">Segments per 360° ring of the sphere.</param>
        /// <param name="abOversize">If abOversize is true, the midpoint of segments will be r from the center of the sphere. Otherwise, vertices will be r from the center of the sphere.</param>
        /// <param name="arIndices">Generated vertices of the sphere.</param>
        /// <param name="arVertices">Generated indices of the sphere.</param>
        /// <remarks>
        /// From: http://www.codeproject.com/KB/WPF/XamlUVSphere.aspx 
        /// - Author: Rainer Stropek
        /// </remarks>
        public static void GenerateSphere(float r, int n, bool abOversize, out int[] arIndices, out float[] arVertices)
        {
            #region Build
            {
                if (n < 4) { n = 4; }
                if (n % 2 != 0) { n++; }
                int vn = (n - 1);
                float step = MathHelper.TwoPi / ((float)n);

                if (abOversize)
                {
                    r = (r / ((float)Math.Cos(step * 0.5)));
                }

                List<int> indices = new List<int>();
                List<float> vertices = new List<float>();

                #region Vertices
                {
                    vertices.Add(0); vertices.Add(r); vertices.Add(0);
                    for (int i = 1; i < (n / 2); i++)
                    {
                        float theta = (step * i);
                        float y = (r * (float)Math.Cos(theta));

                        float c = (r * (float)Math.Sin(theta));
                        for (int j = 0; j < vn; j++)
                        {
                            float ro = (step * j);
                            float x = (c * (float)Math.Sin(ro));
                            float z = (c * (float)Math.Cos(ro));

                            vertices.Add(x); vertices.Add(y); vertices.Add(z);
                        }
                    }
                    vertices.Add(0); vertices.Add(-r); vertices.Add(0);
                }
                #endregion

                #region Top candy wrapper twist.
                {
                    int prev = (vn - 1);
                    for (int i = 0; i < vn; prev = i, i++)
                    {
                        indices.Add(0);
                        indices.Add(1 + i);
                        indices.Add(1 + prev);
                    }
                }
                #endregion

                #region Internal triangles
                {
                    for (int i = 0; i < ((n / 2) - 2); i++)
                    {
                        // +1 to skip top candy wrapper twist.
                        int offset = (i * vn) + 1;
                        int prev = (vn - 1);
                        for (int j = 0; j < vn; prev = j, j++)
                        {
                            int i0 = (offset + prev);
                            int i1 = (offset + j);
                            int i2 = (offset + prev + vn);
                            int i3 = (offset + j + vn);

                            indices.Add(i0); indices.Add(i1); indices.Add(i2);
                            indices.Add(i2); indices.Add(i1); indices.Add(i3);
                        }
                    }
                }
                #endregion

                #region Bottom candy wrapper twist.
                {
                    // +1 to skip top candy wrapper twist.
                    int offset = (((n / 2) - 2) * vn) + 1;
                    int prev = (vn - 1);
                    int last = ((vertices.Count / 3) - 1);

                    for (int i = 0; i < vn; prev = i, i++)
                    {
                        indices.Add(offset + prev);
                        indices.Add(offset + i);
                        indices.Add(last);
                    }
                }
                #endregion

                arIndices = indices.ToArray();
                arVertices = vertices.ToArray();
            }
            #endregion
        }

        public struct IndexComparable : IComparable<IndexComparable>
        {
            public UInt16 I0, I1, I2;
            public readonly bool bIgnoreNormal;

            public IndexComparable(bool abIgnoreNormal)
            {
                I0 = 0;
                I1 = 0;
                I2 = 0;
                bIgnoreNormal = abIgnoreNormal;
            }

            public bool AnyEqual(IndexComparable b)
            {
                if (I0 == b.I0 || I0 == b.I1 || I0 == b.I2) { return true; }
                if (I1 == b.I0 || I1 == b.I1 || I1 == b.I2) { return true; }
                if (I2 == b.I0 || I2 == b.I1 || I2 == b.I2) { return true; }

                return false;
            }

            public int CompareTo(IndexComparable b)
            {
                int ai0 = I0;
                int ai1 = I1;
                int ai2 = I2;
                if (bIgnoreNormal)
                {
                    if (ai0 > ai1) { Utilities.Swap(ref ai0, ref ai1); }
                    if (ai1 > ai2) { Utilities.Swap(ref ai1, ref ai2); }
                    if (ai0 > ai1) { Utilities.Swap(ref ai0, ref ai1); }
                }

                int bi0 = b.I0;
                int bi1 = b.I1;
                int bi2 = b.I2;
                if (bIgnoreNormal)
                {
                    if (bi0 > bi1) { Utilities.Swap(ref bi0, ref bi1); }
                    if (bi1 > bi2) { Utilities.Swap(ref bi1, ref bi2); }
                    if (bi0 > bi1) { Utilities.Swap(ref bi0, ref bi1); }
                }

                int c0 = ai0.CompareTo(bi0);
                int c1 = ai1.CompareTo(bi1);
                int c2 = ai2.CompareTo(bi2);

                if (c0 == 0)
                {
                    if (c1 == 0)
                    {
                        if (c2 == 0) { return 0; }
                        else { return c2; }
                    }
                    else { return c1; }
                }
                else { return c0; }
            }
        }
                
        public sealed class TriangleComparer : IEqualityComparer<Triangle>
        {
            #region Private membets
            private readonly bool mbConsiderNormal = true;
            private readonly float mDistanceSquare;
            #endregion

            public TriangleComparer(bool abConsiderNormal, float aDistanceSquareTolerance)
            {
                mbConsiderNormal = abConsiderNormal;
                mDistanceSquare = aDistanceSquareTolerance;
            }

            public bool Equals(Triangle a, Triangle b)
            {
                if (Vector3.DistanceSquared(a.P0, b.P0) <= mDistanceSquare &&
                    Vector3.DistanceSquared(a.P1, b.P1) <= mDistanceSquare &&
                    Vector3.DistanceSquared(a.P2, b.P2) <= mDistanceSquare)
                {
                    return true;
                }

                if (Vector3.DistanceSquared(a.P0, b.P1) <= mDistanceSquare &&
                    Vector3.DistanceSquared(a.P1, b.P2) <= mDistanceSquare &&
                    Vector3.DistanceSquared(a.P2, b.P0) <= mDistanceSquare)
                {
                    return true;
                }

                if (Vector3.DistanceSquared(a.P0, b.P2) <= mDistanceSquare &&
                    Vector3.DistanceSquared(a.P1, b.P0) <= mDistanceSquare &&
                    Vector3.DistanceSquared(a.P2, b.P1) <= mDistanceSquare)
                {
                    return true;
                }

                if (!mbConsiderNormal)
                {
                    if (Vector3.DistanceSquared(a.P0, b.P0) <= mDistanceSquare &&
                        Vector3.DistanceSquared(a.P1, b.P2) <= mDistanceSquare &&
                        Vector3.DistanceSquared(a.P2, b.P1) <= mDistanceSquare)
                    {
                        return true;
                    }

                    if (Vector3.DistanceSquared(a.P0, b.P2) <= mDistanceSquare &&
                        Vector3.DistanceSquared(a.P1, b.P1) <= mDistanceSquare &&
                        Vector3.DistanceSquared(a.P2, b.P0) <= mDistanceSquare)
                    {
                        return true;
                    }

                    if (Vector3.DistanceSquared(a.P0, b.P1) <= mDistanceSquare &&
                        Vector3.DistanceSquared(a.P1, b.P0) <= mDistanceSquare &&
                        Vector3.DistanceSquared(a.P2, b.P2) <= mDistanceSquare)
                    {
                        return true;
                    }
                }

                return false;
            }

            public int GetHashCode(Triangle t)
            {
                string hashString = string.Empty;
                hashString += t.P0.X.ToString("N0") + ":";
                hashString += t.P0.Y.ToString("N0") + ":";
                hashString += t.P0.Z.ToString("N0") + ":";

                hashString += t.P1.X.ToString("N0") + ":";
                hashString += t.P1.Y.ToString("N0") + ":";
                hashString += t.P1.Z.ToString("N0") + ":";

                hashString += t.P2.X.ToString("N0") + ":";
                hashString += t.P2.Y.ToString("N0") + ":";
                hashString += t.P2.Z.ToString("N0");

                int hash = hashString.GetHashCode();

                return hash;
            }
        }

        public sealed class VertexComparable : IComparable<VertexComparable>
        {
            #region Private members
            private VertexElement[] mDeclaration;
            private readonly float kDistanceSqTolerance;
            private readonly int mStrideInBytes = 0;
            #endregion

            public byte[] Vertices;

            public VertexComparable(VertexElement[] decl, float aDistanceSquareTolerance)
            {
                mDeclaration = decl;
                kDistanceSqTolerance = aDistanceSquareTolerance;
                mStrideInBytes = decl[decl.Length - 1].Offset + Utilities.GetSizeInBytes(decl[decl.Length - 1].VertexElementFormat);

                Vertices = new byte[mStrideInBytes];
            }

            public int StrideInBytes { get { return mStrideInBytes; } }

            public byte this[int i]
            {
                get
                {
                    return Vertices[i];
                }

                set
                {
                    Vertices[i] = value;
                }
            }

            public int CompareTo(VertexComparable b)
            {
                Debug.Assert(Vertices.Length == b.Vertices.Length);
                Debug.Assert(mDeclaration == b.mDeclaration);

                int count = mDeclaration.Length;
                for (int i = 0; i < count; i++)
                {
                    VertexElement e = mDeclaration[i];

                    if (e.VertexElementUsage == VertexElementUsage.Position)
                    {
                        Vector3 p0 = new Vector3(
                            BitConverter.ToSingle(Vertices, e.Offset + 0),
                            BitConverter.ToSingle(Vertices, e.Offset + (sizeof(float))),
                            BitConverter.ToSingle(Vertices, e.Offset + (2 * sizeof(float))));

                        Vector3 p1 = new Vector3(
                            BitConverter.ToSingle(b.Vertices, e.Offset + 0),
                            BitConverter.ToSingle(b.Vertices, e.Offset + (sizeof(float))),
                            BitConverter.ToSingle(b.Vertices, e.Offset + (2 * sizeof(float))));

                        if (Vector3.DistanceSquared(p0, p1) <= kDistanceSqTolerance)
                        {
                            continue;
                        }
                    }

                    int sizeInBytes = Utilities.GetSizeInBytes(e.VertexElementFormat);
                    for (int j = 0; j < sizeInBytes; j++)
                    {
                        if (Vertices[e.Offset + j] < b.Vertices[e.Offset + j]) { return -1; }
                        else if (Vertices[e.Offset + j] > b.Vertices[e.Offset + j]) { return 1; }
                    }
                }

                return 0;
            }
        }

        public sealed class VertexDeclarationComparer : IEqualityComparer<VertexElement[]>
        {
            public bool Equals(VertexElement[] a, VertexElement[] b)
            {
                if (a.Length != b.Length)
                {
                    return false;
                }
                else
                {
                    int count = a.Length;
                    for (int i = 0; i < count; i++)
                    {
                        if (!(a[i].Equals(b[i])))
                        {
                            return false;
                        }
                    }

                    return true;
                }
            }

            public int GetHashCode(VertexElement[] aDeclaration)
            {
                string hashString = string.Empty;
                foreach (VertexElement e in aDeclaration) { hashString += e.ToString(); }

                int hash = hashString.GetHashCode();

                return hash;
            }
        }

        public struct Edge : IComparable<Edge>
        {
            public Edge(int aTriangle, UInt16 aVertexIndex1, UInt16 aVertexIndex2)
            {
                TriangleA = aTriangle;
                TriangleB = aTriangle;
                VertexIndex1ofA = aVertexIndex1;
                VertexIndex2ofA = aVertexIndex2;
            }

            public int TriangleA;
            public int TriangleB;

            public UInt16 VertexIndex1ofA;
            public UInt16 VertexIndex2ofA;

            public int CompareTo(Edge b)
            {
                int c = VertexIndex1ofA.CompareTo(b.VertexIndex1ofA);

                if (c == 0) { return (VertexIndex2ofA.CompareTo(b.VertexIndex2ofA)); }
                else { return c; }
            }

            public bool IsShared
            {
                get
                {
                    bool bReturn = (TriangleA != TriangleB);

                    return bReturn;
                }
            }
        }


        public static Edge[] BuildEdges(UInt16[] aIndices, Vector3[] aNormals)
        {
            int indexCount = aIndices.Length;
            if (indexCount % 3 != 0) { throw new Exception("Indices are not expected size."); }

            List<Edge> edges = new List<Edge>();

            #region Find edges where vi1 < vi2
            for (int i = 0; i < indexCount; i += 3)
            {
                UInt16 vi1 = aIndices[i + 2];

                for (int j = 0; j < 3; j++)
                {
                    UInt16 vi2 = aIndices[i + j];

                    Debug.Assert(vi1 != vi2);
                    if (vi1 < vi2)
                    {
                        Edge edge = new Edge(i, vi1, vi2);
                        int index = edges.BinarySearch(edge);

                        edges.Insert((index < 0) ? ~index : index, edge);
                    }

                    vi1 = vi2;
                }
            }
            #endregion

            #region Find edges where vi1 > vi2
            for (int i = 0; i < indexCount; i += 3)
            {
                UInt16 vi1 = aIndices[i + 2];

                for (int j = 0; j < 3; j++)
                {
                    UInt16 vi2 = aIndices[i + j];

                    Debug.Assert(vi1 != vi2);
                    if (vi1 > vi2)
                    {
                        Edge edg = new Edge(i, vi2, vi1);
                        int bestEdge = -1;
                        float bestDot = float.MinValue;

                        int curEdge = edges.BinarySearch(edg);
                        while (curEdge >= 1)
                        {
                            curEdge--;
                            if (edges[curEdge].CompareTo(edg) != 0)
                            {
                                curEdge++;
                                break;
                            }
                            else
                            {
                                bool bTest = false;
                            }
                        }

                        for (; (curEdge >= 0 && curEdge < edges.Count && (edges[curEdge].CompareTo(edg) == 0)); curEdge++)
                        {
                            if (!edges[curEdge].IsShared)
                            {
                                Vector3 n0 = aNormals[(edges[curEdge].TriangleA / 3)];
                                Vector3 n1 = aNormals[(i / 3)];

                                float dot = Vector3.Dot(n0, n1);

                                if (dot > bestDot)
                                {
                                    bestEdge = curEdge;
                                    bestDot = dot;
                                }
                            }
                        }

                        if (bestEdge >= 0)
                        {
                            #region Silliness to update a value-type.
                            {
                                Edge edge = edges[bestEdge];
                                edge.TriangleB = i;
                                edges[bestEdge] = edge;
                            }
                            #endregion
                        }
                        else
                        {
                            Edge edge = new Edge(i, vi1, vi2);
                            int index = edges.BinarySearch(edge);

                            edges.Insert((index < 0) ? ~index : index, edge);
                        }
                    }

                    vi1 = vi2;
                }
            }
            #endregion

            return edges.ToArray();
        }

        /// <summary>
        /// Processes a mesh so it forms a sealed 2D manifold.
        /// </summary>
        /// <param name="aTrianglesToEdges">An array of indices into aEdges.</param>
        /// <param name="aEdges">An edge list for the triangles in aIndices.</param>
        /// <param name="aIndices">A list of indices, expected to be a TriangleList.</param>
        /// <remarks>
        /// aTrianglesToEdges, aEdges, and aIndices are expected to have the same semantics as expected
        /// or produced by BuildEdges.
        /// </remarks>
        public static void Close(ref Edge[] arEdges, ref UInt16[] arIndices, ref Vector3[] arNormals)
        {
            bool bNeedsProcessing = false;
            foreach (Edge e in arEdges)
            {
                if (!e.IsShared)
                {
                    bNeedsProcessing = true;
                    break;
                }
            }

            if (bNeedsProcessing)
            {
                List<IndexComparable> ihelper = new List<IndexComparable>();
                UInt16[] outIndices = new UInt16[arIndices.Length * 2];
                Vector3[] outNormals = new Vector3[arNormals.Length * 2];

                int count = arIndices.Length;
                int outI = 0;
                for (int i = 0; i < count; i += 3)
                {
                    #region Original triangle
                    {
                        IndexComparable indx = new IndexComparable(false);
                        indx.I0 = arIndices[i + 0];
                        indx.I1 = arIndices[i + 1];
                        indx.I2 = arIndices[i + 2];

                        int index = ihelper.BinarySearch(indx);
                        if (index < 0)
                        {
                            ihelper.Insert(~index, indx);

                            outIndices[outI + 0] = indx.I0;
                            outIndices[outI + 1] = indx.I1;
                            outIndices[outI + 2] = indx.I2;

                            outNormals[(outI / 3)] = arNormals[(i / 3)];

                            outI += 3;
                        }
                    }
                    #endregion

                    #region Flipped triangle
                    {
                        IndexComparable indx = new IndexComparable(false);
                        indx.I0 = arIndices[i + 2];
                        indx.I1 = arIndices[i + 1];
                        indx.I2 = arIndices[i + 0];

                        int index = ihelper.BinarySearch(indx);
                        if (index < 0)
                        {
                            ihelper.Insert(~index, indx);

                            outIndices[outI + 0] = indx.I0;
                            outIndices[outI + 1] = indx.I1;
                            outIndices[outI + 2] = indx.I2;

                            outNormals[(outI / 3)] = -arNormals[(i / 3)];

                            outI += 3;
                        }
                    }
                    #endregion
                }
                Array.Resize(ref outIndices, outI);
                Array.Resize(ref outNormals, (outI / 3));
                Debug.Assert((outIndices.Length / 3) % 2 == 0);
                Debug.Assert((outNormals.Length == (outIndices.Length / 3)) && (outNormals.Length % 2 == 0));

                Edge[] outEdges = BuildEdges(outIndices, outNormals);

                arEdges = outEdges;
                arIndices = outIndices;
                arNormals = outNormals;

#if DEBUG
                foreach (Edge e in arEdges)
                {
                    if (!e.IsShared)
                    {
                        bool bFalse = true;
                    }
                    Debug.Assert(e.IsShared);
                }
#endif
            }
        }

        public static void ValidateSealedConvexGeometry(int[] indices, float[] vertices)
        {
            int count = indices.Length;

            if (count % 3 != 0) { throw new Exception("Invalid number of indices."); }
            if (vertices.Length % 3 != 0) { throw new Exception("Invalid number of floats in vertices."); }

            for (int i = 0; i < count; i++)
            {
                if (indices[i] < 0 || (indices[i] * 3) >= vertices.Length)
                {
                    throw new Exception("Out of range index.");
                }
            }

            Vector3[] normals = new Vector3[indices.Length / 3];
            for (int i = 0; i < count; i += 3)
            {
                int i0 = (indices[i + 0] * 3);
                int i1 = (indices[i + 1] * 3);
                int i2 = (indices[i + 2] * 3);

                Vector3 v0 = (new Vector3(vertices[i0 + 0], vertices[i0 + 1], vertices[i0 + 2]));
                Vector3 v1 = (new Vector3(vertices[i1 + 0], vertices[i1 + 1], vertices[i1 + 2]));
                Vector3 v2 = (new Vector3(vertices[i2 + 0], vertices[i2 + 1], vertices[i2 + 2]));

                Vector3 normal = Vector3.Normalize(Vector3.Cross(v2 - v0, v1 - v0));

                normals[i / 3] = normal;
            }

            Edge[] edges = BuildEdges(Array.ConvertAll<int, ushort>(indices, delegate(int e) { return (UInt16)e; }), normals);

            foreach (Edge e in edges)
            {
                if (!e.IsShared) { throw new Exception("Unshared edge."); }

                Vector3 n0 = normals[e.TriangleA / 3];
                Vector3 n1 = normals[e.TriangleB / 3];

                if (!Utilities.AboutEqual(Vector3.Dot(n0, n1), 1.0f))
                {
                    Vector3 u = Vector3.Normalize(Vector3.Cross(n1, n0));

                    int i0 = (e.VertexIndex1ofA * 3);
                    int i1 = (e.VertexIndex2ofA * 3);

                    Vector3 v0 = (new Vector3(vertices[i0 + 0], vertices[i0 + 1], vertices[i0 + 2]));
                    Vector3 v1 = (new Vector3(vertices[i1 + 0], vertices[i1 + 1], vertices[i1 + 2]));

                    Vector3 v = Vector3.Normalize(v1 - v0);
                    float d = (Vector3.Dot(u, v));

                    if (d < 0.0f) { throw new Exception("Mesh is not convex."); }
                }
            }
        }
    }
}
