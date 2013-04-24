// luanet.h

#pragma once

#include <vcclr.h>

extern "C"
{
	struct lua_State;
};

using namespace System;

namespace luanet {
	ref class LuaValue;

	public class Iterator
	{
	public:
		Iterator(Iterator& o) {
			m_enumerator = o.m_enumerator;
			m_valid = o.m_valid;
		}
		Iterator(System::Collections::IEnumerator^ it) {
			m_enumerator = it;
			m_valid = it->MoveNext();
		}
		~Iterator() {}

		bool isValid() {
			return m_valid;
		}

		bool next() {
			m_valid = m_enumerator->MoveNext();
			return m_valid;
		}

		Object^ value() {
			return m_enumerator->Current;
		}

	private:
		gcroot<System::Collections::IEnumerator^> m_enumerator;
		bool m_valid;
	};
}
typedef luanet::LuaValue^ LuaHostValue;
typedef luanet::LuaValue^ LuaHostValue_Ref;
typedef System::Collections::Generic::IList<luanet::LuaValue^>^ LuaHostArray;
typedef System::Collections::Generic::IList<luanet::LuaValue^>^ LuaHostArray_Ref;
typedef luanet::Iterator LuaHostArrayIterator;
typedef luanet::Iterator& LuaHostArrayIterator_Ref;
typedef System::Collections::Generic::Dictionary<String^, luanet::LuaValue^>^ LuaHostMap;
typedef System::Collections::Generic::Dictionary<String^, luanet::LuaValue^>^ LuaHostMap_Ref;
typedef luanet::Iterator LuaHostMapIterator;
typedef luanet::Iterator& LuaHostMapIterator_Ref;

class LuaHost;

namespace luanet {

	enum class LuaValueType { 
		Null,
		Int,
		Number,
		Boolean,
		String,
		Map,
		Array,
		Object,
		ObjectCall, 
	};

	// A lua variable
	public ref class LuaValue 
	{
	protected:
		LuaValue();
		~LuaValue();

	public:
		static LuaValue^ nullValue();
		static LuaValue^ intValue(int v);
		static LuaValue^ numberValue(double v);
		static LuaValue^ booleanValue(bool v);
		static LuaValue^ stringValue(char* v);
		static LuaValue^ stringValue(String^ v);
		static LuaValue^ mapValue( LuaHostMap_Ref v);
		static LuaValue^ arrayValue(LuaHostArray_Ref v);
		static LuaValue^ objectValue(Object^ v);
		static LuaValue^ ocallValue(Object^ v, String^ name);

		LuaValueType getType() {
			return m_type;
		}
		
		bool isNull() {
			return m_type==LuaValueType::Null;
		}

		bool isInt() {
			return m_type==LuaValueType::Int;
		}
		int intValue() {
			if(m_type==LuaValueType::Int) {
				return m_valueInt;
			}
			if(m_type==LuaValueType::Number) {
				return (int) m_valueNumber;
			}
			return 0;
		}

		bool isNumber() {
			return m_type==LuaValueType::Number;
		}
		float floatValue() {
			if(m_type==LuaValueType::Int) {
				return (float) m_valueInt;
			}
			if(m_type==LuaValueType::Number) {
				return (float) m_valueNumber;
			}
			return 0;
		}
		double numberValue() {
			if(m_type==LuaValueType::Int) {
				return m_valueInt;
			}
			if(m_type==LuaValueType::Number) {
				return m_valueNumber;
			}
			return 0;
		}

		bool isBoolean() {
			return m_type==LuaValueType::Boolean;
		}

		bool booleanValue() {
			if(m_type==LuaValueType::Boolean || m_type==LuaValueType::Int) {
				return m_valueInt!=0;
			}
			if(m_type==LuaValueType::Number) {
				return m_valueNumber!=0;
			}		
			return false;
		}

		bool isString() {
			return m_type==LuaValueType::String;
		}
		String^ stringValue() {
			switch (m_type)
			{
			case LuaValueType::Null:
				return "<null>";
			case LuaValueType::Int:
				return String::Format("{0}", m_valueInt);
			case LuaValueType::Number:
				return String::Format("{0}", m_valueNumber);
			case LuaValueType::Boolean:
				return String::Format("{0}", m_valueInt!=0);
			case LuaValueType::String:
				return m_valueString;
			default:
				break;
			}
			return "";
		}
		
		bool isMap() {
			return m_type==LuaValueType::Map;
		}
		LuaHostMap_Ref mapValue() {
			if(isMap()) {
				return m_valueMap;
			}
			return nullptr;
		}

		bool isArray() {
			return m_type==LuaValueType::Array;
		}
		LuaHostArray_Ref arrayValue() {
			if(isArray()) {
				return m_valueArray;
			}
			return nullptr;
		}

		bool isObject() {
			return m_type==LuaValueType::Object;
		}
		Object^ objectValue() {
			if(isObject()) {
				return m_valueObject;
			}
			return nullptr;
		}

		bool isObjectCall() {
			return m_type==LuaValueType::ObjectCall;
		}
		Object^ ocObject() {
			if(isObjectCall()) {
				return m_valueObject;
			}
			return nullptr;
		}
		String^ ocName() {
			if(isObjectCall()) {
				return m_valueString;
			}
			return nullptr;
		}

		void copy(LuaValue^ v) {
			m_type = v->m_type;
			m_valueInt = v->m_valueInt;
			m_valueNumber = v->m_valueNumber;
			m_valueString = v->m_valueString;
			m_valueMap = v->m_valueMap;
			m_valueArray = v->m_valueArray;
			m_valueObject = v->m_valueObject;
		}

	private:		
		LuaValueType m_type;

		int m_valueInt;
		double m_valueNumber;
		String^ m_valueString;
		LuaHostMap_Ref m_valueMap;
		LuaHostArray_Ref m_valueArray;
		Object^ m_valueObject;
	};

	public ref class LuaError : public System::Exception
	{
	public:
		LuaError(String^ msg) : Exception(msg){};
		~LuaError(){};
	};

	ref class LuaApp;
	public interface class LuaLoader
	{
	public:
		String^ load(LuaApp^ app, String^ name);
	};
	public interface class LuaCall
	{
	public:
		bool call(LuaApp^ app, int callId, LuaHostArray_Ref ctx);
	};
	public interface class LuaObject
	{
	public:
		bool canCall();
		bool canInvoke(String^ name);
		bool call(LuaApp^ app, LuaHostArray_Ref ctx);
		bool invoke(LuaApp^ app, String^ name, LuaHostArray_Ref ctx);
		void discard(LuaApp^ app);
	};
	
	public ref class LuaApp
	{
	protected:
		LuaApp();
		virtual ~LuaApp();
		!LuaApp(){close();};

	public:    
		void open();
		bool isOpen();
		void close();

		void addpath(String^ path);
		void setvar(String^ key, String^ value);
		bool pcall(String^ fun, LuaHostArray_Ref data, LuaHostArray_Ref result);
		String^ eval(String^ content);

		void setLoader(LuaLoader^ loader);
		void setCall(String^ name, LuaCall^ call);	

		bool callResponse(int callId, String^ err, LuaHostArray_Ref params);
		bool closureCall(int callId, LuaHostArray_Ref params);
		void closureRemove(int callId);

		void enablePrintLog();
		void require(String^ package);

		void createApp(String^ appType, 
			System::Collections::Generic::IList<String^>^ pathList, 
			System::Collections::Generic::IList<String^>^ bootstrapList);

	protected:
		LuaLoader^ m_loader;	
		System::Collections::Generic::Dictionary<String^,LuaCall^>^ m_calls;
	
		LuaHost* m_host;		
	};

	ref class LuaAppRealm;
	public interface class TimerHelper
	{
	public:
		bool crreateTimer(LuaAppRealm^ app, int timerId, int delayTime,int fixTime);
	};

	public ref class LuaAppRealm : public LuaApp
	{
	public:
		LuaAppRealm(){};
		virtual ~LuaAppRealm(){};

		TimerHelper^ timer;

		String^ lapi_load(String^ name);
		bool lapi_invoke(String^ method, int callId, LuaHostArray_Ref params);
		int lapi_doTimer(int timerId, int fixTime);
	};

	public ref class LuaObjectClosure : public LuaObject
	{
	public:
		LuaObjectClosure(int callId){
			m_callId = callId;
		};
		~LuaObjectClosure(){};

		virtual bool canCall() {return true;};
		virtual bool canInvoke(String^ name){return false;};
		virtual bool call(LuaApp^ app, LuaHostArray_Ref ctx) {
			return app->closureCall(m_callId, ctx);
		}
		virtual bool invoke(LuaApp^ app, String^ name, LuaHostArray_Ref ctx) {
			return false;
		}
		virtual void discard(LuaApp^ app) {
			app->closureRemove(m_callId);
		}

		int getCallId() {
			return m_callId;
		}

	protected:
		int m_callId;
	};
}
