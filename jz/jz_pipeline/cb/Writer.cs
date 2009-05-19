// #define JZ_OPENGL_TARGET

using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.Content.Pipeline;
using Microsoft.Xna.Framework.Content.Pipeline.Graphics;
using Microsoft.Xna.Framework.Content.Pipeline.Processors;
using Microsoft.Xna.Framework.Content.Pipeline.Serialization.Compiler;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.IO;
using jz;
using jz.pipeline;
using jz.pipeline.collada;

namespace jz.cb
{
    public static class Helpers
    {
        public class HelperGame : Game
        {
            public GraphicsDeviceManager Manager;

            public HelperGame()
            {
                Manager = new GraphicsDeviceManager(this);
                Manager.ApplyChanges();
            }
        }

        public static HelperGame Game = new HelperGame();
        public static readonly TextureCreationParameters kTextureWriteParameters;
        public static readonly TextureCreationParameters kTextureDxt5Parameters;

        static Helpers()
        {
            kTextureWriteParameters = TextureCreationParameters.Default;
            kTextureWriteParameters.Format = SurfaceFormat.Dxt1;

            kTextureDxt5Parameters = TextureCreationParameters.Default;
            kTextureDxt5Parameters.Format = SurfaceFormat.Dxt5;
        }

        public const string kEffectOutExtension = ".cfx";
        public const string kMaterialExtension = ".mat";
        public const string kMeshExtension = ".mesh";
        public const string kVertexDeclarationExtension = ".vdl";

        public static void Write(BinaryWriter aOut, UInt32 v)
        {
            aOut.Write(v);
        }

        public static void Write(BinaryWriter aOut, byte v)
        {
            aOut.Write(v);
        }

        public static void Write(BinaryWriter aOut, UInt16 v)
        {
            aOut.Write(v);
        }

        public static void Write(BinaryWriter aOut, Int32 v)
        {
            aOut.Write(v);
        }

        public static void Write(BinaryWriter aOut, float v)
        {
            aOut.Write(v);
        }

        public static void Write(BinaryWriter aOut, string v)
        {
            Write(aOut, (UInt32)(v.Length + 1));
            foreach (char c in v)
            {
                if (c > byte.MaxValue) { throw new ArgumentOutOfRangeException(); }
                else { aOut.Write((byte)c); }
            }
            aOut.Write((byte)0);
        }

        public static void Write(BinaryWriter aOut, BoundingBox bb)
        {
            Write(aOut, bb.Min);
            Write(aOut, bb.Max);
        }

        public static void Write(BinaryWriter aOut, BoundingSphere bs)
        {
            Write(aOut, bs.Center);
            Write(aOut, bs.Radius);
        }

        // column major
        public static void Write(BinaryWriter aOut, Matrix v)
        {
            Write(aOut, v.M11); Write(aOut, v.M21); Write(aOut, v.M31); Write(aOut, v.M41);
            Write(aOut, v.M12); Write(aOut, v.M22); Write(aOut, v.M32); Write(aOut, v.M42);
            Write(aOut, v.M13); Write(aOut, v.M23); Write(aOut, v.M33); Write(aOut, v.M43);
            Write(aOut, v.M14); Write(aOut, v.M24); Write(aOut, v.M34); Write(aOut, v.M44);
        }

        public static void Write(BinaryWriter aOut, DocInfo aInfo, ExternalReference<TextureContent> v)
        {
            bool bDxt5 = (bool)(v.OpaqueData[ColladaProcessorMaterial.kDxt5Flag]);
            string baseFilename = Path.GetFileNameWithoutExtension(v.Filename);
            string refFilename = Path.Combine(aInfo.OutRelDirectory, baseFilename) + ".dds";
            string outFilename = Path.Combine(aInfo.OutAbsDirectory, baseFilename) + ".dds";

            Write(aOut, refFilename);
            if (!File.Exists(outFilename) || File.GetLastWriteTime(v.Filename).Ticks > File.GetLastWriteTime(outFilename).Ticks)
            {
                Texture conv = Texture.FromFile(Game.Manager.GraphicsDevice, v.Filename, (bDxt5) ? kTextureDxt5Parameters : kTextureWriteParameters);
                conv.Save(outFilename, ImageFileFormat.Dds);
            }
            else if (bDxt5)
            {
                TextureInformation info = Texture.GetTextureInformation(outFilename);
                if (info.Format != kTextureDxt5Parameters.Format)
                {
                    Texture conv = Texture.FromFile(Game.Manager.GraphicsDevice, v.Filename, kTextureDxt5Parameters);
                    conv.Save(outFilename, ImageFileFormat.Dds);
                }
            }
        }

        public static void Write(BinaryWriter aOut, Vector2 v)
        {
            Write(aOut, v.X);
            Write(aOut, v.Y);
        }

        public static void Write(BinaryWriter aOut, Vector3 v)
        {
            Write(aOut, v.X);
            Write(aOut, v.Y);
            Write(aOut, v.Z);
        }

        public static void Write(BinaryWriter aOut, Vector4 v)
        {
            Write(aOut, v.X);
            Write(aOut, v.Y);
            Write(aOut, v.Z);
            Write(aOut, v.W);
        }

        public static void Write(BinaryWriter aOut, AnimationKeyFrame v)
        {
            Write(aOut, v.Key);
            Write(aOut, v.TimeAndPadding);
        }

        public static void Write(BinaryWriter aOut, UInt16[] v)
        {
            Write(aOut, (UInt32)v.Length);
            foreach (UInt16 e in v)
            {
                Write(aOut, e);
            }
        }

        public static void WriteBuffer(BinaryWriter aOut, byte[] v)
        {
            Write(aOut, (UInt32)v.Length);
            foreach (byte e in v)
            {
                Write(aOut, e);
            }
        }

        public static void Write(BinaryWriter aOut, DocInfo aInfo, JzEffectContent aEffect)
        {
#if JZ_OPENGL_TARGET
            if (aEffect.Id.Contains("material"))
            {
                string input = "..\\..\\media\\engine_opengl_material.cgfx";
                string reloutput = Path.Combine(aInfo.OutRelDirectory, "engine_opengl_material.cgfx");
                string output = Path.Combine(aInfo.OutAbsDirectory, "engine_opengl_material.cgfx");

                Write(aOut, reloutput);
                if (!File.Exists(output) || File.GetLastWriteTime(input).Ticks > File.GetLastWriteTime(output).Ticks)
                {
                    File.Copy(input, output);
                }
                return;
            }
#endif

            string baseFilename = Path.GetFileNameWithoutExtension(aEffect.Id);
            string refFilename = Path.Combine(aInfo.OutRelDirectory, baseFilename) + kEffectOutExtension;
            string outFilename = Path.Combine(aInfo.OutAbsDirectory, baseFilename) + kEffectOutExtension;

            Write(aOut, refFilename);
            BinaryWriter writer = new BinaryWriter(new FileStream(outFilename, FileMode.Create));
            writer.Write(aEffect.CompiledEffect.GetEffectCode());
            writer.Close();
        }

        public static void Write(BinaryWriter aOut, DocInfo aInfo, JzMaterialContent aMaterial)
        {
            string baseFilename = Path.GetFileNameWithoutExtension(aMaterial.Id);
            string refFilename = Path.Combine(aInfo.OutRelDirectory, Path.GetFileNameWithoutExtension(aInfo.Filename) + "_" + baseFilename + kMaterialExtension);
            string outFilename = Path.Combine(aInfo.OutAbsDirectory, Path.GetFileNameWithoutExtension(aInfo.Filename) + "_" + baseFilename + kMaterialExtension);

            Helpers.Write(aOut, refFilename);
            BinaryWriter writer = new BinaryWriter(new FileStream(outFilename, FileMode.Create));
            Helpers.Write(writer, (Int32)aMaterial.Parameters.Count);
            foreach (JzMaterialContent.Parameter p in aMaterial.Parameters)
            {
                Helpers.Write(writer, p.Semantic);
                Helpers.Write(writer, (Int32)p.Type);
                switch (p.Type)
                {
                    case ParameterType.kSingle: Helpers.Write(writer, (float)p.Value); break;
                    case ParameterType.kMatrix: Helpers.Write(writer, (Matrix)p.Value); break;
                    case ParameterType.kTexture: Helpers.Write(writer, aInfo, (ExternalReference<TextureContent>)p.Value); break;
                    case ParameterType.kVector2: Helpers.Write(writer, (Vector2)p.Value); break;
                    case ParameterType.kVector3: Helpers.Write(writer, (Vector3)p.Value); break;
                    case ParameterType.kVector4: Helpers.Write(writer, (Vector4)p.Value); break;
                    default:
                        throw new ArgumentOutOfRangeException();
                }
            }
            writer.Close();
        }

        public static void Write(BinaryWriter aOut, DocInfo aInfo, JzMeshContent.Part aIn)
        {
            string baseFilename = aIn.Id;
            string refFilename = Path.Combine(aInfo.OutRelDirectory, Utilities.ExtractBaseFilename(Utilities.ExtractRelativeFilename(aInfo.InAbsDirectory, aInfo.Filename))) + "_" + baseFilename + kMeshExtension;
            string outFilename = Path.Combine(aInfo.OutAbsDirectory, Utilities.ExtractBaseFilename(Utilities.ExtractRelativeFilename(aInfo.InAbsDirectory, aInfo.Filename))) + "_" + baseFilename + kMeshExtension;

            if (aOut != null) { Helpers.Write(aOut, refFilename); }
            BinaryWriter writer = new BinaryWriter(new FileStream(outFilename, FileMode.Create));
            Helpers.Write(writer, aIn.AABB);
            Helpers.Write(writer, aIn.BoundingSphere);
            Helpers.Write(writer, (Int32)aIn.PrimitiveCount);
            Helpers.Write(writer, (Int32)aIn.PrimitiveType);
            Helpers.Write(writer, (Int32)aIn.VertexCount);
            Helpers.Write(writer, aInfo, aIn.VertexDeclaration);
            Helpers.Write(writer, (Int32)aIn.VertexStrideInBytes);
            Helpers.Write(writer, aIn.Indices);
            Helpers.WriteBuffer(writer, aIn.Vertices);
            writer.Close();
        }

        public static void Write(BinaryWriter aOut, DocInfo aInfo, VertexElement[] aVertexDeclaration)
        {
            string baseFilename = (new Utilities.VertexDeclarationComparer()).GetHashCode(aVertexDeclaration).ToString();
            string refFilename = Path.Combine(aInfo.OutRelDirectory, Utilities.ExtractBaseFilename(Utilities.ExtractRelativeFilename(aInfo.InAbsDirectory, aInfo.Filename))) + "_" + baseFilename + kVertexDeclarationExtension;
            string outFilename = Path.Combine(aInfo.OutAbsDirectory, Utilities.ExtractBaseFilename(Utilities.ExtractRelativeFilename(aInfo.InAbsDirectory, aInfo.Filename))) + "_" + baseFilename + kVertexDeclarationExtension;

            Write(aOut, refFilename);
            BinaryWriter writer = new BinaryWriter(new FileStream(outFilename, FileMode.Create));

            Write(writer, (UInt32)aVertexDeclaration.Length);
            foreach (VertexElement e in aVertexDeclaration)
            {
                Write(writer, (UInt16)e.Stream);
                Write(writer, (UInt16)e.Offset);
                Write(writer, (byte)e.VertexElementFormat);
                Write(writer, (byte)e.VertexElementMethod);
                Write(writer, (byte)e.VertexElementUsage);
                Write(writer, (byte)e.UsageIndex);
            }
            writer.Close();
        }
    }

    public static class SceneWriter 
    {
        #region Private members
        private static void _WriteAnimatedMeshPart(BinaryWriter aOut, DocInfo aInfo, AnimatedMeshPartSceneNodeContent aNode)
        {
            _WriteSceneNode(aOut, aInfo, aNode);
            Helpers.Write(aOut, aInfo, aNode.Effect);
            Helpers.Write(aOut, aInfo, aNode.Material);
            Helpers.Write(aOut, aInfo, aNode.MeshPart);
            Helpers.Write(aOut, aNode.BindMatrix);
            Helpers.Write(aOut, (UInt32)aNode.InverseBindTransforms.Length);
            foreach (Matrix e in aNode.InverseBindTransforms)
            {
                Helpers.Write(aOut, e);
            }
            Helpers.Write(aOut, aNode.RootJoint);
            Helpers.Write(aOut, (UInt32)aNode.Joints.Length);
            foreach (string e in aNode.Joints)
            {
                Helpers.Write(aOut, e);
            }
        }

        private static void _WriteDirectionalLight(BinaryWriter aOut, DocInfo aInfo, DirectionalLightSceneNodeContent aNode)
        {
            _WriteSceneNode(aOut, aInfo, aNode);
            Helpers.Write(aOut, aNode.LightColor);
        }

        private static void _WriteJoint(BinaryWriter aOut, DocInfo aInfo, JointSceneNodeContent aNode)
        {
            _WriteSceneNode(aOut, aInfo, aNode);
            Helpers.Write(aOut, aNode.Animation.Id);
            Helpers.Write(aOut, (UInt32)aNode.Animation.KeyFrames.Length);
            foreach (AnimationKeyFrame e in aNode.Animation.KeyFrames)
            {
                Helpers.Write(aOut, e);
            }
        }

        private static void _WriteMeshPart(BinaryWriter aOut, DocInfo aInfo, MeshPartSceneNodeContent aNode)
        {
            _WriteSceneNode(aOut, aInfo, aNode);
            Helpers.Write(aOut, aInfo, aNode.Effect);
            Helpers.Write(aOut, aInfo, aNode.Material);
            Helpers.Write(aOut, aInfo, aNode.MeshPart);
        }

        private static void _WritePhysics(BinaryWriter aOut, DocInfo aInfo, PhysicsSceneNodeContent aNode)
        {
            _WriteSceneNode(aOut, aInfo, aNode);
            aNode.Tree.Write(aOut);
        }

        private static void _WritePointLight(BinaryWriter aOut, DocInfo aInfo, PointLightSceneNodeContent aNode)
        {
            _WriteSceneNode(aOut, aInfo, aNode);
            Helpers.Write(aOut, aNode.LightAttenuation);
            Helpers.Write(aOut, aNode.LightColor);
        }

        private static void _WriteSceneNode(BinaryWriter aOut, DocInfo aInfo, SceneNodeContent aNode)
        {
            Helpers.Write(aOut, (Int32)aNode.ChildrenCount);
            Helpers.Write(aOut, aNode.BaseId);
            Helpers.Write(aOut, aNode.Id);
            Helpers.Write(aOut, aNode.LocalTransform);
            Helpers.Write(aOut, (Int32)aNode.Type);
        }

        private static void _WriteSpotLight(BinaryWriter aOut, DocInfo aInfo, SpotLightSceneNodeContent aNode)
        {
            _WriteSceneNode(aOut, aInfo, aNode);
            Helpers.Write(aOut, aNode.FalloffAngleInRadians);
            Helpers.Write(aOut, aNode.FalloffExponent);
            Helpers.Write(aOut, aNode.LightAttenuation);
            Helpers.Write(aOut, aNode.LightColor);
        }
        #endregion

        public static void Write(BinaryWriter aOut, DocInfo aInfo, SceneContent aContent)
        {
            foreach (SceneNodeContent e in aContent.Nodes)
            {
                switch (e.Type)
                {
                    case SceneNodeType.AnimatedMeshPart: _WriteAnimatedMeshPart(aOut, aInfo, (AnimatedMeshPartSceneNodeContent)e); break;
                    case SceneNodeType.DirectionalLight: _WriteDirectionalLight(aOut, aInfo, (DirectionalLightSceneNodeContent)e); break;
                    case SceneNodeType.Joint: _WriteJoint(aOut, aInfo, (JointSceneNodeContent)e); break;
                    case SceneNodeType.MeshPart: _WriteMeshPart(aOut, aInfo, (MeshPartSceneNodeContent)e); break;
                    case SceneNodeType.Node: _WriteSceneNode(aOut, aInfo, e); break;
                    case SceneNodeType.Physics: _WritePhysics(aOut, aInfo, (PhysicsSceneNodeContent)e); break;
                    case SceneNodeType.PointLight: _WritePointLight(aOut, aInfo, (PointLightSceneNodeContent)e); break;
                    case SceneNodeType.SpotLight: _WriteSpotLight(aOut, aInfo, (SpotLightSceneNodeContent)e); break;
                    default:
                        throw new Exception(Utilities.kShouldNotBeHere);
                }
            }
        }
    }
}
