using KeyFrames = System.Collections.Generic.Dictionary<jz.pipeline.collada.elements.ColladaAnimation, System.Collections.Generic.List<jz.AnimationKeyFrame>>;

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

    public sealed class ColladaProcessor 
    {
        private ColladaProcessorGeometry mGeometryProcessor = null;
        private ColladaProcessorMaterial mMaterialProcessor = null;
        private ColladaProcessorPhysics mPhysicsProcessor = null;

        /// <summary>
        /// Maximum number of skinning matrices.
        /// </summary>
        /// <seealso cref="jz.scene.AnimatedMeshPartNode"/>
        public const uint kSkinningMatricesCount = 72;
        public const uint kSkinningMatricesSize = kSkinningMatricesCount * 3;

        /// <summary>
        /// Maximum number of joints that can influence a single vertex.
        /// </summary>
        public const uint kJointInfluencesPerVertex = 4;

        #region Constants
        public const string kMeshPartPostfix = "_part";
        public const string kColladaExtension = ".dae";
        #endregion

        #region Private members
        private WeakRefContainer<string, JointSceneNodeContent> msJoints = new WeakRefContainer<string, JointSceneNodeContent>(string.Empty);

        private bool mbProcessPhysics = false;
        private string mBaseName = string.Empty;
        private ContentIdentity mIdentity = null;
        private ContentProcessorContext mContext = null;
        private SceneContent mScene = new SceneContent();

        #region Controller processing
        private struct IwEntry : IComparable<IwEntry>
        {
            public IwEntry(float aIndex, float aWeight)
            {
                Index = aIndex;
                Weight = aWeight;
            }

            public float Index;
            public float Weight;

            // Intentionally sort in descending order.
            public int CompareTo(IwEntry b)
            {
                if (Weight < b.Weight)
                {
                    return 1;
                }
                else if (Weight > b.Weight)
                {
                    return -1;
                }
                else
                {
                    return Index.CompareTo(b.Index);
                }
            }
        };

        private struct JointEntry : IComparable<JointEntry>
        {
            public JointEntry(float aIndex)
            {
                Index = aIndex;
                MaxWeight = 0.0f;
                bInUse = false;
                bExclusiveToVertex = false;
            }

            public float MaxWeight;
            public bool bExclusiveToVertex;
            public bool bInUse;
            public float Index;

            public int CompareTo(JointEntry b)
            {
                if (!bInUse && b.bInUse)
                {
                    return -1;
                }
                else if (bInUse && !b.bInUse)
                {
                    return 1;
                }
                else
                {
                    return MaxWeight.CompareTo(b.MaxWeight);
                }
            }
        }

        private int[] _GetBlendIndicesAndWeights(ColladaSkin aSkin, out float[] arIndices, out float[] arWeights)
        {
            ColladaVertexWeights vertexWeights = aSkin.GetFirst<ColladaVertexWeights>();
            ColladaVcount vcount = vertexWeights.GetFirst<ColladaVcount>();
            ColladaPrimitives v = vertexWeights.GetFirst<ColladaPrimitives>();

            arIndices = new float[vcount.Count * kJointInfluencesPerVertex];
            arWeights = new float[vcount.Count * kJointInfluencesPerVertex];

            ColladaInputGroupB input = vertexWeights.GetFirst<ColladaInputGroupB>(
               delegate(ColladaInputGroupB e)
               {
                   return (e.Semantic == _ColladaElement.Enums.InputSemantic.kWeight);
               });

            ColladaSource source = (ColladaSource)input.Source;
            ColladaFloatArray array = source.GetFirst<ColladaFloatArray>();

            uint outIndex = 0;
            uint vCountIndex = 0;
            uint vIndex = 0;
            uint vSize = v.Count;

            List<IwEntry> buf = new List<IwEntry>();

            bool bLoggedZeroCountWarning = false;
            bool bLoggedInfluenceWarning = false;

            JointEntry[] jointsUsed = new JointEntry[0];

            while (vIndex < vSize)
            {
                #region Get the number of influences for this vertex.
                uint vCount = vcount[vCountIndex++];
                #endregion

                #region Gather and sort the indices and weights for each influence by weight.
                buf.Clear();
                for (int i = 0; i < vCount; i++)
                {
                    IwEntry entry = new IwEntry(v[vIndex++], array[(uint)v[vIndex++]]);
                    if (entry.Weight > 0.0f) buf.Add(entry);
                }
                buf.Sort();
                vCount = (uint)buf.Count;

                if (vCount < 1)
                {
                    if (mContext != null && !bLoggedZeroCountWarning)
                    {
                        mContext.Logger.LogImportantMessage("A vertex of input \"" +
                            source.Id + "\" has no influences.");
                        bLoggedZeroCountWarning = true;
                    }
                }

                if (vCount > kJointInfluencesPerVertex)
                {
                    if (mContext != null && !bLoggedInfluenceWarning)
                    {
                        mContext.Logger.LogImportantMessage("Jz XNA supports only " +
                            kJointInfluencesPerVertex.ToString() + " bones per vertex. A vertex of input \"" +
                            source.Id + "\" is influenced by " + vCount + " bones. The highest weight bones " +
                            "will be used and the others will be ignored.");
                        bLoggedInfluenceWarning = true;
                    }
                }
                #endregion

                #region Populate the output weight and indice buffers with applicable inputs.
                uint count = 0;
                for (int i = 0; i < vCount && count < kJointInfluencesPerVertex; i++)
                {
                    float index = buf[i].Index;
                    float weight = buf[i].Weight;

                    if (index < 0.0f)
                    {
                        throw new Exception("Jz XNA does not support the bind matrix in bone weights. " +
                            "A vertex of input \"" + source.Id + "\" refers to the bind matrix.");
                    }
                    else
                    {
                        int intIndex = (int)index;
                        if (jointsUsed.Length <= intIndex)
                        {
                            int oldLength = jointsUsed.Length;
                            Array.Resize(ref jointsUsed, intIndex + 1);
                            for (int j = oldLength; j < jointsUsed.Length; j++)
                            {
                                jointsUsed[j] = new JointEntry(j);
                            }
                        }
                        else
                        {
                            jointsUsed[intIndex].Index = index;
                        }

                        jointsUsed[intIndex].MaxWeight = Utilities.Max(weight, jointsUsed[intIndex].MaxWeight);
                        jointsUsed[intIndex].bInUse = true;
                        jointsUsed[intIndex].bExclusiveToVertex = (vCount == 1);

                        arIndices[outIndex] = index;
                        arWeights[outIndex++] = weight;
                        count++;
                    }
                }
                outIndex += (kJointInfluencesPerVertex - count);
                #endregion

                #region Normalize the last four weights
                float sum = 0.0f;
                for (uint i = outIndex - kJointInfluencesPerVertex; i < outIndex - 1; i++)
                {
                    sum += arWeights[i];
                }

                if (!Utilities.AboutZero(sum))
                {
                    arWeights[outIndex - 1] = (1.0f - sum);
                }
                #endregion
            }

            #region Adjust the joint references to fit this skeleton in kSkinningMatricsCount
            if (jointsUsed.Length > kSkinningMatricesCount)
            {
                Array.Sort(jointsUsed);

                uint count = ((uint)jointsUsed.Length - kSkinningMatricesCount);
                int[] toRemove = new int[count];
                uint index = 0;
                for (int i = 0; i < jointsUsed.Length && index < count; i++)
                {
                    if (!jointsUsed[i].bInUse || !jointsUsed[i].bExclusiveToVertex)
                    {
                        toRemove[index++] = (int)jointsUsed[i].Index;
                    }
                }

                if (index < count)
                {
                    throw new Exception("Joints of input \"" + source.Id + "\" could not be fitted into " +
                        "the max joint count of " + kSkinningMatricesCount.ToString());
                }

                for (int i = 0; i < arIndices.Length; i++)
                {
                    for (int j = 0; j < count; j++)
                    {
                        if (arIndices[i] == (float)toRemove[j])
                        {
                            arIndices[i] = 0.0f;
                            arWeights[i] = 0.0f;
                        }
                        else if (arIndices[i] > toRemove[j])
                        {
                            arIndices[i] = arIndices[i] - 1.0f;
                        }
                    }
                }

                return toRemove;
            }
            else
            {
                return new int[0];
            }
            #endregion
        }

        private void _ProcessInstanceController(ColladaNode aNode, int aChildrenCount, ref Matrix aLocalTransform)
        {
            ColladaInstanceController instanceController = aNode.GetFirst<ColladaInstanceController>();
            ColladaController controller = instanceController.Instance;
            ColladaSkin skin = (ColladaSkin)controller.FirstChild;

            float[] indices;
            float[] weights;
            int[] toRemove = _GetBlendIndicesAndWeights(skin, out indices, out weights);

            JzMeshContent mesh;
            MaterialsBySymbol materials;
            EffectsBySymbol effects;
            _GetMeshAndMaterials(aNode, out mesh, out materials, out effects, indices, weights);

            Matrix bindMatrix = skin.XnaBindShapeTransform;
            Matrix[] invBindTransforms = skin.InverseBindTransforms;
            int invBindCount = invBindTransforms.Length;
            for (int i = 0; i < invBindCount; i++)
            {
                invBindTransforms[i] = invBindTransforms[i];
            }

            _ColladaArray<object> colladaJoints = skin.Joints;
            string rootJoint = ((ColladaNode)instanceController.SkeletonRoot).Id;

            uint jointCount = colladaJoints.Count;
            string[] joints = new string[jointCount];
            for (uint i = 0; i < jointCount; i++)
            {
                ColladaNode node = (ColladaNode)colladaJoints[i];
                joints[i] = node.Id;
            }

            #region Remove any joints and inv bind transforms for those joints that are not going to be used.
            if (invBindTransforms.Length != joints.Length)
            {
                throw new Exception("The number of inverse bind transforms and joints for controller node \"" +
                    aNode.Id + "\" is not the same.");
            }

            Utilities.Remove(toRemove, ref invBindTransforms);
            Utilities.Remove(toRemove, ref joints);

            // This can happen if the number of joints was larger than the number of joints influencing vertices.
            if (invBindTransforms.Length > kSkinningMatricesCount)
            {
                Array.Resize(ref invBindTransforms, (int)kSkinningMatricesCount);
                Array.Resize(ref joints, (int)kSkinningMatricesCount);
            }
            #endregion

            SceneNodeContent parentNode = new SceneNodeContent(
                mBaseName, aNode.Id, (aChildrenCount + mesh.Parts.Count), ref aLocalTransform);
            mScene.Nodes.Add(parentNode);

            int count = 0;
            foreach (JzMeshContent.Part e in mesh.Parts)
            {
                AnimatedMeshPartSceneNodeContent meshPartNode = new
                    AnimatedMeshPartSceneNodeContent(mBaseName, aNode.Id + kMeshPartPostfix + count.ToString(),
                    0, ref Utilities.kIdentity, effects[e.Effect],
                    materials[e.Effect], e, ref bindMatrix, invBindTransforms, rootJoint, joints);

                mScene.Nodes.Add(meshPartNode);
                count++;
            }
        }
        #endregion

        #region Joint processing
        private int _ProcessJointHelper(List<AnimationKeyFrame> aKeyFrames, AnimationKeyFrame aNewFrame)
        {
            int count = aKeyFrames.Count;
            for (int i = 0; i < count; i++)
            {
                if (Utilities.AboutEqual(aKeyFrames[i].TimeAndPadding.X, aNewFrame.TimeAndPadding.X, Utilities.kLooseToleranceFloat))
                {
                    return i;
                }
            }

            return -1;
        }

        /// <summary>
        /// Parses animation into a joint.
        /// </summary>
        /// <param name="aNode"></param>
        /// <param name="childrenCount"></param>
        /// <param name="aLocalTransform"></param>
        /// <returns></returns>
        /// <remarks>
        /// COLLADA's animation specification is poorly defined with regards to clustering
        /// of animations. There is no standard for grouping, for example, a set of animations
        /// into a "jump" animation. This function currently guesses and may have to be changed
        /// in the long run.
        /// </remarks>
        private void _ProcessJoint(ColladaNode aNode, int childrenCount, ref Matrix aLocalTransform)
        {
            // TODO: this function will not handle oddly mixed animation channels properly.
            string animationId = mBaseName + aNode.Id + "_animation";

            #region Validate key frames
            uint keyFramesCount = 0;
            foreach (_ColladaTransformElement e in aNode.GetEnumerable<_ColladaTransformElement>())
            {
                uint currentCount = (uint)e.GetKeyFrames(0).Length;
                if (keyFramesCount == 0)
                {
                    keyFramesCount = currentCount;
                }
                else if (currentCount != 0 && keyFramesCount != currentCount)
                {
                    throw new Exception("<node> \"" + aNode.Id + "\" has transform elements with " +
                        "different numbers of key frames.");
                }
            }
            #endregion

            #region Initialize key frames
            AnimationKeyFrame[] keyFrames = new AnimationKeyFrame[keyFramesCount];
            for (uint i = 0; i < keyFramesCount; i++)
            {
                keyFrames[i] = new AnimationKeyFrame(0.0f, Matrix.Identity);
            }
            #endregion

            #region Combine key frames into one channel of matrix key frames and create node.
            if (keyFramesCount > 0)
            {
                foreach (_ColladaTransformElement e in aNode.GetEnumerable<_ColladaTransformElement>())
                {
                    AnimationKeyFrame[] newFrames = e.GetKeyFrames(0);

                    if (newFrames.Length == 0)
                    {
                        for (uint i = 0; i < keyFramesCount; i++)
                        {
                            keyFrames[i] = new AnimationKeyFrame(keyFrames[i].TimeAndPadding.X,
                                e.XnaMatrix * keyFrames[i].Key);
                        }
                    }
                    else
                    {
                        for (uint i = 0; i < keyFramesCount; i++)
                        {
                            keyFrames[i] = new AnimationKeyFrame(newFrames[i].TimeAndPadding.X,
                                newFrames[i].Key * keyFrames[i].Key);
                        }
                    }
                }

                Array.Sort(keyFrames, delegate(AnimationKeyFrame a, AnimationKeyFrame b)
                {
                    if (Utilities.LessThan(a.TimeAndPadding.X, b.TimeAndPadding.X))
                    {
                        return -1;
                    }
                    else if (Utilities.GreaterThan(a.TimeAndPadding.X, b.TimeAndPadding.X))
                    {
                        return 1;
                    }
                    else
                    {
                        return 0;
                    }
                });
            }
            else
            {
                keyFramesCount = 1;
                keyFrames = new AnimationKeyFrame[keyFramesCount];
                keyFrames[0].Key = aLocalTransform;
                keyFrames[0].TimeAndPadding.X = 0.0f;
            }

            for (int i = 0; i < keyFramesCount; i++)
            {
                keyFrames[i].Key = keyFrames[i].Key;
            }

            JointSceneNodeContent jointNode = new JointSceneNodeContent(mBaseName, aNode.Id, childrenCount,
                ref aLocalTransform, new AnimationContent(animationId, keyFrames));

            msJoints.Add(jointNode.Id, jointNode);

            mScene.Nodes.Add(jointNode);
            #endregion
        }
        #endregion

        private void _GetMeshAndMaterials(ColladaNode aNode, out JzMeshContent arMesh,
            out MaterialsBySymbol arMaterials, out EffectsBySymbol arEffects,
            float[] aBoneIndices, float[] aBoneWeights)
        {
            ColladaInstanceController instanceController = aNode.GetFirst<ColladaInstanceController>();
            ColladaController controller = instanceController.Instance;
            ColladaSkin skin = (ColladaSkin)controller.FirstChild;
            ColladaGeometry geometry = (ColladaGeometry)skin.Source;

            EffectsBySymbol effectsBySymbol = new EffectsBySymbol();
            MaterialsBySymbol materialsBySymbol = new MaterialsBySymbol();
            ColladaBindMaterial bindMaterial = instanceController.GetFirstOptional<ColladaBindMaterial>();

            if (bindMaterial != null)
            {
                mMaterialProcessor.GetMaterials(bindMaterial, ref effectsBySymbol, ref materialsBySymbol, true);
            }

            arEffects = effectsBySymbol;
            arMaterials = materialsBySymbol;
            // Temp: need to determine bump map tex coord properly.
            arMesh = mGeometryProcessor.GetMesh(geometry, aBoneIndices, aBoneWeights, arEffects);
        }

        private void _GetMeshAndMaterials(ColladaNode aNode, out JzMeshContent arMesh, 
            out MaterialsBySymbol arMaterials, out EffectsBySymbol arEffects)
        {
            ColladaInstanceGeometry instanceGeometry = aNode.GetFirst<ColladaInstanceGeometry>();

            EffectsBySymbol effectsBySymbol = new EffectsBySymbol();
            MaterialsBySymbol materialsBySymbol = new MaterialsBySymbol();
            ColladaBindMaterial bindMaterial = instanceGeometry.GetFirstOptional<ColladaBindMaterial>();

            if (bindMaterial != null)
            {
                mMaterialProcessor.GetMaterials(bindMaterial, ref effectsBySymbol, ref materialsBySymbol, false);
            }

            arEffects = effectsBySymbol;
            arMaterials = materialsBySymbol;
            arMesh = mGeometryProcessor.GetMesh(instanceGeometry.Instance, null, null, arEffects);
        }
        
        private void _ProcessInstanceGeometry(ColladaNode aNode, int aChildrenCount, ref Matrix aLocalTransform, ref Matrix aWorldTransform)
        {
            JzMeshContent mesh;
            MaterialsBySymbol materials;
            EffectsBySymbol effects;
            _GetMeshAndMaterials(aNode, out mesh, out materials, out effects);

            SceneNodeContent parentNode = new SceneNodeContent(mBaseName, aNode.Id, (aChildrenCount + mesh.Parts.Count), ref aLocalTransform);
            mScene.Nodes.Add(parentNode);

            int count = 0;
            foreach (JzMeshContent.Part e in mesh.Parts)
            {
                MeshPartSceneNodeContent meshPartNode = new MeshPartSceneNodeContent(mBaseName, aNode.Id + kMeshPartPostfix + count.ToString(), 0, ref Utilities.kIdentity, ref aWorldTransform, effects[e.Effect], materials[e.Effect], e);
                mScene.Nodes.Add(meshPartNode);
                count++;
            }
            parentNode.ChildrenCount = count;
        }
        #endregion

        private void _ProcessInstanceLight(ColladaNode aNode, int childrenCount, ref Matrix aLocalTransform)
        {
            ColladaLight light = aNode.GetFirst<ColladaInstanceLight>().Instance;
            ColladaLightData lightData = light.LightData;

            Vector3 lightColor = light.MayaIntensity * lightData.Color;

            if (lightData.Type == _ColladaElement.Enums.LightType.kAmbient)
            {
                if (mContext != null)
                {
                    mContext.Logger.LogImportantMessage("Warning: Jz XNA does not explicitly support " +
                        "light type <ambient>. The light referenced by <node> element \"" +
                        aNode.Id + "\" has been converted into a directional light.");
                }

                mScene.Nodes.Add(new DirectionalLightSceneNodeContent(mBaseName, aNode.Id, childrenCount, ref aLocalTransform, lightColor));
                return;
            }
            else if (lightData.Type == _ColladaElement.Enums.LightType.kDirectional)
            {
                mScene.Nodes.Add(new DirectionalLightSceneNodeContent(mBaseName, aNode.Id, childrenCount, ref aLocalTransform, lightColor));
                return;
            }
            else if (lightData.Type == _ColladaElement.Enums.LightType.kPoint)
            {
                Vector3 attenuation = new Vector3(lightData.ConstantAttenuation, lightData.LinearAttenuation, lightData.QuadraticAttenuation);
                mScene.Nodes.Add(new PointLightSceneNodeContent(mBaseName, aNode.Id, childrenCount, ref aLocalTransform, lightColor, attenuation));
                return;
            }
            else if (lightData.Type == _ColladaElement.Enums.LightType.kSpot)
            {
                float dropoff = light.MayaDropoff;
                Vector3 attenuation = new Vector3(lightData.ConstantAttenuation, lightData.LinearAttenuation, lightData.QuadraticAttenuation);
                float angleInRadians = MathHelper.ToRadians(lightData.FalloffAngleInDegrees);
                float falloffExponent = Utilities.AboutZero(dropoff, Utilities.kLooseToleranceFloat) ? lightData.FalloffExponent : dropoff;
                mScene.Nodes.Add(new SpotLightSceneNodeContent(mBaseName, aNode.Id, childrenCount, ref aLocalTransform, lightColor, attenuation, angleInRadians, falloffExponent));
                return;
            }

            throw new Exception(Utilities.kShouldNotBeHere);
        }

        #region Scene node processing
        private void _CheckAndSetAlreadyFoundInstance(ref bool abFoundInstanceElement, string aNodeId)
        {
            if (abFoundInstanceElement)
            {
                throw new Exception("Jz XNA does not support multiple <instance_*> elements " +
                    "as children of <node> elements. Please remove multiple <instance_*> " +
                    "elements from <node> element \"" + aNodeId + "\" and try this build " +
                    "again.");
            }
            else
            {
                abFoundInstanceElement = true;
            }
        }

        private void _ProcessNode(ColladaNode aNode, ref Matrix aParentTransform)
        {
            bool bFoundInstanceElement = false;
            int childrenCount = aNode.GetChildCount<ColladaNode>();
            Matrix localTransform = aNode.LocalXnaTransform;
            Matrix worldTransform = localTransform * aParentTransform;

            // This node is expected to be part of a bone hiearchy.
            if (aNode.Type == _ColladaElement.Enums.InputSemantic.kJoint)
            {
                _ProcessJoint(aNode, childrenCount, ref localTransform);
                _CheckAndSetAlreadyFoundInstance(ref bFoundInstanceElement, aNode.Id);
            }

            // Process any special nodes. 
            if (!bFoundInstanceElement)
            {
                foreach (_ColladaElement e in aNode.GetEnumerable<_ColladaElement>())
                {
                    if (e is ColladaInstanceCamera)
                    {
                        _CheckAndSetAlreadyFoundInstance(ref bFoundInstanceElement, aNode.Id);
                        if (mContext != null)
                        {
                            mContext.Logger.LogImportantMessage("Jz XNA currently does not support " +
                                "<instance_camera> as a child of <node>. This will be added in a future " +
                                "release but the <node> \"" + aNode.Id + "\" has been converted to a " +
                                "standard scene node for now.");
                        }
                        mScene.Nodes.Add(new SceneNodeContent(mBaseName, aNode.Id, childrenCount, ref localTransform));
                    }
                    else if (e is ColladaInstanceController)
                    {
                        _CheckAndSetAlreadyFoundInstance(ref bFoundInstanceElement, aNode.Id);
                        _ProcessInstanceController(aNode, childrenCount, ref localTransform);
                    }
                    else if (e is ColladaInstanceGeometry)
                    {
                        _CheckAndSetAlreadyFoundInstance(ref bFoundInstanceElement, aNode.Id);
                        _ProcessInstanceGeometry(aNode, childrenCount, ref localTransform, ref worldTransform);
                    }
                    else if (e is ColladaInstanceLight)
                    {
                        _CheckAndSetAlreadyFoundInstance(ref bFoundInstanceElement, aNode.Id);
                        _ProcessInstanceLight(aNode, childrenCount, ref localTransform);
                    }
                    else if (e is ColladaInstanceNode)
                    {
                        _CheckAndSetAlreadyFoundInstance(ref bFoundInstanceElement, aNode.Id);
                        if (mContext != null)
                        {
                            mContext.Logger.LogImportantMessage("Jz XNA current does not support " +
                                "<instance_node> as a child of <node>. This will be added in a future " +
                                "release but the <node> \"" + aNode.Id + "\" has been converted to a " +
                                "standard scene node for now.");
                        }
                        mScene.Nodes.Add(new SceneNodeContent(mBaseName, aNode.Id, childrenCount, ref localTransform));
                    }
                }
            }

            // If this node wasn't a special node, add a regular scene node.
            if (!bFoundInstanceElement)
            {
                mScene.Nodes.Add(new SceneNodeContent(mBaseName, aNode.Id, childrenCount, ref localTransform));
            }

            // Process children.
            if (childrenCount > 0)
            {
                foreach (ColladaNode n in aNode.GetEnumerable<ColladaNode>())
                {
                    _ProcessNode(n, ref worldTransform);
                }
            }
        }

        private void _ProcessRoot(ColladaCOLLADA aRoot)
        {
            ColladaVisualScene visualScene = aRoot.GetFirst<ColladaScene>().GetFirst<ColladaInstanceVisualScene>().Instance;

            int childrenCount = visualScene.GetChildCount<ColladaNode>();
            SceneNodeContent root = new SceneNodeContent(mBaseName, visualScene.Id, childrenCount, ref Utilities.kIdentity);
            mScene.Nodes.Add(root);

            foreach (ColladaNode n in visualScene.GetEnumerable<ColladaNode>())
            {
                _ProcessNode(n, ref Utilities.kIdentity);
            }
        }
        #endregion

        public SceneContent Process(ColladaCOLLADA aRoot, ContentProcessorContext aContext)
        {
            mBaseName = Path.GetFileNameWithoutExtension(aRoot.SourceFile);
            mIdentity = new ContentIdentity(aRoot.SourceFile);
            mContext = aContext;

            if (aRoot.GetFirst<ColladaAsset>().UpAxis != Vector3.Up) { throw new Exception(Utilities.kNotImplemented); }

            ColladaProcessorGeometry.Settings settings = new ColladaProcessorGeometry.Settings();
            settings.Flags |= (ColladaProcessorGeometry.FilterFlags.FlagsInclusive |
                ColladaProcessorGeometry.FilterFlags.Position |
                ColladaProcessorGeometry.FilterFlags.Normal |
                ColladaProcessorGeometry.FilterFlags.TextureCoordinate |
                ColladaProcessorGeometry.FilterFlags.BlendIndices |
                ColladaProcessorGeometry.FilterFlags.BlendWeight);
            settings.bGenerateBinormals = false;
            settings.bGenerateNormals = false;
            settings.bGenerateTangents = true;
            settings.bReprocessWhenGenerateFails = false;
            settings.WeldingDistanceTolerance = Utilities.kLooseToleranceFloat;

            mGeometryProcessor = new ColladaProcessorGeometry(settings);
            mMaterialProcessor = new ColladaProcessorMaterial(mIdentity);
            mPhysicsProcessor = new ColladaProcessorPhysics();

            _ProcessRoot(aRoot);

            if (mbProcessPhysics)
            {
                TriangleTree tree = mPhysicsProcessor.ProcessPhysics(aRoot);

                if (mScene.Nodes.Count < 1 || mScene.Nodes[0] == null) { throw new ArgumentNullException(); }

                mScene.Nodes[0].ChildrenCount++;
                mScene.Nodes.Insert(1, new PhysicsSceneNodeContent(mBaseName, "physics_node", tree));
            }

            return mScene;
        }

        public bool bProcessPhysics { get { return mbProcessPhysics; } set { mbProcessPhysics = value; } }
    }
}
