using System;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    public sealed class ColladaSource : _ColladaElementWithIdAndName
    {
        public ColladaSource(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.kAsset);

            // add zero or one and only zero or one of (IDREF_array, Name_array, bool_array, float_array, int_array)
            {
                int arrayCount = 0;
                arrayCount += _AddOptionalChild(aReader, Elements.kIdrefArray);
                arrayCount += _AddOptionalChild(aReader, Elements.kNameArray);
                arrayCount += _AddOptionalChild(aReader, Elements.kBoolArray);
                arrayCount += _AddOptionalChild(aReader, Elements.kFloatArray);
                arrayCount += _AddOptionalChild(aReader, Elements.kIntArray);

                if (arrayCount > 1)
                {
                    throw new Exception("# of (IDREF_array, Name_array, bool_array, float_array, int_array) not equal to 0 or 1");
                }
            }

            if (_AddOptionalChild(aReader, Elements.kTechniqueCommonOfSource) > 0)
            {
                #region Deal with Name_array fugliness
                // Note: Name_array can either contain plain string values (in the case of
                //  an array of interpolation types) or it can contain sids (in the case of
                //  animation joints). The trick is, unlike Idref_array, Name_array provides no
                //  context for this within the Name_array element itself. So, this is an ugly hack.
                //  Basically, if the accessor for the technique of this source is of type "JOINT",
                //  then we spawn a Name_array of sids, Otherwise, a Name_array of strings. This
                //  can easily break as the spec does not enforce the accessor to even exist (the
                //  actual way of knowing this is a sid_array is by ending up here through references
                //  from <joint>).
                ColladaNameArray nameArray = GetFirstOptional<ColladaNameArray>();

                if (nameArray != null)
                {
                    ColladaAccessor accessor = GetFirst<ColladaTechniqueCommonOfSource>().GetFirst<ColladaAccessor>();
                    for (_ColladaElement e = accessor.FirstChild; e != null; e = e.NextSibling)
                    {
                        if (e is ColladaParam)
                        {
                            ColladaParam param = (ColladaParam)e;
                            if (param.Name == Enums.ParamName.kJoint && param.Type == Enums.Type.kName)
                            {
                                nameArray.ResolveToSids();
                                break;
                            }
                        }
                    }
                }
                #endregion
            }

            _AddZeroToManyChildren(aReader, Elements.kTechnique);
            #endregion   
        }
    }
}
