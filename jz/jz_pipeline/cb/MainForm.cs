using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using jz;
using jz.pipeline;
using jz.pipeline.collada;

namespace jz.cb
{
    public partial class MainForm : Form
    {
        public const int kMaxLines = 80;

        #region Private members
        private static int kMaxInstances = 80;

        private static readonly int[] mkUnitBoxIndices = new int[] { 0, 1, 2, 2, 1, 3, 1, 4, 3, 3, 4, 6, 4, 5, 6, 6, 5, 7, 5, 0, 7, 7, 0, 2, 2, 3, 7, 7, 3, 6, 5, 4, 0, 0, 4, 1 };
        private static readonly float[] mkUnitBoxVertices = new float[] { -1, -1, 1, 1, -1, 1, -1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, 1, 1, -1, -1, 1, -1 };
        private static readonly int[] mkUnitFrustumIndices = new int[] { 0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1, 1, 4, 2, 2, 4, 3 };
        private static readonly float[] mkUnitFrustumVertices = new float[] { 0, 0, 0, -1, -1, -1, -1, 1, -1, 1, 1, -1, 1, -1, -1 };
        private static readonly int[] mkUnitQuadIndices = new int[] { 0, 1, 2, 1, 3, 2 };
        private static readonly float[] mkUnitQuadVertices = new float[] { -1, -1, 0,  -1,  1, 0,  1, -1, 0,  1,  1, 0 };
        private static readonly int[] mkUnitSphereIndices = new int[] { 0, 2, 1, 3, 5, 4, 0, 6, 2, 3, 4, 7, 0, 8, 6, 3, 7, 9, 0, 10, 8, 3, 9, 11, 0, 12, 10, 3, 11, 13, 0, 14, 12, 3, 13, 15, 0, 16, 14, 3, 15, 17, 0, 18, 16, 3, 17, 19, 0, 20, 18, 3, 19, 21, 0, 22, 20, 3, 21, 23, 0, 24, 22, 3, 23, 25, 0, 1, 24, 3, 25, 5, 1, 27, 26, 1, 2, 27, 2, 6, 27, 27, 6, 28, 6, 29, 28, 6, 8, 29, 8, 10, 29, 29, 10, 30, 10, 31, 30, 10, 12, 31, 12, 14, 31, 31, 14, 32, 14, 33, 32, 14, 16, 33, 16, 18, 33, 33, 18, 34, 18, 35, 34, 18, 20, 35, 20, 22, 35, 35, 22, 36, 22, 37, 36, 22, 24, 37, 24, 1, 37, 37, 1, 26, 26, 27, 38, 38, 27, 39, 27, 40, 39, 27, 28, 40, 28, 29, 40, 40, 29, 41, 29, 42, 41, 29, 30, 42, 30, 31, 42, 42, 31, 43, 31, 44, 43, 31, 32, 44, 32, 33, 44, 44, 33, 45, 33, 46, 45, 33, 34, 46, 34, 35, 46, 46, 35, 47, 35, 48, 47, 35, 36, 48, 36, 37, 48, 48, 37, 49, 37, 38, 49, 37, 26, 38, 38, 51, 50, 38, 39, 51, 39, 40, 51, 51, 40, 52, 40, 53, 52, 40, 41, 53, 41, 42, 53, 53, 42, 54, 42, 55, 54, 42, 43, 55, 43, 44, 55, 55, 44, 56, 44, 57, 56, 44, 45, 57, 45, 46, 57, 57, 46, 58, 46, 59, 58, 46, 47, 59, 47, 48, 59, 59, 48, 60, 48, 61, 60, 48, 49, 61, 49, 38, 61, 61, 38, 50, 50, 51, 5, 5, 51, 4, 51, 7, 4, 51, 52, 7, 52, 53, 7, 7, 53, 9, 53, 11, 9, 53, 54, 11, 54, 55, 11, 11, 55, 13, 55, 15, 13, 55, 56, 15, 56, 57, 15, 15, 57, 17, 57, 19, 17, 57, 58, 19, 58, 59, 19, 19, 59, 21, 59, 23, 21, 59, 60, 23, 60, 61, 23, 23, 61, 25, 61, 5, 25, 61, 50, 5 };
        private static readonly float[] mkUnitSphereVertices = new float[] { 8.71482E-10f, -0.998307f, -2.4683E-08f, 0.500784f, -0.86456f, -2.4683E-08f, 0.433692f, -0.86456f, 0.250679f, 8.71482E-10f, 0.998308f, -2.4683E-08f, 0.433692f, 0.864559f, 0.250679f, 0.500784f, 0.864559f, -2.4683E-08f, 0.250392f, -0.86456f, 0.434188f, 0.250392f, 0.864559f, 0.434188f, 8.71482E-10f, -0.86456f, 0.501357f, 8.71482E-10f, 0.864559f, 0.501357f, -0.250392f, -0.86456f, 0.434188f, -0.250392f, 0.864559f, 0.434188f, -0.433692f, -0.86456f, 0.250679f, -0.433692f, 0.864559f, 0.250679f, -0.500784f, -0.86456f, -2.4683E-08f, -0.500784f, 0.864559f, -2.4683E-08f, -0.433692f, -0.86456f, -0.250679f, -0.433692f, 0.864559f, -0.250679f, -0.250392f, -0.86456f, -0.434188f, -0.250392f, 0.864559f, -0.434188f, 8.71481E-10f, -0.86456f, -0.501357f, 8.71481E-10f, 0.864559f, -0.501357f, 0.250392f, -0.86456f, -0.434188f, 0.250392f, 0.864559f, -0.434188f, 0.433692f, -0.86456f, -0.250679f, 0.433692f, 0.864559f, -0.250679f, 0.867384f, -0.499154f, -2.4683E-08f, 0.751177f, -0.499154f, 0.434188f, 0.433692f, -0.499154f, 0.752036f, 8.71482E-10f, -0.499154f, 0.868377f, -0.433692f, -0.499154f, 0.752036f, -0.751177f, -0.499154f, 0.434188f, -0.867384f, -0.499154f, -2.4683E-08f, -0.751177f, -0.499154f, -0.434188f, -0.433692f, -0.499154f, -0.752036f, 8.71481E-10f, -0.499154f, -0.868377f, 0.433692f, -0.499154f, -0.752036f, 0.751177f, -0.499154f, -0.434188f, 1.00157f, -1.93919E-07f, -2.4683E-08f, 0.867384f, -1.93919E-07f, 0.501357f, 0.500784f, -1.93919E-07f, 0.868376f, 8.71482E-10f, -1.93919E-07f, 1.00271f, -0.500784f, -1.93919E-07f, 0.868376f, -0.867384f, -1.93919E-07f, 0.501357f, -1.00157f, -1.93919E-07f, -2.4683E-08f, -0.867384f, -1.93919E-07f, -0.501357f, -0.500784f, -1.93919E-07f, -0.868377f, 8.71481E-10f, -1.93919E-07f, -1.00271f, 0.500784f, -1.93919E-07f, -0.868377f, 0.867384f, -1.93919E-07f, -0.501357f, 0.867384f, 0.499154f, -2.4683E-08f, 0.751176f, 0.499154f, 0.434188f, 0.433692f, 0.499154f, 0.752036f, 8.71482E-10f, 0.499154f, 0.868376f, -0.433692f, 0.499154f, 0.752036f, -0.751176f, 0.499154f, 0.434188f, -0.867384f, 0.499154f, -2.4683E-08f, -0.751176f, 0.499154f, -0.434188f, -0.433692f, 0.499154f, -0.752036f, 8.71481E-10f, 0.499154f, -0.868377f, 0.433692f, 0.499154f, -0.752036f, 0.751176f, 0.499154f, -0.434188f };
        private static readonly VertexElement[] mkDeclaration = new VertexElement[]
        {
            new VertexElement(0, 0, VertexElementFormat.Vector3, VertexElementMethod.Default, VertexElementUsage.Position, 0)
        };
        private static readonly VertexElement[] mkInstancingDeclaration = new VertexElement[]
        {
            new VertexElement(0, 0, VertexElementFormat.Vector3, VertexElementMethod.Default, VertexElementUsage.Position, 0),
            new VertexElement(0, (3 * sizeof(float)), VertexElementFormat.Vector3, VertexElementMethod.Default, VertexElementUsage.TextureCoordinate, 0)
        };

        private static readonly UInt16[] mkInstancingIndices;
        private static readonly float[] mkInstancingVertices;

        static MainForm()
        {
            mkInstancingIndices = new UInt16[6 * kMaxInstances];
            mkInstancingVertices = new float[24 * kMaxInstances];

            for (int i = 0; i < kMaxInstances; i++)
            {
                float instanceIndex = (float)(3 * i);
                int indexOffset = (4 * i);
                int indicesIndex = (6 * i);
                int verticesIndex = (24 * i);

                mkInstancingIndices[indicesIndex + 0] = (UInt16)(0 + indexOffset);
                mkInstancingIndices[indicesIndex + 1] = (UInt16)(1 + indexOffset);
                mkInstancingIndices[indicesIndex + 2] = (UInt16)(2 + indexOffset);
                mkInstancingIndices[indicesIndex + 3] = (UInt16)(1 + indexOffset);
                mkInstancingIndices[indicesIndex + 4] = (UInt16)(3 + indexOffset);
                mkInstancingIndices[indicesIndex + 5] = (UInt16)(2 + indexOffset);

                mkInstancingVertices[verticesIndex + 0]  = -1;
                mkInstancingVertices[verticesIndex + 1]  = -1;
                mkInstancingVertices[verticesIndex + 2]  =  0;
                mkInstancingVertices[verticesIndex + 3]  =  0;
                mkInstancingVertices[verticesIndex + 4]  =  1;
                mkInstancingVertices[verticesIndex + 5] = instanceIndex;

                mkInstancingVertices[verticesIndex + 6]  = -1;
                mkInstancingVertices[verticesIndex + 7]  =  1;
                mkInstancingVertices[verticesIndex + 8]  =  0;
                mkInstancingVertices[verticesIndex + 9]  =  0;
                mkInstancingVertices[verticesIndex + 10] =  0;
                mkInstancingVertices[verticesIndex + 11] = instanceIndex;

                mkInstancingVertices[verticesIndex + 12] =  1;
                mkInstancingVertices[verticesIndex + 13] = -1;
                mkInstancingVertices[verticesIndex + 14] =  0;
                mkInstancingVertices[verticesIndex + 15] =  1;
                mkInstancingVertices[verticesIndex + 16] =  1;
                mkInstancingVertices[verticesIndex + 17] = instanceIndex;

                mkInstancingVertices[verticesIndex + 18] =  1;
                mkInstancingVertices[verticesIndex + 19] =  1;
                mkInstancingVertices[verticesIndex + 20] =  0;
                mkInstancingVertices[verticesIndex + 21] =  1;
                mkInstancingVertices[verticesIndex + 22] =  0;
                mkInstancingVertices[verticesIndex + 23] = instanceIndex;
            }
        }

        private string mInDirectory = string.Empty;
        private string mOutDirectory = string.Empty;
        private volatile Thread mWorker = null;
        private BuilderFlags mFlags = (BuilderFlags.ProcessColladaFiles | BuilderFlags.ProcessHeightMaps | BuilderFlags.ProcessSprites);

        private void _Handler(string e)
        {
            if (mWorker != null)
            {
                if (Output.InvokeRequired) { Invoke(new Action<string>(_Handler), new object[] { e }); }
                else
                {
                    string[] lines = Output.Lines;
                    int newLength = (lines.Length >= kMaxLines) ? kMaxLines : lines.Length + 1;

                    string[] n = new string[newLength];
                    Array.Copy(lines, 0, n, 1, (newLength - 1));
                    n[0] = e;

                    Output.Lines = n;
                }
            }
        }

        private void _HandleClose(object aSender, CancelEventArgs e)
        {
            if (mWorker != null)
            {
                e.Cancel = true;
                MessageBox.Show("Cannot exit during a build.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            }
        }

        void _HandleKeyUp(object aSender, KeyEventArgs e)
        {
            if (e != null && e.KeyCode == Keys.F5)
            {
                e.Handled = true;
                RunMenuItem_Click(this, new EventArgs());
            }
        }

        private void _Worker(object obj)
        {
            try
            {
                _Handler("Building...");
                Builder.Process(mInDirectory, mOutDirectory, _Handler, mFlags);
            }
            catch (Exception e) { _Handler(e.Message); }

            _Handler("Done.");
            mWorker = null;
        }
        #endregion

        public MainForm() : this(Directory.GetCurrentDirectory()) {}
        public MainForm(string aInDirectory)
        {
            mInDirectory = Path.GetFullPath(aInDirectory);
            mOutDirectory = Path.Combine(mInDirectory, "compiled");

            InitializeComponent();
            InFolder.Text = mInDirectory;
            OutFolder.Text = mOutDirectory;
            Closing += _HandleClose;
            
            KeyPreview = true;
            KeyUp += _HandleKeyUp;
        }

        private void FolderMenuItem_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog fd = new FolderBrowserDialog();
            fd.Description = "Input directory";
            fd.SelectedPath = mInDirectory;

            if (fd.ShowDialog() == DialogResult.OK)
            {
                mInDirectory = fd.SelectedPath;
                mInDirectory = Path.GetFullPath(mInDirectory);
                InFolder.Text = mInDirectory;
            }
        }


        private void OutFolderMenuItem_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog fd = new FolderBrowserDialog();
            fd.Description = "Output directory";
            fd.SelectedPath = mOutDirectory;

            if (fd.ShowDialog() == DialogResult.OK)
            {
                mOutDirectory = fd.SelectedPath;
                mOutDirectory = Path.GetFullPath(mOutDirectory);
                OutFolder.Text = mOutDirectory;
            }
        }

        private void RunMenuItem_Click(object sender, EventArgs e)
        {
            if (mWorker == null)
            {
                mWorker = new Thread(_Worker);
                menuStrip1_Click(this, new EventArgs());
                mWorker.Start();
            }
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void menuStrip1_Click(object send, EventArgs e)
        {
            exitToolStripMenuItem.Enabled = (mWorker == null);
            RunMenuItem.Enabled = (mWorker == null);
        }

        private const string kFilename = "built-in";

        private void _Helper(int[] indices, float[] vertices, string id)
        {
            DocInfo info;
            info.BaseFilename = kFilename;
            info.Filename = Path.Combine(mInDirectory, kFilename);
            info.InAbsDirectory = mInDirectory;
            info.OutRelDirectory = "";
            info.OutAbsDirectory = mOutDirectory;

            JzMeshContent.Part part = new JzMeshContent.Part();
            part.Id = id;
            part.Indices = Array.ConvertAll<int, UInt16>(indices, delegate(int e) { return (UInt16)e; });
            part.PrimitiveType = PrimitiveType.TriangleList;
            part.VertexDeclaration = mkDeclaration;
            part.Vertices = Utilities.Convert(vertices);

            Helpers.Write(null, info, part);
        }

        private void unitBoxToolStripMenuItem_Click(object sender, EventArgs e)
        {
            _Helper(mkUnitBoxIndices, mkUnitBoxVertices, "unit-box");
        }

        private void unitFrustumToolStripMenuItem_Click(object sender, EventArgs e)
        {
            _Helper(mkUnitFrustumIndices, mkUnitFrustumVertices, "unit-frustum");
        }

        private void unitQuadToolStripMenuItem_Click(object sender, EventArgs e)
        {
            _Helper(mkUnitQuadIndices, mkUnitQuadVertices, "unit-quad");
        }

        private void unitSphereToolStripMenuItem_Click(object sender, EventArgs e)
        {
            _Helper(mkUnitSphereIndices, mkUnitSphereVertices, "unit-sphere");
        }

        private void instancingQuadToolStripMenuItem_Click(object sender, EventArgs e)
        {
            DocInfo info;
            info.BaseFilename = kFilename;
            info.Filename = Path.Combine(mInDirectory, kFilename);
            info.InAbsDirectory = mInDirectory;
            info.OutRelDirectory = "";
            info.OutAbsDirectory = mOutDirectory;

            JzMeshContent.Part part = new JzMeshContent.Part();
            part.Id = "instancing-quad";
            part.Indices = mkInstancingIndices;
            part.PrimitiveType = PrimitiveType.TriangleList;
            part.VertexDeclaration = mkInstancingDeclaration;
            part.Vertices = Utilities.Convert(mkInstancingVertices);

            Helpers.Write(null, info, part);
        }

        private void processColladaToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ToolStripMenuItem item = (ToolStripMenuItem)sender;

            if (item.Checked) { mFlags |= BuilderFlags.ProcessColladaFiles; }
            else { mFlags &= ~BuilderFlags.ProcessColladaFiles; }
        }

        private void processHeightmapsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ToolStripMenuItem item = (ToolStripMenuItem)sender;

            if (item.Checked) { mFlags |= BuilderFlags.ProcessHeightMaps; }
            else { mFlags &= ~BuilderFlags.ProcessHeightMaps; }
        }

        private void processSpritesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ToolStripMenuItem item = (ToolStripMenuItem)sender;

            if (item.Checked) { mFlags |= BuilderFlags.ProcessSprites; }
            else { mFlags &= ~BuilderFlags.ProcessSprites; }
        }
    }
}