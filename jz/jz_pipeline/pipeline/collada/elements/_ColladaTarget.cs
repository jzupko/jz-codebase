using System;
using System.Collections.Generic;
using System.Text;

namespace jz.pipeline.collada.elements
{
    public sealed class _ColladaTarget
    {
        #region Private members
        private readonly _ColladaElement mTarget;
        private readonly string mAccessors;
        #endregion

        public _ColladaTarget(_ColladaElement aTarget, string aAccessors)
        {
            mTarget = aTarget;
            mAccessors = aAccessors;
        }

        public _ColladaElement Target { get { return mTarget; } }
        public string Accessors { get { return mAccessors; } }
    }
}
