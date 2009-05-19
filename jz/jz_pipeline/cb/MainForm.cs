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

        private static readonly int[] mkUnitBoxIndices = new int[] { 0, 2, 1, 2, 3, 1, 1, 3, 4, 3, 6, 4, 4, 6, 5, 6, 7, 5, 5, 7, 0, 7, 2, 0, 2, 7, 3, 7, 6, 3, 5, 0, 4, 0, 1, 4 };
        private static readonly float[] mkUnitBoxVertices = new float[] { -1, -1, 1, 1, -1, 1, -1, 1, 1, 1, 1, 1, 1, -1, -1, -1, -1, -1, 1, 1, -1, -1, 1, -1 };
        private static readonly int[] mkUnitFrustumIndices = new int[] { 0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 1, 1, 4, 2, 2, 4, 3 };
        private static readonly float[] mkUnitFrustumVertices = new float[] { 
            0, 0, 0, 
            -1, -1, -1, 
            -1, 1, -1, 
            1, 1, -1, 
            1, -1, -1 };
        private static readonly int[] mkUnitQuadIndices = new int[] { 0, 1, 2, 1, 3, 2 };
        private static readonly float[] mkUnitQuadVertices = new float[] { -1, -1, 0,  -1,  1, 0,  1, -1, 0,  1,  1, 0 };
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

        private bool mbDelayedQuit = false;
        private string mInDirectory = string.Empty;
        private string mOutDirectory = string.Empty;
        private volatile Thread mWorker = null;
        private BuilderFlags mFlags = (BuilderFlags.ProcessColladaFiles);

        private void _Handler(string e)
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

        private void _HandleClose(object aSender, CancelEventArgs e)
        {
            if (mWorker != null)
            {
                _Handler("Stopping worker...");
                e.Cancel = true;
                mbDelayedQuit = true;

                if (Builder.bProcessing)
                {
                    Builder.StopProcessing();
                }
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
                Builder.Process(mInDirectory, mOutDirectory, _Handler, mFlags);
            }
            catch (Exception e) { _Handler(e.Message); }

            _Handler("Worker stopped.");
            mWorker = null;

            if (mbDelayedQuit)
            {
                Invoke(new Action(delegate { Close(); }));
            }
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
            else
            {
                _Handler("Builder is already running.");
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

        private void _Helper(int[] indices, float[] vertices, string id, bool abValidate)
        {
#if DEBUG
            if (abValidate)
            {
                Utilities.ValidateSealedConvexGeometry(indices, vertices);
            }
#endif

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
            _Helper(mkUnitBoxIndices, mkUnitBoxVertices, "unit-box", true);
            _Handler("Built unit-box.");
        }

        private void unitFrustumToolStripMenuItem_Click(object sender, EventArgs e)
        {
            _Helper(mkUnitFrustumIndices, mkUnitFrustumVertices, "unit-frustum", true);
            _Handler("Built unit-frustum.");
        }

        private void unitQuadToolStripMenuItem_Click(object sender, EventArgs e)
        {
            _Helper(mkUnitQuadIndices, mkUnitQuadVertices, "unit-quad", false);
            _Handler("Built unit-quad.");
        }

        private void unitSphereToolStripMenuItem_Click(object sender, EventArgs e)
        {
            int[] indices;
            float[] vertices;

            Utilities.GenerateSphere(1.0f, 12, true, out indices, out vertices);

            _Helper(indices, vertices, "unit-sphere", true);
            _Handler("Built unit-sphere.");
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
            _Handler("Built instancing-quad.");
        }

        private void processColladaToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ToolStripMenuItem item = (ToolStripMenuItem)sender;

            if (item.Checked) { mFlags |= BuilderFlags.ProcessColladaFiles; }
            else { mFlags &= ~BuilderFlags.ProcessColladaFiles; }
        }

    }
}