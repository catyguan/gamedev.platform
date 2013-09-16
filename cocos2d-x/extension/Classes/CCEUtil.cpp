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

void StringUtil::split(std::string& s, std::string delim,std::vector< std::string >* ret)
{
	split(s,delim,ret,-1);
}

void StringUtil::split(std::string& s, std::string delim,std::vector< std::string >* ret,int limit)
{
	size_t last = 0;
	size_t index=s.find_first_of(delim,last);
	while (index!=std::string::npos)
	{
		ret->push_back(s.substr(last,index-last));
		last=index+1;
		if(limit>1 && ret->size()==limit-1) {
			index = s.npos;
		} else {
			index=s.find_first_of(delim,last);
		}
	}
	if (last<s.size())
	{
		ret->push_back(s.substr(last));
	}
}

void StringUtil::parseProperties(std::string& s, const char* propDelim, const char* keyValueDelim, std::map<std::string, std::string>* ret)
{
	if(propDelim!=NULL) {
		std::vector<std::string> plist;
		split(s, propDelim, &plist);
		std::vector<std::string>::const_iterator it = plist.begin();
		for(;it!=plist.end();it++) {
			std::string str = *it;
			parseProperties(str, NULL, keyValueDelim, ret);
		}
		return;
	}

	std::string str = trim(s);
	if(str.size()==0)return;
	size_t idx = str.find_first_of(keyValueDelim);
	if(idx==std::string::npos) {
		(*ret)[str] = "";
	} else {
		std::string key = str.substr(0, idx);
		std::string value = str.substr(idx+1);
		(*ret)[key] = value;
	}
}

int StringUtil::replaceAll(std::string& str,  const std::string& pattern,  const std::string& newpat) 
{ 
	int count = 0; 
	const size_t nsize = newpat.size(); 
	const size_t psize = pattern.size(); 
 
	for(size_t pos = str.find(pattern, 0);  
		pos != std::string::npos; 
		pos = str.find(pattern,pos + nsize)) 
	{ 
		str.replace(pos, psize, newpat); 
		count++; 
	} 
	return count; 
}

std::string StringUtil::trim(std::string& str)
{	
	std::string s = str;
	if(s.size()==0)return s;
	std::string::iterator c;
	for (c = s.begin(); c != s.end() && iswspace(*c++);); s.erase(s.begin(), --c);
	for (c = s.end(); c != s.begin() && iswspace(*--c);); s.erase(++c, s.end());
	return s;
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