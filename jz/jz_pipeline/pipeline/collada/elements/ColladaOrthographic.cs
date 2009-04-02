using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    // Note: this is one of the few times that I am not extracting information from
    //     the COLLADA file. Each of the child elements of an Orthographi element
    //     <xmag>, <ymag>, <aspect_ratio>, <znear>, <zfar> can have an individual sid.
    //     These are not stored, nor are all of the original values (whatever combination
    //     of <xmag>, <ymag>, <aspect_ratio> are convereted into width, height.
    // Todo: verify that the calculations for the case of only <xmag> or only <ymag> are
    //     correct.
    public sealed class ColladaOrthographic : _ColladaElement
    {
        #region Private members
        private readonly float mWidth;
        private readonly float mHeight;
        private readonly float mNear;
        private readonly float mFar;
        #endregion

        public ColladaOrthographic(XmlReader aReader)
        {
            #region Children
            bool bXmag = false;
            bool bYmag = false;
            bool bAspectRatio = false;
            float xmag = 0.0f;
            float ymag = 0.0f;
            float aspectRatio = 0.0f;

            _NextElement(aReader);

            bXmag = _SetValueOptional(aReader, Elements.kXmag.Name, ref xmag);
            bYmag = _SetValueOptional(aReader, Elements.kYmag.Name, ref ymag);
            bAspectRatio = _SetValueOptional(aReader, Elements.kAspectRatio.Name, ref aspectRatio);
            _SetValueRequired(aReader, Elements.kZnear.Name, out mNear);
            _SetValueRequired(aReader, Elements.kZfar.Name, out mFar);

            if (bXmag && !bYmag && !bAspectRatio)
            {
                mWidth = xmag * 2.0f;
                mHeight = xmag * 2.0f;
            }
            else if (!bXmag && bYmag && !bAspectRatio)
            {
                mWidth = ymag * 2.0f;
                mHeight = ymag * 2.0f;
            }
            else if (bXmag && bYmag && !bAspectRatio)
            {
                mWidth = xmag * 2.0f;
                mHeight = ymag * 2.0f;
            }
            else if (bXmag && !bYmag && bAspectRatio)
            {
                mWidth = xmag * 2.0f;
                mHeight = (xmag / aspectRatio) * 2.0f;
            }
            else if (!bXmag && bYmag && bAspectRatio)
            {
                mWidth = (ymag * aspectRatio) * 2.0f;
                mHeight = ymag * 2.0f;
            }
            else
            {
                throw new Exception("incorrect combination of child elements for <orthographi>.");
            }
            #endregion
        }
    }
}
