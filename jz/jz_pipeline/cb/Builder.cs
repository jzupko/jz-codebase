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
        ProcessColladaFiles = (1 << 0)
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
        private static bool msbProcessing = false;
        #endregion

        public delegate void MessageHandler(string e);
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
