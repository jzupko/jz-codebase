namespace jz.cb
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.toolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.InFolderMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.OutFolderMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.RunMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.exitToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.createToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.unitBoxToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.unitFrustumToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.unitQuadToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.unitSphereToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.instancingQuadToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.processOptionsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.processColladaToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.Output = new System.Windows.Forms.TextBox();
            this.InFolder = new System.Windows.Forms.TextBox();
            this.OutFolder = new System.Windows.Forms.TextBox();
            this.InputFolder = new System.Windows.Forms.Label();
            this.OutputFolder = new System.Windows.Forms.Label();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolStripMenuItem1,
            this.createToolStripMenuItem,
            this.processOptionsToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(592, 24);
            this.menuStrip1.TabIndex = 2;
            this.menuStrip1.Text = "menuStrip1";
            this.menuStrip1.Click += new System.EventHandler(this.menuStrip1_Click);
            // 
            // toolStripMenuItem1
            // 
            this.toolStripMenuItem1.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.InFolderMenuItem,
            this.OutFolderMenuItem,
            this.RunMenuItem,
            this.exitToolStripMenuItem});
            this.toolStripMenuItem1.Name = "toolStripMenuItem1";
            this.toolStripMenuItem1.Size = new System.Drawing.Size(35, 20);
            this.toolStripMenuItem1.Text = "&File";
            // 
            // InFolderMenuItem
            // 
            this.InFolderMenuItem.Name = "InFolderMenuItem";
            this.InFolderMenuItem.Size = new System.Drawing.Size(152, 22);
            this.InFolderMenuItem.Text = "&In Folder";
            this.InFolderMenuItem.Click += new System.EventHandler(this.FolderMenuItem_Click);
            // 
            // OutFolderMenuItem
            // 
            this.OutFolderMenuItem.Name = "OutFolderMenuItem";
            this.OutFolderMenuItem.Size = new System.Drawing.Size(152, 22);
            this.OutFolderMenuItem.Text = "&Out Folder";
            this.OutFolderMenuItem.Click += new System.EventHandler(this.OutFolderMenuItem_Click);
            // 
            // RunMenuItem
            // 
            this.RunMenuItem.Name = "RunMenuItem";
            this.RunMenuItem.Size = new System.Drawing.Size(152, 22);
            this.RunMenuItem.Text = "&Run";
            this.RunMenuItem.Click += new System.EventHandler(this.RunMenuItem_Click);
            // 
            // exitToolStripMenuItem
            // 
            this.exitToolStripMenuItem.Name = "exitToolStripMenuItem";
            this.exitToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.exitToolStripMenuItem.Text = "E&xit";
            this.exitToolStripMenuItem.Click += new System.EventHandler(this.exitToolStripMenuItem_Click);
            // 
            // createToolStripMenuItem
            // 
            this.createToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.unitBoxToolStripMenuItem,
            this.unitFrustumToolStripMenuItem,
            this.instancingQuadToolStripMenuItem,
            this.unitQuadToolStripMenuItem,
            this.unitSphereToolStripMenuItem});
            this.createToolStripMenuItem.Name = "createToolStripMenuItem";
            this.createToolStripMenuItem.Size = new System.Drawing.Size(52, 20);
            this.createToolStripMenuItem.Text = "Create";
            // 
            // unitBoxToolStripMenuItem
            // 
            this.unitBoxToolStripMenuItem.Name = "unitBoxToolStripMenuItem";
            this.unitBoxToolStripMenuItem.Size = new System.Drawing.Size(175, 22);
            this.unitBoxToolStripMenuItem.Text = "Unit Box";
            this.unitBoxToolStripMenuItem.Click += new System.EventHandler(this.unitBoxToolStripMenuItem_Click);
            // 
            // unitFrustumToolStripMenuItem
            // 
            this.unitFrustumToolStripMenuItem.Name = "unitFrustumToolStripMenuItem";
            this.unitFrustumToolStripMenuItem.Size = new System.Drawing.Size(175, 22);
            this.unitFrustumToolStripMenuItem.Text = "Unit Frustum";
            this.unitFrustumToolStripMenuItem.Click += new System.EventHandler(this.unitFrustumToolStripMenuItem_Click);
            // 
            // unitQuadToolStripMenuItem
            // 
            this.unitQuadToolStripMenuItem.Name = "unitQuadToolStripMenuItem";
            this.unitQuadToolStripMenuItem.Size = new System.Drawing.Size(175, 22);
            this.unitQuadToolStripMenuItem.Text = "Unit Quad";
            this.unitQuadToolStripMenuItem.Click += new System.EventHandler(this.unitQuadToolStripMenuItem_Click);
            // 
            // unitSphereToolStripMenuItem
            // 
            this.unitSphereToolStripMenuItem.Name = "unitSphereToolStripMenuItem";
            this.unitSphereToolStripMenuItem.Size = new System.Drawing.Size(175, 22);
            this.unitSphereToolStripMenuItem.Text = "Unit Sphere";
            this.unitSphereToolStripMenuItem.Click += new System.EventHandler(this.unitSphereToolStripMenuItem_Click);
            // 
            // instancingQuadToolStripMenuItem
            // 
            this.instancingQuadToolStripMenuItem.Name = "instancingQuadToolStripMenuItem";
            this.instancingQuadToolStripMenuItem.Size = new System.Drawing.Size(175, 22);
            this.instancingQuadToolStripMenuItem.Text = "Unit Instancing Quad";
            this.instancingQuadToolStripMenuItem.Click += new System.EventHandler(this.instancingQuadToolStripMenuItem_Click);
            // 
            // processOptionsToolStripMenuItem
            // 
            this.processOptionsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.processColladaToolStripMenuItem});
            this.processOptionsToolStripMenuItem.Name = "processOptionsToolStripMenuItem";
            this.processOptionsToolStripMenuItem.Size = new System.Drawing.Size(96, 20);
            this.processOptionsToolStripMenuItem.Text = "Process Options";
            // 
            // processColladaToolStripMenuItem
            // 
            this.processColladaToolStripMenuItem.Checked = true;
            this.processColladaToolStripMenuItem.CheckOnClick = true;
            this.processColladaToolStripMenuItem.CheckState = System.Windows.Forms.CheckState.Checked;
            this.processColladaToolStripMenuItem.Name = "processColladaToolStripMenuItem";
            this.processColladaToolStripMenuItem.Size = new System.Drawing.Size(152, 22);
            this.processColladaToolStripMenuItem.Text = "Process Collada";
            this.processColladaToolStripMenuItem.Click += new System.EventHandler(this.processColladaToolStripMenuItem_Click);
            // 
            // Output
            // 
            this.Output.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.Output.Location = new System.Drawing.Point(0, 87);
            this.Output.Multiline = true;
            this.Output.Name = "Output";
            this.Output.ReadOnly = true;
            this.Output.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.Output.Size = new System.Drawing.Size(592, 175);
            this.Output.TabIndex = 3;
            this.Output.WordWrap = false;
            // 
            // InFolder
            // 
            this.InFolder.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.InFolder.Location = new System.Drawing.Point(93, 33);
            this.InFolder.Name = "InFolder";
            this.InFolder.ReadOnly = true;
            this.InFolder.Size = new System.Drawing.Size(499, 20);
            this.InFolder.TabIndex = 4;
            this.InFolder.WordWrap = false;
            // 
            // OutFolder
            // 
            this.OutFolder.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.OutFolder.Location = new System.Drawing.Point(93, 59);
            this.OutFolder.Name = "OutFolder";
            this.OutFolder.ReadOnly = true;
            this.OutFolder.Size = new System.Drawing.Size(499, 20);
            this.OutFolder.TabIndex = 5;
            this.OutFolder.WordWrap = false;
            // 
            // InputFolder
            // 
            this.InputFolder.AutoSize = true;
            this.InputFolder.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.InputFolder.Location = new System.Drawing.Point(15, 36);
            this.InputFolder.Name = "InputFolder";
            this.InputFolder.Size = new System.Drawing.Size(75, 13);
            this.InputFolder.TabIndex = 6;
            this.InputFolder.Text = "Input Folder";
            // 
            // OutputFolder
            // 
            this.OutputFolder.AutoSize = true;
            this.OutputFolder.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.OutputFolder.Location = new System.Drawing.Point(6, 62);
            this.OutputFolder.Name = "OutputFolder";
            this.OutputFolder.Size = new System.Drawing.Size(84, 13);
            this.OutputFolder.TabIndex = 7;
            this.OutputFolder.Text = "Output Folder";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(592, 273);
            this.Controls.Add(this.OutputFolder);
            this.Controls.Add(this.InputFolder);
            this.Controls.Add(this.OutFolder);
            this.Controls.Add(this.InFolder);
            this.Controls.Add(this.Output);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "MainForm";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "JZ Content Builder";
            this.TopMost = true;
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem InFolderMenuItem;
        private System.Windows.Forms.ToolStripMenuItem RunMenuItem;
        private System.Windows.Forms.TextBox Output;
        private System.Windows.Forms.TextBox InFolder;
        private System.Windows.Forms.ToolStripMenuItem exitToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem createToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem unitBoxToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem unitFrustumToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem unitQuadToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem unitSphereToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem instancingQuadToolStripMenuItem;
        private System.Windows.Forms.TextBox OutFolder;
        private System.Windows.Forms.Label InputFolder;
        private System.Windows.Forms.Label OutputFolder;
        private System.Windows.Forms.ToolStripMenuItem OutFolderMenuItem;
        private System.Windows.Forms.ToolStripMenuItem processOptionsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem processColladaToolStripMenuItem;
    }
}