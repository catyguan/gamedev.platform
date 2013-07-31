namespace SQLiteToolApp
{
    partial class MainForm
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.panelSQLite = new System.Windows.Forms.Panel();
            this.buttonNewDB = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.textKey = new System.Windows.Forms.TextBox();
            this.buttonBrowse = new System.Windows.Forms.Button();
            this.textFile = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.tabTool = new System.Windows.Forms.TabControl();
            this.pageList = new System.Windows.Forms.TabPage();
            this.panel2 = new System.Windows.Forms.Panel();
            this.listviewDBFiles = new System.Windows.Forms.ListView();
            this.colFile = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.colSize = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.colTime = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.panel1 = new System.Windows.Forms.Panel();
            this.buttonRefresh = new System.Windows.Forms.Button();
            this.pagePackage = new System.Windows.Forms.TabPage();
            this.panel4 = new System.Windows.Forms.Panel();
            this.listDirs = new System.Windows.Forms.CheckedListBox();
            this.panel3 = new System.Windows.Forms.Panel();
            this.buttonPackage = new System.Windows.Forms.Button();
            this.buttonSaveDirs = new System.Windows.Forms.Button();
            this.buttonLoadDirs = new System.Windows.Forms.Button();
            this.buttonDownDir = new System.Windows.Forms.Button();
            this.buttonUpDir = new System.Windows.Forms.Button();
            this.buttonDelDir = new System.Windows.Forms.Button();
            this.buttonAddDir = new System.Windows.Forms.Button();
            this.pageRekey = new System.Windows.Forms.TabPage();
            this.buttonRekey = new System.Windows.Forms.Button();
            this.textNewKey = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.panelSQLite.SuspendLayout();
            this.tabTool.SuspendLayout();
            this.pageList.SuspendLayout();
            this.panel2.SuspendLayout();
            this.panel1.SuspendLayout();
            this.pagePackage.SuspendLayout();
            this.panel4.SuspendLayout();
            this.panel3.SuspendLayout();
            this.pageRekey.SuspendLayout();
            this.SuspendLayout();
            // 
            // panelSQLite
            // 
            this.panelSQLite.Controls.Add(this.buttonNewDB);
            this.panelSQLite.Controls.Add(this.label2);
            this.panelSQLite.Controls.Add(this.textKey);
            this.panelSQLite.Controls.Add(this.buttonBrowse);
            this.panelSQLite.Controls.Add(this.textFile);
            this.panelSQLite.Controls.Add(this.label1);
            this.panelSQLite.Dock = System.Windows.Forms.DockStyle.Top;
            this.panelSQLite.Location = new System.Drawing.Point(0, 0);
            this.panelSQLite.Name = "panelSQLite";
            this.panelSQLite.Size = new System.Drawing.Size(661, 76);
            this.panelSQLite.TabIndex = 0;
            // 
            // buttonNewDB
            // 
            this.buttonNewDB.Location = new System.Drawing.Point(525, 9);
            this.buttonNewDB.Name = "buttonNewDB";
            this.buttonNewDB.Size = new System.Drawing.Size(75, 23);
            this.buttonNewDB.TabIndex = 5;
            this.buttonNewDB.Text = "新建";
            this.buttonNewDB.UseVisualStyleBackColor = true;
            this.buttonNewDB.Click += new System.EventHandler(this.buttonNewDB_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 39);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(29, 12);
            this.label2.TabIndex = 4;
            this.label2.Text = "密码";
            // 
            // textKey
            // 
            this.textKey.Location = new System.Drawing.Point(47, 36);
            this.textKey.Name = "textKey";
            this.textKey.Size = new System.Drawing.Size(100, 21);
            this.textKey.TabIndex = 3;
            this.textKey.Text = "abc";
            // 
            // buttonBrowse
            // 
            this.buttonBrowse.Location = new System.Drawing.Point(444, 9);
            this.buttonBrowse.Name = "buttonBrowse";
            this.buttonBrowse.Size = new System.Drawing.Size(75, 23);
            this.buttonBrowse.TabIndex = 2;
            this.buttonBrowse.Text = "浏览";
            this.buttonBrowse.UseVisualStyleBackColor = true;
            this.buttonBrowse.Click += new System.EventHandler(this.buttonBrowse_Click);
            // 
            // textFile
            // 
            this.textFile.Location = new System.Drawing.Point(47, 9);
            this.textFile.Name = "textFile";
            this.textFile.Size = new System.Drawing.Size(391, 21);
            this.textFile.TabIndex = 1;
            this.textFile.Text = "E:\\encrypt.dat";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 12);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(29, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "文件";
            // 
            // tabTool
            // 
            this.tabTool.Controls.Add(this.pageList);
            this.tabTool.Controls.Add(this.pagePackage);
            this.tabTool.Controls.Add(this.pageRekey);
            this.tabTool.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabTool.Location = new System.Drawing.Point(0, 76);
            this.tabTool.Name = "tabTool";
            this.tabTool.SelectedIndex = 0;
            this.tabTool.Size = new System.Drawing.Size(661, 381);
            this.tabTool.TabIndex = 1;
            // 
            // pageList
            // 
            this.pageList.BackColor = System.Drawing.SystemColors.Control;
            this.pageList.Controls.Add(this.panel2);
            this.pageList.Controls.Add(this.panel1);
            this.pageList.Location = new System.Drawing.Point(4, 22);
            this.pageList.Name = "pageList";
            this.pageList.Padding = new System.Windows.Forms.Padding(3);
            this.pageList.Size = new System.Drawing.Size(653, 355);
            this.pageList.TabIndex = 1;
            this.pageList.Text = "内容";
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.listviewDBFiles);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel2.Location = new System.Drawing.Point(3, 39);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(647, 313);
            this.panel2.TabIndex = 1;
            // 
            // listviewDBFiles
            // 
            this.listviewDBFiles.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.colFile,
            this.colSize,
            this.colTime});
            this.listviewDBFiles.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listviewDBFiles.FullRowSelect = true;
            this.listviewDBFiles.Location = new System.Drawing.Point(0, 0);
            this.listviewDBFiles.Name = "listviewDBFiles";
            this.listviewDBFiles.Size = new System.Drawing.Size(647, 313);
            this.listviewDBFiles.TabIndex = 0;
            this.listviewDBFiles.UseCompatibleStateImageBehavior = false;
            this.listviewDBFiles.View = System.Windows.Forms.View.Details;
            // 
            // colFile
            // 
            this.colFile.Text = "文件";
            this.colFile.Width = 362;
            // 
            // colSize
            // 
            this.colSize.Text = "大小";
            this.colSize.Width = 93;
            // 
            // colTime
            // 
            this.colTime.Text = "时间";
            this.colTime.Width = 174;
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.buttonRefresh);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(3, 3);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(647, 36);
            this.panel1.TabIndex = 0;
            // 
            // buttonRefresh
            // 
            this.buttonRefresh.Location = new System.Drawing.Point(7, 7);
            this.buttonRefresh.Name = "buttonRefresh";
            this.buttonRefresh.Size = new System.Drawing.Size(75, 23);
            this.buttonRefresh.TabIndex = 0;
            this.buttonRefresh.Text = "刷新";
            this.buttonRefresh.UseVisualStyleBackColor = true;
            this.buttonRefresh.Click += new System.EventHandler(this.buttonRefresh_Click);
            // 
            // pagePackage
            // 
            this.pagePackage.BackColor = System.Drawing.SystemColors.Control;
            this.pagePackage.Controls.Add(this.panel4);
            this.pagePackage.Controls.Add(this.panel3);
            this.pagePackage.Location = new System.Drawing.Point(4, 22);
            this.pagePackage.Name = "pagePackage";
            this.pagePackage.Size = new System.Drawing.Size(653, 355);
            this.pagePackage.TabIndex = 2;
            this.pagePackage.Text = "打包";
            // 
            // panel4
            // 
            this.panel4.Controls.Add(this.listDirs);
            this.panel4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel4.Location = new System.Drawing.Point(0, 0);
            this.panel4.Name = "panel4";
            this.panel4.Padding = new System.Windows.Forms.Padding(8);
            this.panel4.Size = new System.Drawing.Size(408, 355);
            this.panel4.TabIndex = 1;
            // 
            // listDirs
            // 
            this.listDirs.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listDirs.FormattingEnabled = true;
            this.listDirs.Location = new System.Drawing.Point(8, 8);
            this.listDirs.Name = "listDirs";
            this.listDirs.Size = new System.Drawing.Size(392, 339);
            this.listDirs.TabIndex = 0;
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.buttonPackage);
            this.panel3.Controls.Add(this.buttonSaveDirs);
            this.panel3.Controls.Add(this.buttonLoadDirs);
            this.panel3.Controls.Add(this.buttonDownDir);
            this.panel3.Controls.Add(this.buttonUpDir);
            this.panel3.Controls.Add(this.buttonDelDir);
            this.panel3.Controls.Add(this.buttonAddDir);
            this.panel3.Dock = System.Windows.Forms.DockStyle.Right;
            this.panel3.Location = new System.Drawing.Point(408, 0);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(245, 355);
            this.panel3.TabIndex = 0;
            // 
            // buttonPackage
            // 
            this.buttonPackage.Location = new System.Drawing.Point(15, 245);
            this.buttonPackage.Name = "buttonPackage";
            this.buttonPackage.Size = new System.Drawing.Size(75, 23);
            this.buttonPackage.TabIndex = 6;
            this.buttonPackage.Text = "打包";
            this.buttonPackage.UseVisualStyleBackColor = true;
            this.buttonPackage.Click += new System.EventHandler(this.buttonPackage_Click);
            // 
            // buttonSaveDirs
            // 
            this.buttonSaveDirs.Location = new System.Drawing.Point(15, 216);
            this.buttonSaveDirs.Name = "buttonSaveDirs";
            this.buttonSaveDirs.Size = new System.Drawing.Size(75, 23);
            this.buttonSaveDirs.TabIndex = 5;
            this.buttonSaveDirs.Text = "保存目录";
            this.buttonSaveDirs.UseVisualStyleBackColor = true;
            this.buttonSaveDirs.Click += new System.EventHandler(this.buttonSaveDirs_Click);
            // 
            // buttonLoadDirs
            // 
            this.buttonLoadDirs.Location = new System.Drawing.Point(15, 187);
            this.buttonLoadDirs.Name = "buttonLoadDirs";
            this.buttonLoadDirs.Size = new System.Drawing.Size(75, 23);
            this.buttonLoadDirs.TabIndex = 4;
            this.buttonLoadDirs.Text = "加载目录";
            this.buttonLoadDirs.UseVisualStyleBackColor = true;
            this.buttonLoadDirs.Click += new System.EventHandler(this.buttonLoadDirs_Click);
            // 
            // buttonDownDir
            // 
            this.buttonDownDir.Location = new System.Drawing.Point(15, 124);
            this.buttonDownDir.Name = "buttonDownDir";
            this.buttonDownDir.Size = new System.Drawing.Size(75, 23);
            this.buttonDownDir.TabIndex = 3;
            this.buttonDownDir.Text = "向下移动";
            this.buttonDownDir.UseVisualStyleBackColor = true;
            this.buttonDownDir.Click += new System.EventHandler(this.buttonDownDir_Click);
            // 
            // buttonUpDir
            // 
            this.buttonUpDir.Location = new System.Drawing.Point(15, 94);
            this.buttonUpDir.Name = "buttonUpDir";
            this.buttonUpDir.Size = new System.Drawing.Size(75, 23);
            this.buttonUpDir.TabIndex = 2;
            this.buttonUpDir.Text = "向上移动";
            this.buttonUpDir.UseVisualStyleBackColor = true;
            this.buttonUpDir.Click += new System.EventHandler(this.buttonUpDir_Click);
            // 
            // buttonDelDir
            // 
            this.buttonDelDir.Location = new System.Drawing.Point(15, 44);
            this.buttonDelDir.Name = "buttonDelDir";
            this.buttonDelDir.Size = new System.Drawing.Size(75, 23);
            this.buttonDelDir.TabIndex = 1;
            this.buttonDelDir.Text = "删除";
            this.buttonDelDir.UseVisualStyleBackColor = true;
            this.buttonDelDir.Click += new System.EventHandler(this.buttonDelDir_Click);
            // 
            // buttonAddDir
            // 
            this.buttonAddDir.Location = new System.Drawing.Point(15, 15);
            this.buttonAddDir.Name = "buttonAddDir";
            this.buttonAddDir.Size = new System.Drawing.Size(75, 23);
            this.buttonAddDir.TabIndex = 0;
            this.buttonAddDir.Text = "添加";
            this.buttonAddDir.UseVisualStyleBackColor = true;
            this.buttonAddDir.Click += new System.EventHandler(this.buttonAddDir_Click);
            // 
            // pageRekey
            // 
            this.pageRekey.BackColor = System.Drawing.SystemColors.Control;
            this.pageRekey.Controls.Add(this.buttonRekey);
            this.pageRekey.Controls.Add(this.textNewKey);
            this.pageRekey.Controls.Add(this.label3);
            this.pageRekey.Location = new System.Drawing.Point(4, 22);
            this.pageRekey.Name = "pageRekey";
            this.pageRekey.Padding = new System.Windows.Forms.Padding(3);
            this.pageRekey.Size = new System.Drawing.Size(653, 355);
            this.pageRekey.TabIndex = 0;
            this.pageRekey.Text = "REKEY";
            // 
            // buttonRekey
            // 
            this.buttonRekey.Location = new System.Drawing.Point(170, 13);
            this.buttonRekey.Name = "buttonRekey";
            this.buttonRekey.Size = new System.Drawing.Size(75, 23);
            this.buttonRekey.TabIndex = 2;
            this.buttonRekey.Text = "修改";
            this.buttonRekey.UseVisualStyleBackColor = true;
            this.buttonRekey.Click += new System.EventHandler(this.buttonRekey_Click);
            // 
            // textNewKey
            // 
            this.textNewKey.Location = new System.Drawing.Point(64, 15);
            this.textNewKey.Name = "textNewKey";
            this.textNewKey.Size = new System.Drawing.Size(100, 21);
            this.textNewKey.TabIndex = 1;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(17, 18);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(41, 12);
            this.label3.TabIndex = 0;
            this.label3.Text = "新密码";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(661, 457);
            this.Controls.Add(this.tabTool);
            this.Controls.Add(this.panelSQLite);
            this.Name = "MainForm";
            this.Text = "SQLite Tool";
            this.panelSQLite.ResumeLayout(false);
            this.panelSQLite.PerformLayout();
            this.tabTool.ResumeLayout(false);
            this.pageList.ResumeLayout(false);
            this.panel2.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.pagePackage.ResumeLayout(false);
            this.panel4.ResumeLayout(false);
            this.panel3.ResumeLayout(false);
            this.pageRekey.ResumeLayout(false);
            this.pageRekey.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panelSQLite;
        private System.Windows.Forms.TabControl tabTool;
        private System.Windows.Forms.TabPage pageRekey;
        private System.Windows.Forms.TabPage pageList;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button buttonBrowse;
        private System.Windows.Forms.TextBox textFile;
        private System.Windows.Forms.TextBox textKey;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button buttonRekey;
        private System.Windows.Forms.TextBox textNewKey;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.ListView listviewDBFiles;
        private System.Windows.Forms.Button buttonRefresh;
        private System.Windows.Forms.ColumnHeader colFile;
        private System.Windows.Forms.ColumnHeader colSize;
        private System.Windows.Forms.ColumnHeader colTime;
        private System.Windows.Forms.Button buttonNewDB;
        private System.Windows.Forms.TabPage pagePackage;
        private System.Windows.Forms.Panel panel4;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.Button buttonAddDir;
        private System.Windows.Forms.CheckedListBox listDirs;
        private System.Windows.Forms.Button buttonDownDir;
        private System.Windows.Forms.Button buttonUpDir;
        private System.Windows.Forms.Button buttonDelDir;
        private System.Windows.Forms.Button buttonPackage;
        private System.Windows.Forms.Button buttonSaveDirs;
        private System.Windows.Forms.Button buttonLoadDirs;
    }
}

