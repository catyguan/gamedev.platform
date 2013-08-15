#include "CCELayoutUtil.h"

#include "cocoa/CCValueSupport.h"
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

struct LayoutItem
{
	CCNode* node;
	int dock;		// 0:none,1-left,2-right,3-top,4-right,5-all
	int align;		// -1:none, 0-left, 1-middle, 2-right
	int valign;		// -1:none, 0-top, 1-center, 2-bottom
	int order;		// default:0
};

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
				item.dock = 0;
				item.align = item.valign = -1;
				item.order = 0;

				std::string sv;
				sv = ch->attribute("layout_dock").stringValue();
				if(sv.size()>0) {
					if(sv.compare("left")==0) {
						item.dock = 1;
					} else if(sv.compare("right")==0) {
						item.dock = 2;
					} else if(sv.compare("top")==0) {
						item.dock = 3;
					} else if(sv.compare("bottom")==0) {
						item.dock = 4;
					} else if(sv.compare("all")==0) {
						item.dock = 5;
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

				item.order = ch->attribute("layout_order").intValue();

				if(item.dock!=0 || item.align!=-1 || item.valign!=-1) {
					item.node = ch;
					ilist.push_back(item);
				}
			}
		}
	}

	if(ilist.size()>0) {
		sort(ilist.begin(),ilist.end(),compare_item);

		CCSize sz = node->getContentSize();
		CCRect rect(0,0,sz.width,sz.height);
		if(node->canCall("margin")) {
			CCValueArray ps;
			CCValue marginValue = node->call("margin", ps);
			if(marginValue.isMap()) {
				float l,t,r,b;
				l = t = r = b = 0;
				margin(marginValue, &t, &l, &b, &r);
				float x1 = rect.origin.x + l;
				float y1 = rect.origin.y + t;
				float x2 = rect.size.width - r;
				float y2 = rect.size.height - b;			
				rect = CCRect(x1,y1,x2-x1,y2-y1);
			}
		}
		std::vector<LayoutItem>::const_iterator it = ilist.begin();
		for(;it!=ilist.end();it++) {
			const LayoutItem& item = (*it);
			alignLayout(item.node, rect, item.align, item.valign);			
		}
	}

	if(deep && children!=NULL) {		
		for(size_t i=0;i<children->count();i++) {
			CCNode* node = dynamic_cast<CCNode*>(children->objectAtIndex(i));
			layout(node, deep);
		}
	}
}

void LayoutUtil::alignLayout(CCNode* node, CCRect& rect, int align, int valign)
{
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