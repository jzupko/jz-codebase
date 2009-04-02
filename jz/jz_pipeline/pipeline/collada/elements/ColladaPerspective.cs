using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    // Note: this is one of the few times that I am not extracting information from
    //     the COLLADA file. Each of the child elements of a Perspective element
    //     <xfov>, <yfov>, <aspect_ratio>, <znear>, <zfar> can have an individual sid.
    //     These are not stored, nor are all of the original values (whatever combination
    //     of <xfov>, <yfov>, <aspect_ratio> are convereted into yfov, aspectratio.
    // Todo: I currently assume an aspect ratio of 1 if only <xfov> or only <yfov> is specified.
    //     This is probably not correct behavior.
    public sealed class ColladaPerspective : _ColladaElement
    {
        #region Private members
        private readonly float mYfov;
        private readonly float mAspectRatio;
        private readonly float mNear;
        private readonly float mFar;
        #endregion

        public ColladaPerspective(XmlReader aReader)
        {
            #region Children
            bool bXfov = false;
            bool bYfov = false;
            bool bAspectRatio = false;
            float xfov = 0.0f;
            float yfov = 0.0f;
            float aspectRatio = 0.0f;

            _NextElement(aReader);

            bXfov = _SetValueOptional(aReader, Elements.kXfov.Name, ref xfov);
            bYfov = _SetValueOptional(aReader, Elements.kYfov.Name, ref yfov);
            bAspectRatio = _SetValueOptional(aReader, Elements.kAspectRatio.Name, ref aspectRatio);
            _SetValueRequired(aReader, Elements.kZnear.Name, out mNear);
            _SetValueRequired(aReader, Elements.kZfar.Name, out mFar);

            if (bXfov && !bYfov && !bAspectRatio)
            {
                mYfov = xfov;
                mAspectRatio = 1.0f;
            }
            else if (!bXfov && bYfov && !bAspectRatio)
            {
                mYfov = yfov;
                mAspectRatio = 1.0f;
            }
            else if (bXfov && bYfov && !bAspectRatio)
            {
                mYfov = yfov;
                mAspectRatio = (yfov / xfov);
            }
            else if (bXfov && !bYfov && bAspectRatio)
            {
                mYfov = aspectRatio * xfov;
                mAspectRatio = aspectRatio;
            }
            else if (!bXfov && bYfov && bAspectRatio)
            {
                mYfov = yfov;
                mAspectRatio = aspectRatio;
            }
            else
            {
                throw new Exception("incorrect combination of child elements for <perspective>.");
            }
            #endregion
        }

        public float Yfov { get { return mYfov; } }
        public float AspectRatio { get { return mAspectRatio; } }
        public float Near { get { return mNear; } }
        public float Far { get { return mFar; } }
    }
}
