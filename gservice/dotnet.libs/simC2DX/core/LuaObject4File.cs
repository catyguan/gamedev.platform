using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using luanet;
using bmalangutil.core;
using System.IO;

namespace simC2DX.core
{
    public class LuaObject4File : LuaObjectAbstract
    {
        public String rootDir
        {
            get
            {
                if(_rootDir!=null)
                    return _rootDir;
                return System.Environment.CurrentDirectory + "\\datafile";
            }
            set
            {
                _rootDir = value;
            }
        }
        private String _rootDir;

        public override bool canInvoke(string name)
        {
            if (name == "exists")
            {
                return true;
            }
            if (name == "load")
            {
                return true;
            }
            if (name == "save")
            {
                return true;
            }
            if (name == "delete")
            {
                return true;
            }
            return base.canInvoke(name);
        }

        public override bool invoke(luanet.LuaApp app, string name, System.Collections.Generic.IList<luanet.LuaValue> ctx)
        {
            String rootdir = this.rootDir;
            String vfile = ctx.Count() > 0 ? ctx[0].stringValue() : "";
            if (name == "exists")
            {
                bool r = false;
                if (vfile != "")
                {
                    String file = VPath.create(vfile).bindRootFile(rootdir);
                    r = File.Exists(file);
                }
                ctx.Clear();
                ctx.Add(LuaValue.booleanValue(r));
                return true;
            }
            if (name == "load")
            {
                string r = "";
                if (vfile != "")
                {
                    String file = VPath.create(vfile).bindRootFile(rootdir);
                    if (File.Exists(file))
                    {
                        r = File.ReadAllText(file, Encoding.UTF8);
                    }
                }
                ctx.Clear();
                ctx.Add(LuaValue.stringValue(r));
                return true;
            }
            if (name == "save")
            {
                bool r = false;
                if (vfile != "")
                {
                    String content = ctx.Count() > 1 ? ctx[1].stringValue() : "";
                    String file = VPath.create(vfile).bindRootFile(rootdir);
                    File.WriteAllText(file, content, Encoding.UTF8);
                    r = true;
                }
                ctx.Clear();
                ctx.Add(LuaValue.booleanValue(r));
                return true;
            }
            if (name == "delete")
            {
                bool r = true;
                if (vfile != "")
                {
                    String file = VPath.create(vfile).bindRootFile(rootdir);
                    if (File.Exists(file))
                    {
                        File.Delete(file);
                        r = File.Exists(file);
                    }
                }
                ctx.Clear();
                ctx.Add(LuaValue.booleanValue(r));
                return true;
            }
            return base.invoke(app, name, ctx);
        }
    }
}
