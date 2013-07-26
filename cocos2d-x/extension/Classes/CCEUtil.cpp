#include "CCEUtil.h"

#define BUFF_SIZE 4096
std::string StringUtil::format( const char* format, ... )
{
	va_list ap;
	char text[BUFF_SIZE] = {0,};
	va_start(ap, format);
	vsnprintf(text, BUFF_SIZE, format, ap);
	va_end(ap);
	return std::string( text );
}

void StringUtil::split(std::string& s, std::string& delim,std::vector< std::string >* ret)
{
	size_t last = 0;
	size_t index=s.find_first_of(delim,last);
	while (index!=std::string::npos)
	{
		ret->push_back(s.substr(last,index-last));
		last=index+1;
		index=s.find_first_of(delim,last);
	}
	if (index-last>0)
	{
		ret->push_back(s.substr(last,index-last));
	}
}

CCRect RectUtil::rectUnion(const CCRect& src1, const CCRect& src2) 
{
    CCRect result;
    
    float x1 = MIN(src1.getMinX(), src2.getMinX());
    float y1 = MIN(src1.getMinY(), src2.getMinY());
    float x2 = MAX(src1.getMaxX(), src2.getMaxX());
    float y2 = MAX(src1.getMaxY(), src2.getMaxY());
    
    result.origin=ccp(x1,x2);
    result.size=CCSizeMake(x2-x1, y2-y1);
    return result;
}