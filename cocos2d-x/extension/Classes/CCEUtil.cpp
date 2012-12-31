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