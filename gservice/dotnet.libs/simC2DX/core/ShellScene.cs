using bmalangutil.forms.shellctrl;
using luanet;
using simC2DX.core;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace simC2DX.core
{
    public interface CommandHandler
    {
        bool processCommand(String cmd);
    }

    public interface ShellControlProvider
    {
        ShellControl getShell();
    }

    public class ShellScene : SimC2DXScene, SimC2DXSceneMaster, ShellControlProvider, CommandHandler
    {
        protected ShellControl shell;
        public ShellScene()
        {
            shell = new ShellControl();
            shell.Prompt = ">>>";
            shell.ShellTextBackColor = System.Drawing.Color.Black;
            shell.ShellTextFont = new System.Drawing.Font("Tahoma", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
            shell.ShellTextForeColor = System.Drawing.Color.LimeGreen;

            shell.CommandEntered += new EventCommandEntered(shellControl_CommandEntered);
        }

        public ShellControl getShell()
        {
            return shell;
        }

        public virtual Control control
        {
            get { return shell; }
        }

        public virtual void onEnter()
        {
            shell.Prompt = shell.Name + ">>>";
            shell.WriteText(getWellcome(), true);   
        }

        public virtual void onActive()
        {

        }

        public virtual void onExit()
        {

        }

        protected virtual string getWellcome()
        {
            return "Wellcome to " + shell.Name;
        }

        protected virtual string getHelp()
        {
            StringBuilder stringBuilder = new StringBuilder();
            stringBuilder.Append(shell.Name + " Help");
            stringBuilder.Append(System.Environment.NewLine);
            stringBuilder.Append("*******************************************");
            stringBuilder.Append(System.Environment.NewLine);
            stringBuilder.Append("Commands Available");
            stringBuilder.Append(System.Environment.NewLine);
            stringBuilder.Append("(1) prompt - Changes prompt. Usage (prompt=<desired_prompt>");
            stringBuilder.Append(System.Environment.NewLine);
            stringBuilder.Append("(2) history - prints history of entered commands");
            stringBuilder.Append(System.Environment.NewLine);
            stringBuilder.Append("(3) cls - Clears the screen");
            stringBuilder.Append(System.Environment.NewLine);
            return stringBuilder.ToString();
        }

        void shellControl_CommandEntered(object sender, CommandEnteredEventArgs e)
        {
            string command = e.Command;
            if (command == "/")
            {
                command = lastCommand;
            }
            if (processCommand(command))
            {
                if (command != "/") lastCommand = command;
            }
        }

        private String lastCommand = "";
        public bool processCommand(String command)
        {            
            if (processInternalCommand(command))
            {                
                return true;
            }

            String[] cmds = command.Split(new char[]{' '}, 2);
            String cmd = cmds[0];
            String args = cmds.Length>1?cmds[1]:"";
            if (!processCommand(cmd, args))
            {
                shell.WriteText("???");
                return false;
            }
            return true;
        }
        
        private bool processInternalCommand(string command)
        {
            if (command == "cls")
            {
                shell.Clear();
                return true;
            }
            if (command == "history")
            {
                string[] commands = shell.GetCommandHistory();
                StringBuilder stringBuilder = new StringBuilder(commands.Length);
                foreach (string str in commands)
                {
                    stringBuilder.Append(str);
                    stringBuilder.Append(System.Environment.NewLine);
                }
                shell.WriteText(stringBuilder.ToString());
                return true;
            }            
            if (command == "help")
            {
                shell.WriteText(getHelp());
                return true;
            }
            if (command.StartsWith("prompt"))
            {
                string[] parts = command.Split(new char[] { '=' });
                if (parts.Length == 2 && parts[0].Trim() == "prompt")
                    shell.Prompt = parts[1].Trim();
                return true;
            }
            return false;
        }

        protected LuaValue parseArg(String s)
        {
            if (s != null)
            {
                return LuaValue.stringValue(s);
            }
            return LuaValue.nullValue();
        }
        protected void parseArgs(String cmd, IList<LuaValue> ps)
        {
            char[] chs = cmd.ToCharArray();
            int larg = -1;
            for (int i = 0; i < chs.Length; i++)
            {
                char ch = chs[i];
                if (larg == -1)
                {
                    if (Char.IsWhiteSpace(ch))
                    {
                        continue;
                    }
                    larg = i;
                }
                else
                {
                    if (Char.IsWhiteSpace(ch))
                    {
                        string s = new string(chs, larg, i - larg);
                        ps.Add(parseArg(s));
                        larg = -1;
                    }
                }
            }
            if (larg != -1)
            {
                string s = new string(chs, larg, chs.Length - larg);
                ps.Add(parseArg(s));
            }
        }

        protected String includePrex
        {
            get;
            set;
        }

        protected String packageName(string name)
        {
            if (name.StartsWith("@"))
            {
                return name.Substring(1);
            }
            else
            {
                return includePrex+name;
            }
        }

        protected virtual bool processCommand(string cmd, string args)
        {
            if (cmd == "include")
            {
                if (args != "")
                {
                    String fn = packageName(args);
                    Console.WriteLine("include " + fn);
                    List<LuaValue> ps = new List<LuaValue>();
                    ps.Add(LuaValue.stringValue(fn));
                    SimC2DXDirector._instance.apiCall("include", ps);
                }
                else
                {
                    shell.WriteText("format: include [package name]");
                }
                return true;
            }
            if (cmd == "tc" || cmd=="cmd")
            {
                if (args != "")
                {
                    String file = packageName(args + (cmd == "tc" ? "_testcase" : "_command"));
                    Console.WriteLine("include "+file);
                    List<LuaValue> ps = new List<LuaValue>();
                    ps.Add(LuaValue.stringValue(file));
                    SimC2DXDirector._instance.apiCall("include", ps);
                }
                else
                {
                    shell.WriteText("format: tc|cmd [name]");
                }
                return true;
            }
            if (cmd == "oc")
            {
                String[] tmp = args.Split(new char[] { ' ' }, 2);
                String name = tmp[0];
                if (name != "")
                {
                    var ps = new List<LuaValue>();
                    if (tmp.Length > 1)
                    {
                        parseArgs(tmp[1], ps);
                    }
                    LuaValue r = SimC2DXDirector._instance.apiObjectCall(name, ps);
                    if (r == null) r = LuaValue.nullValue();
                    shell.WriteText("==> " + r.dumpValue());
                }
                else
                {
                    shell.WriteText("format: oc name ...");
                }
                return true;
            }
            if (cmd == "dataget")
            {
                String name = args;
                if (name != "")
                {
                    LuaValue r = SimC2DXDirector._instance.apiDataGet(name);
                    if (r == null) r = LuaValue.nullValue();
                    shell.WriteText("==> " + r.dumpValue());
                }
                else
                {
                    shell.WriteText("format: dataget name");
                }
                return true;
            }
            return false;
        }       
    }

    class ShellTabScene : ShellScene
    {
        private TabControl _tabControl;
        private TabPage _tabUI;

        public ShellTabScene()
        {
            _tabControl = new System.Windows.Forms.TabControl();
            TabPage tabShell = new System.Windows.Forms.TabPage();
            TabPage tabUI = new System.Windows.Forms.TabPage();
            _tabControl.SuspendLayout();

            // 
            // _tabControl1
            // 
            _tabControl.Controls.Add(tabShell);
            _tabControl.Controls.Add(tabUI);            
            _tabControl.SelectedIndex = 0;            
            _tabControl.TabIndex = 0;

            // 
            // tabShell
            // 
            tabShell.Name = "tabShell";
            tabShell.Padding = new System.Windows.Forms.Padding(3);
            tabShell.TabIndex = 0;
            tabShell.Text = "Shell";
            tabShell.UseVisualStyleBackColor = true;
            shell.Dock = DockStyle.Fill;
            tabShell.Controls.Add(shell);

            // 
            // tabUI
            //             
            tabUI.Name = "tabUI";
            tabUI.Padding = new System.Windows.Forms.Padding(3);
            tabUI.TabIndex = 1;
            tabUI.Text = "UI";
            tabUI.UseVisualStyleBackColor = true;
            _tabUI = tabUI;

            _tabControl.ResumeLayout();
        }

        public TabPage tabUI
        {
            get
            {
                return _tabUI;
            }
        }

        public override Control control
        {
            get
            {
                return _tabControl;
            }
        }
    }
}
