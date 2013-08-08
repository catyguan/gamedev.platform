#include "TScheme.h"

namespace apache { namespace thrift { namespace protocol {

// TSchemeService
TSchemeBase* g_scheme = NULL;

TScheme* TSchemeService::findScheme(std::string module, std::string name, int type)
{
	TSchemeBase* s = g_scheme;
	while(s!=NULL) {
		if(type==s->m_type
			&& s->m_module.compare(module)==0
			&& s->m_name.compare(name)==0) {
			return s;
		}
		s = s->m_nextScheme;
	}
	return NULL;
}

// TSchemeBase
TSchemeBase::TSchemeBase(std::string m,std::string n,int type)
{
	m_module = m;
	m_name = n;
	m_type = type;

	m_nextScheme = g_scheme;
	g_scheme = this;
}

TSchemeBase* TSchemeBase::field(int16_t id, const char* name, TType type, const char* refType, bool optional)
{
	TSchemeField f;
	f.id = id;
	f.name =name;
	f.type = type;
	if(refType!=NULL) {
		f.refType = refType;
	}
	f.optional = optional;
	m_fields.push_back(f);
	return this;
}

const TSchemeField* TSchemeBase::getField(int fid)
{
	std::list<TSchemeField>::const_iterator it = m_fields.begin();
	while(it!=m_fields.end()) {
		if(it->id==fid)return it.operator->();
		it++;
	}
	return NULL;
}

const TSchemeField* TSchemeBase::getField(std::string name)
{
	std::list<TSchemeField>::const_iterator it = m_fields.begin();
	while(it!=m_fields.end()) {
		if(it->name.compare(name)==0)return it.operator->();
		it++;
	}
	return NULL;
}

uint32_t TSchemeBase::read(::apache::thrift::protocol::TProtocol* iprot, cocos2d::CCValueBuilder* builder) {

  uint32_t xfer = 0;
  std::string fname;
  ::apache::thrift::protocol::TType ftype;
  int16_t fid;

  xfer += iprot->readStructBegin(fname);
  builder->mapBegin();

  using ::apache::thrift::protocol::TProtocolException;

  while (true)
  {
    xfer += iprot->readFieldBegin(fname, ftype, fid);
    if (ftype == ::apache::thrift::protocol::T_STOP) {
      break;
    }
	const TSchemeField* field = getField(fid);
	if(field!=NULL && ftype==field->type) {
		xfer += readField(iprot,builder,field);
	} else {
        xfer += iprot->skip(ftype);
        break;
    }
    xfer += iprot->readFieldEnd();
  }

  xfer += iprot->readStructEnd();
  builder->mapEnd();

  return xfer;
}

uint32_t TSchemeBase::write(::apache::thrift::protocol::TProtocol* oprot, cocos2d::CCValue* value) {
  std::string n = m_module+"_"+m_name;
  if(m_type==SCHEME_TYPE_ARGS) {
	  n+="_args";
  } else if(m_type==SCHEME_TYPE_RETURN) {
	  n+="_result";
  }
  uint32_t xfer = 0;
  xfer += oprot->writeStructBegin(n.c_str());
  std::list<TSchemeField>::const_iterator it = m_fields.begin();
  while(it!=m_fields.end()) {
	if(!it->optional || isset(value, it->name.c_str())) {
		xfer += oprot->writeFieldBegin(it->name.c_str(), it->type, it->id);		
		xfer += writeField(oprot, value, it.operator->());
		xfer += oprot->writeFieldEnd();
	}
	it++;
  }
  xfer += oprot->writeFieldStop();
  xfer += oprot->writeStructEnd();
  return xfer;
}

uint32_t TSchemeBase::readField(TProtocol* iprot, cocos2d::CCValueBuilder* builder, const TSchemeField* field)
{
	switch(field->type) {		
		case T_BOOL: {
			return readBool(iprot,builder,field->name.c_str());
		} break;
		case T_BYTE : {
			return readByte(iprot,builder,field->name.c_str());
		} break;
		case T_DOUBLE: {
			return readDouble(iprot,builder,field->name.c_str());
		} break;
		case T_I16: {
			return readI16(iprot,builder,field->name.c_str());
		} break;
		case T_I32: {
			return readI32(iprot,builder,field->name.c_str());
		} break;
		case T_I64: {
			return readI64(iprot,builder,field->name.c_str());
		} break;
		case T_LIST:
		case T_MAP:
		case T_SET: {			
		} break;
		case T_STRING: {
			return readString(iprot,builder,field->name.c_str());
		} break;
		case T_STRUCT: {			
		} break;
	}
	return iprot->skip(field->type);
}

uint32_t TSchemeBase::readString(TProtocol* iprot, cocos2d::CCValueBuilder* builder, const char* name)
{
	std::string val;
	uint32_t r = iprot->readString(val);
	builder->beString(val);
	builder->addMap(name);
	return r;
}

uint32_t TSchemeBase::readBinary(TProtocol* iprot, cocos2d::CCValueBuilder* builder, const char* name)
{
	std::string val;
	uint32_t r = iprot->readBinary(val);
	builder->beString(val);
	builder->addMap(name);
	return r;
}

uint32_t TSchemeBase::readBool(TProtocol* iprot, cocos2d::CCValueBuilder* builder, const char* name)
{
	bool val;
	uint32_t r = iprot->readBool(val);
	builder->beBoolean(val);
	builder->addMap(name);
	return r;
}

uint32_t TSchemeBase::readByte(TProtocol* iprot, cocos2d::CCValueBuilder* builder, const char* name)
{
	int8_t val;
	uint32_t r = iprot->readByte(val);
	builder->beInt(val);
	builder->addMap(name);
	return r;
}

uint32_t TSchemeBase::readI16(TProtocol* iprot, cocos2d::CCValueBuilder* builder, const char* name)
{
	int16_t val;
	uint32_t r = iprot->readI16(val);
	builder->beInt(val);
	builder->addMap(name);
	return r;
}

uint32_t TSchemeBase::readI32(TProtocol* iprot, cocos2d::CCValueBuilder* builder, const char* name)
{
	int32_t val;
	uint32_t r = iprot->readI32(val);
	builder->beInt(val);
	builder->addMap(name);
	return r;
}

uint32_t TSchemeBase::readI64(TProtocol* iprot, cocos2d::CCValueBuilder* builder, const char* name)
{
	int64_t val;
	uint32_t r = iprot->readI64(val);
	builder->beNumber((double) val);
	builder->addMap(name);
	return r;
}

uint32_t TSchemeBase::readDouble(TProtocol* iprot, cocos2d::CCValueBuilder* builder, const char* name)
{
	double val;
	uint32_t r = iprot->readDouble(val);
	builder->beNumber(val);
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

const cocos2d::CCValue* TSchemeBase::prop(cocos2d::CCValue* value, const char* name)
{
	if(value->isMap()) {
		cocos2d::CCValueMap* map = value->mapValue();
		cocos2d::CCValueMapIterator it = map->find(name);
		if(it!=map->end()) {
			return &(it->second);
		}		
	}
	return NULL;
}

uint32_t TSchemeBase::writeField(TProtocol* iprot, cocos2d::CCValue* value, const TSchemeField* field)
{
	switch(field->type) {		
		case T_BOOL: {
			return writeBool(iprot,value,field->name.c_str());
		} break;
		case T_BYTE : {
			return writeByte(iprot,value,field->name.c_str());
		} break;
		case T_DOUBLE: {
			return writeDouble(iprot,value,field->name.c_str());
		} break;
		case T_I16: {
			return writeI16(iprot,value,field->name.c_str());
		} break;
		case T_I32: {
			return writeI32(iprot,value,field->name.c_str());
		} break;
		case T_I64: {
			return writeI64(iprot,value,field->name.c_str());
		} break;
		case T_LIST:
		case T_MAP:
		case T_SET: {			
		} break;
		case T_STRING: {
			return writeString(iprot,value,field->name.c_str());
		} break;
		case T_STRUCT: {			
		} break;
	}
	return iprot->skip(field->type);
}

uint32_t TSchemeBase::writeString(TProtocol* oprot, cocos2d::CCValue* value, const char* name)
{
	if(value->isMap()) {		
		const cocos2d::CCValue* v = prop(value,name);
		if(v!=NULL) {
			return oprot->writeString(v->stringValue());
		}
	}
	return oprot->writeString("");	
}

uint32_t TSchemeBase::writeBinary(TProtocol* oprot, cocos2d::CCValue* value, const char* name)
{
	if(value->isMap()) {		
		const cocos2d::CCValue* v = prop(value,name);
		if(v!=NULL) {
			return oprot->writeBinary(v->stringValue());
		}
	}
	return oprot->writeString("");	
}

uint32_t TSchemeBase::writeBool(TProtocol* oprot, cocos2d::CCValue* value, const char* name)
{
	if(value->isMap()) {		
		const cocos2d::CCValue* v = prop(value,name);
		if(v!=NULL) {
			return oprot->writeBool(v->booleanValue());
		}
	}
	return oprot->writeBool(false);	
}

uint32_t TSchemeBase::writeByte(TProtocol* oprot, cocos2d::CCValue* value, const char* name)
{
	if(value->isMap()) {		
		const cocos2d::CCValue* v = prop(value,name);
		if(v!=NULL) {
			return oprot->writeByte((int8_t) v->intValue());
		}
	}
	return oprot->writeByte(0);	
}

uint32_t TSchemeBase::writeI16(TProtocol* oprot, cocos2d::CCValue* value, const char* name)
{
	if(value->isMap()) {		
		const cocos2d::CCValue* v = prop(value,name);
		if(v!=NULL) {
			return oprot->writeI16((int16_t) v->intValue());
		}
	}
	return oprot->writeI16(0);	
}

uint32_t TSchemeBase::writeI32(TProtocol* oprot, cocos2d::CCValue* value, const char* name)
{
	if(value->isMap()) {		
		const cocos2d::CCValue* v = prop(value,name);
		if(v!=NULL) {
			return oprot->writeI32((int32_t) v->intValue());
		}
	}
	return oprot->writeI32(0);
}

uint32_t TSchemeBase::writeI64(TProtocol* oprot, cocos2d::CCValue* value, const char* name)
{
	if(value->isMap()) {		
		const cocos2d::CCValue* v = prop(value,name);
		if(v!=NULL) {			
			return oprot->writeI64((int64_t) v->floatValue());
		}
	}
	return oprot->writeI64(0);
}

uint32_t TSchemeBase::writeDouble(TProtocol* oprot, cocos2d::CCValue* value, const char* name)
{
	if(value->isMap()) {		
		const cocos2d::CCValue* v = prop(value,name);
		if(v!=NULL) {			
			return oprot->writeDouble(v->floatValue());
		}
	}
	return oprot->writeDouble(0);
}

}}} // apache::thrift::protocol
