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

    public class SimC2DXDirector
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

        protected void runScene(Control scene)
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
            _currentScene = scene;
            scene.Size = new Size(960, 640);
            
            _mainFrame.Controls.Add(scene);
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
        }

        protected Control popScene()
        {
            return null;
        }

        protected void popAll()
        {

        }

        public void endDirector()
        {
            popAll();
        }

        public virtual void startDirector()
        {
            
        }
        
        
    }
}
