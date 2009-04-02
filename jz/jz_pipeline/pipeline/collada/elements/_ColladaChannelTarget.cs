using System;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    /// <summary>
    /// This is a helper class to link targets of animation channels back to the animation channel.
    /// </summary>
    /// <remarks>
    /// This class is valid currently as the only animation resolution I am doing is skeletal animation.
    /// Eventually this may need to be further generalized to support texture animation, for example.
    /// </remarks>
    public abstract class _ColladaChannelTarget : _ColladaElementWithSid
    {
        #region Protected members
        protected List<ColladaChannel> mTargetOf = new List<ColladaChannel>();
        #endregion

        public _ColladaChannelTarget(XmlReader aReader)
            : base(aReader)
        { }

        public void AddTargetOf(ColladaChannel a) { mTargetOf.Add(a); }
        public ColladaChannel[] TargetOf { get { return mTargetOf.ToArray(); } }
        public bool IsTargetted { get { return (mTargetOf.Count != 0); } }

        /// <summary>
        /// If this element is a target of a "channel" element, this returns the key frames described
        /// by that channel element as an array of time/matrix pairs.
        /// </summary>
        /// <param name="i">The index of the channel target to get the key frames from.</param>
        /// <returns>Key frames as time/matrix pairs.</returns>
        public virtual AnimationKeyFrame[] GetKeyFrames(int i)
        {
            throw new Exception(Utilities.kNotImplemented);
        }
    }

}
