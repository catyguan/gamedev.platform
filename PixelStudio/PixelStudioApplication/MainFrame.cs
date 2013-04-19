using PixelStudioApplication.C;
using PixelStudioApplication.M;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace PixelStudioApplication
{
    public partial class MainFrame : Form
    {
        private int childFormNumber = 0;

        public MainFrame()
        {
            InitializeComponent();
        }


        private void NewProject(object sender, EventArgs e)
        {
            if (ProjectManager.getInstance().current.isModify)
            {
                DialogResult r =  MessageBox.Show("是否保存当前项目的修改?", "确认", MessageBoxButtons.YesNoCancel);
                if(r == DialogResult.Cancel) {
                    return;
                }
                if (r == DialogResult.Yes)
                {
                    // a 'DialogResult.Yes' value was returned from the MessageBox
                    // proceed with your deletion
                    if (!doSaveProject(false))
                    {
                        return;
                    }
                }
            }
            ProjectManager.getInstance().newProject();
        }

        private bool doSaveProject(bool saveAs)
        {
            ProjectManager pm = ProjectManager.getInstance();
            if (!saveAs && pm.current.fileName != null)
            {
                pm.saveProject(pm.current.fileName);
            }
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.Personal);
            saveFileDialog.Filter = "项目文件(*.psp)|*.psp|所有文件(*.*)|*.*";
            if (saveFileDialog.ShowDialog(this) == DialogResult.OK)
            {
                string FileName = saveFileDialog.FileName;
                ProjectManager.getInstance().saveProject(FileName);
                return true;
            }
            return false;
        }

        private void SaveProject(object sender, EventArgs e)
        {
            doSaveProject(false);            
        }
        
        private void OpenProject(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.Personal);
            openFileDialog.Filter = "项目文件(*.psp)|*.psp|所有文件(*.*)|*.*";
            if (openFileDialog.ShowDialog(this) == DialogResult.OK)
            {
                string FileName = openFileDialog.FileName;
                ProjectManager.getInstance().openProject(FileName);
            }
        }

        private void SaveAsProfile(object sender, EventArgs e)
        {
            doSaveProject(true);
        }

        private void ExitToolsStripMenuItem_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void CutToolStripMenuItem_Click(object sender, EventArgs e)
        {
        }

        private void CopyToolStripMenuItem_Click(object sender, EventArgs e)
        {
        }

        private void PasteToolStripMenuItem_Click(object sender, EventArgs e)
        {
        }

        private void ToolBarToolStripMenuItem_Click(object sender, EventArgs e)
        {
            toolStrip.Visible = toolBarToolStripMenuItem.Checked;
        }

        private void StatusBarToolStripMenuItem_Click(object sender, EventArgs e)
        {
            statusStrip.Visible = statusBarToolStripMenuItem.Checked;
        }

        private void CascadeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            LayoutMdi(MdiLayout.Cascade);
        }

        private void TileVerticalToolStripMenuItem_Click(object sender, EventArgs e)
        {
            LayoutMdi(MdiLayout.TileVertical);
        }

        private void TileHorizontalToolStripMenuItem_Click(object sender, EventArgs e)
        {
            LayoutMdi(MdiLayout.TileHorizontal);
        }

        private void ArrangeIconsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            LayoutMdi(MdiLayout.ArrangeIcons);
        }

        private void CloseAllToolStripMenuItem_Click(object sender, EventArgs e)
        {
            foreach (Form childForm in MdiChildren)
            {
                childForm.Close();
            }
        }

        private void MainFrame_Load(object sender, EventArgs e)
        {
            ProjectManager pm = ProjectManager.getInstance();
            pm.eventHandler += new PropChangeHandler(this.ProjectManager_Change);

            bindProjectToTree();
        }

        private void ProjectManager_Change(object sender, String type, String prop)
        {
            if (type.Equals(TYPE.NEW))
            {
                bindProjectToTree();
            }
        }

        private void bindProjectToTree()
        {
            Project proj = ProjectManager.getInstance().current;

            resourceTreeView.Nodes.Clear();

            TreeNode root = new TreeNode();
            root.Text = "Project["+(proj.Title==null?"":proj.Title)+"]";
            resourceTreeView.Nodes.Add(root);
        }

        private PixelEditor tester;
        private void resourceTreeView_AfterSelect(object sender, TreeViewEventArgs e)
        {
            if (tester == null)
            {
                tester = new PixelEditor();
                tester.Dock = DockStyle.Fill;
                tester.pixelSize = new Size(12, 8);
                tester.cellSize = 24;
                scContainer.Panel2.Controls.Add(tester);
            }
        }
       
    }
}
