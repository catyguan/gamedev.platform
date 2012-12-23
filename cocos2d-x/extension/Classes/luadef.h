#ifndef  __CCE_LUADEF_WIN32_H__
#define  __CCE_LUADEF_WIN32_H__

#include "ccTypes.h"
#include <map>
#include <vector>

class CCELuaValue;
typedef std::map<std::string, CCELuaValue>   CCELuaValueTable;
typedef CCELuaValueTable::const_iterator     CCELuaValueTableIterator;
typedef std::vector<CCELuaValue>               CCELuaValueArray;
typedef CCELuaValueArray::const_iterator     CCELuaValueArrayIterator;

typedef enum {
	CCELuaValueTypeNull,
    CCELuaValueTypeInt,
    CCELuaValueTypeNumber,
    CCELuaValueTypeBoolean,
    CCELuaValueTypeString,
    CCELuaValueTypeTable,
    CCELuaValueTypeArray,
} CCELuaValueType;

typedef union {
    int                 intValue;
    double              numberValue;
    bool                booleanValue;
    std::string*        stringValue;
    CCELuaValueTable*     tableValue;
    CCELuaValueArray*    arrayValue;
} CCELuaValueField;

class CCELuaValue
{
public:
	static std::string EMPTY;

public:
	static const CCELuaValue nullValue();
    static const CCELuaValue intValue(const int intValue);
    static const CCELuaValue numberValue(const double numberValue);
    static const CCELuaValue booleanValue(const bool booleanValue);
    static const CCELuaValue stringValue(const char* stringValue);
    static const CCELuaValue stringValue(const std::string& stringValue);
    static const CCELuaValue tableValue(const CCELuaValueTable& dictValue);
    static const CCELuaValue arrayValue(const CCELuaValueArray& arrayValue);    

    CCELuaValue(void)
        : m_type(CCELuaValueTypeInt)
    {
        memset(&m_field, 0, sizeof(m_field));
    }
    CCELuaValue(const CCELuaValue& rhs);
    CCELuaValue& operator=(const CCELuaValue& rhs);
    ~CCELuaValue(void);

    const CCELuaValueType getType(void) const {
        return m_type;
    }

	bool isInt() const {
		return m_type==CCELuaValueTypeInt;
	}
    int intValue(void) const {
		if(m_type==CCELuaValueTypeInt) {
			return m_field.intValue;
		}
		return 0;
    }

	bool isNumber() const {
		return m_type==CCELuaValueTypeNumber;
	}
    double numberValue(void) const {
		if(m_type==CCELuaValueTypeNumber) {
			return m_field.numberValue;
		}
		return 0;
    }

	bool isBoolean() const {
		return m_type==CCELuaValueTypeBoolean;
	}
    bool booleanValue(void) const {
		if(m_type==CCELuaValueTypeBoolean) {
			return m_field.booleanValue;
		}
		return false;
    }

	bool isString() const {
		return m_type==CCELuaValueTypeString;
	}
    const std::string& stringValue(void) const {
		if(m_type==CCELuaValueTypeString) {
			return *m_field.stringValue;
		}
		return EMPTY;
    }

	bool isTable() const {
		return m_type==CCELuaValueTypeTable;
	}
    const CCELuaValueTable& tableValue(void) const {
		CC_ASSERT(isTable());
        return *m_field.tableValue;
    }

	bool isArray() const {
		return m_type==CCELuaValueTypeArray;
	}
    const CCELuaValueArray& arrayValue(void) const {
		CC_ASSERT(isArray());
        return *m_field.arrayValue;
    }

private:
    CCELuaValueField m_field;
    CCELuaValueType  m_type;

    void copy(const CCELuaValue& rhs);
};

#endif // __CCE_LUADEF_WIN32_H__