#include "CCEDirector.h"
#include "cocoa/CCValueSupport.h"
#include <vector>
#include <algorithm>
#include <functional>

USING_NS_CC;

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

void CCEDirector::layout(CCNode* node, bool deep)
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
		std::vector<LayoutItem>::const_iterator it = ilist.begin();
		for(;it!=ilist.end();it++) {
			const LayoutItem& item = (*it);
			CCPoint pt = item.node->getAnchorPoint();
			CCSize sz = item.node->getContentSize();
			switch(item.align) {
			case 0: {
				float x = rect.origin.x + sz.width*pt.x;
				item.node->setPositionX(x);
				break;
				}
			case 1: {
				float x = rect.origin.x+rect.size.width/2 - sz.width/2 + sz.width*pt.x;
				item.node->setPositionX(x);
				break;
				}
			case 2: {
				float x = rect.getMaxX() - sz.width + sz.width*pt.x;
				item.node->setPositionX(x);
				break;
				}
			}
			switch(item.valign) {
			case 0: {
				float y = rect.origin.y + sz.height*pt.y;
				item.node->setPositionY(y);
				break;
				}
			case 1: {
				float y = rect.origin.y+rect.size.height/2 - sz.height/2 + sz.height*pt.y;
				item.node->setPositionY(y);
				break;
				}
			case 2: {
				float y = rect.getMaxY() - sz.height + sz.height*pt.y;
				item.node->setPositionY(y);
				break;
				}
			}
		}
	}

	if(deep && children!=NULL) {		
		for(size_t i=0;i<children->count();i++) {
			CCNode* node = dynamic_cast<CCNode*>(children->objectAtIndex(i));
			layout(node, deep);
		}
	}
}

