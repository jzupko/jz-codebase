using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using Microsoft.Xna.Framework.Content.Pipeline;
using Microsoft.Xna.Framework.Content.Pipeline.Graphics;
using Microsoft.Xna.Framework.Content.Pipeline.Processors;
using Microsoft.Xna.Framework.Content.Pipeline.Serialization.Compiler;
using System;
using System.Collections.Generic;
using System.Text;
using jz.pipeline.collada.elements;
using jz.pipeline.collada.elements.fx;

namespace jz.pipeline.collada
{
    public sealed class BoundEffect
    {
        public sealed class Binding
        {
            #region Private members
            private string mSemantic;
            private uint mUsageIndex;
            #endregion

            public Binding(string aSemantic, uint aUsageIndex)
            {
                mSemantic = aSemantic;
                mUsageIndex = aUsageIndex;
            }

            public bool Has(string s) { return mSemantic == s; }
            public uint UsageIndex { get { return mUsageIndex; } }
        }

        public List<Binding> Bindings = new List<Binding>();

        public BoundEffect(ColladaInstanceMaterial m)
        {
            foreach (ColladaBindVertexInput e in m.GetEnumerable<ColladaBindVertexInput>())
            {
                ColladaBindVertexInput binding = (ColladaBindVertexInput)e;
                Bindings.Add(new Binding(binding.Semantic, binding.InputSet));
            }
        }

        public bool FindUsageIndex(string aSemantic, ref uint arOut)
        {
            foreach (Binding b in Bindings)
            {
                if (b.Has(aSemantic))
                {
                    arOut = b.UsageIndex;
                    return true;
                }
            }

            return false;
        }
    }
}
