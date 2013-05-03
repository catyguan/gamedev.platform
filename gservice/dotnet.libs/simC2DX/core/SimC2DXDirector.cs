using luanet;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace simC2DX.core
{
    public interface SimC2DXScene
    {
        void onEnter();
        void onExit();
    }

    public class SimC2DXDirector : LuaObjectAbstract
    {
        protected static SimC2DXDirector INSTANCE
        {
            get
            {
                return _instance;
            }
        }
        internal static SimC2DXDirector _instance;

        internal Form _mainFrame;
        Control _currentScene;
        Stack<Control> _sceneStack = new Stack<Control>();
        internal LuaApp _luaApp;

        public LuaApp luaApp
        {
            get
            {
                return _luaApp;
            }
        }

        private void onEnter(Control scene)
        {
            _currentScene = scene;
            scene.Size = new Size(960, 640);
            _mainFrame.Controls.Add(scene);
            if (scene is SimC2DXScene)
            {
                (scene as SimC2DXScene).onEnter();
            }
        }

        private Control onExit()
        {
            Control c = _currentScene;
            if (c != null)
            {
                _mainFrame.Controls.Remove(c);
                if (c is SimC2DXScene)
                {
                    (c as SimC2DXScene).onExit();
                }
            }
            _currentScene = null;
            return c;
        }

        protected void runScene(Control scene)
        {
            onExit();
            onEnter(scene);
        }

        protected Control currentScene
        {
            get
            {
                return _currentScene;
            }
        }

        protected void pushScene(Control scene)
        {
            Control c = _currentScene;
            if(c!=null) {
                _mainFrame.Controls.Remove(c);
                _sceneStack.Push(c);
            }
            onEnter(scene);
        }

        protected Control popScene()
        {
            Control c = onExit();
            if (_sceneStack.Count > 0)
            {
                Control s = _sceneStack.Pop();
                _mainFrame.Controls.Add(s);
                _currentScene = s;
            }
            return c;
        }

        protected void popAll()
        {
            while (_currentScene!=null)
            {
                popScene();
            }
        }

        protected LuaValue apiCall(String name, IList<LuaValue> ps)
        {
            if (ps == null) ps = new List<LuaValue>();
            if (!luaApp.pcall(name, ps, ps))
            {
                String err = "Unknow Error";
                if (ps.Count > 0)
                {
                    err = ps[0].stringValue();
                }
                MessageBox.Show(err, "LUA ERROR");
                return null;
            }
            return ps.Count > 0 ? ps[0] : null;
        }

        public void endDirector()
        {
            popAll();
        }

        public virtual void startDirector()
        {
            
        }

        // LuaObject
        public override bool canInvoke(string name)
        {
            if(name==null)return false;
            String[] mlist = { "runScene","pushScene","canPopScene","popScene" };
            foreach (String n in mlist)
            {
                if (name.Equals(n))
                {
                    return true;
                }
            }
            return false;
        }

        public override bool invoke(LuaApp app, string name, IList<LuaValue> ctx)
        {
            if (name != null)
            {
                if (name.Equals("runScene"))
                {
                    if (ctx.Count == 0) throw new ArgumentException("miss param");
                    Object obj = ctx[0].objectValue();
                    if (obj == null || !(obj is Control))
                    {
                        throw new ArgumentException(obj+" not a Control");
                    }
                    bool bPopAll = false;
                    if (ctx.Count > 1)
                    {
                        bPopAll = ctx[1].booleanValue();
                    }
                    if (bPopAll)
                    {
                        popAll();
                    }
                    runScene(obj as Control);
                    ctx.Clear();
                    return true;
                }


                if (name.Equals("pushScene"))
                {
                    if (ctx.Count == 0) throw new ArgumentException("miss param");
                    Object obj = ctx[0].objectValue();
                    if (obj == null || !(obj is Control))
                    {
                        throw new ArgumentException(obj + " not a Control");
                    }                    
                    pushScene(obj as Control);
                    ctx.Clear();
                    return true;
                }

                if (name.Equals("popScene"))
                {
                    popScene();
                    ctx.Clear();
                    return true;
                }

                if (name.Equals("canPopScene"))
                {                    
                    ctx.Clear();
                    ctx.Add(LuaValue.booleanValue(_sceneStack.Count > 0));
                    return true;
                }
            }
            ctx.Clear();
            return false;
        }

        public override bool canGetValue(string name)
        {
            return name != null && name.Equals("scene");
        }

        public override LuaValue getValue(LuaApp app, string name)
        {
            if (name != null)
            {
                if (name.Equals("scene"))
                {
                    return LuaValue.objectValue(_currentScene);
                }
            }
            return LuaValue.nullValue();            
        }
    }
}
