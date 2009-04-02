using Microsoft.Xna.Framework;
using System;
using System.Xml;

namespace jz.pipeline.collada.elements.physics
{
    public sealed class ColladaTechniqueCommonOfPhysicsScene : _ColladaElement
    {
        #region Private members
        private readonly Vector3 mGravity = default(Vector3);
        private readonly float mTimeStep = 0.0f;
        #endregion

        public ColladaTechniqueCommonOfPhysicsScene(XmlReader aReader)
        {
            #region Children
            _NextElement(aReader);

            #region Gravity
            {
                string gravity = "";
                if (_SetValueOptional(aReader, Elements.Physics.kGravity.Name, ref gravity))
                {
                    float[] gravityVector = new float[3];
                    Utilities.Tokenize(gravity, gravityVector, XmlConvert.ToSingle);
                    mGravity.X = gravityVector[0];
                    mGravity.Y = gravityVector[1];
                    mGravity.Z = gravityVector[2];
                }
            }
            #endregion

            _SetValueOptional(aReader, Elements.Physics.kTimeStep.Name, ref mTimeStep);
            #endregion
        }

        public Vector3 Gravity { get { return mGravity; } }
        public float TimeStep { get { return mTimeStep; } }
    }
}
