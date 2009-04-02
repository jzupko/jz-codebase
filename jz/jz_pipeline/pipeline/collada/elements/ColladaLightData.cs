using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    /// <summary>
    /// Encapsulates a COLLADA light element (ambient, directional, point, or spot).
    /// </summary>
    /// <remarks>
    /// Color is currently folded into the class although it can have a sid (used for
    /// targetting animations). This should be extracted eventually.
    /// </remarks>
    public sealed class ColladaLightData : _ColladaElement
    {
        #region Private members
        private readonly Vector3 mColor;
        private readonly float mConstantAttenuation = Defaults.kConstantAttenuation;
        private readonly float mLinearAttenuation = Defaults.kLinearAttenuation;
        private readonly float mQuadraticAttenuation = Defaults.kQuadraticAttenuation;
        private readonly float mFalloffAngleDegrees = Defaults.kFalloffAngle;
        private readonly float mFalloffExponent = Defaults.kFalloffExponent;
        private readonly Enums.LightType mType = Enums.LightType.kAmbient;
        #endregion

        public const string kColorElement = "color";

        public ColladaLightData(XmlReader aReader, Enums.LightType aType)
        {
            mType = aType;

            #region Children
            _NextElement(aReader);

            #region Color
            string color;
            float[] colorValues = new float[3];
            _SetValueRequired(aReader, kColorElement, out color);
            Utilities.Tokenize(color, colorValues, XmlConvert.ToSingle);
            mColor = new Vector3(colorValues[0], colorValues[1], colorValues[2]);
            #endregion

            if (mType == Enums.LightType.kPoint || mType == Enums.LightType.kSpot)
            {
                _SetValueOptional(aReader, Elements.kConstantAttenuation.Name, ref mConstantAttenuation);
                _SetValueOptional(aReader, Elements.kLinearAttenuation.Name, ref mLinearAttenuation);
                _SetValueOptional(aReader, Elements.kQuadraticAttenuation.Name, ref mQuadraticAttenuation);
            }

            if (mType == Enums.LightType.kSpot)
            {
                _SetValueOptional(aReader, Elements.kFalloffAngle.Name, ref mFalloffAngleDegrees);
                _SetValueOptional(aReader, Elements.kFalloffExponent.Name, ref mFalloffExponent);
            }
            #endregion
        }

        public Vector3 Color { get { return mColor; } }
        public float ConstantAttenuation { get { return mConstantAttenuation; } }
        public float LinearAttenuation { get { return mLinearAttenuation; } }
        public float QuadraticAttenuation { get { return mQuadraticAttenuation; } }
        public float FalloffAngleInDegrees { get { return mFalloffAngleDegrees; } }
        public float FalloffExponent { get { return mFalloffExponent; } }
        public Enums.LightType Type { get { return mType; } }
    }
}
