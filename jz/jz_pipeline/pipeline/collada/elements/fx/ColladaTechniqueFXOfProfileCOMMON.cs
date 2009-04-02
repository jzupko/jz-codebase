using System;
using System.Xml;

namespace jz.pipeline.collada.elements.fx
{
    public sealed class ColladaTechniqueFXOfProfileCOMMON : _ColladaTechniqueFX
    {
        public ColladaTechniqueFXOfProfileCOMMON(XmlReader aReader)
            : base(aReader)
        {
            #region Children
            _NextElement(aReader);

            _AddOptionalChild(aReader, Elements.kAsset);
            _AddZeroToManyChildren(aReader, Elements.kImage);

            #region Effect child
            ColladaEffectOfProfileCOMMON effect = null;
            {
                if (Enums.IsValidEffectType(aReader.Name))
                {
                    XmlReader sub = _Sub(aReader);
                    effect = new ColladaEffectOfProfileCOMMON(sub, Enums.GetEffectType(sub.Name));
                    _CheckSubFinished(sub);
                    _NextElement(aReader);
                    effect.Parent = this;
                }
                else
                {
                    throw new Exception("<technique> of <profile_COMMON> must have one material of type <constant>, <lambert>, <phong>, or <blinn> defined.");
                }
            }
            #endregion

            _AddZeroToManyChildren(aReader, Elements.kExtra);

            // Although the bump map is really part of the material child it is under the technique
            // element because it is a non-standard element and must be located under the <extra>
            // element of <technique_common>
            #region Bump map
            ColladaExtra extra = GetFirstOptional<ColladaExtra>();
            if (extra != null)
            {
                ColladaTechnique technique = extra.GetFirstOptional<ColladaTechnique>();
                if (technique != null)
                {
                    _ColladaGenericElement bump = technique.GetFirstOptional<_ColladaGenericElement>(delegate(_ColladaGenericElement e)
                    {
                        if (e.Name == Elements.FX.kBump.Name)
                        {
                            return true;
                        }

                        return false;
                    });

                    if (bump != null)
                    {
                        _ColladaGenericElement texture = bump.GetFirstOptional<_ColladaGenericElement>(delegate(_ColladaGenericElement e)
                        {
                            if (e.Name == Elements.FX.kTexture.Name)
                            {
                                return true;
                            }

                            return false;
                        });

                        if (texture != null)
                        {
                            _ColladaTexture colladaTexture = new _ColladaTexture();
                            effect.mBump = colladaTexture;

                            if (texture.GetContains(Attributes.kTexturecoord)) colladaTexture.Texcoords = texture[Attributes.kTexturecoord];

                            if (texture.GetContains(Attributes.kTexture))
                            {
                                ColladaDocument.QueueSidForResolution(texture[Attributes.kTexture], delegate(_ColladaElement e) { colladaTexture.Element = e; });
                            }
                        }
                    }
                }
            }
            #endregion

            #endregion
        }
    }
}
