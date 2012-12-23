using System;
using System.Collections;
using System.Collections.Generic;
using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using System.Diagnostics;
using Lua;

namespace UnitTest4luahost
{   

    [TestClass]
    public class luanet
    {
        static public bool print2(LuaState L,List<Object> ps)
        {            
            String msg = "";
            foreach(Object v in ps) {
                if(msg!="")msg += "\t";
                msg += v;
            }
            Debug.WriteLine(msg);
            return true;
        }

        [TestMethod]
        public void openClose()
        {
            LuaState L = new LuaState();
            L.open();
            L.close();
        }

        public static List<Object> buildAllType()
        {
            List<Object> p1 = new List<Object>();
            p1.Add(true);
            p1.Add(100);
            p1.Add(null);
            p1.Add(24.3);
            p1.Add("string");

            Dictionary<String, Object> table = new Dictionary<string, object>();
            table.Add("k1", "v1");
            table.Add("k2", 123);
            table.Add("k3", true);
            table.Add("k4", null);

            Dictionary<String, Object> t2 = new Dictionary<String, Object>();
            t2.Add("kk", 234);

            List<Object> a1 = new List<Object>();
            a1.Add(true);
            a1.Add(t2);

            table.Add("k5", a1);
            p1.Add(table);
            return p1;
        }

        [TestMethod]
        public void full()
        {
            LuaState L = new LuaState();
            L.open();

            L.callback(print2);
            L.eval("print = dotnet.call");
            L.eval("function printall(...)\nlocal json = require(\"cjson\")\nprint(json.encode({...}))\nend");
            L.eval("function mulret()\nlocal t1 = {k1=\"v1\",k2=123,k3=true}\nlocal t2 = {true,t1}\nreturn true,100,nil,24.3,\"bye\",t2\nend");
            L.eval("function error_fun()\nerror(\"error test\")\nend");

            L.addpath("e:/tmp/");
		    L.setvar("env", "EnvString");

            
		    Debug.WriteLine("eval-R:" + L.eval("print(\"eval-string 中文 \"..env)"));

            List<Object> p1 = buildAllType();

            Debug.WriteLine("pcall-R:" + L.pcall("printall", p1));
            Debug.WriteLine("result:" + LuaApp.Dump(p1));

            p1.Clear();
            Debug.WriteLine("mulret >> " + L.pcall("mulret", p1) + "-" + LuaApp.Dump(p1));

		    p1.Clear();
		    Debug.WriteLine("error >> " + L.pcall("error_fun", p1));
            Debug.WriteLine("message:" + LuaApp.Dump(p1));

            L.close();
        }
    }

    [TestClass]
    public class luaapp
    {
        static public LuaApp create()
        {
            LuaApp app = new LuaApp();

            app.setCall("say", (lua, callId, ps) => {
				    Debug.WriteLine("LuaCall say => {}", LuaApp.Dump(ps));
				    return true;
			    });
		    app.setCall("asyn", (lua, callId, ps) => {
				    Debug.WriteLine("LuaCall asyn => "+LuaApp.Dump(ps));
				    List<Object> rdata = LuaApp.Copy(ps);;
				    //main.execute(null, new Runnable() {
				    // log.info("LuaCall asyn do => {}/{}", rdata,
				    // System.identityHashCode(rdata));
				    // app.luaCallResponse(callId, null, rdata);
				    // }
				    // });
				    return false;			
		        });
            app.setDebugPrintCall();
            
            String dir = "D:\\workroom\\gamedev.platform\\lua.libs\\";
            List<String> pathList = new List<string>();
            pathList.Add(dir+"src");
            pathList.Add(dir + "test");

            List<String> bootstrapList = new List<String>();
            bootstrapList.Add("bma.host.dotnet.DebugPrint");
            
            app.Create("test", "test",pathList, bootstrapList,null,LuaLoader);
            return app;
        }

        public static String LuaLoader(LuaState luaState, String name)
        {
            if (name.Equals("bma.host.dotnet.DebugPrint"))
            {
                return "print = function(...)\ndotnet.call(\"hostCall\",1,\"print\",...)\nend";
            }
            return null;
        }

        [TestMethod]
        public void startstop()
        {            
            LuaApp app = create();
            app.Eval("print(1234)");
            app.Close();
        }
    }
}
