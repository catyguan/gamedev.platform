#ifndef _THRIFT_SCHEME_H_
#define _THRIFT_SCHEME_H_ 1

#include "cocos2d.h"
#include "../Thrift.h"
#include "TProtocol.h"

namespace apache { namespace thrift { namespace protocol {

class TSchemeService
{
public:
	static TScheme* findScheme(std::string module, std::string name, bool args);
};

class TSchemeBase : public TScheme
{
public:
	TSchemeBase(std::string module, std::string name, bool args);
	virtual ~TSchemeBase(){};

protected:
	uint32_t readString(TProtocol* iprot, cocos2d::CCValueBuilder* builder, const char* name);

	bool isset(cocos2d::CCValue* value, const char* name);
	uint32_t writeString(TProtocol* oprot, cocos2d::CCValue* value, const char* name);

protected:
	std::string m_module;
	std::string m_name;
	bool m_args;
	TSchemeBase* m_nextScheme;

	friend class TSchemeService;
};

}}} // apache::thrift::protocol

#endif // #define _THRIFT_SCHEME_H_
