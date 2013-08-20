#include "CCEButton.h"
#include "actions/CCAction.h"
#include "actions/CCActionInterval.h"
#include "CCEUtil.h"
#include "cocoa\CCValueSupport.h"

enum
{
    kZoomActionTag = 0xCCCB0001,
};

CCEButton::CCEButton()
: m_node(NULL)
, m_background(NULL)
, m_zoomOnTouchDown(true)
{

}

CCEButton::~CCEButton()
{
	releaseSettings();    
	CC_SAFE_RELEASE_NULL(m_node);
    CC_SAFE_RELEASE_NULL(m_background);
}

//initialisers
bool CCEButton::init()
{
    return this->init(NULL, NULL);
}

bool CCEButton::init(CCNode* node, CCNode* background)
{
    if (!CCEControl::init())
		return false;
    
    // Zooming button by default
    m_zoomOnTouchDown = true;

	// Set the default color and opacity
    setColor(ccc3(255, 255, 255));
    setOpacity(255);
    setOpacityModifyRGB(true);
        
    // Set the default anchor point
    ignoreAnchorPointForPosition(false);
    setAnchorPoint(ccp(0.5f, 0.5f));
        
    // Set the nodes
	setNode(CONTROL_STATE_NORMAL, node);
	setBackground(CONTROL_STATE_NORMAL, background);    
        
    return true;
}

CCEButton* CCEButton::create(CCNode* node, CCNode* background)
{
    CCEButton *pRet = new CCEButton();
    if(pRet->init(node, background))
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

void CCEButton::setHighlighted(bool enabled)
{
    CCAction *action =getActionByTag(kZoomActionTag);
    if (action)
    {
        stopAction(action);        
    }
    CCEControl::setHighlighted(enabled);
    if( m_node!=NULL && m_zoomOnTouchDown )
    {
        float scaleValue = (isHighlighted() && isEnabled()) ? 1.1f : 1.0f;
		// CCLOG("scale %f", scaleValue);
        CCAction *zoomAction =CCScaleTo::create(0.05f, scaleValue);
        zoomAction->setTag(kZoomActionTag);
        runAction(zoomAction);
    }
}

void CCEButton::setZoomOnTouchDown(bool zoomOnTouchDown)
{
    m_zoomOnTouchDown = zoomOnTouchDown;
}

bool CCEButton::getZoomOnTouchDown()
{
    return m_zoomOnTouchDown;
}

CCEButtonSetting* CCEButton::getSetting(int state,bool create)
{
	std::map<int,CCEButtonSetting*>::const_iterator it = m_settings.find(state);
	if(it!=m_settings.end()) {
		return it->second;
	}
	if(create) {
		CCEButtonSetting* r = new CCEButtonSetting();
		r->node = NULL;
		r->backgroup = NULL;
		m_settings[state] = r;
		return r;
	}
	return NULL;
}

CCNode* CCEButton::getNode(int state)
{
	CCEButtonSetting* set = getSetting(state, false);
	if(set!=NULL && set->node!=NULL) {
        return set->node;
    }
	if(state!=CONTROL_STATE_NORMAL) {
		return getNode(CONTROL_STATE_NORMAL);
	}
    return NULL;
}

void CCEButton::setNode(int state, CCNode* node)
{
	CCEButtonSetting* set = getSetting(state, true);
	if(set->node!=NULL) {
		removeChild(set->node, true);
		set->node = NULL;
    }
	set->node = node;

    node->setVisible(false);
	node->setAnchorPoint(CCPointZero);
    addChild(node, 2);

    // If the current state if equal to the given state we update the layout
    if (getState() == state)
    {
		updateControl();
    }
}

CCNode* CCEButton::getBackground(int state)
{
	CCEButtonSetting* set = getSetting(state, false);
	if(set!=NULL && set->backgroup!=NULL) {
        return set->backgroup;
    }
	if(state!=CONTROL_STATE_NORMAL) {
		return getBackground(CONTROL_STATE_NORMAL);
	}
    return NULL;
}

void CCEButton::setBackground(int state, CCNode* node)
{
	CCEButtonSetting* set = getSetting(state, true);
	if(set->backgroup!=NULL) {
		removeChild(set->backgroup, true);
		set->backgroup = NULL;
    }
	set->backgroup = node;

	if(node!=NULL) {
		node->setVisible(false);
		node->setAnchorPoint(CCPointZero);
		addChild(node, 1);
	}

    // If the current state if equal to the given state we update the layout
    if (getState() == state)
    {
		updateControl();
    }
}

void CCEButton::updateControl()
{
    // Hide the background and the label
    if (m_node != NULL) {
        m_node->setVisible(false);
    }
    if (m_background) {
        m_background->setVisible(false);
    }
   
	int m_eState = getState();    

	CC_SAFE_RELEASE_NULL(m_node);	
	m_node = getNode(m_eState);
	CC_SAFE_RETAIN(m_node);
    
    // Update the background sprite
	CC_SAFE_RELEASE_NULL(m_background);
	m_background = getBackground(m_eState);
	CC_SAFE_RETAIN(m_background);    
   
    // Set the content size
    CCRect rectTitle;
    if (m_node != NULL)
    {
        rectTitle = m_node->boundingBox();
    }
    CCRect rectBackground;
    if (m_background != NULL)
    {
        rectBackground = m_background->boundingBox();
    }

    CCRect maxRect = RectUtil::rectUnion(rectTitle, rectBackground);
    setContentSize(CCSizeMake(maxRect.size.width, maxRect.size.height));        
    
	// Make visible the background and the label
    if (m_node != NULL)
    {   
		//m_node->setPosition(m_node->getPosition());
        m_node->setVisible(true);
    }  
    if (m_background != NULL)
    {
		//m_background->setPosition(m_node->getPosition());
        m_background->setVisible(true);   
    }   
}

CCEButton* CCEButton::create()
{
    CCEButton *pControlButton = new CCEButton();
    if (pControlButton && pControlButton->init())
    {
        pControlButton->autorelease();
        return pControlButton;
    }
    CC_SAFE_DELETE(pControlButton);
    return NULL;
}

void CCEButton::cleanup()
{
	CCEControl::cleanup();	
	releaseSettings();
	CC_SAFE_RELEASE_NULL(m_node);
    CC_SAFE_RELEASE_NULL(m_background);
}

void CCEButton::releaseSettings()
{
	std::map<int,CCEButtonSetting*>::const_iterator it;
	for(it=m_settings.begin();it!=m_settings.end();it++) {
		delete it->second;
    }
	m_settings.clear();
}

CC_BEGIN_CALLS(CCEButton, CCEControl)	
	CC_DEFINE_CALL(CCEButton, setNode)
	CC_DEFINE_CALL(CCEButton, getNode)
	CC_DEFINE_CALL(CCEButton, setBackground)
	CC_DEFINE_CALL(CCEButton, getBackground)
	CC_DEFINE_CALL(CCEButton, state)
CC_END_CALLS(CCEButton, CCEControl)

CCValue CCEButton::CALLNAME(setNode)(CCValueArray& params) {	
	std::string v = ccvpString(params, 0);
	CCNode* node = ccvpObject(params,1,CCNode);	
	int state = toState(v.c_str());
	setNode(state, node);
	return CCValue::nullValue();
}

CCValue CCEButton::CALLNAME(getNode)(CCValueArray& params) {
	std::string v = ccvpString(params, 0);
	int state = toState(v.c_str());
	return CCValue::objectValue(getNode(state));
}

CCValue CCEButton::CALLNAME(setBackground)(CCValueArray& params) {	
	std::string v = ccvpString(params, 0);
	CCNode* node = ccvpObject(params,1,CCNode);	
	int state = toState(v.c_str());
	setBackground(state, node);
	return CCValue::nullValue();
}

CCValue CCEButton::CALLNAME(getBackground)(CCValueArray& params) {
	std::string v = ccvpString(params, 0);
	int state = toState(v.c_str());
	return CCValue::objectValue(getBackground(state));
}

CCValue CCEButton::CALLNAME(state)(CCValueArray& params) {
	CCValueMap* map = ccvpMap(params,0);
	if(map!=NULL) {
		CCValueMapIterator it = map->begin();
		for(;it!=map->end();it++) {
			int st = toState(it->first.c_str());
			if(it->second.isArray())
			{
				CCValueArray* arr = it->second.arrayValue();
				if(arr!=NULL && arr->size()==2) {
					CCNode* n1 = dynamic_cast<CCNode*>((*arr)[0].objectValue());
					CCNode* n2 = dynamic_cast<CCNode*>((*arr)[1].objectValue());
					if(n1!=NULL) {
						setNode(st, n1);
					}
					if(n2!=NULL) {
						setBackground(st, n2);
					}
				}
			} else if(it->second.isMap()) {
				CCValueMap* map2 = it->second.mapValue();
				if(map2!=NULL) {
					CCValueMapIterator it2;
					it2 = map2->find("node");
					if(it2!=map2->end()) {
						CCNode* n = dynamic_cast<CCNode*>(it2->second.objectValue());
						if(n!=NULL)setNode(st,n);
					}
					it2 = map2->find("background");
					if(it2!=map2->end()) {
						CCNode* n = dynamic_cast<CCNode*>(it2->second.objectValue());
						if(n!=NULL)setBackground(st,n);
					}
				}
			} else if(it->second.isObject()) {
				CCNode* obj = dynamic_cast<CCNode*>(it->second.objectValue());
				setNode(st, obj);
			}
		}
	}
	return CCValue::nullValue();
}