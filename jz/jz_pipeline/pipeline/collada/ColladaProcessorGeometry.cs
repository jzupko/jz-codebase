using EffectsBySymbol = System.Collections.Generic.Dictionary<string, jz.pipeline.JzEffectContent>;
using MaterialsBySymbol = System.Collections.Generic.Dictionary<string, jz.pipeline.JzMaterialContent>;

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.Content.Pipeline;
using Microsoft.Xna.Framework.Content.Pipeline.Graphics;
using Microsoft.Xna.Framework.Content.Pipeline.Processors;
using Microsoft.Xna.Framework.Content.Pipeline.Serialization.Compiler;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Xml;
using jz.pipeline.collada.elements;
using jz.pipeline.collada.elements.fx;
using System.ComponentModel;

namespace jz.pipeline.collada
{
    public sealed class ColladaProcessorGeometry
    {
        #region Private members
        private Settings mSettings;
        
        private static readonly byte[] kMaxUsageCount;
        static ColladaProcessorGeometry()
        {
            kMaxUsageCount = new byte[(int)VertexElementUsage.Sample + 1];

            kMaxUsageCount[(int)VertexElementUsage.Position] = 1;
            kMaxUsageCount[(int)VertexElementUsage.BlendWeight] = 1;
            kMaxUsageCount[(int)VertexElementUsage.BlendIndices] = 1;
            kMaxUsageCount[(int)VertexElementUsage.Normal] = 1;
            kMaxUsageCount[(int)VertexElementUsage.PointSize] = 1;
            kMaxUsageCount[(int)VertexElementUsage.TextureCoordinate] = 8;
            kMaxUsageCount[(int)VertexElementUsage.Tangent] = 1;
            kMaxUsageCount[(int)VertexElementUsage.Binormal] = 1;
            kMaxUsageCount[(int)VertexElementUsage.TessellateFactor] = 1;
            kMaxUsageCount[(int)VertexElementUsage.Color] = 2;
            kMaxUsageCount[(int)VertexElementUsage.Fog] = 1;
            kMaxUsageCount[(int)VertexElementUsage.Depth] = 1;
            kMaxUsageCount[(int)VertexElementUsage.Sample] = 1;
        }

        private static bool[][] GetInUse()
        {
            bool[][] ret = new bool[(int)VertexElementUsage.Sample + 1][];
            for (int i = 0; i < (int)VertexElementUsage.Sample + 1; i++)
            {
                ret[i] = new bool[kMaxUsageCount[i]];
            }

            return ret;
        }

        private bool Use(VertexElement e, bool[][] abInUse)
        {
            FilterFlags channelFlag = (FilterFlags)(1 << (int)e.VertexElementUsage);

            // when NOT in inclusive mode, skip this channel if its flag bit is set.
            if ((mSettings.Flags & FilterFlags.FlagsInclusive) == 0 && (mSettings.Flags & channelFlag) != 0) { goto dontuse; }
            // when in inclusive mode, skip this channel if its flag bit is NOT set.
            if ((mSettings.Flags & FilterFlags.FlagsInclusive) != 0 && (mSettings.Flags & channelFlag) == 0) { goto dontuse; }
            // skip channels with usage indices above hard-coded max. i.e. position > 0
            if (e.UsageIndex >= kMaxUsageCount[(int)e.VertexElementUsage]) { goto dontuse; }
            // skip channels whos usage/usageindex already exists (ColladaMaya likes to spit out 
            // vertices with 2 color0 semantics
            if (abInUse[(int)e.VertexElementUsage][e.UsageIndex]) { goto dontuse; }

            abInUse[(int)e.VertexElementUsage][e.UsageIndex] = true;
            return true;

        dontuse:
            abInUse[(int)e.VertexElementUsage][e.UsageIndex] = false;
            return false;
        }

        private Dictionary<string, JzMeshContent> mMeshes = new Dictionary<string, JzMeshContent>();
        private Dictionary<VertexElement[], VertexElement[]> mVertexDeclarations = new Dictionary<VertexElement[], VertexElement[]>(new Utilities.VertexDeclarationComparer());

        private JzMeshContent _ProcessGeometry(ColladaGeometry aGeometry, float[] aBoneIndices, float[] aBoneWeights, EffectsBySymbol aEffectsBySymbol)
        {
            if (!aGeometry.IsMesh) { throw new Exception(Utilities.kNotImplemented); }

            JzMeshContent jzMesh = new JzMeshContent();
            ColladaMesh colladaMesh = aGeometry.Mesh;

            int partCount = 0;
            foreach (_ColladaPrimitive p in colladaMesh.GetEnumerable<_ColladaPrimitive>())
            {
                string partId = aGeometry.Id + ColladaProcessor.kMeshPartPostfix + partCount.ToString();
                jzMesh.Parts.Add(_ProcessPrimitives(p, partId, aBoneIndices, aBoneWeights, aEffectsBySymbol));
                partCount++;
            }

            return jzMesh;
        }

        private JzMeshContent.Part _ProcessPrimitives(_ColladaPrimitive p, string aId, float[] aBoneIndices, float[] aBoneWeights, EffectsBySymbol aEffectsBySymbol)
        {
            JzMeshContent.Part jzPart = new JzMeshContent.Part();

            jzPart.Id = aId;
            _ProcessPrimitivesHelper(p, aBoneIndices, aBoneWeights, aEffectsBySymbol, ref jzPart);

            return jzPart;
        }

        private UInt16[][] _ExtractColladaIndices(_ColladaPrimitive p)
        {
            uint offsetCount = p.OffsetCount;

            // p.Indices.Count is the total number of indices in the index buffer for this primitive.
            // The number of indices per vertex channel offset is equal to this value / the offset count.
            Debug.Assert(p.Indices.Count % offsetCount == 0);
            uint indexCount = (p.Indices.Count / offsetCount);

            UInt16[][] ret = new UInt16[offsetCount][];

            // Collada supports individual index buffers per vertex channel. i.e. the
            // TEXCOORD channel can have its own separate index buffer from the POSITION
            // channel (and often does). Real-time rendering only supports one index buffer
            // so the multiple index buffers must be compacted into a single index buffer.
            // This step gathers the indices into a more usable form.
            #region Gather COLLADA Indices
            for (uint i = 0; i < offsetCount; i++) { ret[i] = new UInt16[indexCount]; }

            for (uint i = 0u; i < indexCount; i++)
            {
                for (uint j = 0u; j < offsetCount; j++)
                {
                    uint index = (i * offsetCount) + j;

                    int e = (p.Indices[index]);
                    if (e < 0 || e > UInt16.MaxValue) { throw new ArgumentOutOfRangeException(); }

                    ret[j][i] = (UInt16)e;
                }
            }
            #endregion

            return ret;
        }

        #region VertexChannel
        private struct VertexChannel
        {
            public VertexChannel(_ColladaInput aInput)
            {
                bUse = true;
                Data = Utilities.Convert(aInput.GetArray<float>().Array);
                Format = ColladaDocument.ColladaSemanticToXnaFormat(aInput.Semantic, aInput.Stride);
                StrideInBytes = (short)(aInput.Stride * sizeof(float));
                Usage = ColladaDocument.ColladaSemanticToXnaUsage(aInput.Semantic);
                UsageIndex = (byte)aInput.Set;
            }

            public VertexChannel(float[] aData, VertexElementFormat aFormat,
                short aStrideInSingles, VertexElementUsage aUsage,
                byte aUsageIndex)
            {
                bUse = true;
                Data = Utilities.Convert(aData);
                Format = aFormat;
                StrideInBytes = (short)(aStrideInSingles * sizeof(float));
                Usage = aUsage;
                UsageIndex = aUsageIndex;
            }

            public bool bUse;

            public readonly byte[] Data;
            public readonly VertexElementFormat Format;
            public readonly short StrideInBytes;
            public readonly VertexElementUsage Usage;
            public readonly byte UsageIndex;

            public byte this[uint i] { get { return Data[i]; } }
        };

        private static VertexChannel[] _Convert(_ColladaInput[] aIn)
        {
            VertexChannel[] ret = new VertexChannel[aIn.Length];

            int count = aIn.Length;
            for (int i = 0; i < count; i++) { ret[i] = new VertexChannel(aIn[i]); }

            return ret;
        }
        #endregion

        private VertexChannel[][] _ExtractColladaVertices(_ColladaPrimitive p)
        {
            uint offsetCount = p.OffsetCount;

            // This gathers all the vertex buffers that are matched with an index buffer.
            // Often there is only one vertex buffer per index buffer, but their can be
            // any number > 0. Vertex buffers are represented by <input> elements.
            VertexChannel[][] ret = new VertexChannel[offsetCount][];

            for (uint i = 0; i < offsetCount; i++) { ret[i] = _Convert(p.FindInputs(i)); }

            return ret;
        }

        private void _InsertBoneIndicesAndWeights(_ColladaPrimitive p, float[] aBoneIndices, float[] aBoneWeights, ref VertexChannel[][] arColladaVertices)
        {
            uint offsetCount = p.OffsetCount;

            // This inserts the bone indices and weights if they are not null. This is necessary
            // for skeletal animated meshes. They are attached to the same index buffer as
            // the vertex channel with the POSITION semantic.
            #region Insert bone indices and weights
            if (aBoneIndices != null && aBoneWeights != null)
            {
                for (uint i = 0; i < offsetCount; i++)
                {
                    int verticesLength = arColladaVertices[i].Length;

                    if (verticesLength > 0 && arColladaVertices[i][0].Usage == VertexElementUsage.Position)
                    {
                        Array.Resize<VertexChannel>(ref arColladaVertices[i], verticesLength + 2);
                        arColladaVertices[i][verticesLength + 0] = new VertexChannel(aBoneIndices, VertexElementFormat.Vector4,
                            (short)ColladaProcessor.kJointInfluencesPerVertex, VertexElementUsage.BlendIndices, 0);
                        arColladaVertices[i][verticesLength + 1] = new VertexChannel(aBoneWeights, VertexElementFormat.Vector4,
                            (short)ColladaProcessor.kJointInfluencesPerVertex, VertexElementUsage.BlendWeight, 0);
                        break;
                    }
                }
            }
            #endregion
        }

        private VertexElement[] _CalculateVertexDeclaration(_ColladaPrimitive p, VertexChannel[][] colladaVertices)
        {
            uint offsetCount = p.OffsetCount;

            // This builds a VertexElement array which describes a single element of the final
            // vertex buffer that will be generated. It also marks any vertex channels that should
            // be filtered out (based on properties or user settings) to be removed later.
            #region Calculate vertex declaration
            List<VertexElement> ret = new List<VertexElement>();

            bool[][] bInUse = GetInUse();
            short offset = 0;
            for (uint i = 0; i < offsetCount; i++)
            {
                int innerCount = colladaVertices[i].Length;
                for (uint j = 0; j < innerCount; j++)
                {
                    VertexChannel channel = colladaVertices[i][j];
                    VertexElement e = new VertexElement(0, offset, channel.Format, VertexElementMethod.Default, channel.Usage, channel.UsageIndex);

                    if (Use(e, bInUse))
                    {
                        ret.Add(e);
                        offset += (short)Utilities.GetSizeInBytes(e.VertexElementFormat);
                    }
                    else
                    {
                        channel.bUse = false;
                        colladaVertices[i][j] = channel;
                    }
                }
            }
            #endregion

            return (ret.ToArray());
        }

        private void _Flatten(_ColladaPrimitive p, UInt16[][] colladaIndices, VertexChannel[][] colladaVertices, VertexElement[] decl, out UInt16[] arIndices, out byte[] arVertices)
        {
            // Builds a single compact index and vertex buffer. The vertex format is "interleaved"
            // in Open GL terms or a non-FVF vertex buffer in Direct X terms. Any flagged vertex
            // channels in the previous steps are now removed as well and vertex welding occurs here.

            SortedList<Utilities.VertexComparable, UInt16> cache = new SortedList<Utilities.VertexComparable, UInt16>();
            List<UInt16> indices = new List<UInt16>();
            List<byte> vertices = new List<byte>();

            uint indexCount = (uint)(colladaIndices[0].Length);
            uint offsetCount = (p.OffsetCount);
            UInt16 vertexCount = 0;

            for (uint i = 0; i < indexCount; i++)
            {
                Utilities.VertexComparable vertex = new Utilities.VertexComparable(decl, (mSettings.WeldingDistanceTolerance * mSettings.WeldingDistanceTolerance));

                int offset = 0;
                for (uint j = 0; j < offsetCount; j++)
                {
                    uint index = (uint)colladaIndices[j][i];
                    uint vertexChannels = (uint)colladaVertices[j].Length;
                    for (uint k = 0; k < vertexChannels; k++)
                    {
                        VertexChannel channel = colladaVertices[j][k];
                        if (channel.bUse)
                        {
                            uint stride = (uint)(channel.StrideInBytes);
                            uint start = index * stride;
                            uint end = start + stride;

                            for (uint l = start; l < end; l++)
                            {
                                vertex[offset++] = (channel)[l];
                            }
                        }
                    }
                }

                #region Add the vertex - if it exists, pointer the index to the existing vertex.
                UInt16 existingIndex = 0;
                if (cache.TryGetValue(vertex, out existingIndex))
                {
                    indices.Add(existingIndex);
                }
                else
                {
                    indices.Add(vertexCount);
                    cache[vertex] = vertexCount++;
                    vertices.AddRange(vertex.Vertices);
                }
                #endregion
            }

            arIndices = (indices.ToArray());
            arVertices = (vertices.ToArray());
        }

        private void _Clean(JzMeshContent.Part m)
        {
            #region Remove degenerate triangles and duplicate triangles
            {
                // Implement for other primitive types.
                if (m.PrimitiveType != PrimitiveType.TriangleList) { throw new Exception(Utilities.kNotImplemented); }
                bool[] bRemove = new bool[m.Indices.Length];

                int positionOffset = m.GetOffsetInBytes(VertexElementUsage.Position, 0);

                if (positionOffset >= 0)
                {
                    bool bConsiderNormals = (m.GetOffsetInBytes(VertexElementUsage.Normal, 0) >= 0);

                    Dictionary<Triangle, Triangle> duplicateRemove = new Dictionary<Triangle, Triangle>(new Utilities.TriangleComparer(bConsiderNormals, (mSettings.WeldingDistanceTolerance * mSettings.WeldingDistanceTolerance)));
                    for (int i = 0; i < m.Indices.Length; i += 3)
                    {
                        Vector3 p0 = m.GetVector3(m.Indices[i + 0], positionOffset);
                        Vector3 p1 = m.GetVector3(m.Indices[i + 1], positionOffset);
                        Vector3 p2 = m.GetVector3(m.Indices[i + 2], positionOffset);
                        Triangle t = new Triangle(p0, p1, p2);

                        if (t.IsDegenerate || duplicateRemove.ContainsKey(t))
                        {
                            bRemove[i + 0] = true;
                            bRemove[i + 1] = true;
                            bRemove[i + 2] = true;
                        }
                        else
                        {
                            duplicateRemove.Add(t, t);
                        }
                    }
                }

                int outIndex = 0;
                for (int inIndex = 0; inIndex < m.Indices.Length; inIndex++)
                {
                    if (!bRemove[inIndex])
                    {
                        m.Indices[outIndex++] = m.Indices[inIndex];
                    }
                }
                Array.Resize(ref m.Indices, outIndex);
            }

            m.Rebuild();
            #endregion
        }

        private void _Insert(VertexElementUsage aUsage, byte aUsageIndex, Vector3[] aChannel,
            ref VertexElement[] aDeclaration, ref byte[] aVertexBuffer)
        {
            int elementSizeInBytes = (3 * sizeof(float));
            int vertexStrideInBytes = (aDeclaration[aDeclaration.Length - 1].Offset + Utilities.GetSizeInBytes(aDeclaration[aDeclaration.Length - 1].VertexElementFormat));

            VertexElement e = new VertexElement();
            e.Offset = (short)vertexStrideInBytes;
            e.Stream = 0;
            e.UsageIndex = aUsageIndex;
            e.VertexElementFormat = VertexElementFormat.Vector3;
            e.VertexElementMethod = VertexElementMethod.Default;
            e.VertexElementUsage = aUsage;

            int vertexCount = (aVertexBuffer.Length / vertexStrideInBytes);
            int newVertexStride = (vertexStrideInBytes + elementSizeInBytes);
            byte[] newVertexBuffer = new byte[vertexCount * newVertexStride];

            #region Populate new vertex buffer
            {
                int count = aVertexBuffer.Length;
                for (int i = 0, input = 0, output = 0; input < count; i++, input += vertexStrideInBytes, output += newVertexStride)
                {
                    Array.Copy(aVertexBuffer, input, newVertexBuffer, output, vertexStrideInBytes);

                    int i0 = (output + vertexStrideInBytes);
                    int i1 = (i0 + sizeof(float));
                    int i2 = (i1 + sizeof(float));

                    Array.Copy(BitConverter.GetBytes(aChannel[i].X), 0, newVertexBuffer, i0, sizeof(float));
                    Array.Copy(BitConverter.GetBytes(aChannel[i].Y), 0, newVertexBuffer, i1, sizeof(float));
                    Array.Copy(BitConverter.GetBytes(aChannel[i].Z), 0, newVertexBuffer, i2, sizeof(float));
                }
            }
            #endregion

            Array.Resize(ref aDeclaration, (aDeclaration.Length + 1));
            aDeclaration[aDeclaration.Length - 1] = e;
            aVertexBuffer = newVertexBuffer;
        }

        private bool _Generate(JzMeshContent.Part m, int aBumpMapTexCoord)
        {
            if (m.PrimitiveType != PrimitiveType.TriangleList) { throw new Exception(Utilities.kNotImplemented); }

            int positionOffset = m.GetOffsetInBytes(VertexElementUsage.Position, 0);
            int bumpMapOffset = (aBumpMapTexCoord >= 0)
                ? m.GetOffsetInBytes(VertexElementUsage.TextureCoordinate, (byte)aBumpMapTexCoord)
                : aBumpMapTexCoord;

            #region Generate normals
            if (positionOffset >= 0 && mSettings.bGenerateNormals)
            {
                int vertexCount = (m.VertexCount);
                Vector3[] buf = new Vector3[vertexCount];
                if (m.GetOffsetInBytes(VertexElementUsage.Normal, 0) < 0)
                {
                    _Insert(VertexElementUsage.Normal, 0, buf, ref m.VertexDeclaration, ref m.Vertices);
                }

                for (int i = 0; i < m.Indices.Length; i += 3)
                {
                    int i0 = m.Indices[i + 0];
                    int i1 = m.Indices[i + 1];
                    int i2 = m.Indices[i + 2];

                    Vector3 p0 = m.GetVector3(i0, positionOffset);
                    Vector3 p1 = m.GetVector3(i1, positionOffset);
                    Vector3 p2 = m.GetVector3(i2, positionOffset);

                    Vector3 unormNormal = Vector3.Zero;
                    if (Utilities.kWinding == Winding.Clockwise) { unormNormal = Vector3.Cross(p2 - p0, p1 - p0); }
                    else { unormNormal = Vector3.Cross(p1 - p0, p2 - p0); }

                    Vector3 normal = Vector3.Normalize(unormNormal);

                    for (int j = 0; j < 3; j++)
                    {
                        int index = (m.Indices[i + j]);
                        float dot = Vector3.Dot(normal, Vector3.Normalize(buf[index]));

                        buf[index] += (dot < 0.0f) ? -unormNormal : unormNormal;
                    }
                }

                for (int i = 0; i < vertexCount; i++)
                {
                    if (Utilities.AboutZero(buf[i])) { return false; }
                    buf[i] = Utilities.SafeNormalize(buf[i]);
                }

                int normalOffset = m.GetOffsetInBytes(VertexElementUsage.Normal, 0);
                Debug.Assert(normalOffset >= 0);
                for (int i = 0; i < vertexCount; i++)
                {
                    m.SetVector3(i, normalOffset, buf[i]);
                }
            }
            #endregion

            #region Generate binormals and/or tangents
            if (positionOffset >= 0 && bumpMapOffset >= 0 && (mSettings.bGenerateTangents || mSettings.bGenerateBinormals))
            {
                int vertexCount = m.VertexCount;
                Vector3[] tans = (mSettings.bGenerateTangents) ? new Vector3[vertexCount] : null;
                Vector3[] bins = (mSettings.bGenerateBinormals) ? new Vector3[vertexCount] : null;

                if (mSettings.bGenerateTangents && m.GetOffsetInBytes(VertexElementUsage.Tangent, 0) < 0)
                {
                    _Insert(VertexElementUsage.Tangent, 0, tans, ref m.VertexDeclaration, ref m.Vertices);
                }

                if (mSettings.bGenerateBinormals && m.GetOffsetInBytes(VertexElementUsage.Binormal, 0) < 0)
                {
                    _Insert(VertexElementUsage.Binormal, 0, bins, ref m.VertexDeclaration, ref m.Vertices);
                }

                for (int i = 0; i < m.Indices.Length; i += 3)
                {
                    int i0 = m.Indices[i + 0];
                    int i1 = m.Indices[i + 1];
                    int i2 = m.Indices[i + 2];

                    Vector3 p0 = m.GetVector3(i0, positionOffset);
                    Vector3 p1 = m.GetVector3(i1, positionOffset);
                    Vector3 p2 = m.GetVector3(i2, positionOffset);

                    Vector2 uv0 = m.GetVector2(i0, bumpMapOffset);
                    Vector2 uv1 = m.GetVector2(i1, bumpMapOffset);
                    Vector2 uv2 = m.GetVector2(i2, bumpMapOffset);

                    Vector3 q1 = (p1 - p0);
                    Vector3 q2 = (p2 - p0);

                    Vector2 uva = (uv1 - uv0);
                    Vector2 uvb = (uv2 - uv0);

                    float denom = 1.0f / Utilities.Cross(uva, uvb);

                    if (mSettings.bGenerateTangents)
                    {
                        Vector3 t = new Vector3(
                            (q1.X * uvb.Y) + (q2.X * -uva.Y),
                            (q1.Y * uvb.Y) + (q2.Y * -uva.Y),
                            (q1.Z * uvb.Y) + (q2.Z * -uva.Y));
                        t *= denom;
                        Vector3 tNormal = Vector3.Normalize(t);

                        for (int j = 0; j < 3; j++)
                        {
                            int index = (m.Indices[i + j]);

                            float dot = Vector3.Dot(tNormal, Vector3.Normalize(tans[index]));

                            tans[index] += (dot < 0.0f) ? -t : t;
                        }
                    }

                    if (mSettings.bGenerateBinormals)
                    {
                        Vector3 b = new Vector3(
                            (q1.X * -uvb.X) + (q2.X * uva.X),
                            (q1.Y * -uvb.X) + (q2.Y * uva.X),
                            (q1.Z * -uvb.X) + (q2.Z * uva.X));
                        b *= denom;
                        Vector3 bNormal = Vector3.Normalize(b);

                        for (int j = 0; j < 3; j++)
                        {
                            int index = (m.Indices[i + j]);

                            float dot = Vector3.Dot(bNormal, Vector3.Normalize(bins[index]));

                            bins[index] += (dot < 0.0f) ? -b : b;
                        }
                    }
                }

                if (mSettings.bGenerateTangents)
                {
                    for (int i = 0; i < vertexCount; i++)
                    {
                        if (Utilities.AboutZero(tans[i])) { return false; }
                        tans[i] = Utilities.SafeNormalize(tans[i]);
                    }

                    int tanOffset = m.GetOffsetInBytes(VertexElementUsage.Tangent, 0);
                    Debug.Assert(tanOffset >= 0);
                    for (int i = 0; i < vertexCount; i++)
                    {
                        m.SetVector3(i, tanOffset, tans[i]);
                    }
                }

                if (mSettings.bGenerateBinormals)
                {
                    for (int i = 0; i < vertexCount; i++)
                    {
                        if (Utilities.AboutZero(bins[i])) { return false; }
                        bins[i] = Utilities.SafeNormalize(bins[i]);
                    }

                    int binOffset = m.GetOffsetInBytes(VertexElementUsage.Binormal, 0);
                    Debug.Assert(binOffset >= 0);
                    for (int i = 0; i < vertexCount; i++)
                    {
                        m.SetVector3(i, binOffset, bins[i]);
                    }
                }
            }
            #endregion

            return true;
        }

        private void _ProcessPrimitivesHelper(_ColladaPrimitive p, float[] aBoneIndices, float[] aBoneWeights, EffectsBySymbol aEffectsBySymbol, ref JzMeshContent.Part arOut)
        {
            #region Build mesh
            {
                UInt16[][] colladaIndices = _ExtractColladaIndices(p);
                VertexChannel[][] colladaVertices = _ExtractColladaVertices(p);

                _InsertBoneIndicesAndWeights(p, aBoneIndices, aBoneWeights, ref colladaVertices);

                VertexElement[] decl = _CalculateVertexDeclaration(p, colladaVertices);

                UInt16[] indices;
                byte[] vertices;
                _Flatten(p, colladaIndices, colladaVertices, decl, out indices, out vertices);

                #region Check for duplicate vertex declaration
                if (mVertexDeclarations.ContainsKey(decl)) { decl = mVertexDeclarations[decl]; }
                else { mVertexDeclarations[decl] = decl; }
                #endregion

                arOut.Effect = p.Material;
                arOut.Indices = indices;
                arOut.PrimitiveType = ColladaDocument.ColladaPrimitiveToXnaPrimitive(p);
                arOut.VertexDeclaration = decl;
                arOut.Vertices = vertices;
            }
            #endregion

            if (Utilities.kImageOrigin == ImageOrigin.TopLeft)
            {
                Utilities.FlipTextureV(arOut.VertexDeclaration, arOut.Vertices);
            }

            if (Utilities.kWinding == Winding.Clockwise)
            {
                Utilities.FlipWindingOrder(arOut.PrimitiveType, arOut.Indices);
            }

            _Clean(arOut);

            if (aEffectsBySymbol.ContainsKey(arOut.Effect))
            {
                JzEffectContent eff = aEffectsBySymbol[arOut.Effect];
                int bumpMapTexCoord = -1;
                foreach (CompilerMacro e in eff.Macros)
                {
                    if (e.Name == (ColladaProcessorMaterial.kBumpPrefix + ColladaProcessorMaterial.kTexcoordsPostfix))
                    {
                        int.TryParse(e.Definition, out bumpMapTexCoord);
                    }
                }

                if (!_Generate(arOut, bumpMapTexCoord) && mSettings.bReprocessWhenGenerateFails)
                {
                    // If generation of normal, binormal, or tangent fails (a vertex has one of these
                    // components == 0) and the appropriate setting is set, the entire mesh is reprocessed
                    // with generation disabled and any input data for these channels is used instead.
                    Settings orig = mSettings;
                    {
                        #region Use passed-in normals instead of generating them.
                        if (mSettings.bGenerateNormals)
                        {
                            mSettings.bGenerateNormals = false;
                            if ((mSettings.Flags & FilterFlags.FlagsInclusive) != 0) { mSettings.Flags |= FilterFlags.Normal; }
                            else { mSettings.Flags &= ~FilterFlags.Normal; }
                        }
                        #endregion

                        #region Use passed-in tangents instead of generating them.
                        if (mSettings.bGenerateTangents)
                        {
                            mSettings.bGenerateTangents = false;
                            if ((mSettings.Flags & FilterFlags.FlagsInclusive) != 0) { mSettings.Flags |= FilterFlags.Tangent; }
                            else { mSettings.Flags &= ~FilterFlags.Tangent; }
                        }
                        #endregion

                        #region Use passed-in binormals instead of generating them.
                        if (mSettings.bGenerateBinormals)
                        {
                            mSettings.bGenerateBinormals = false;
                            if ((mSettings.Flags & FilterFlags.FlagsInclusive) != 0) { mSettings.Flags |= FilterFlags.Binormal; }
                            else { mSettings.Flags &= ~FilterFlags.Binormal; }
                        }
                        #endregion

                        _ProcessPrimitivesHelper(p, aBoneIndices, aBoneWeights, aEffectsBySymbol, ref arOut);
                    }
                    mSettings = orig;
                    return;
                }
            }
        }
        #endregion

        #region Types
        [Flags]
        public enum FilterFlags
        {
            None = 0,
            Position = (1 << VertexElementUsage.Position),
            BlendWeight = (1 << VertexElementUsage.BlendWeight),
            BlendIndices = (1 << VertexElementUsage.BlendIndices),
            Normal = (1 << VertexElementUsage.Normal),
            PointSize = (1 << VertexElementUsage.PointSize),
            TextureCoordinate = (1 << VertexElementUsage.TextureCoordinate),
            Tangent = (1 << VertexElementUsage.Tangent),
            Binormal = (1 << VertexElementUsage.Binormal),
            TessellateFactor = (1 << VertexElementUsage.TessellateFactor),
            Color = (1 << VertexElementUsage.Color),
            Fog = (1 << VertexElementUsage.Fog),
            Depth = (1 << VertexElementUsage.Depth),
            Sample = (1 << VertexElementUsage.Sample),

            // By default, these flags indicate that vertex data with the given vertex semantic
            // should be filtered from the output data. Setting this flag inverts the meaning.
            // All vertex semantics are filtered out and only those with an explicit set flag
            // are included in mesh output.
            FlagsInclusive = (1 << (VertexElementUsage.Sample + 1))
        }

        public struct Settings
        {
            public FilterFlags Flags;

            /// <summary>
            /// Determines whether normals, tangents, and/or binormals are automatically generated
            /// from input position vertex data.
            /// </summary>
            /// <remarks>
            /// Note that these booleans imply that input normal, tangent, and/or binormal
            /// data will be ignored and that the output data will have normal, tangent, and/or 
            /// binormal data.
            /// </remarks>
            public bool bGenerateNormals;
            public bool bGenerateTangents;
            public bool bGenerateBinormals;
            public bool bReprocessWhenGenerateFails;

            public float WeldingDistanceTolerance;
        }
        #endregion

        public static Settings DefaultSettings
        {
            get
            {
                Settings ret;
                ret.Flags = FilterFlags.None;
                ret.bGenerateNormals = false;
                ret.bGenerateTangents = false;
                ret.bGenerateBinormals = false;
                ret.bReprocessWhenGenerateFails = false;
                ret.WeldingDistanceTolerance = 0.0f;

                return ret;
            }
        }

        public ColladaProcessorGeometry() : this(DefaultSettings) {}
        public ColladaProcessorGeometry(Settings aSettings)
        {
            mSettings = aSettings;
        }

        public JzMeshContent GetMesh(ColladaGeometry aGeometry, float[] aBoneIndices, float[] aBoneWeights, EffectsBySymbol aEffectsBySymbol)
        {
            string geometryId = aGeometry.Id;

            JzMeshContent ret = null;
            if (!mMeshes.ContainsKey(geometryId))
            {
                ret = _ProcessGeometry(aGeometry, aBoneIndices, aBoneWeights, aEffectsBySymbol);
                mMeshes[geometryId] = ret;
            }
            else
            {
                ret = mMeshes[geometryId];
            }

            return ret;
        }
    }
}
