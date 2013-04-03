#ifndef _THRIFT_SCHEME_H_
#define _THRIFT_SCHEME_H_ 1

#include "cocos2d.h"
#include "../Thrift.h"
#include "TProtocol.h"

namespace apache { namespace thrift { namespace protocol {

#define SCHEME_TYPE_STRUCT	0
#define SCHEME_TYPE_ARGS	1
#define SCHEME_TYPE_RETURN	2


class TSchemeService
{
public:
	static TScheme* findScheme(std::string module, std::string name, int type);
};

class TSchemeField
{
public:
	int16_t id;
	std::string name;
	TType type;
	std::string refType;
	bool optional;
};

class TSchemeBase : public TScheme
{
public:
	TSchemeBase(std::string module, std::string name, int type);
	virtual ~TSchemeBase(){};

	virtual uint32_t read(protocol::TProtocol* iprot, cocos2d::CCValueBuilder* vbuilder);
	virtual uint32_t write(protocol::TProtocol* oprot, cocos2d::CCValue* value);

	const TSchemeField* getField(int fid);
	const TSchemeField* getField(std::string name);

	TSchemeBase* field(int16_t id, const char* name, TType type, const char* refType, bool optional);

protected:
	uint32_t readField(TProtocol* iprot, cocos2d::CCValueBuilder* builder, const TSchemeField* field);

	uint32_t readString(TProtocol* iprot, cocos2d::CCValueBuilder* builder, const char* name);
	uint32_t readBinary(TProtocol* iprot, cocos2d::CCValueBuilder* builder, const char* name);
	uint32_t readBool(TProtocol* iprot, cocos2d::CCValueBuilder* builder, const char* name);
	uint32_t readByte(TProtocol* iprot, cocos2d::CCValueBuilder* builder, const char* name);
	uint32_t readI16(TProtocol* iprot, cocos2d::CCValueBuilder* builder, const char* name);
	uint32_t readI32(TProtocol* iprot, cocos2d::CCValueBuilder* builder, const char* name);
	uint32_t readI64(TProtocol* iprot, cocos2d::CCValueBuilder* builder, const char* name);
	uint32_t readDouble(TProtocol* iprot, cocos2d::CCValueBuilder* builder, const char* name);

	bool isset(cocos2d::CCValue* value, const char* name);
	const cocos2d::CCValue* prop(cocos2d::CCValue* value, const char* name);
	uint32_t writeField(TProtocol* oprot, cocos2d::CCValue* value, const TSchemeField* field);

	uint32_t writeString(TProtocol* oprot, cocos2d::CCValue* value, const char* name);
	uint32_t writeBinary(TProtocol* oprot, cocos2d::CCValue* value, const char* name);
	uint32_t writeBool(TProtocol* oprot, cocos2d::CCValue* value, const char* name);
	uint32_t writeByte(TProtocol* oprot, cocos2d::CCValue* value, const char* name);
	uint32_t writeI16(TProtocol* oprot, cocos2d::CCValue* value, const char* name);
	uint32_t writeI32(TProtocol* oprot, cocos2d::CCValue* value, const char* name);
	uint32_t writeI64(TProtocol* oprot, cocos2d::CCValue* value, const char* name);
	uint32_t writeDouble(TProtocol* oprot, cocos2d::CCValue* value, const char* name);

protected:
	std::string m_module;
	std::string m_name;
	int m_type;
	TSchemeBase* m_nextScheme;

	std::list<TSchemeField> m_fields;

	friend class TSchemeService;
};

class TSchemeResult : public TSchemeBase {
public:
	TSchemeResult(std::string module, std::string name, TType type, const char* refType) 
		: TSchemeBase(module,name,SCHEME_TYPE_RETURN) {
			if(type!=T_VOID) {
				this->field(0,"success",type,refType,true);
			}
	}
};

}}} // apache::thrift::protocol

#endif // #define _THRIFT_SCHEME_H_
