using ExternalCompiledEffectReference = Microsoft.Xna.Framework.Content.Pipeline.ExternalReference<Microsoft.Xna.Framework.Graphics.CompiledEffect>;
using ExternalTextureReference = Microsoft.Xna.Framework.Content.Pipeline.ExternalReference<Microsoft.Xna.Framework.Content.Pipeline.Graphics.TextureContent>;

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;

// Note: this file contains intermediary content representations.
namespace jz.pipeline
{
    /// <summary>
    /// Encapsulates animation at content processing time.
    /// </summary>
    public sealed class AnimationContent
    {
        public AnimationContent(string aId, AnimationKeyFrame[] aKeyFrames)
        {
            Id = aId;
            KeyFrames = aKeyFrames;
        }

        public override bool Equals(object obj)
        {
            if (obj is AnimationContent)
            {
                AnimationContent b = (AnimationContent)obj;
                return (b.Id == Id);
            }

            return false;
        }

        public override int GetHashCode()
        {
            return Id.GetHashCode();
        }

        public override string ToString()
        {
            return Id;
        }

        public readonly string Id;
        public readonly AnimationKeyFrame[] KeyFrames;
    }

    public struct BoundingFrame
    {
        public BoundingBox BoundingBox;
        public BoundingSphere BoundingSphere;
        public float Time;
    }

    /// <summary>
    /// Encapsulates an effect at content processing time.
    /// </summary>
    public sealed class JzEffectContent
    {
        public string Id;
        public CompiledEffect CompiledEffect;
        public List<CompilerMacro> Macros = new List<CompilerMacro>();

        public JzEffectContent(CompiledEffect aEffect)
        {
            CompiledEffect = aEffect;
        }
    }

    /// <summary>
    /// Encapsulates a material at content processing time.
    /// </summary>
    /// <remarks>
    /// A material is used to set the parameters of an effect.
    /// </remarks>
    public sealed class JzMaterialContent
    {
        public sealed class Parameter
        {
            #region Private members
            private string mSemantic;
            private ParameterType mType;
            private object mValue;
            #endregion

            public Parameter(string aSemantic, ParameterType aType, object aValue)
            {
                mSemantic = aSemantic;
                mType = aType;
                mValue = aValue;
            }

            public string Id
            {
                get
                {
                    if (Value != null)
                    {
                        if (Value is ExternalTextureReference) { return (Semantic + "_" + ((ExternalTextureReference)Value).Filename); }
                        else { return Value.ToString(); }
                    }
                    else
                    {
                        return Semantic + "_";
                    }
                }
            }

            public string Semantic { get { return mSemantic; } }
            public ParameterType Type { get { return mType; } }
            public object Value { get { return mValue; } }
        }

        public List<Parameter> Parameters = new List<Parameter>();

        public string Id
        {
            get
            {
                string ret = string.Empty;
                foreach (Parameter p in Parameters) { ret += "_" + p.Id; }

                return (ret.GetHashCode().ToString());
            }
        }
    }

    public sealed class JzMeshContent
    {
        public List<Part> Parts = new List<Part>();
        public sealed class Part
        {
            #region Enumerators
            public sealed class SingleEnumerator : IEnumerator<float>
            {
                private readonly byte[] kBytes = null;
                private readonly int kElementSize = -1;
                private readonly int kInitialPosition = -1;
                private readonly int kStride = -1;
                private int mElementPos = -1;
                private int mCurrentPos = -1;

                public SingleEnumerator(byte[] aBytes, int aInitialPosition, int aStrideInBytes, int aElementSizeInBytes)
                {
                    kBytes = aBytes;
                    kStride = aStrideInBytes;
                    kElementSize = aElementSizeInBytes;
                    kInitialPosition = aInitialPosition;

                    mCurrentPos = (kInitialPosition - kStride);
                    mElementPos = 0;
                }

                object System.Collections.IEnumerator.Current { get { return Current; } }
                public float Current
                {
                    get
                    {
                        float ret = BitConverter.ToSingle(kBytes, (mCurrentPos + mElementPos));

                        return ret;
                    }
                }

                public void Dispose() { }

                public bool MoveNext()
                {
                    if (mElementPos < kElementSize) { mElementPos += sizeof(float); }
                    else
                    {
                        mElementPos = 0;
                        mCurrentPos += kStride;
                    }

                    return (mCurrentPos + kStride <= kBytes.Length);
                }

                public void Reset()
                {
                    mCurrentPos = (kInitialPosition - kStride);
                    mElementPos = 0;
                }
            }
            public sealed class SingleEnumerable : IEnumerable<float>
            {
                private readonly byte[] kBytes = null;
                private readonly int kElementSize = -1;
                private readonly int kInitialPosition = -1;
                private readonly int kStride = -1;

                public SingleEnumerable(byte[] aBytes, int aInitialPosition, int aStrideInBytes, int aElementSizeInBytes)
                {
                    kBytes = aBytes;
                    kStride = aStrideInBytes;
                    kElementSize = aElementSizeInBytes;
                    kInitialPosition = aInitialPosition;
                }

                System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
                {
                    return new SingleEnumerator(kBytes, kInitialPosition, kStride, kElementSize);
                }

                public IEnumerator<float> GetEnumerator()
                {
                    return new SingleEnumerator(kBytes, kInitialPosition, kStride, kElementSize);
                }
            }

            public sealed class Vector2Enumerator : IEnumerator<Vector2>
            {
                private const int i0 = (0 * sizeof(float));
                private const int i1 = (1 * sizeof(float));

                private readonly byte[] kBytes = null;
                private readonly int kInitialPosition = -1;
                private readonly int kStride = -1;
                private int mCurrentPos = -1;

                public Vector2Enumerator(byte[] aBytes, int aInitialPosition, int aStrideInBytes)
                {
                    kBytes = aBytes;
                    kInitialPosition = aInitialPosition;
                    kStride = aStrideInBytes;

                    mCurrentPos = (kInitialPosition - kStride);
                }

                object System.Collections.IEnumerator.Current { get { return Current; } }
                public Vector2 Current
                {
                    get
                    {
                        Vector2 ret;
                        ret.X = BitConverter.ToSingle(kBytes, mCurrentPos + i0);
                        ret.Y = BitConverter.ToSingle(kBytes, mCurrentPos + i1);

                        return ret;
                    }
                }

                public void Dispose() { }

                public bool MoveNext()
                {
                    mCurrentPos += kStride;

                    return (mCurrentPos + kStride <= kBytes.Length);
                }

                public void Reset()
                {
                    mCurrentPos = (kInitialPosition - kStride);
                }
            }
            public sealed class Vector2Enumerable : IEnumerable<Vector2>
            {
                private readonly byte[] kBytes = null;
                private readonly int kInitialPosition = -1;
                private readonly int kStride = -1;

                public Vector2Enumerable(byte[] aBytes, int aInitialPosition, int aStrideInBytes)
                {
                    kBytes = aBytes;
                    kStride = aStrideInBytes;
                    kInitialPosition = aInitialPosition;
                }

                System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
                {
                    return (new Vector2Enumerator(kBytes, kInitialPosition, kStride));
                }

                public IEnumerator<Vector2> GetEnumerator()
                {
                    return (new Vector2Enumerator(kBytes, kInitialPosition, kStride));
                }
            }

            public sealed class Vector3Enumerator : IEnumerator<Vector3>
            {
                private const int i0 = (0 * sizeof(float));
                private const int i1 = (1 * sizeof(float));
                private const int i2 = (2 * sizeof(float));

                private readonly byte[] kBytes = null;
                private readonly int kInitialPosition = -1;
                private readonly int kStride = -1;
                private int mCurrentPos = -1;

                public Vector3Enumerator(byte[] aBytes, int aInitialPosition, int aStrideInBytes)
                {
                    kBytes = aBytes;
                    kInitialPosition = aInitialPosition;
                    kStride = aStrideInBytes;

                    mCurrentPos = (kInitialPosition - kStride);
                }

                object System.Collections.IEnumerator.Current { get { return Current; } }
                public Vector3 Current
                {
                    get
                    {
                        Vector3 ret;
                        ret.X = BitConverter.ToSingle(kBytes, mCurrentPos + i0);
                        ret.Y = BitConverter.ToSingle(kBytes, mCurrentPos + i1);
                        ret.Z = BitConverter.ToSingle(kBytes, mCurrentPos + i2);

                        return ret;
                    }
                }

                public void Dispose() { }

                public bool MoveNext()
                {
                    mCurrentPos += kStride;

                    return (mCurrentPos + kStride <= kBytes.Length);
                }

                public void Reset()
                {
                    mCurrentPos = (kInitialPosition - kStride);
                }
            }
            public sealed class Vector3Enumerable : IEnumerable<Vector3>
            {
                private readonly byte[] kBytes = null;
                private readonly int kInitialPosition = -1;
                private readonly int kStride = -1;

                public Vector3Enumerable(byte[] aBytes, int aInitialPosition, int aStrideInBytes)
                {
                    kBytes = aBytes;
                    kStride = aStrideInBytes;
                    kInitialPosition = aInitialPosition;
                }

                System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
                {
                    return (new Vector3Enumerator(kBytes, kInitialPosition, kStride));
                }

                public IEnumerator<Vector3> GetEnumerator()
                {
                    return (new Vector3Enumerator(kBytes, kInitialPosition, kStride));
                }
            }

            public sealed class Vector4Enumerator : IEnumerator<Vector4>
            {
                private const int i0 = (0 * sizeof(float));
                private const int i1 = (1 * sizeof(float));
                private const int i2 = (2 * sizeof(float));
                private const int i3 = (3 * sizeof(float));

                private readonly byte[] kBytes = null;
                private readonly int kInitialPosition = -1;
                private readonly int kStride = -1;
                private int mCurrentPos = -1;

                public Vector4Enumerator(byte[] aBytes, int aInitialPosition, int aStrideInBytes)
                {
                    kBytes = aBytes;
                    kInitialPosition = aInitialPosition;
                    kStride = aStrideInBytes;

                    mCurrentPos = (kInitialPosition - kStride);
                }

                object System.Collections.IEnumerator.Current { get { return Current; } }
                public Vector4 Current
                {
                    get
                    {
                        Vector4 ret;
                        ret.X = BitConverter.ToSingle(kBytes, mCurrentPos + i0);
                        ret.Y = BitConverter.ToSingle(kBytes, mCurrentPos + i1);
                        ret.Z = BitConverter.ToSingle(kBytes, mCurrentPos + i2);
                        ret.W = BitConverter.ToSingle(kBytes, mCurrentPos + i3);

                        return ret;
                    }
                }

                public void Dispose() { }

                public bool MoveNext()
                {
                    mCurrentPos += kStride;

                    return (mCurrentPos + kStride <= kBytes.Length);
                }

                public void Reset()
                {
                    mCurrentPos = (kInitialPosition - kStride);
                }
            }
            public sealed class Vector4Enumerable : IEnumerable<Vector4>
            {
                private readonly byte[] kBytes = null;
                private readonly int kInitialPosition = -1;
                private readonly int kStride = -1;

                public Vector4Enumerable(byte[] aBytes, int aInitialPosition, int aStrideInBytes)
                {
                    kBytes = aBytes;
                    kStride = aStrideInBytes;
                    kInitialPosition = aInitialPosition;
                }

                System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
                {
                    return (new Vector4Enumerator(kBytes, kInitialPosition, kStride));
                }

                public IEnumerator<Vector4> GetEnumerator()
                {
                    return (new Vector4Enumerator(kBytes, kInitialPosition, kStride));
                }
            }
            #endregion

            public int GetElementSizeInBytes(VertexElementUsage aUsage, byte aUsageIndex)
            {
                foreach (VertexElement e in VertexDeclaration)
                {
                    if (e.VertexElementUsage == aUsage && e.UsageIndex == aUsageIndex)
                    {
                        return (Utilities.GetSizeInBytes(e.VertexElementFormat));
                    }
                }

                return 0;
            }

            public int GetOffsetInBytes(VertexElementUsage aUsage, byte aUsageIndex)
            {
                foreach (VertexElement e in VertexDeclaration)
                {
                    if (e.VertexElementUsage == aUsage && e.UsageIndex == aUsageIndex)
                    {
                        return (e.Offset);
                    }
                }

                return -1;
            }

            public BoundingBox AABB
            {
                get
                {
                    BoundingBox ret = Utilities.kZeroBox;

                    if (Vertices != null && Vertices.Length > 0)
                    {
                        // ret = BoundingBox.CreateFromPoints(EnumerateVector3(VertexElementUsage.Position, 0));
                        ret = Utilities.CreateBoxFromPoints(EnumerateVector3(VertexElementUsage.Position, 0));
                    }

                    return ret;
                }
            }

            public BoundingSphere BoundingSphere
            {
                get
                {
                    BoundingSphere ret = Utilities.kZeroSphere;

                    if (Vertices != null && Vertices.Length > 0)
                    {
                        // ret = BoundingSphere.CreateFromPoints(EnumerateVector3(VertexElementUsage.Position, 0));
                        ret = Utilities.CreateSphereFromPoints(EnumerateVector3(VertexElementUsage.Position, 0));
                    }

                    return ret;
                }
            }

            public SingleEnumerable EnumerateSingle(VertexElementUsage aUsage, byte aUsageIndex)
            {
                SingleEnumerable ret = new SingleEnumerable(Vertices, GetOffsetInBytes(aUsage, aUsageIndex), VertexStrideInBytes, GetElementSizeInBytes(aUsage, aUsageIndex));

                return ret;
            }

            public Vector2Enumerable EnumerateVector2(VertexElementUsage aUsage, byte aUsageIndex)
            {
                Vector2Enumerable ret = new Vector2Enumerable(Vertices, GetOffsetInBytes(aUsage, aUsageIndex), VertexStrideInBytes);

                return ret;
            }

            public Vector3Enumerable EnumerateVector3(VertexElementUsage aUsage, byte aUsageIndex)
            {
                Vector3Enumerable ret = new Vector3Enumerable(Vertices, GetOffsetInBytes(aUsage, aUsageIndex), VertexStrideInBytes);

                return ret;
            }

            public Vector4Enumerable EnumerateVector4(VertexElementUsage aUsage, byte aUsageIndex)
            {
                Vector4Enumerable ret = new Vector4Enumerable(Vertices, GetOffsetInBytes(aUsage, aUsageIndex), VertexStrideInBytes);

                return ret;
            }

            public Vector2 GetVector2(int aVertexIndex, int aOffsetInBytes)
            {
                int kIndex = (aVertexIndex * VertexStrideInBytes) + aOffsetInBytes;

                Vector2 ret = new Vector2(
                    BitConverter.ToSingle(Vertices, kIndex + 0),
                    BitConverter.ToSingle(Vertices, kIndex + sizeof(float)));

                return ret;
            }

            public Vector3 GetVector3(int aVertexIndex, int aOffsetInBytes)
            {
                int kIndex = (aVertexIndex * VertexStrideInBytes) + aOffsetInBytes;

                Vector3 ret = new Vector3(
                    BitConverter.ToSingle(Vertices, kIndex + 0),
                    BitConverter.ToSingle(Vertices, kIndex + sizeof(float)),
                    BitConverter.ToSingle(Vertices, kIndex + (2 * sizeof(float))));

                return ret;
            }

            public void SetVector3(int aVertexIndex, int aOffsetInBytes, Vector3 v)
            {
                int kIndex = (aVertexIndex * VertexStrideInBytes) + aOffsetInBytes;

                Array.Copy(BitConverter.GetBytes(v.X), 0, Vertices, (kIndex + 0), sizeof(float));
                Array.Copy(BitConverter.GetBytes(v.Y), 0, Vertices, (kIndex + sizeof(float)), sizeof(float));
                Array.Copy(BitConverter.GetBytes(v.Z), 0, Vertices, (kIndex + (2 * sizeof(float))), sizeof(float));
            }

            public string Id = string.Empty;
            public string Effect = string.Empty;
            public UInt16[] Indices = new UInt16[0];
            public PrimitiveType PrimitiveType = PrimitiveType.TriangleList;
            public VertexElement[] VertexDeclaration = new VertexElement[0];
            public byte[] Vertices = new byte[0];

            public int PrimitiveCount
            {
                get
                {
                    if (Indices != null)
                    {
                        int ret = Utilities.PrimitiveCount(PrimitiveType, Indices.Length);

                        return ret;
                    }
                    else
                    {
                        return 0;
                    }
                }
            }

            /// <summary>
            /// Rebuilds the vertex buffer by iterating through the index buffer and removing any unreferenced vertices.
            /// </summary>
            public void Rebuild()
            {
                SortedList<Utilities.VertexComparable, UInt16> cache = new SortedList<Utilities.VertexComparable, ushort>();
                List<byte> vertices = new List<byte>();

                UInt16 vertexCount = 0;
                int stride = VertexStrideInBytes;
                int count = Indices.Length;
                for (int i = 0; i < count; i++)
                {
                    Utilities.VertexComparable container = new Utilities.VertexComparable(VertexDeclaration, (Utilities.kZeroToleranceFloat * Utilities.kZeroToleranceFloat));
                    Array.Copy(Vertices, (Indices[i] * stride), container.Vertices, 0, stride);
                    if (cache.ContainsKey(container))
                    {
                        Indices[i] = cache[container];
                    }
                    else
                    {
                        Indices[i] = vertexCount;
                        cache.Add(container, vertexCount++);
                        vertices.AddRange(container.Vertices);
                    }
                }

                Vertices = vertices.ToArray();
                Debug.Assert(vertexCount == VertexCount);
            }

            public int VertexCount
            {
                get
                {
                    if (Vertices != null)
                    {
                        int ret = (Vertices.Length / VertexStrideInBytes);

                        return ret;
                    }
                    else
                    {
                        return 0;
                    }
                }
            }

            public short VertexStrideInBytes
            {
                get
                {
                    short ret = 0;

                    if (VertexDeclaration != null)
                    {
                        VertexElement e = VertexDeclaration[VertexDeclaration.Length - 1];
                        ret = (short)(e.Offset + Utilities.GetSizeInBytes(e.VertexElementFormat));
                    }

                    return ret;
                }
            }

            public int VertexStrideInSingles { get { return (VertexStrideInBytes / sizeof(float)); } }
        }
    }

    #region Scene
    public sealed class AnimatedMeshPartSceneNodeContent : SceneNodeContent
    {
        public AnimatedMeshPartSceneNodeContent(string aBaseId, string aId, int aChildrenCount, ref Matrix aLocalTransform,
            JzEffectContent aEffect, JzMaterialContent aMaterial, JzMeshContent.Part aMeshPart,
            ref Matrix aBindMatrix, Matrix[] aInverseBindTransforms, string aRootJoint, string[] aJoints)
            : base(aBaseId, aId, aChildrenCount, ref aLocalTransform, SceneNodeType.AnimatedMeshPart)
        {
            Effect = aEffect;
            Material = aMaterial;
            MeshPart = aMeshPart;
            BindMatrix = aBindMatrix;
            InverseBindTransforms = aInverseBindTransforms;
            RootJoint = aRootJoint;
            Joints = aJoints;
        }

        public readonly JzEffectContent Effect;
        public readonly JzMaterialContent Material;
        public readonly JzMeshContent.Part MeshPart;
        public readonly Matrix BindMatrix;
        public readonly Matrix[] InverseBindTransforms;
        public readonly string RootJoint;
        public readonly string[] Joints;
    }

    public sealed class DirectionalLightSceneNodeContent : SceneNodeContent
    {
        public DirectionalLightSceneNodeContent(string aBaseId, string aId, int aChildrenCount, ref Matrix aLocalTransform, Vector3 aLightColor)
            : base(aBaseId, aId, aChildrenCount, ref aLocalTransform, SceneNodeType.DirectionalLight)
        {
            LightColor = aLightColor;
        }

        public readonly Vector3 LightColor;
    }

    public sealed class JointSceneNodeContent : SceneNodeContent
    {
        public JointSceneNodeContent(string aBaseId, string aId, int aChildrenCount, ref Matrix aLocalTransform, AnimationContent aAnimation)
            : base(aBaseId, aId, aChildrenCount, ref aLocalTransform, SceneNodeType.Joint)
        {
            Animation = aAnimation;
        }

        public readonly AnimationContent Animation;
    }

    public sealed class MeshPartSceneNodeContent : SceneNodeContent
    {
        public MeshPartSceneNodeContent(string aBaseId, string aId, int aChildrenCount, ref Matrix aLocalTransform, ref Matrix aWorldTransform, JzEffectContent aEffect, JzMaterialContent aMaterial, JzMeshContent.Part aMeshPart)
            : base(aBaseId, aId, aChildrenCount, ref aLocalTransform, SceneNodeType.MeshPart)
        {
            Effect = aEffect;
            Material = aMaterial;
            MeshPart = aMeshPart;
            WorldTransform = aWorldTransform;
        }

        public readonly JzEffectContent Effect;
        public readonly JzMaterialContent Material;
        public readonly JzMeshContent.Part MeshPart;
        public readonly Matrix WorldTransform;
    }

    public sealed class PhysicsSceneNodeContent : SceneNodeContent
    {
        public PhysicsSceneNodeContent(string aBaseId, string aId, TriangleTree aTree)
            : base(aBaseId, aId, 0, ref Utilities.kIdentity, SceneNodeType.Physics)
        {
            Tree = aTree;
        }

        public readonly TriangleTree Tree;
    }

    public sealed class PointLightSceneNodeContent : SceneNodeContent
    {
        public PointLightSceneNodeContent(string aBaseId, string aId, int aChildrenCount, ref Matrix aLocalTransform, Vector3 aLightColor, Vector3 aAttenuation)
            : base(aBaseId, aId, aChildrenCount, ref aLocalTransform, SceneNodeType.PointLight)
        {
            LightAttenuation = aAttenuation;
            LightColor = aLightColor;
        }

        public readonly Vector3 LightAttenuation;
        public readonly Vector3 LightColor;
    }

    public class SceneNodeContent
    {
        #region Protected members
        protected SceneNodeContent(string aBaseId, string aId, int aChildrenCount, ref Matrix aLocalTransform, SceneNodeType aType)
        {
            BaseId = aBaseId;
            Id = aId;
            ChildrenCount = aChildrenCount;
            LocalTransform = aLocalTransform;
            Type = aType;
        }
        #endregion

        public SceneNodeContent(string aBaseId, string aId, int aChildrenCount, ref Matrix aLocalTransform)
            : this(aBaseId, aId, aChildrenCount, ref aLocalTransform, SceneNodeType.Node)
        { }

        public int ChildrenCount;
        public string BaseId;
        public string Id;
        public Matrix LocalTransform;
        public SceneNodeType Type;
    }

    public sealed class SpotLightSceneNodeContent : SceneNodeContent
    {
        public SpotLightSceneNodeContent(string aBaseId, string aId, int aChildrenCount, ref Matrix aLocalTransform, Vector3 aLightColor, Vector3 aAttenuation, float aFalloffAngleInRadians, float aFalloffExponent)
            : base(aBaseId, aId, aChildrenCount, ref aLocalTransform, SceneNodeType.SpotLight)
        {
            FalloffAngleInRadians = aFalloffAngleInRadians;
            FalloffExponent = aFalloffExponent;
            LightAttenuation = aAttenuation;
            LightColor = aLightColor;
        }

        public readonly float FalloffAngleInRadians;
        public readonly float FalloffExponent;
        public readonly Vector3 LightAttenuation;
        public readonly Vector3 LightColor;
    }

    public sealed class SceneContent
    {
        public List<SceneNodeContent> Nodes = new List<SceneNodeContent>();
    }
    #endregion
}
