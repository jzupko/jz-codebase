using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;

using jz.pipeline.collada.elements;

namespace jz.pipeline.collada
{
    public static class ColladaDocument
    {
        public static PrimitiveType ColladaPrimitiveToXnaPrimitive(_ColladaPrimitive aColladaPrimitive)
        {
            if (aColladaPrimitive is ColladaTriangles) { return PrimitiveType.TriangleList; }
            else if (aColladaPrimitive is ColladaTrifans) { return PrimitiveType.TriangleFan; }
            else if (aColladaPrimitive is ColladaTristrips) { return PrimitiveType.TriangleStrip; }
            else if (aColladaPrimitive is ColladaLines) { return PrimitiveType.LineList; }
            else if (aColladaPrimitive is ColladaLinestrips) { return PrimitiveType.LineStrip; }
            else
            {
                throw new Exception("The COLLADA primitive type \"" + aColladaPrimitive.GetType().Name + "\" is not yet supported.");
            }
        }

        public static VertexElementFormat ColladaSemanticToXnaFormat(string aColladaSemantic, uint aStride)
        {
            switch (aStride)
            {
                case 1u: return VertexElementFormat.Single;
                case 2u: return VertexElementFormat.Vector2;
                case 3u: return VertexElementFormat.Vector3;
                case 4u: return VertexElementFormat.Vector4;
                default:
                    throw new Exception("Cannot convert COLLADA semantic \"" + aColladaSemantic + "\" with stride " + aStride.ToString() + " to an XNA vertex format.");
            }
        }

        public static VertexElementUsage ColladaSemanticToXnaUsage(string aColladaSemantic)
        {
            switch (aColladaSemantic)
            {
                case _ColladaElement.Enums.InputSemantic.kColor: return VertexElementUsage.Color;
                case _ColladaElement.Enums.InputSemantic.kNormal: return VertexElementUsage.Normal;
                case _ColladaElement.Enums.InputSemantic.kPosition: return VertexElementUsage.Position;
                case _ColladaElement.Enums.InputSemantic.kTexbinormal: return VertexElementUsage.Binormal;
                case _ColladaElement.Enums.InputSemantic.kTexcoord: return VertexElementUsage.TextureCoordinate;
                case _ColladaElement.Enums.InputSemantic.kTextangent: return VertexElementUsage.Tangent;
                default:
                    throw new Exception("cannot convert COLLADA semantic \"" + aColladaSemantic + "\" to an XNA vertex usage.");
            }
        }

        public static string ColladaSurfaceFilterToHlsl(_ColladaElement.Enums.SamplerFilter aFilter)
        {
            // const string kAnisotropic = "Anisotropic";
            const string kLinear = "Linear";
            const string kNone = "None";
            const string kPoint = "Point";

            switch (aFilter)
            {
                case _ColladaElement.Enums.SamplerFilter.Linear: return kLinear;
                case _ColladaElement.Enums.SamplerFilter.LinearMipmapLinear: return kLinear;
                case _ColladaElement.Enums.SamplerFilter.LinearMipmapNearest: return kLinear;
                case _ColladaElement.Enums.SamplerFilter.Nearest: return kPoint;
                case _ColladaElement.Enums.SamplerFilter.NearestMipmapLinear: return kPoint;
                case _ColladaElement.Enums.SamplerFilter.NearestMipmapNearest: return kPoint;
                case _ColladaElement.Enums.SamplerFilter.None: return kNone;
                default:
                    throw new Exception(Utilities.kShouldNotBeHere);
            }
        }

        public static string ColladaSurfaceWrapToHlsl(_ColladaElement.Enums.SamplerWrap aWrap)
        {
            const string kBorder = "Border";
            const string kClamp = "Clamp";
            const string kMirror = "Mirror";
            const string kWrap = "Wrap";

            switch (aWrap)
            {
                case _ColladaElement.Enums.SamplerWrap.Border: return kBorder;
                case _ColladaElement.Enums.SamplerWrap.Clamp: return kClamp;
                case _ColladaElement.Enums.SamplerWrap.Mirror: return kMirror;
                case _ColladaElement.Enums.SamplerWrap.None: return kBorder;
                case _ColladaElement.Enums.SamplerWrap.Wrap: return kWrap;
                default:
                    throw new Exception(Utilities.kShouldNotBeHere);
            }
        }      

        #region Private members
        private struct ResolutionEntry
        {
            public string Element;
            public ResolutionAction Action;
        }

        private static string msCurrentBase = string.Empty;
        private static SortedList<string, _ColladaElement> msElements = new SortedList<string, _ColladaElement>();
        private static List<string> msLoggedMessages = new List<string>();
        private static List<string> msToLoad = new List<string>();
        private static LinkedList<ResolutionEntry> msToResolveIds = new LinkedList<ResolutionEntry>();
        private static LinkedList<ResolutionEntry> msToResolveSids = new LinkedList<ResolutionEntry>();

        private static string _GetAbsoluteId(string aId)
        {
            string retId = aId.Trim();

            if (retId.Length == 0)
            {
                throw new Exception("An empty uri is not valid.");
            }
            else if (retId[0] == _ColladaElement.Settings.kFragmentDelimiter)
            {
                retId = msCurrentBase + retId;
            }
            else
            {
                int pos = retId.IndexOf(_ColladaElement.Settings.kFragmentDelimiter);

                if (pos < 1)
                {
                    retId = msCurrentBase + _ColladaElement.Settings.kFragmentDelimiter + retId;
                }
                else
                {
                    string baseName = Utilities.FromUriFileToPath(msCurrentBase, retId.Substring(0, pos));
                    string id = retId.Substring(pos);

                    if (!Path.IsPathRooted(baseName))
                    {
                        string previousDirectory = Environment.CurrentDirectory;
                        string desiredDirectory = Path.GetDirectoryName(msCurrentBase);
                        Environment.CurrentDirectory = desiredDirectory;
                        {
                            baseName = Path.GetFullPath(baseName);
                        }
                        Environment.CurrentDirectory = previousDirectory;
                    }

                    retId = baseName + id;
                }
            }

            return retId;
        }

        private static XmlReaderSettings _GetReaderSettings()
        {
            XmlReaderSettings ret = new XmlReaderSettings();

            ret.CheckCharacters = true;
            ret.ConformanceLevel = ConformanceLevel.Document;
            ret.ProhibitDtd = true;
            ret.ValidationFlags |= System.Xml.Schema.XmlSchemaValidationFlags.ReportValidationWarnings;
            ret.ValidationType = ValidationType.Schema;

            return ret;
        }

        #region Sid handling
        private static bool _ResolveSid(Stack<string> aSids, ref LinkedListNode<ResolutionEntry> e, _ColladaElement aElement)
        {
            string sid = aSids.Peek();

            if ((aElement is _ColladaElementWithSid && ((_ColladaElementWithSid)aElement).Sid == sid) ||
                (aElement is ColladaNode && ((ColladaNode)aElement).Sid == sid))
            {
                aSids.Pop();

                if (aSids.Count == 0)
                {
                    e.Value.Action(aElement);
                    LinkedListNode<ResolutionEntry> t = e;
                    e = e.Next;
                    msToResolveSids.Remove(t);
                    return true;
                }
                else
                {
                    return _ResolveSid(aSids, ref e, aElement);
                }
            }

            for (_ColladaElement child = aElement.FirstChild; child != null; child = child.NextSibling)
            {
                if (_ResolveSid(aSids, ref e, child))
                {
                    return true;
                }
            }

            return false;
        }

        private static void _ResolveIdElement(ref LinkedListNode<ResolutionEntry> e)
        {
            _ColladaElement element;

            if (msElements.TryGetValue(e.Value.Element, out element))
            {
                e.Value.Action(element);
                LinkedListNode<ResolutionEntry> t = e;
                e = e.Next;
                msToResolveIds.Remove(t);
            }
            else
            {
                e = e.Next;
            }
        }

        private static void _CleanSid(string aSid, out string arSid)
        {
            string sid = aSid;
            int index = sid.IndexOf(_ColladaElement.Settings.kTargetAddressSeperator);
            sid = sid.Substring(index + 1);

            index = sid.LastIndexOf(_ColladaElement.Settings.kTargetAddressMemberSelect);
            if (index < 0)
            {
                index = sid.LastIndexOf(_ColladaElement.Settings.kTargetAddressArrayAccessLeft);
            }

            if (index > 0)
            {
                sid = sid.Substring(0, index);
            }

            arSid = sid;
        }

        private static void _ParseSid(string aSid, out Stack<string> arSidStack)
        {
            string sid = aSid;
            int index = aSid.LastIndexOf(_ColladaElement.Settings.kTargetAddressSeperator);
            arSidStack = new Stack<string>();

            while (index > 0)
            {
                arSidStack.Push(sid.Substring(index + 1));
                sid = sid.Substring(0, index);
                index = sid.LastIndexOf(_ColladaElement.Settings.kTargetAddressSeperator);
            }

            if (sid.Length > 0)
            {
                arSidStack.Push(sid);
            }
        }

        private static _ColladaElement _GetRootFromId(LinkedListNode<ResolutionEntry> e, _ColladaElement aRoot)
        {
            if (e.Value.Element.StartsWith(_ColladaElement.Settings.kTargetContainingElement))
            {
                return aRoot;
            }
            else
            {
                string sid = e.Value.Element;
                int index = sid.IndexOf(_ColladaElement.Settings.kTargetAddressSeperator);
                string id = _ColladaElement.Settings.kFragmentDelimiter + sid.Substring(0, index);
                id = _GetAbsoluteId(id);

                if (msElements.ContainsKey(id))
                {
                    return msElements[id];
                }
                else
                {
                    return null;
                }
            }
        }

        private static void _ResolveSidElement(ref LinkedListNode<ResolutionEntry> e, _ColladaElement aRoot)
        {
            _ColladaElement root = _GetRootFromId(e, aRoot);

            if (root != null)
            {
                string sid;
                _CleanSid(e.Value.Element, out sid);
                Stack<string> sidStack;
                _ParseSid(sid, out sidStack);

                if (!_ResolveSid(sidStack, ref e, root))
                {
                    e = e.Next;
                }
            }
            else
            {
                e = e.Next;
            }
        }
        #endregion

        private static ColladaCOLLADA _Load(string aFile, out string arBaseUri)
        {
            ColladaCOLLADA ret = null;

            string fullPath = Path.GetFullPath(aFile);
            if (!File.Exists(fullPath))
            {
                throw new Exception("The file \"" + aFile + "\" was resolved to \"" + fullPath + "\" and this could not be found.");
            }

            StreamReader stream = new System.IO.StreamReader(fullPath);
            XmlTextReader textReader = new XmlTextReader(fullPath, stream);
            XmlReader reader = XmlReader.Create(textReader, _GetReaderSettings());

            string oldBase = msCurrentBase;
            msCurrentBase = fullPath;
            {
                while (reader.Name != _ColladaElement.Elements.kCollada.Name && reader.Read()) ;

                if (reader.Name == _ColladaElement.Elements.kCollada.Name)
                {
                    try
                    {
                        ret = new ColladaCOLLADA(reader, msCurrentBase);
                    }
                    catch (Exception e)
                    {
                        string msg = e.Message;

                        if (textReader.HasLineInfo())
                        {
                            msg += "\nXML context [element: \"" + textReader.Name + "\", value: \"" + textReader.Value + "\", line number: " + Convert.ToString(textReader.LineNumber) + ", position: " + Convert.ToString(textReader.LinePosition) + "]";
                        }

                        throw new Exception(msg);
                    }
                }
            }
            arBaseUri = fullPath;
            msCurrentBase = oldBase;

            reader.Close();

            return ret;
        }

        private static int _LogRemainingToResolve()
        {
            string prefix = "COLLADA " + Enum.GetName(typeof(MessageType), MessageType.Warning) + ": ";
            int count = 0;

            string innerprefix = "An element with id = \"";
            string innerpostfix = "\" was referenced in the document but could not be found.";

            for (LinkedListNode<ResolutionEntry> e = msToResolveIds.First; e != null; e = e.Next)
            {
                string message = prefix + innerprefix + e.Value.Element + innerpostfix;
                msLoggedMessages.Add(message);
                count++;
            }

            innerprefix = "An element with sid = \"";

            for (LinkedListNode<ResolutionEntry> e = msToResolveSids.First; e != null; e = e.Next)
            {
                string message = prefix + innerprefix + e.Value.Element + innerpostfix;
                msLoggedMessages.Add(message);
                count++;
            }

            return count;
        }

        private static bool _Resolve(_ColladaElement aRoot, string aBaseUri)
        {
            string oldBase = msCurrentBase;
            msCurrentBase = aBaseUri;
            {
                for (LinkedListNode<ResolutionEntry> e = msToResolveIds.First; e != null; )
                {
                    _ResolveIdElement(ref e);
                }

                for (LinkedListNode<ResolutionEntry> e = msToResolveSids.First; e != null; )
                {
                    _ResolveSidElement(ref e, aRoot);
                }
            }
            msCurrentBase = oldBase;

            return (msToResolveIds.Count == 0 && msToResolveSids.Count == 0);
        }
        #endregion

        public enum MessageType
        {
            Normal,
            Warning,
            Error
        };

        public delegate void ResolutionAction(_ColladaElement aNode);
        public static void AddElementWithId(string aId, _ColladaElement aElement)
        {
            string id = _GetAbsoluteId(aId);

            if (msElements.ContainsKey(id))
            {
                throw new Exception("There are two COLLADA elements with the id attribute \"" +
                    aId + "\". This is not allowed, each id attribute must be unique within a " +
                    "single COLLADA document.");
            }
            else
            {
                msElements.Add(id, aElement);
            }
        }

        public static void Clear()
        {
            msCurrentBase = string.Empty;
            msElements.Clear();
            msLoggedMessages.Clear();
            msToLoad.Clear();
            msToResolveIds.Clear();
            msToResolveSids.Clear();
        }

        public static string CurrentBase { get { return msCurrentBase; } }

        public static int Load(string aFile, out ColladaCOLLADA arOut)
        {
            Clear();

            ColladaCOLLADA ret = null;
            SortedList<string, ColladaCOLLADA> toResolve = new SortedList<string, ColladaCOLLADA>();

            string baseUri = string.Empty;
            ColladaCOLLADA element = null;

            ret = _Load(aFile, out baseUri);

            toResolve.Add(baseUri, ret);

            #region To load
            while (msToLoad.Count > 0)
            {
                string s = msToLoad[msToLoad.Count - 1];
                msToLoad.RemoveAt(msToLoad.Count - 1);
                element = _Load(s, out baseUri);
                toResolve.Add(baseUri, element);
            }

            foreach (KeyValuePair<string, ColladaCOLLADA> e in toResolve)
            {
                _Resolve(e.Value, e.Key);
            }
            #endregion

            int leftToResolve = _LogRemainingToResolve();

            arOut = ret;

            return leftToResolve;
        }

        public static void LogMessage(string aMessage, MessageType aType)
        {
            string prefix = "COLLADA " + Enum.GetName(typeof(MessageType), aType) + ": ";
            string message = prefix + aMessage;

            msLoggedMessages.Add(message);
        }

        public static List<string> LoggedMessages { get { return msLoggedMessages; } }
        
        public static void QueueIdForResolution(string aId, ResolutionAction aAction)
        {
            _ColladaElement element;
            string id = _GetAbsoluteId(aId);
            string path = id.Substring(0, id.IndexOf(_ColladaElement.Settings.kFragmentDelimiter));

            if (path == msCurrentBase)
            {
                if (msElements.TryGetValue(id, out element))
                {
                    aAction(element);
                    return;
                }
            }
            else if (!msToLoad.Contains(path))
            {
                if (File.Exists(path))
                {
                    msToLoad.Add(path);
                }
                else
                {
                    throw new Exception("Dependency .dae file \"" + path + "\" for .dae file \"" + msCurrentBase + "\" does not exist.");
                }
            }

            ResolutionEntry entry;
            entry.Element = id;
            entry.Action = aAction;

            msToResolveIds.AddLast(entry);
        }

        public static void QueueSidForResolution(string aSid, ResolutionAction aAction)
        {
            string sid = "";

            if (aSid.IndexOf(_ColladaElement.Settings.kTargetAddressSeperator) < 0)
            {
                sid += _ColladaElement.Settings.kTargetAddressContainingElement;
                sid += _ColladaElement.Settings.kTargetAddressSeperator;
                sid += aSid;
            }
            else
            {
                sid = aSid;
            }

            ResolutionEntry entry;
            entry.Action = aAction;
            entry.Element = sid;

            msToResolveSids.AddLast(entry);
        }
    }
}
