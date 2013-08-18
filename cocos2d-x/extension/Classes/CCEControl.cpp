#include "CCEControl.h"
#include "cocoa\CCValueSupport.h"
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

void CCEControl::onEnter()
{
	CCEContainer::onEnter();
	updateControl();
	if(m_touchType.size()>0) {
		CCETouchBuilder b;
		b.bind(this).on(m_touchType).createTouch(NULL);
	}
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

void CCEControl::setTouchType(const char* type)
{
	m_touchType = type;
}

const char* CCEControl::getTouchType()
{
	return m_touchType.c_str();
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
	if(canCall("doUpdateControl")) {
		CCValueArray ps;
		this->call("doUpdateControl",ps);
	}
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

CC_BEGIN_CALLS(CCEControl, CCEContainer)	
	CC_DEFINE_CALL(CCEControl, enabled)
	CC_DEFINE_CALL(CCEControl, selected)
	CC_DEFINE_CALL(CCEControl, highlighted)
	CC_DEFINE_CALL(CCEControl, touch)
CC_END_CALLS(CCEControl, CCEContainer)

CCValue CCEControl::CALLNAME(enabled)(CCValueArray& params) {
	if(params.size()>0) {
		bool v = params[0].booleanValue();
		setEnabled(v);
	}
	return CCValue::booleanValue(isEnabled());
}

CCValue CCEControl::CALLNAME(selected)(CCValueArray& params) {
	if(params.size()>0) {
		bool v = params[0].booleanValue();
		setSelected(v);
	}
	return CCValue::booleanValue(isSelected());
}

CCValue CCEControl::CALLNAME(highlighted)(CCValueArray& params) {
	if(params.size()>0) {
		bool v = params[0].booleanValue();
		setHighlighted(v);
	}
	return CCValue::booleanValue(isHighlighted());
}

CCValue CCEControl::CALLNAME(touch)(CCValueArray& params) {
	if(params.size()>0) {
		std::string v;
		if(params[0].isArray()) {
			CCValueArray* arr = params[0].arrayValue();
			for(size_t i=0;arr!=NULL && i<arr->size();i++) {
				if(i!=0)v+=",";
				v+= (*arr)[i].stringValue();
			}
		} else {
			v = params[0].stringValue();
		}
		setTouchType(v.c_str());
	}
	return CCValue::stringValue(getTouchType());
}