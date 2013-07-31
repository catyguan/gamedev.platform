using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using netsqlite;

namespace SQLiteToolApp
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }


        static DateTime dtStart = DateTime.SpecifyKind(new DateTime(1970, 1, 1), DateTimeKind.Utc).ToLocalTime();
        public long timestamp(DateTime dtNow)
        {   
            TimeSpan toNow = dtNow.Subtract(dtStart);
            return (long)Math.Round(toNow.TotalSeconds);
        }

        public DateTime timestamp(long time)
        {
            return dtStart.Add(new TimeSpan(time * 10000000));  
        }

        private SQLite create()
        {
            if (textFile.Text == "")
            {
                MessageBox.Show("请选择文件");
                return null;
            }            
            try
            {
                SQLite r = new SQLite();
                r.open(textFile.Text);
                if (textKey.Text != "")
                {
                    r.key(textKey.Text);
                }
                string sql;
                sql = "create table if not exists files(path TEXT PRIMARY KEY, content BLOB, time INTEGER);";
                r.doExec(sql);
                sql = "create table if not exists dirs(path TEXT PRIMARY KEY, orderkey INTEGER);";
                r.doExec(sql);
                return r;
            }
            catch (Exception e)
            {
                MessageBox.Show("打开错误:" + e.Message);
                return null;
            }
        }

        private void buttonBrowse_Click(object sender, EventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Title = "";
            dlg.Filter = "数据文件|*.dat|所有文件|*.*";
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                string path = dlg.FileName;
                textFile.Text = path;
            }
        }
        private void buttonNewDB_Click(object sender, EventArgs e)
        {
            SaveFileDialog dlg = new SaveFileDialog();
            dlg.Title = "";
            dlg.Filter = "数据文件|*.dat|所有文件|*.*";
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                string path = dlg.FileName;
                textFile.Text = path;
            }
        }

        private void buttonRekey_Click(object sender, EventArgs e)
        {
            SQLite db = create();
            if (db == null) return;
            try
            {
                db.rekey(textNewKey.Text);
            }
            catch (Exception err)
            {
                MessageBox.Show("fail: " + err.Message);
            }
            finally
            {
                db.close();
            }
        }

        private void buttonRefresh_Click(object sender, EventArgs e)
        {
            listviewDBFiles.Items.Clear();

            SQLite db = create();
            if (db == null) return;

            try
            {
                db.prepare("SELECT path, content, time FROM files ORDER BY path");
                Dictionary<String, Object> rs = new Dictionary<string, object>();
                while (db.queryNext(rs, true))
                {
                    var path = rs["path"];
                    var size = rs["content"];
                    var time = rs["time"];
                    rs.Clear();

                    ListViewItem item = new ListViewItem();
                    item.Text = path.ToString();
                    item.SubItems.Add(size.ToString());
                    long ts = 0;
                    long.TryParse(time.ToString(),out ts);
                    item.SubItems.Add(timestamp(ts).ToString());
                    listviewDBFiles.Items.Add(item);
                }
                db.closeStmt();
                MessageBox.Show("操作成功");
            }
            catch (Exception err)
            {
                MessageBox.Show("fail: " + err.Message);
            }
            finally
            {
                db.close();
            }
        }

        private void buttonAddDir_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog dialog = new FolderBrowserDialog();
            dialog.Description = "请选择文件路径";
            if (dialog.ShowDialog() == DialogResult.OK)
            {
                string foldPath = dialog.SelectedPath;
                listDirs.Items.Add(foldPath);
            }
        }

        private bool isDirSelected(int i)
        {
            if (listDirs.GetItemChecked(i)) return true;            
            return false;
        }


        private void buttonDelDir_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < listDirs.Items.Count; i++)
            {
                if (isDirSelected(i))
                {
                    listDirs.Items.RemoveAt(i);
                    i--;
                }
            }
        }

        private void swapDir(int i1, int i2)
        {
            bool ck1 = listDirs.GetItemChecked(i1);
            bool ck2 = listDirs.GetItemChecked(i2);
            var v1 = listDirs.Items[i1];
            var v2 = listDirs.Items[i2];

            listDirs.Items[i1] = v2;
            listDirs.Items[i2] = v1;
            listDirs.SetItemChecked(i1, ck2);
            listDirs.SetItemChecked(i2, ck1);
        }

        private void buttonUpDir_Click(object sender, EventArgs e)
        {
            for (int i = 1; i < listDirs.Items.Count; i++)
            {
                if(isDirSelected(i)) {
                    swapDir(i, i - 1);                    
                }
            }
        }

        private void buttonDownDir_Click(object sender, EventArgs e)
        {
            for (int i = listDirs.Items.Count - 2; i >= 0;  i--)
            {
                if (isDirSelected(i))
                {
                    swapDir(i, i + 1);                    
                }
            }
        }

        private void buttonLoadDirs_Click(object sender, EventArgs e)
        {
            SQLite db = create();
            if (db == null) return;
            listDirs.Items.Clear();
            try
            {
                db.prepare("SELECT * FROM dirs ORDER BY orderkey");
                var rs = new Dictionary<String, Object>();
                while (db.queryNext(rs, false))
                {
                    listDirs.Items.Add(rs["path"].ToString());
                    rs.Clear();
                }
                db.closeStmt();
                MessageBox.Show("操作成功");
            }
            catch (Exception err)
            {
                MessageBox.Show("fail: "+err.Message);
            }
            finally
            {
                db.close();
            }
        }

        private void buttonSaveDirs_Click(object sender, EventArgs e)
        {
            SQLite db = create();
            if (db == null) return;
            try
            {
                db.doExec("DELETE FROM dirs;");
                db.prepare("INSERT INTO dirs VALUES(?, ?);");
                int okey = 1;
                foreach (var dir in listDirs.Items)
                {                    
                    db.bindString(1, dir.ToString());
                    db.bindInt(2, okey++);
                    db.execute();
                    db.reset();
                }
                db.closeStmt();
                MessageBox.Show("操作成功");
            }
            catch (Exception err)
            {
                MessageBox.Show("fail: " + err.Message);
            }
            finally
            {
                db.close();
            }
        }

        private void packageDir(SQLite db, string dir,string pdir)
        {
            var files = Directory.EnumerateFileSystemEntries(dir.ToString());
            foreach (var fileName in files)            
            {
                FileInfo finfo = new FileInfo(fileName);
                string vpath = pdir + "/" + finfo.Name;
                
                if (finfo.Attributes.HasFlag(FileAttributes.Directory))
                {
                    packageDir(db, fileName, vpath);
                }
                else
                {
                    Console.WriteLine("package "+ fileName + " >> " + vpath);
                    int len = (int) finfo.Length;
                    byte[] buf;
                    using (var fs = finfo.OpenRead())
                    {          
                        buf = new byte[len];
                        fs.Read(buf, 0, len);
                    }

                    db.bindString(1, vpath.ToString());
                    db.bindBlob(2, buf);
                    db.bindLong(3, timestamp(finfo.LastWriteTime));
                    db.execute();
                    db.reset();
                }
            }            
        }

        private void buttonPackage_Click(object sender, EventArgs e)
        {
            SQLite db = create();
            if (db == null) return;
            try
            {
                db.doExec("DELETE FROM files;");
                db.prepare("INSERT OR REPLACE INTO files VALUES(?, ?, ?);");
                foreach (var dir in listDirs.Items)
                {
                    packageDir(db, dir.ToString(), "");
                }
                db.closeStmt();
                MessageBox.Show("操作成功");
            }
            catch (Exception err)
            {
                MessageBox.Show("fail: " + err.Message);
            }
            finally
            {
                db.close();
            }
        }

        
    }
}
