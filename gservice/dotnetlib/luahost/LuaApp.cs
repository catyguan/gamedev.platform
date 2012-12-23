using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Windows.System.Threading;
using Windows.UI.Core;
using System.Threading.Tasks;
using System.Diagnostics;

namespace Lua
{
    public delegate bool LuaCall(LuaApp app, int callId, List<Object> ps);

    public class LuaException : Exception
    {
        public LuaException(String msg)
            : base(msg)
        {

        }
    }

    public class LuaApp : IDisposable
    {
        protected CoreDispatcher dispatcher;
        protected LuaState luaState;
        protected Dictionary<String, LuaCall> calls = new Dictionary<String, LuaCall>();

        public bool Open(CoreDispatcher disp)
        {
            if (IsOpen())
            {
                return true;
            }
            luaState = new LuaState();
            luaState.open();
            luaState.callback(new LuaCallback(this.handleCallback));
            this.dispatcher = disp;
            return true;
        }

        public void SetLoader(LuaLoader loader)
        {
            if (IsOpen())
            {
                luaState.loader(loader);
            }
        }

        public void setCall(String name, LuaCall call)
        {
            calls[name] = call;
        }

        public void setDebugPrintCall()
        {
            calls["print"] = (lua, callId, ps) =>
            {
                Debug.WriteLine(Dump(ps, "\t"));
                return true;
            };
        }

        public static bool LuaCallError(List<Object> r, String msg)
        {
            r.Clear();
            r.Add(msg);
            return false;
        }

        private bool handleCallback(LuaState luaState, List<object> data)
        {
            if(data.Count==0) {
                return LuaCallError(data, "invalid call type");
            }
            String name = (String) data[0];
            data.RemoveAt(0);
		    if (name.Equals("hostCall")) {
                if(data.Count<2) {
                    return LuaCallError(data, "invalid callId & methodName");
                }
			    int cid = (int) data[0];
                String method = (String) data[1];
                data.RemoveRange(0,2);			    
			    if (cid > 0) {				    				    
					LuaCall call = calls[method];
					if (call != null) {
						bool r = false;
						try {
							r = call(this, cid, data);
						} catch (Exception e) {
                            Debug.WriteLine("call fail "+e);
							return LuaCallError(data, e.Message);
						}
						if (r) {
                            data.Insert(0, true);
						} else {
                            data.Clear();
							data.Add(false);
						}
						return true;
					}				    
				    return LuaCallError(data, "invalid method '" + method + "'");
			    }
			    return LuaCallError(data, "invalid callId '" + cid + "'");
		    } else if (name.Equals("hostTimer")) {
                if (data.Count < 3)
                {
                    return LuaCallError(data, "invalid timerId,fix,delay");
                }
			    int tid = (int) data[0];
			    if (tid > 0) {                    
				    int fix = (int) data[1];
				    int delay = (int) data[2];
				    if (delay <= 0) {
					    return LuaCallError(data, "invalid timer delay");
				    }

                    Debug.WriteLine("setTimer - " + tid + ", " + fix + ", " + delay);
                    createTimer(tid, fix, delay);                                 
				    data.Clear();
				    return true;
			    }
			    return LuaCallError(data, "invalid timerId '" + tid + "'");
		    }
		    return LuaCallError(data, "invalid callType '" + name + "'");            
        }

        private void createTimer(int tid, int fix, int delay)
        {
            ThreadPoolTimer.CreateTimer(
                (timer) =>
                {
                    Windows.Foundation.IAsyncAction delayTask = dispatcher.RunAsync(CoreDispatcherPriority.High,
                        () =>
                        {
                            try
                            {
                                if (IsOpen())
                                {
                                    List<Object> rdata = new List<Object>();
                                    rdata.Add(tid);
                                    if (fix > 0)
                                    {
                                        rdata.Add(true);
                                    }
                                    bool r = luaState.pcall("luaTimerResponse", rdata);
                                    Debug.WriteLine("timer[" + tid + "] => " + r + "-" + Dump(rdata));
                                    if (r && fix > 0)
                                    {
                                        bool bv = (bool)rdata[0];
                                        if (bv)
                                        {
                                            createTimer(tid, fix, fix);
                                        }
                                        else
                                        {
                                            Debug.WriteLine("timer[" + tid + "] => stop fix timer");
                                        }
                                    }
                                }
                                else
                                {
                                    Debug.WriteLine("timer[" + tid + "] => invalid LuaState");
                                }
                            }
                            catch (Exception err)
                            {
                                Debug.WriteLine("timer[" + tid + "] error => " + err.Message);
                            }
                        });
                },
                TimeSpan.FromMilliseconds(delay));    
        }

        public bool IsOpen()
        {
            return luaState != null;
        }

        public void Close()
        {
            if (IsOpen())
            {
                luaState.callback(null);
                luaState.close();
                luaState = null;
            }
        }

        void IDisposable.Dispose()
        {
            Close();
        }

        public void Addpath(String path)
        {
            if (IsOpen())
            {
                luaState.addpath(path);
            }
        }

        public void Setvar(String key, String value)
        {
            if (IsOpen())
            {
                luaState.setvar(key, value);
            }
        }

        public void Eval(String content)
        {
            if (IsOpen())
            {
                String r = luaState.eval(content);
                if (r != null)
                {
                    throw new LuaException(r);
                }
            }
            else
            {
                throw new LuaException("LuaState not open");
            }
        }

        public bool Pcall(String fun, List<Object> data)
        {
            return Pcall(fun, data, true);
        }

        public bool Pcall(String fun, List<Object> data, bool throwError)
        {
            if (IsOpen())
            {
                bool r = luaState.pcall(fun, data);
                if (!r)
                {
                    if (throwError && data.Count>1)
                    {
                        throw new LuaException(data[0].ToString());
                    }                    
                }
                return r;
            }
            else
            {
                throw new LuaException("LuaState not open");
            }
        }

        public bool Create(String id, String appType, List<String> pathList, List<String> bootstrapList, CoreDispatcher disp,LuaLoader loader)
        {
            Open(disp);
            if (loader != null)
            {
                SetLoader(loader);
            }

            Setvar("LUA_APP_ID", id);
			Setvar("LUA_APP_TYPE", appType);

            if(pathList!=null) {
                foreach(String path in pathList) {
                    Addpath(path);
				}
			}

			if(bootstrapList!=null) {
				foreach (String boot in bootstrapList) {
					List<Object> data = new List<Object>();
					data.Add(boot);
					Pcall("require", data, false);
					Debug.WriteLine("boot("+boot+") => "+Dump(data));					
				}
			}

            return true;
        }

        public void Shutdown(List<String> shutdownList)
        {
            if (shutdownList != null)
            {
                foreach(String s in shutdownList) {
                    List<Object> data = new List<Object>();
					data.Add(s);
					Pcall("require", data, false);
                }
            }
            Close();
        }

        public static String Dump(Dictionary<String, Object> t)
        {
            return Dump(t, ",");
        }

        public static String Dump(Dictionary<String, Object> t,String sp)
        {
            if (t == null) return null;
            StringBuilder sb = new StringBuilder(128);
            foreach (KeyValuePair<String,Object> kv in t)
            {
                if (sb.Length > 0)
                {
                    sb.Append(sp);
                }
                sb.Append(kv.Key).Append("=");
                Object v = kv.Value;
                if (v == null)
                {
                    sb.Append("nil");
                    continue;
                }
                if (v is List<Object>)
                {
                    sb.Append(Dump((List<Object>)v, sp));
                    continue;
                }
                if (v is Dictionary<String, Object>)
                {
                    sb.Append(Dump((Dictionary<String, Object>)v, sp));
                    continue;
                }
                sb.Append(v);
            }
            return sb.ToString();
        }

        public static String Dump(List<Object> ps)
        {
            return Dump(ps, ",");
        }

        public static String Dump(List<Object> ps,String sp)
        {
            if(ps==null)return null;
            StringBuilder sb = new StringBuilder(128);
            foreach (Object v in ps)
            {
                if (sb.Length > 0)
                {
                    sb.Append(sp);
                }
                if (v == null)
                {
                    sb.Append("nil");
                    continue;
                }
                if (v is List<Object>)
                {
                    List<Object> l = (List<Object>)v;
                    sb.Append("L").Append(l.Count).Append("[");
                    sb.Append(Dump(l, sp));
                    sb.Append("]");
                    continue;
                }
                if (v is Dictionary<String, Object>)
                {
                    Dictionary<String, Object> t = (Dictionary<String, Object>)v;
                    sb.Append("T").Append(t.Count).Append("{");
                    sb.Append(Dump(t, sp));
                    sb.Append("}");
                    continue;
                }
                sb.Append(v);
            }
            return sb.ToString();
        }

        public static List<Object> Copy(List<Object> ps)
        {
            return null;
        }
    }
}
