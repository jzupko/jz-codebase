using System;
using System.IO;
using System.Xml;

namespace jz.pipeline.collada.elements
{
    // Todo: need to support <data> element for embedded image data,
    //    first need to figure out what the format is precisely.
    public sealed class ColladaImage : _ColladaElementWithIdAndName
    {
        #region Private members
        private readonly string mFormat = "";
        private readonly uint mHeight = 0;
        private readonly uint mWidth = 0;
        private readonly uint mDepth = Defaults.kImageDepth;
        private readonly object mDataOrLocation;
        #endregion

        public ColladaImage(XmlReader aReader)
            : base(aReader)
        {
            #region Attributes
            _SetOptionalAttribute(aReader, Attributes.kFormat, ref mFormat);
            _SetOptionalAttribute(aReader, Attributes.kHeight, ref mHeight);
            _SetOptionalAttribute(aReader, Attributes.kWidth, ref mWidth);
            _SetOptionalAttribute(aReader, Attributes.kDepth, ref mDepth);
            #endregion

            #region Children
            _NextElement(aReader);
            _AddOptionalChild(aReader, Elements.kAsset);

            #region <init_from> or <data>
            {
                byte[] data = null;
                string location = string.Empty;

                _SetValueOptional(aReader, Elements.kInitFrom.Name, ref location);
                _SetValueOptional(aReader, Elements.kData.Name, ref data);

                if (data != null && location != string.Empty)
                {
                    throw new Exception("one and only one of <data> or <init_from> must be defined under element <image>.");
                }
                else if (data != null)
                {
                    mDataOrLocation = data;
                }
                else
                {
                    mDataOrLocation = Utilities.FromUriFileToPath(ColladaDocument.CurrentBase, location);
                }
            }
            #endregion

            _AddZeroToManyChildren(aReader, Elements.kExtra);
            #endregion
        }

        public byte[] Data { get { return (byte[])mDataOrLocation; } }
        public uint Depth { get { return mDepth; } }
        public string Format { get { return mFormat; } }
        public uint Height { get { return mHeight; } }
        public bool IsData { get { return (mDataOrLocation is byte[]); } }
        public bool IsLocation { get { return (mDataOrLocation is string); } }
        public string Location { get { return (string)mDataOrLocation; } }
        public uint Width { get { return mWidth; } }
    }
}
