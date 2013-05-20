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
        void onActive();
        void onExit();
    }
    public interface SimC2DXSceneMaster
    {
        Control control
        {
            get;
        }
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
        Object _currentScene;
        Stack<Object> _sceneStack = new Stack<Object>();
        internal LuaApp _luaApp;

        public LuaApp luaApp
        {
            get
            {
                return _luaApp;
            }
        }

        protected Control control(Object s)
        {
            if (s is Control) return s as Control;
            if (s is SimC2DXSceneMaster) return (s as SimC2DXSceneMaster).control;
            return null;
        }

        private void onEnter(Object scene)
        {
            _currentScene = scene;
            Control s = control(scene);
            s.Dock = DockStyle.Fill;
            s.Size = new Size(960, 640);
            _mainFrame.Controls.Add(s);
            if (scene is SimC2DXScene)
            {
                (scene as SimC2DXScene).onEnter();
            }
            s.Focus();
        }

        private Control onExit()
        {
            Control c = control(_currentScene);
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

        protected void runScene(Object scene)
        {
            onExit();
            onEnter(scene);
        }

        protected Object currentScene
        {
            get
            {
                return _currentScene;
            }
        }

        protected void pushScene(Object scene)
        {
            Control c = control(_currentScene);
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
                Object o = _sceneStack.Pop();
                Control s = control(o);
                _mainFrame.Controls.Add(s);
                s.Focus();
                _currentScene = o;

                if (o is SimC2DXScene)
                {
                    (o as SimC2DXScene).onActive();
                }
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

        public LuaValue apiCall(String name, IList<LuaValue> ps)
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
                return LuaValue.nullValue();
            }
            return ps.Count > 0 ? ps[0] : LuaValue.nullValue();
        }

        public bool apiStartup()
        {
            LuaValue r = apiCall("_API_application_startup", null);
            return r.booleanValue();
        }

        public void apiShutdown()
        {
            apiCall("_API_application_shutdown", null);
        }

        public LuaValue apiObjectCall(String name, IList<LuaValue> ps)
        {
            if (ps == null) ps = new List<LuaValue>();
            ps.Insert(0, LuaValue.stringValue(name));
            return apiCall("_API_application_objectcall", ps);
        }
        public LuaValue apiObjectCall(String name)
        {
            var ps = new List<LuaValue>();
            ps.Add(LuaValue.stringValue(name));
            return apiCall("_API_application_objectcall", ps);
        }
        public LuaValue apiObjectCall(String name, LuaValue p1)
        {
            var ps = new List<LuaValue>();
            ps.Add(LuaValue.stringValue(name));
            ps.Add(p1);
            return apiCall("_API_application_objectcall", ps);
        }
        public LuaValue apiObjectCall(String name, LuaValue p1,LuaValue p2)
        {
            var ps = new List<LuaValue>();
            ps.Add(LuaValue.stringValue(name));
            ps.Add(p1);
            ps.Add(p2);
            return apiCall("_API_application_objectcall", ps);
        }
        public LuaValue apiObjectCall(String name, LuaValue p1, LuaValue p2, LuaValue p3)
        {
            var ps = new List<LuaValue>();
            ps.Add(LuaValue.stringValue(name));
            ps.Add(p1);
            ps.Add(p2);
            ps.Add(p3);
            return apiCall("_API_application_objectcall", ps);
        }

        public LuaValue apiDataGet(String name)
        {
            var ps = new List<LuaValue>();
            ps.Add(LuaValue.stringValue(name));
            return apiCall("_API_application_dataget", ps);
        }
        public bool apiDataGet(String name, LuaValue val)
        {
            var ps = new List<LuaValue>();
            ps.Add(LuaValue.stringValue(name));
            ps.Add(val);
            LuaValue r = apiCall("_API_application_dataset", ps);
            return r.booleanValue();
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
