#include "TScheme.h"

namespace apache { namespace thrift { namespace protocol {

// TSchemeService
TSchemeBase* g_scheme = NULL;

TScheme* TSchemeService::findScheme(std::string module, std::string name, bool args)
{
	TSchemeBase* s = g_scheme;
	while(s!=NULL) {
		if((args?1:0) == (s->m_args?1:0)
			&& s->m_module.compare(module)==0
			&& s->m_name.compare(name)==0) {
			return s;
		}
		s = s->m_nextScheme;
	}
	return NULL;
}

// TSchemeBase
TSchemeBase::TSchemeBase(std::string m,std::string n,bool args)
{
	m_module = m;
	m_name = n;
	m_args = args;

	m_nextScheme = g_scheme;
	g_scheme = this;
}

uint32_t TSchemeBase::readString(TProtocol* iprot, cocos2d::CCValueBuilder* builder, const char* name)
{
	std::string val;
	uint32_t r = iprot->readString(val);
	builder->beString(val);
	builder->addMap(name);
	return r;
}

bool TSchemeBase::isset(cocos2d::CCValue* value, const char* name)
{
	if(value->isMap()) {
		cocos2d::CCValueMap* map = value->mapValue();
		if(map!=NULL) {
			return map->find(name)!=map->end();
		}
	}
	return false;
}

uint32_t TSchemeBase::writeString(TProtocol* oprot, cocos2d::CCValue* value, const char* name)
{
	if(value->isMap()) {		
		return oprot->writeString(value->mapValue()->at(name).stringValue());
	} else {
		return oprot->writeString("");
	}
}

}}} // apache::thrift::protocol
