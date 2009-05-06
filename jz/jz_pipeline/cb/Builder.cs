using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Content;
using Microsoft.Xna.Framework.Content.Pipeline;
using Microsoft.Xna.Framework.Content.Pipeline.Graphics;
using Microsoft.Xna.Framework.Content.Pipeline.Processors;
using Microsoft.Xna.Framework.Content.Pipeline.Serialization.Compiler;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using jz.pipeline;
using jz.pipeline.collada;
using jz.pipeline.collada.elements;

namespace jz.cb
{
    public struct DocInfo
    {
        public string BaseFilename;
        public string Filename;
        public string InAbsDirectory;
        public string OutRelDirectory;
        public string OutAbsDirectory;
    }

    [Flags]
    public enum BuilderFlags
    {
        ProcessColladaFiles = (1 << 0),
        ProcessHeightMaps = (1 << 1),
        ProcessSprites = (1 << 2)
    }

    /// <summary>
    /// Builder that wraps MS build environment.
    /// </summary>
    /// <remarks>
    /// Based on WinFormsContentLoading project: http://www.ziggyware.com/readarticle.php?article_id=183
    /// </remarks>
    public static class Builder
    {
        public const string kBuildConfigExtension = ".build_config";
        public const string kOutDir = "compiled\\";
        public const string kColladaExtension = ".dae";
        public const string kColladaOutExtension = ".scn";
        public const string kHeightMapExtension = "_heightmap.*";
        public const string kSpriteExtension = "_sprite.*";
        public const string kStripHeightMap = "_heightmap";
        public const string kStripSprite = "_sprite";
        public const string kHeightMapOut = "_NRM.dds";
        public const string kSpriteOut = ".dds";
        public const string kEdgeOut = "_EDGE.dat";
        public const string kEdgeImageOut = "_EDGE.png";

        #region Private members
        private const string kProcessPhysicsKey = "ProcessPhysics";
        private static void _ProcessCollada(string aInDirectory, string aOutDirectory, MessageHandler aHandler)
        {
            if (Directory.Exists(aInDirectory))
            {
                string[] files = Directory.GetFiles(aInDirectory, "*" + kColladaExtension, SearchOption.AllDirectories);

                foreach (string e in files)
                {
                    string relativeFilename = Utilities.ExtractRelativeFilename(aInDirectory, e);
                    int pos = relativeFilename.IndexOf(Path.DirectorySeparatorChar, 0);
                    while (pos >= 0)
                    {
                        string dir = Path.Combine(aOutDirectory, relativeFilename.Substring(0, pos));
                        if (!Directory.Exists(dir))
                        {
                            Directory.CreateDirectory(dir);
                        }
                        pos = relativeFilename.IndexOf(Path.DirectorySeparatorChar, (pos + 1));
                    }

                    string outFile = Path.Combine(aOutDirectory, Utilities.ExtractBaseFilename(relativeFilename) + kColladaOutExtension);

                    if (!File.Exists(outFile) || File.GetLastWriteTime(e).Ticks > File.GetLastWriteTime(outFile).Ticks)
                    {
                        aHandler("Building: \"" + outFile + "\" from \"" + e + "\".");

                        #region Parse input
                        ColladaCOLLADA root;
                        if (ColladaDocument.Load(e, out root) > 0)
                        {
                            foreach (string f in ColladaDocument.LoggedMessages)
                            {
                                aHandler(f);
                            }
                        }
                        #endregion

                        ColladaProcessor processor = new ColladaProcessor();
                        processor.bProcessPhysics = true;

                        #region Load config file
                        string configFile = Utilities.RemoveExtension(e) + kBuildConfigExtension;
                        if (File.Exists(configFile))
                        {
                            try
                            {
                                ConfigFile cfg = new ConfigFile(configFile);
                                if (cfg.Exists(kProcessPhysicsKey))
                                {
                                    processor.bProcessPhysics = cfg.GetBoolean(kProcessPhysicsKey);
                                }
                            }
                            catch
                            {
                                aHandler("COLLADA config file for \"" + e + "\" exists but loading failed.");
                            }
                        }
                        #endregion

                        #region Process
                        SceneContent content = processor.Process(root, null);
                        #endregion

                        #region Write output
                        {
                            BinaryWriter writer = new BinaryWriter(new FileStream(outFile, FileMode.Create));
                            DocInfo info = new DocInfo();
                            info.BaseFilename = Utilities.ExtractBaseFilename(Utilities.ExtractRelativeFilename(aInDirectory, e));
                            info.Filename = relativeFilename;
                            info.InAbsDirectory = aInDirectory;
                            info.OutRelDirectory = Path.GetDirectoryName(Utilities.ExtractRelativeFilename(aOutDirectory, outFile));
                            info.OutAbsDirectory = Path.Combine(aOutDirectory, info.OutRelDirectory);
                            SceneWriter.Write(writer, info, content);
                            writer.Close();
                            File.SetLastWriteTime(outFile, File.GetLastWriteTime(e));
                        }
                        #endregion

                        aHandler("Done building \"" + outFile + "\".");
                    }
                }
            }
        }

        private static void _ProcessData(int width, int height, Color[] inData, Color[] outData)
        {
            const float kFactor = (float)(1.0 / 255.0);
            const float kByteFactor = 255.0f;
            
            #region Clear edges
            {
                int length = outData.Length;
                for (int i = 0; i < length; i++) { outData[i] = (new Color(127, 127, 255, (byte)(255 - inData[i].R))); }
            }
            #endregion

            #region Center of image
            {
                #region Normal
                {
                    const float kNormalAvgFactor = (float)(1.0 / 8.0);

                    for (int y = 1; y < (height - 1); y++)
                    {
                        for (int x = 1; x < (width - 1); x++)
                        {
                            int i0 = (y * width) + x;
                            int i1 = (y * width) + (x + 1);
                            int i2 = ((y + 1) * width) + (x + 1);
                            int i3 = ((y + 1) * width) + x;
                            int i4 = ((y + 1) * width) + (x - 1);
                            int i5 = (y * width) + (x - 1);
                            int i6 = ((y - 1) * width) + (x - 1);
                            int i7 = ((y - 1) * width) + x;
                            int i8 = ((y - 1) * width) + (x + 1);

                            Vector3 v0 = new Vector3(0, 0, ((float)inData[i0].R) * kFactor);
                            Vector3 v1 = new Vector3(1, 0, ((float)inData[i1].R) * kFactor);
                            Vector3 v2 = new Vector3(1, -1, ((float)inData[i2].R) * kFactor);
                            Vector3 v3 = new Vector3(0, -1, ((float)inData[i3].R) * kFactor);
                            Vector3 v4 = new Vector3(-1, -1, ((float)inData[i4].R) * kFactor);
                            Vector3 v5 = new Vector3(-1, 0, ((float)inData[i5].R) * kFactor);
                            Vector3 v6 = new Vector3(-1, 1, ((float)inData[i6].R) * kFactor);
                            Vector3 v7 = new Vector3(0, 1, ((float)inData[i7].R) * kFactor);
                            Vector3 v8 = new Vector3(1, 1, ((float)inData[i8].R) * kFactor);

                            Vector3 n0 = Vector3.Cross(v2 - v0, v1 - v0);
                            Vector3 n1 = Vector3.Cross(v3 - v0, v2 - v0);
                            Vector3 n2 = Vector3.Cross(v4 - v0, v3 - v0);
                            Vector3 n3 = Vector3.Cross(v5 - v0, v4 - v0);
                            Vector3 n4 = Vector3.Cross(v6 - v0, v5 - v0);
                            Vector3 n5 = Vector3.Cross(v7 - v0, v6 - v0);
                            Vector3 n6 = Vector3.Cross(v8 - v0, v7 - v0);
                            Vector3 n7 = Vector3.Cross(v1 - v0, v8 - v0);

                            Vector3 n = (n0 + n1 + n2 + n3 + n4 + n5 + n6 + n7) * kNormalAvgFactor;
                            n = Vector3.Normalize(n);

                            Color c = default(Color);
                            c.R = (byte)((n.X * 0.5f + 0.5f) * kByteFactor);
                            c.G = (byte)((n.Y * 0.5f + 0.5f) * kByteFactor);
                            c.B = (byte)((n.Z * 0.5f + 0.5f) * kByteFactor);
                            c.A = (byte)(255 - inData[i0].R);

                            outData[i0] = c;
                        }
                    }
                }
                #endregion
            }
            #endregion
        }

        private static void _ProcessHeightMaps(string aInDirectory, string aOutDirectory, MessageHandler aHandler)
        {
            if (Directory.Exists(aInDirectory))
            {
                TextureCreationParameters textureLoadParams;
                textureLoadParams = TextureCreationParameters.Default;
                textureLoadParams.Format = SurfaceFormat.Color;

                string[] files = Directory.GetFiles(aInDirectory, "*" + kHeightMapExtension, SearchOption.AllDirectories);

                foreach (string e in files)
                {
                    string relativeFilename = Utilities.ExtractRelativeFilename(aInDirectory, e);
                    int pos = relativeFilename.IndexOf(Path.DirectorySeparatorChar, 0);
                    while (pos >= 0)
                    {
                        string dir = Path.Combine(aOutDirectory, relativeFilename.Substring(0, pos));
                        if (!Directory.Exists(dir))
                        {
                            Directory.CreateDirectory(dir);
                        }
                        pos = relativeFilename.IndexOf(Path.DirectorySeparatorChar, (pos + 1));
                    }

                    string baseFilename = Utilities.ExtractBaseFilename(relativeFilename);
                    baseFilename = baseFilename.Substring(0, baseFilename.IndexOf(kStripHeightMap));

                    string outFile = Path.Combine(aOutDirectory, (baseFilename + kHeightMapOut));

                    if (!File.Exists(outFile) || File.GetLastWriteTime(e).Ticks > File.GetLastWriteTime(outFile).Ticks)
                    {
                        aHandler("Processing heightmap \"" + e + "\".");
                        Texture input = Texture.FromFile(Helpers.Game.Manager.GraphicsDevice, e, textureLoadParams);

                        if (input is Texture2D)
                        {
                            Texture2D texture = (Texture2D)input;
                            int width = (texture.Width);
                            int height = (texture.Height);

                            Color[] inData = new Color[width * height];
                            Color[] outData = new Color[width * height];
                            texture.GetData<Color>(inData);

                            _ProcessData(width, height, inData, outData);

                            Texture2D outTexture = new Texture2D(Helpers.Game.Manager.GraphicsDevice, width, height, 1, TextureUsage.None, SurfaceFormat.Color);
                            outTexture.SetData<Color>(outData);

                            // Temp:
                            string tempFile = Path.GetTempFileName();
                            outTexture.Save(tempFile, ImageFileFormat.Dds);
                            outTexture = (Texture2D)Texture.FromFile(Helpers.Game.Manager.GraphicsDevice, tempFile, Helpers.kTextureDxt5Parameters);
                            outTexture.Save(outFile, ImageFileFormat.Dds);
                            File.SetLastWriteTime(outFile, File.GetLastWriteTime(e));
                        }
                        aHandler("Done processing \"" + outFile + "\".");
                    }
                }
            }
        }

        const byte MinValueForOpaque = 255;

        private struct Edge
        {
            public int Next;
            public int Prev;
            public int ChainId;
        }

        private static void _EdgeHelper(int i0, int j0, int j1, Color[] data, Edge[] edge)
        {
            if (edge[j0].Next == 0 && edge[j1].Prev == 0)
            {
                if (data[i0].A < MinValueForOpaque)
                {
                    if (data[j0].A >= MinValueForOpaque && data[j1].A >= MinValueForOpaque)
                    {
                        edge[j0].Next = j1;
                        edge[j1].Prev = j0;
                    }
                }
            }
        }

        private static void _EdgeHelper(int i0, int i1, int j0, int j1, Color[] data, Edge[] edge)
        {
            if (edge[j0].Next == 0 && edge[j1].Prev == 0)
            {
                if (data[i0].A < MinValueForOpaque && data[i1].A < MinValueForOpaque)
                {
                    if (data[j0].A >= MinValueForOpaque && data[j1].A >= MinValueForOpaque)
                    {
                        edge[j0].Next = j1;
                        edge[j1].Prev = j0;
                    }
                }
            }
        }

        const float kDistanceDenominator = 16.0f;
        private static void _GenerateEdges(Texture2D aIn, List<Vector2> arOut, Color[] arEdgeImage, MessageHandler aHandler)
        {
            int width = aIn.Width;
            int height = aIn.Height;
            Color[] data = new Color[width * height];
            aIn.GetData<Color>(data);
            Edge[] edge = new Edge[width * height];
            List<bool> chains = new List<bool>();

            float distanceTolerance = (float)Utilities.Min(width, height) / kDistanceDenominator;

            #region Initial edges
            for (int y = 1; y < (height - 1); y++)
            {
                for (int x = 1; x < (width - 1); x++)
                {
                    int i0 = (y * width) + (x);
                    int w0 = (y * width) + (x - 1);
                    int w1 = (y * width) + (x + 1);
                    int h0 = ((y - 1) * width) + (x);
                    int h1 = ((y + 1) * width) + (x);
                    int w0h0 = ((y - 1) * width) + (x - 1);
                    int w1h0 = ((y - 1) * width) + (x + 1);
                    int w0h1 = ((y + 1) * width) + (x - 1);
                    int w1h1 = ((y + 1) * width) + (x + 1);

                    // XOX
                    // OTO
                    // XOX
                    _EdgeHelper(i0, w0, h0, data, edge);
                    _EdgeHelper(i0, h0, w1, data, edge);
                    _EdgeHelper(i0, w1, h1, data, edge);
                    _EdgeHelper(i0, h1, w0, data, edge);

                    // OOO
                    // OTO
                    // OOO
                    _EdgeHelper(i0, w0, w0h0, data, edge);
                    _EdgeHelper(i0, w0h0, h0, data, edge);
                    _EdgeHelper(i0, h0, w1h0, data, edge);
                    _EdgeHelper(i0, w1h0, w1, data, edge);
                    _EdgeHelper(i0, w1, w1h1, data, edge);
                    _EdgeHelper(i0, w1h1, h1, data, edge);
                    _EdgeHelper(i0, h1, w0h1, data, edge);
                    _EdgeHelper(i0, w0h1, w0, data, edge);
                }
            }
            #endregion

            #region Edge image
            {
                for (int y = 0; y < height; y++)
                {
                    for (int x = 0; x < width; x++)
                    {
                        int i = (y * width) + x;
                        if (edge[i].Next > 0)
                        {
                            arEdgeImage[i] = Color.White;
                        }
                        else
                        {
                            arEdgeImage[i] = Color.Black;
                        }
                    }
                }
            }
            #endregion

            #region Simplify
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    int i = (y * width) + (x);

                    if (edge[i].Next > 0)
                    {
                        int j = (edge[i].Next);
                        int jx = (j % width);
                        int jy = (j / width);

                        Vector2 p = (new Vector2((float)x, (float)y));
                        Vector2 t = Vector2.Normalize(new Vector2((float)jx, (float)jy) - p);

                        // normal and d of plane.
                        Vector2 n = (new Vector2(-t.Y, t.X));
                        float d = Vector2.Dot(-n, p);

                        j = (edge[j].Next);
                        while (j > 0)
                        {
                            jx = (j % width);
                            jy = (j / width);
                            p = (new Vector2((float)jx, (float)jy));

                            // signed distance to plane
                            float sd = (d + Vector2.Dot(n, p));
                            if (sd >= 0.0f && sd < distanceTolerance)
                            {
                                edge[edge[i].Next].Prev = 0;
                                edge[edge[i].Next].Next = 0;

                                edge[i].Next = j;
                                edge[j].Prev = i;
                            }
                            else
                            {
                                break;
                            }

                            j = edge[j].Next;
                        }
                    }
                }
            }
            #endregion

            #region Find complete chains
            for (int y = 1; y < (height - 1); y++)
            {
                for (int x = 1; x < (width - 1); x++)
                {
                    int i = (y * width) + (x);

                    if (edge[i].Next > 0 && edge[i].ChainId == 0)
                    {
                        chains.Add(false);
                        int chainId = chains.Count;

                        while (edge[i].Next > 0 && edge[i].ChainId == 0)
                        {
                            edge[i].ChainId = chainId;
                            i = edge[i].Next;
                        }

                        Edge e = edge[i];
                        if (e.ChainId == chainId && e.Next > 0)
                        {
                            chains[(chainId - 1)] = true;
                        }
                    }
                }
            }
            #endregion

            #region Issue warnings about incomplete chains
            {
                int count = chains.Count;
                for (int i = 0; i < count; i++)
                {
                    if (!chains[i])
                    {
                        aHandler("Warning: edge chain " + i.ToString() + " is not closed, chain will not be saved.");
                    }
                }
            }
            #endregion

            #region Output complete chains
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    int i = (y * width) + (x);

                    if (edge[i].Next > 0 && chains[edge[i].ChainId - 1])
                    {
                        int next = (edge[i].Next);
                        int ix = (next % width);
                        int iy = (next / width);

                        Vector2 u = (new Vector2(((float)x / (float)(width - 1)) * 2.0f - 1.0f, -(((float)y / (float)(height - 1) * 2.0f - 1.0f))));
                        Vector2 v = (new Vector2(((float)ix / (float)(width - 1)) * 2.0f - 1.0f, -(((float)iy / (float)(height - 1) * 2.0f - 1.0f))));

                        arOut.Add(u);
                        arOut.Add(v);
                    }
                }
            }
            #endregion
        }

        private static void _ProcessSprites(string aInDirectory, string aOutDirectory, MessageHandler aHandler)
        {
            if (Directory.Exists(aInDirectory))
            {
                TextureCreationParameters textureLoadParams;
                textureLoadParams = TextureCreationParameters.Default;
                textureLoadParams.Format = SurfaceFormat.Color;

                string[] files = Directory.GetFiles(aInDirectory, "*" + kSpriteExtension, SearchOption.AllDirectories);

                foreach (string e in files)
                {
                    string relativeFilename = Utilities.ExtractRelativeFilename(aInDirectory, e);
                    int pos = relativeFilename.IndexOf(Path.DirectorySeparatorChar, 0);
                    while (pos >= 0)
                    {
                        string dir = Path.Combine(aOutDirectory, relativeFilename.Substring(0, pos));
                        if (!Directory.Exists(dir))
                        {
                            Directory.CreateDirectory(dir);
                        }
                        pos = relativeFilename.IndexOf(Path.DirectorySeparatorChar, (pos + 1));
                    }

                    string baseFilename = Utilities.ExtractBaseFilename(relativeFilename);
                    baseFilename = baseFilename.Substring(0, baseFilename.IndexOf(kStripSprite));

                    string outFile = Path.Combine(aOutDirectory, (baseFilename + kSpriteOut));
                    string outEdgeFile = Path.Combine(aOutDirectory, (baseFilename + kEdgeOut));
                    string outEdgeImage = Path.Combine(aOutDirectory, (baseFilename + kEdgeImageOut));

                    if (!File.Exists(outFile) || File.GetLastWriteTime(e).Ticks > File.GetLastWriteTime(outFile).Ticks)
                    {
                        aHandler("Processing sprite \"" + e + "\".");

                        Texture2D conv = (Texture2D)Texture.FromFile(Helpers.Game.Manager.GraphicsDevice, e, textureLoadParams);
                        #region Edges
                        {
                            Color[] edgeImage = (new Color[conv.Width * conv.Height]);
                            List<Vector2> edges = new List<Vector2>();
                            _GenerateEdges(conv, edges, edgeImage, aHandler);

                            if (edges.Count == 0)
                            {
                                edges.Add(new Vector2(-1, -1)); edges.Add(new Vector2(-1, 1));
                                edges.Add(new Vector2(-1, 1)); edges.Add(new Vector2(1, 1));
                                edges.Add(new Vector2(1, 1)); edges.Add(new Vector2(1, -1));
                                edges.Add(new Vector2(1, -1)); edges.Add(new Vector2(-1, -1));
                            }

                            aHandler("Generated " + (edges.Count / 2).ToString() + " edges, saved to \"" + outEdgeFile + "\".");

                            #region Save edge image
                            {
                                Texture2D t = (new Texture2D(Helpers.Game.GraphicsDevice, conv.Width, conv.Height));
                                t.SetData<Color>(edgeImage);
                                t.Save(outEdgeImage, ImageFileFormat.Png);
                            }
                            #endregion

                            #region Save edge data
                            {
                                BinaryWriter edgeWriter = new BinaryWriter(new FileStream(outEdgeFile, FileMode.Create));
                                Helpers.Write(edgeWriter, (UInt32)edges.Count);
                                for (int i = 0; i < edges.Count; i++)
                                {
                                    Helpers.Write(edgeWriter, edges[i]);
                                }
                                edgeWriter.Close();
                            }
                            #endregion
                        }
                        #endregion
                        string tempFile = Path.GetTempFileName();
                        conv.Save(tempFile, ImageFileFormat.Dds);
                        conv = (Texture2D)Texture.FromFile(Helpers.Game.Manager.GraphicsDevice, tempFile, Helpers.kTextureWriteParameters);
                        conv.Save(outFile, ImageFileFormat.Dds);
                        File.SetLastWriteTime(outFile, File.GetLastWriteTime(e));

                        aHandler("Done processing \"" + outFile + "\".");
                    }
                }
            }
        }
        #endregion

        public delegate void MessageHandler(string e);
        private static bool msbProcessing = false;
        public static bool bProcessing { get { return msbProcessing; } }
        public static void StopProcessing() { msbProcessing = false; }

        public static void Process(string aInDirectory, string aOutDirectory, MessageHandler aHandler, BuilderFlags aFlags)
        {
            if (!msbProcessing)
            {
                msbProcessing = true;
                int count = 0;
                while (msbProcessing)
                {
                    if (count == 0)
                    {
                        aHandler("Scanning...");
                        count++;
                    }
                    else if (count >= 59) { count = 0; }
                    else { count++; }

                    try
                    {
                        if (!Directory.Exists(aOutDirectory))
                        {
                            DirectoryInfo info = Directory.CreateDirectory(aOutDirectory);
                            if (!info.Exists) { throw new Exception("Could not create output directory \"" + aOutDirectory + "\"."); }
                        }

                        if ((aFlags & BuilderFlags.ProcessColladaFiles) != 0) { _ProcessCollada(aInDirectory, aOutDirectory, aHandler); }
                        if ((aFlags & BuilderFlags.ProcessSprites) != 0) { _ProcessSprites(aInDirectory, aOutDirectory, aHandler); }
                        if ((aFlags & BuilderFlags.ProcessHeightMaps) != 0) { _ProcessHeightMaps(aInDirectory, aOutDirectory, aHandler); }
                    }
                    catch
                    {
                        msbProcessing = false;
                        throw;
                    }
                    Thread.Sleep(1000);
                }
            }
        }

        public static void Process(string aInDirectory, string aOutDirectory, BuilderFlags aFlags)
        {
            Process(aInDirectory, aOutDirectory, null, aFlags);
        }
    }
}
