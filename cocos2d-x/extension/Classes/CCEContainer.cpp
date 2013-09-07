#include "CCEContainer.h"
#include "cocoa\CCValueSupport.h"
#include "CCEUtil.h"
#include "CCELayoutUtil.h"

// CCEContainer
CCEContainer::CCEContainer()
: m_bIsOpacityModifyRGB(false)
{
	setCascadeColorEnabled(true);
	setCascadeOpacityEnabled(true);
}

CCEContainer::~CCEContainer()
{
	clearRefs();
}

void CCEContainer::clearRefs()
{
	if(m_refs.size()>0) {
		std::list<CCObject*>::const_iterator it = m_refs.begin();
		for(;it!=m_refs.end();it++) {
			(*it)->release();
		}
		m_refs.clear();
	}
}

void CCEContainer::setOpacityModifyRGB(bool bOpacityModifyRGB)
{
    m_bIsOpacityModifyRGB=bOpacityModifyRGB;
    CCObject* child;
    CCArray* children=getChildren();
    CCARRAY_FOREACH(children, child)
    {
        CCRGBAProtocol* pNode = dynamic_cast<CCRGBAProtocol*>(child);        
        if (pNode)
        {
            pNode->setOpacityModifyRGB(bOpacityModifyRGB);
        }
    }
}

bool CCEContainer::isOpacityModifyRGB()
{
    return m_bIsOpacityModifyRGB;
}

CCEContainer* CCEContainer::create()
{
	CCEContainer *pRet = new CCEContainer();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }    
    return pRet;
}

void CCEContainer::cleanup()
{
	if(m_refs.size()>0) {
		removeAllRefs();
	}
	CCNodeRGBA::cleanup();
}

void CCEContainer::addRef(CCObject* obj)
{
	if(obj!=NULL) {
		CC_SAFE_RETAIN(obj);
		m_refs.push_back(obj);
	}
}

CCObject* CCEContainer::getRefById(const char* id)
{
	if(m_refs.size() > 0)
    {
		std::list<CCObject*>::const_iterator it = m_refs.begin();        
		for(;it!=m_refs.end();it++)
        {
			CCNode* pNode = dynamic_cast<CCNode*>((*it));
			if(pNode && pNode->getId().compare(id)==0) {
                return pNode;
			}
        }		
    }
    return NULL;
}

CCObject* CCEContainer::getRefByTag(int tag)
{
	if(m_refs.size() > 0)
    {
		std::list<CCObject*>::const_iterator it = m_refs.begin();        
		for(;it!=m_refs.end();it++)
        {
			CCNode* pNode = dynamic_cast<CCNode*>((*it));
			if(pNode && pNode->getTag()==tag) {
                return pNode;
			}
        }		
    }
    return NULL;
}

void CCEContainer::removeRefById(const char* id)
{
	CCObject* obj = getRefById(id);
	if(obj!=NULL) {
		CCNode* node = dynamic_cast<CCNode*>(obj);
		if(node!=NULL && node->getParent()==NULL) {
			node->cleanup();
		}
		m_refs.remove(node);
		node->release();
	}
}

void CCEContainer::removeRefByTag(int tag)
{
	CCObject* obj = getRefByTag(tag);
	if(obj!=NULL) {
		CCNode* node = dynamic_cast<CCNode*>(obj);
		if(node!=NULL && node->getParent()==NULL) {
			node->cleanup();
		}
		m_refs.remove(node);
		node->release();
	}
}

std::list<CCObject*>& CCEContainer::getRefs()
{
	return m_refs;
}

void CCEContainer::removeAllRefs()
{
	if(m_refs.size()>0) {
		std::list<CCObject*>::const_iterator it = m_refs.begin();        
		for(;it!=m_refs.end();it++)
        {
			CCNode* pNode = dynamic_cast<CCNode*>((*it));
			if(pNode && pNode->getParent()==NULL) {
				pNode->cleanup();
			}
        }	
		clearRefs();
	}
}

CC_BEGIN_CALLS(CCEContainer, CCNodeRGBA)
	CC_DEFINE_CALL(CCEContainer, addRef)
	CC_DEFINE_CALL(CCEContainer, getRef)
	CC_DEFINE_CALL(CCEContainer, removeRef)
	CC_DEFINE_CALL(CCEContainer, removeAllRefs)
	CC_DEFINE_CALL(CCEContainer, getAllRefs)
CC_END_CALLS(CCEContainer, CCNodeRGBA)

CCValue CCEContainer::CALLNAME(addRef)(CCValueArray& params) {
	CCObject* obj = ccvpObject(params, 0, CCObject);
	if(obj==NULL) {
		throw new std::string("param 1 expect CCObject");
	}
	addRef(obj);
	return CCValue::nullValue();
}

CCValue CCEContainer::CALLNAME(getRef)(CCValueArray& params) {
	if(params.size()>0) {
		if(params[0].isString()) {
			return CCValue::objectValue(getRefById(params[0].stringValue().c_str()));
		}
		if(params[0].isInt()) {
			return CCValue::objectValue(getRefByTag(params[0].intValue()));
		}		
	}
	return CCValue::nullValue();
}

CCValue CCEContainer::CALLNAME(removeRef)(CCValueArray& params) {
	if(params.size()>0) {
		if(params[0].isString()) {
			removeRefById(params[0].stringValue().c_str());
		} else if(params[0].isInt()) {
			removeRefByTag(params[0].intValue());
		}		
	}
	return CCValue::nullValue();
}

CCValue CCEContainer::CALLNAME(removeAllRefs)(CCValueArray& params) {
	removeAllRefs();
	return CCValue::nullValue();
}

CCValue CCEContainer::CALLNAME(getAllRefs)(CCValueArray& params) {
	CCValueArray arr;
	if(m_refs.size()>0) {
		std::list<CCObject*>::const_iterator it = m_refs.begin();        
		for(;it!=m_refs.end();it++)
        {
			arr.push_back(CCValue::objectValue(*it));
        }	
	}
	return CCValue::arrayValue(arr);
}

// CCEPanel
CCEPanel::CCEPanel()
{
	m_padding = 0;
	m_gridWidth = m_gridHeight = 1;
}

CCEPanel::~CCEPanel()
{

}

CCEPanel* CCEPanel::create()
{
	CCEPanel *pRet = new CCEPanel();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    return pRet;
}

void CCEPanel::setItems(std::vector<CCEPanelGridItem>& items)
{
	m_items.clear();
	std::vector<CCEPanelGridItem>::const_iterator it = items.begin();
	int mrow,mcol;
	mrow = mcol = 0;
	for(;it!=items.end();it++) {		
		m_items.push_back(*it);
		int col = it->col;
		col+=it->colspan-1;
		int row = it->row;
		row+=it->rowspan-1;
		if(mrow<row)mrow=row;
		if(mcol<col)mcol=col;
		if(it->node!=NULL && it->node->getParent()==NULL) {
			addChild(it->node);
		}
	}
	setGridWidth(mcol);
	setGridHeight(mrow);
}

typedef struct _LGridItem {
	CCEPanelGridItem* item;
	CCRect rect;
	bool span;
} LGridItem;
void CCEPanel::doLayout(bool deep)
{
	if(m_items.size()==0 && getChildrenCount()>0) {
		LayoutUtil::doLayout(this, deep);
		return;
	}

	if(m_gridHeight>0) {
		LGridItem** g = new LGridItem*[m_gridHeight];
		for(int i=0;i<m_gridHeight;i++) {
			g[i] = new LGridItem[m_gridWidth];
			for(int j=0;j<m_gridWidth;j++) {
				g[i][j].item = NULL;
			}
		}

		std::vector<CCEPanelGridItem>::const_iterator it = m_items.begin();
		for(;it!=m_items.end();it++) {
			for(int row=0;row<it->rowspan;row++)
			{
				for(int col=0;col<it->colspan;col++)
				{			
					LGridItem& o = g[row+it->row-1][col+it->col-1];
					o.item = (CCEPanelGridItem*) &(*it);
					if(row==0 && col==0) {
						o.span = false;
					} else {
						o.span = true;
					}
				}
			}
		}

		CCSize sz = getContentSize();
		float pwidth = sz.width - m_padding*(m_gridWidth-1);
		float pheight = sz.height - m_padding*(m_gridHeight-1);
		float h = sz.height;
		int freerow = 0;		
		for(int row=0;row<m_gridHeight;row++) {
			float w = pwidth;
			int freecol=0;
			float maxheight = -1;
			for(int col=0;col<m_gridWidth;col++) {
				LGridItem& o = g[row][col];
				if(o.span || o.item==NULL) {
					freecol++;
					o.rect.size.width=-1;
					o.rect.size.height=-1;
					continue;
				}
				
				int per, val;
				if(!LayoutUtil::parseSize(o.item->width, &per, &val)) {
					per = -1;
					val = -1;
				}
				if(per==-1) {
					freecol++;
					o.rect.size.width = -1;
				} else {
					if(per>0) {
						o.rect.size.width = pwidth*per/100;
					} else {
						o.rect.size.width= (float) val;							 
					}
					w-=o.rect.size.width;
				}

				if(!LayoutUtil::parseSize(o.item->height, &per, &val)) {
					per = -1;
					val = -1;
				}
				if(per==-1) {
					o.rect.size.height = -1;
				} else {
					if(per>0) {
						o.rect.size.height = pheight*per/100;
					} else {
						o.rect.size.height=(float) val;							 
					}
					if(maxheight<o.rect.size.height)maxheight=o.rect.size.height;
				}
			}
			float freew = 0;
			if(freecol>0) {
				freew = w/freecol;
			}
			float x = 0;
			for(int col=0;col<m_gridWidth;col++) {
				LGridItem& o = g[row][col];
				if(o.span)continue;
				if(o.rect.size.width==-1) {
					int c = 1;
					if(o.item!=NULL)c=o.item->colspan;
					o.rect.size.width = freew*c;
				}
				o.rect.origin.x = x;
				x += (o.rect.size.width + m_padding);
			}

			if(maxheight==-1) {
				freerow ++;
			} else {
				h -= maxheight;
			}			
		}

		float freeh = 0;
		if(freerow>0)freeh=h/freerow;		
		float y = sz.height;
		for(int row=0;row<m_gridHeight;row++) {
			float mheight = 0;
			for(int col=0;col<m_gridWidth;col++) {
				LGridItem& o = g[row][col];
				if(o.rect.size.height==-1) {
					int c = 1;
					if(o.item!=NULL)c=o.item->rowspan;
					o.rect.size.height = freeh*c;
				}
				o.rect.origin.y=y-o.rect.size.height;
				if(mheight<o.rect.size.height)mheight = o.rect.size.height;				
			}
			y-= (mheight+m_padding);
		}

		for(int row=0;row<m_gridHeight;row++) {
			for(int col=0;col<m_gridWidth;col++) {
				LGridItem& o = g[row][col];
				if(o.item!=NULL && o.item->node!=NULL && !o.span) {
					LayoutItem litem;
					LayoutUtil::createLayoutItem(o.item->node, litem);
					LayoutUtil::doLayout(o.rect, litem);
				}
			}
		}

		for(int i=0;i<m_gridHeight;i++) {
			delete[] g[i];
		}
		delete[] g;
	}
}

CC_BEGIN_CALLS(CCEPanel, CCEContainer)
	CC_DEFINE_CALL(CCEPanel, doLayout)
	CC_DEFINE_CALL(CCEPanel, padding)
	CC_DEFINE_CALL(CCEPanel, grid)
CC_END_CALLS(CCEPanel, CCEContainer)

CCValue CCEPanel::CALLNAME(padding)(CCValueArray& params) {
	if(params.size()>0) {
		float v = params[0].floatValue();
		setPadding(v);
	}
	return CCValue::numberValue(getPadding());
}

static void createItem(CCValue* val, CCEPanelGridItem& item)
{
	if(val->isNull()) {
		item.node = NULL;
		return;
	}
	if(val->isObject()) {
		CCNode* node = dynamic_cast<CCNode*>(val->objectValue());
		if(node!=NULL) {
			item.node = node;			
		}
		return;
	}
	if(val->isMap()) {
		CCValueReader vr(val);
		CCValue* v;
		v = vr.getNull("node");
		if(v!=NULL) {
			createItem(v, item);
		} else {
			item.node = NULL;
		}

		v = vr.get("width");
		if(v!=NULL)
		{
			if(v->isString()) {
				item.width = v->stringValue();
			}
			if(v->isInt() || v->isNumber()) {
				item.width = StringUtil::format("%d", v->intValue());
			}
		}
		if(item.width.size()==0)item.width="*";

		v = vr.get("height");
		if(v!=NULL)
		{
			if(v->isString()) {
				item.height = v->stringValue();
			}
			if(v->isInt() || v->isNumber()) {
				item.height = StringUtil::format("%d", v->intValue());
			}
		}
		if(item.height.size()==0)item.height="*";
		
		item.rowspan = vr.get("rowspan")->intValue();
		if(item.rowspan<1)item.rowspan=1;
		item.colspan = vr.get("colspan")->intValue();
		if(item.colspan<1)item.colspan=1;
		return;
	}
}

CCValue CCEPanel::CALLNAME(grid)(CCValueArray& params) {
	std::vector<CCEPanelGridItem> list;

	if(params.size()>0) {
		CCValueReader vr(&params[0]);
		if(vr.isArray()) {
			int c = vr.arraySize();
			int row = 1;
			for(int i=0;i<c;i++) {
				if(vr.beArray(i)) {
					int c2 = vr.arraySize();
					int col = 1;
					for(int j=0;j<c2;j++) {
						CCValue* val = vr.get(j);
						CCEPanelGridItem item;
						item.node = NULL;
						item.row = row;
						item.col = col;
						item.rowspan = item.rowspan = 1;
						createItem(val, item);
						if(item.colspan>1) {
							col+=item.colspan-1;
						}	
						list.push_back(item);
						col++;
					}
					vr.pop();
				} else {
					CCValue* val = vr.get(i);
					CCEPanelGridItem item;
					item.node = NULL;
					item.row = row;
					item.col = 1;
					item.rowspan = item.rowspan = 1;
					createItem(val, item);
					if(item.rowspan>1) {
						row+=item.rowspan-1;
					}	
					list.push_back(item);
				}
				row++;
			}
		}
	}
	setItems(list);
	return CCValue::nullValue();
}

CCValue CCEPanel::CALLNAME(doLayout)(CCValueArray& params) {
	doLayout(ccvpBoolean(params, 0));
	return CCValue::nullValue();
}