#include "CCEControl.h"
#include "base_nodes/CCNode_Events.h"
#include "CCELayerTouch.h"

CCEControl::CCEControl()
: m_bEnabled(false)
, m_bSelected(false)
, m_bHighlighted(false)
{

}

CCEControl* CCEControl::create()
{
    CCEControl* pRet = new CCEControl();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
}

bool CCEControl::init()
{
    if (true)
    {
        setEnabled(true);
        setSelected(false);
        setHighlighted(false);
		
        return true;
    }
    else
    {
        return false;
    }
}

CCEControl::~CCEControl()
{
    
}

void CCEControl::setEnabled(bool bEnabled)
{    
	m_bEnabled = bEnabled;
	CCPropertyChangeEvent e(NODE_PROPERTY_ENABLE, CCValue::booleanValue(bEnabled));
	raiseEvent(NODE_EVENT_PROPERTY_CHANGE, &e);
	updateControl();
}

bool CCEControl::isEnabled()
{
    return m_bEnabled;
}

void CCEControl::setSelected(bool bSelected)
{
    m_bSelected = bSelected;
    CCPropertyChangeEvent e(NODE_PROPERTY_SELECT, CCValue::booleanValue(bSelected));
	raiseEvent(NODE_EVENT_PROPERTY_CHANGE, &e);
	updateControl();
}

bool CCEControl::isSelected()
{
    return m_bSelected;
}

void CCEControl::setHighlighted(bool bHighlighted)
{
    m_bHighlighted = bHighlighted;
    CCPropertyChangeEvent e(NODE_PROPERTY_HIGHLIGHT, CCValue::booleanValue(bHighlighted));
	raiseEvent(NODE_EVENT_PROPERTY_CHANGE, &e);
	updateControl();
}

bool CCEControl::isHighlighted()
{
    return m_bHighlighted;
}

int CCEControl::getState()
{
	if(!isEnabled()) {
		return CONTROL_STATE_DISABLED;
	}
	if(isSelected()) {
		return CONTROL_STATE_SELECTED;
	}
	if(isHighlighted()) {
		return CONTROL_STATE_HIGHLIGHTED;
	}
	return CONTROL_STATE_NORMAL;
}

void CCEControl::updateControl()
{
	
}

bool CCEControl::raiseEvent(const char* name, CCNodeEvent* e)
{
	bool r = CCNode::raiseEvent(name,e);
	if(strcmp(name,NODE_EVENT_FOCUS)==0) {
		CCETouchFocusEvent* ev = (CCETouchFocusEvent*) e;
		if(ev->isFocus()) {
			if(!isHighlighted()) {
				setHighlighted(true);
				r = true;
			}
		} else {
			if(isHighlighted()) {
				setHighlighted(false);
				r = true;
			}
		}
	}
	return r;
}