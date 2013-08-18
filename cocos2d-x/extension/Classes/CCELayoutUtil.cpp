#include "CCELayoutUtil.h"

#include "cocoa/CCValueSupport.h"
#include "CCEUtil.h"
#include <vector>
#include <algorithm>
#include <functional>

USING_NS_CC;

void LayoutUtil::margin(CCValue& cfg, float* top, float* left, float* bottom, float* right)
{
	CCValueReader vr(&cfg);
	if(vr.isMap()) {
		CCValue* v;

		v = vr.getNull("left");
		if(left!=NULL && v!=NULL)*left = v->floatValue();

		v = vr.getNull("top");
		if(top!=NULL && v!=NULL)*top = v->floatValue();

		v = vr.getNull("right");
		if(right!=NULL && v!=NULL)*right = v->floatValue();

		v = vr.getNull("bottom");
		if(bottom!=NULL && v!=NULL)*bottom = v->floatValue();
	}
}

CCValue LayoutUtil::margin(float top, float left, float bottom, float right) {
	CCValueMap map;
	map["left"] = CCValue::numberValue(left);
	map["top"] = CCValue::numberValue(top);
	map["right"] = CCValue::numberValue(right);
	map["bottom"] = CCValue::numberValue(bottom);		
	return CCValue::mapValue(map);
}

bool LayoutUtil::createLayoutItem(CCNode* ch, LayoutItem& item)
{
	item.node  = NULL;
	item.align = item.valign = -1;
	item.order = 0;

	std::string sv;
	CCValue v;

	sv = ch->attribute("layout_dock").stringValue();
	if(sv.size()>0) {
		if(sv.compare("left")==0) {
			item.align = 0;
			item.height = "*";
		} else if(sv.compare("top")==0) {
			item.valign = 0;
			item.width = "*";
		} else if(sv.compare("right")==0) {
			item.align = 2;
			item.height = "*";
		} else if(sv.compare("bottom")==0) {
			item.valign = 2;
			item.width = "*";
		} else if(sv.compare("fill")==0) {
			item.align = 1;
			item.valign = 1;
			item.width = "*";
			item.height = "*";
		}
	}
	
	sv = ch->attribute("layout_align").stringValue();
	if(sv.size()>0) {
		if(sv.compare("left")==0) {
			item.align = 0;
		} else if(sv.compare("middle")==0) {
			item.align = 1;
		} else if(sv.compare("right")==0) {
			item.align = 2;
		}
	}

	sv = ch->attribute("layout_valign").stringValue();
	if(sv.size()>0) {
		if(sv.compare("top")==0) {
			item.valign = 0;
		} else if(sv.compare("center")==0) {
			item.valign = 1;
		} else if(sv.compare("bottom")==0) {
			item.valign = 2;
		}
	}

	v = ch->attribute("layout_width");
	if(v.isString()) {
		sv = v.stringValue();
		if(sv.size()>0)item.width = sv;
	} else if(v.isInt() || v.isNumber()) {
		item.width = StringUtil::format("%d", v.intValue());
	}
	
	v = ch->attribute("layout_height");
	if(v.isString()) {
		sv = v.stringValue();
		if(sv.size()>0)item.height = sv;
	} else if(v.isInt() || v.isNumber()) {
		item.height = StringUtil::format("%d", v.intValue());
	}

	item.order = ch->attribute("layout_order").intValue();

	if(item.align!=-1 || item.valign!=-1 || item.width.size()>0 || item.height.size()>0) {
		item.node = ch;
		return true;
	}
	return false;
}

bool compare_item(const LayoutItem& t1,const LayoutItem& t2){     
	return t1.order < t2.order;  
}  

void LayoutUtil::layout(CCNode* node, bool deep)
{
	if(node->canCall("doLayout")) {
		CCValueArray ps;
		ps.push_back(CCValue::booleanValue(deep));
		node->call("doLayout", ps);
	} else {
		doLayout(node, deep);
	}

	if(deep) {
		CCArray* children = node->getChildren();
		if(children!=NULL) {		
			for(size_t i=0;i<children->count();i++) {
				CCNode* node = dynamic_cast<CCNode*>(children->objectAtIndex(i));
				layout(node, deep);
			}
		}
	}
}

void LayoutUtil::doLayout(CCRect& rect, LayoutItem& item)
{
	resizeLayout(item.node, rect, item.width, item.height);
	alignLayout(item.node, rect, item.align, item.valign);
}

void LayoutUtil::doLayout(CCNode* node,bool deep)
{
	CCArray* children = node->getChildren();
	std::vector<LayoutItem> ilist;

	if(children!=NULL) {		
		for(size_t i=0;i<children->count();i++) {
			CCNode* ch = dynamic_cast<CCNode*>(children->objectAtIndex(i));

			if(ch!=NULL) {
				LayoutItem item;
				if(createLayoutItem(ch, item)) {				
					ilist.push_back(item);
				}
			}
		}
	}

	if(ilist.size()>0) {
		sort(ilist.begin(),ilist.end(),compare_item);

		CCSize sz = node->getContentSize();
		CCRect rect(0,0,sz.width,sz.height);		
		std::vector<LayoutItem>::const_iterator it = ilist.begin();
		for(;it!=ilist.end();it++) {
			const LayoutItem& item = (*it);
			doLayout(rect, (LayoutItem&) item);
		}
	}	
}

void LayoutUtil::alignLayout(CCNode* node, CCRect& rect, int align, int valign)
{
	if(node==NULL)return;

	CCPoint pt = node->isIgnoreAnchorPointForPosition()?CCPointZero:node->getAnchorPoint();
	CCSize sz = node->getContentSize();
	switch(align) {
	case 0: {
		float x = rect.origin.x + sz.width*pt.x;
		node->setPositionX(x);
		break;
		}
	case 1: {
		float x = rect.origin.x+rect.size.width/2 - sz.width/2 + sz.width*pt.x;
		node->setPositionX(x);
		break;
		}
	case 2: {
		float x = rect.getMaxX() - sz.width + sz.width*pt.x;
		node->setPositionX(x);
		break;
		}
	}
	switch(valign) {
	case 2: {
		float y = rect.origin.y + sz.height*pt.y;
		node->setPositionY(y);
		break;
		}
	case 1: {
		float y = rect.origin.y+rect.size.height/2 - sz.height/2 + sz.height*pt.y;
		node->setPositionY(y);
		break;
		}
	case 0: {
		float y = rect.getMaxY() - sz.height + sz.height*pt.y;
		node->setPositionY(y);
		break;
		}
	}
}

bool LayoutUtil::parseSize(std::string& str, int* percent, int* value)
{
	*percent = 0;
	*value = 0;
	if(str.length()>0) {		
		if(str.compare("*")==0) {
			*percent = -1;
			return true;
		} else if(str[str.size()-1]=='%') {
			*percent = atoi(str.substr(0, str.size()-1).c_str());
			if(*percent>0)return true;
			return false;
		}		
		*value = atoi(str.c_str());
		return *value!=0;
	}
	return false;
}

void LayoutUtil::resizeLayout(CCNode* node, CCRect& rect, std::string width, std::string height)
{
	if(node==NULL)return;

	bool ch = false;
	CCSize sz = node->getContentSize();
	int per;
	int val;
	if(parseSize(width, &per, &val)) {
		if(per==-1)per=100;
		if(per>0) {
			sz.width = rect.size.width*per/100;
		} else {			
			if(val>0) {
				sz.width = (float) val;
			} else {
				sz.width = rect.size.width+val;		
			}
		}
		ch = true;
	}
	if(parseSize(height, &per, &val)) {
		if(per==-1)per=100;
		if(per>0) {
			sz.height = rect.size.height*per/100;
		} else {
			if(val>0) {
				sz.height = (float) val;
			} else {
				sz.height = rect.size.height+val;
			}
		}
		ch = true;
	}
	if(ch) {
		node->setContentSize(sz);
	}
}