using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using luanet;
using System.Windows.Forms;
using bmalangutil.forms;

namespace simC2DX.core
{
    class LuaTimer : TimerHelper
    {
        struct Info
        {
            public LuaAppRealm app;
            public int timerId;
            public int fixTime;
        }

        public bool crreateTimer(LuaAppRealm app, int timerId, int delayTime, int fixTime)
        {
            Timer timer = new Timer();
            timer.Interval = delayTime;
            timer.Tick += timer_Tick;
            Info info = new Info();
            info.app = app;
            info.timerId = timerId;
            info.fixTime = fixTime;
            timer.Tag = info;
            timer.Start();
            return true;
        }

        void timer_Tick(object sender, EventArgs e)
        {
            Timer timer = (Timer)sender;
            Info info = (Info)timer.Tag;
            int r = info.app.lapi_doTimer(info.timerId, info.fixTime);
            if (r <= 0)
            {
                timer.Stop();
            }
            else
            {
                if (timer.Interval != r)
                {
                    timer.Interval = r;
                }
            }
        }

    }

    public class SimC2DXApp
    {
        LuaAppRealm luaRealm;

        protected LuaAppRealm createLuaAppRealm()
        {
            return new LuaAppRealm();
        }

        public void run()
        {
            Form form = new SimC2DXFrame();
            AppThread.instance.start(form.Handle);

            // create luaRealm            
            createLuaRealm();

            SimC2DXDirector director = createDirector();
            director._luaApp = this.luaRealm;
            director._mainFrame = form;
            SimC2DXDirector._instance = director;

            form.Load += mainform_Load;

            try
            {
                Application.Run(form);
            }
            finally
            {
                director.endDirector();
                clear();
            }
        }

        private void mainform_Load(object sender, EventArgs e)
        {
            SimC2DXDirector._instance.startDirector();
        }

        private void clear()
        {
            if (luaRealm != null)
            {
                luaRealm.close();
            }
        }

        private List<String> _pathList = new List<String>();
        public void addPath(String path)
        {
            _pathList.Add(path);
        }

        private List<String> _bootList = new List<String>();
        public void addBoot(String path)
        {
            _bootList.Add(path);
        }

        public String appType
        {
            get
            {
                return _appType;
            }
            set
            {
                _appType = value;
            }
        }
        private String _appType;

        private void createLuaRealm()        
        {
            luaRealm = createLuaAppRealm();
            luaRealm.timer = new LuaTimer();

            luaRealm.open();
            luaRealm.enablePrintLog();

            luaRealm.createApp(_appType, _pathList, _bootList);

            initLuaRealm(luaRealm);
        }

        protected virtual void initLuaRealm(LuaAppRealm lua)
        {
        }

        protected virtual SimC2DXDirector createDirector()
        {
            return new SimC2DXDirector();
        }
    }
}
