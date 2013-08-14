#include "CCELabelButton.h"
#include "label_nodes/CCLabelTTF.h"
#include "label_nodes/CCLabelBMFont.h"
#include "actions/CCAction.h"
#include "actions/CCActionInterval.h"
#include "CCEUtil.h"

enum
{
    kZoomActionTag = 0xCCCB0001,
};

CCELabelButton::CCELabelButton()
: m_doesAdjustBackgroundImage(false)
, m_titleLabel(NULL)
, m_backgroundSprite(NULL)
, m_zoomOnTouchDown(false)
, m_marginH(CCELabelButtonMarginLR)
, m_marginV(CCELabelButtonMarginTB)
{

}

CCELabelButton::~CCELabelButton()
{
	releaseSettings();    
    CC_SAFE_RELEASE_NULL(m_backgroundSprite);
}

bool CCELabelButton::initWithLabelAndBackgroundSprite(CCNode* node, CCScale9Sprite* backgroundSprite)
{
    if (CCEControl::init())
    {
        CCAssert(node != NULL, "Label must not be nil.");
        CCLabelProtocol* label = dynamic_cast<CCLabelProtocol*>(node);
        CCRGBAProtocol* rgbaLabel = dynamic_cast<CCRGBAProtocol*>(node);
        CCAssert(backgroundSprite != NULL, "Background sprite must not be nil.");
        CCAssert(label != NULL || rgbaLabel!=NULL || backgroundSprite != NULL, "");
        
        m_zoomOnTouchDown = true;

        // Adjust the background image by default
        setAdjustBackgroundImage(true);
        setPreferredSize(CCSizeZero);
        // Zooming button by default
        m_zoomOnTouchDown = true;
        
        // Set the default anchor point
        ignoreAnchorPointForPosition(false);
        setAnchorPoint(ccp(0.5f, 0.5f));
        
        // Set the nodes
		addChild(node,2);
		m_titleLabel = node;
        
        // Set the default color and opacity
        setColor(ccc3(255, 255, 255));
        setOpacity(255);
        setOpacityModifyRGB(true);
        
        setTitle(CONTROL_STATE_NORMAL, label->getString());
        setTitleColor(CONTROL_STATE_NORMAL, rgbaLabel->getColor() );        
		setBackgroundSprite(CONTROL_STATE_NORMAL, backgroundSprite);
                
        setLabelAnchorPoint(ccp(0.5f, 0.5f));

        // Layout update
        updateControl();

        return true;
    }
    //couldn't init the CCControl
    else
    {
        return false;
    }
}

CCELabelButton* CCELabelButton::create(CCNode* label, CCScale9Sprite* backgroundSprite)
{
    CCELabelButton *pRet = new CCELabelButton();
    if(pRet->initWithLabelAndBackgroundSprite(label, backgroundSprite))
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

bool CCELabelButton::initWithTitleAndFontNameAndFontSize(std::string title, const char * fontName, float fontSize, CCScale9Sprite* backgroundSprite)
{
    CCLabelTTF *label = CCLabelTTF::create(title.c_str(), fontName, fontSize);
	setTitle(CONTROL_STATE_NORMAL, title.c_str());
	return initWithLabelAndBackgroundSprite(label, backgroundSprite);
}

CCELabelButton* CCELabelButton::create(const char* title, const char * fontName, float fontSize, CCScale9Sprite* backgroundSprite)
{
    CCELabelButton *pRet = new CCELabelButton();
    if(pRet->initWithTitleAndFontNameAndFontSize(title, fontName, fontSize,backgroundSprite))
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

void CCELabelButton::setMargins(int marginH, int marginV)
{
    m_marginV=marginV;
    m_marginH=marginH;
    updateControl();
}

void CCELabelButton::setHighlighted(bool enabled)
{
    CCEControl::setHighlighted(enabled);

    CCAction *action =getActionByTag(kZoomActionTag);
    if (action)
    {
        stopAction(action);        
    }
    updateControl();
    if( m_zoomOnTouchDown )
    {
        float scaleValue = (isHighlighted() && isEnabled()) ? 1.1f : 1.0f;
        CCAction *zoomAction =CCScaleTo::create(0.05f, scaleValue);
        zoomAction->setTag(kZoomActionTag);
        runAction(zoomAction);
    }
}

void CCELabelButton::setZoomOnTouchDown(bool zoomOnTouchDown)
{
    m_zoomOnTouchDown = zoomOnTouchDown;
}

bool CCELabelButton::getZoomOnTouchDown()
{
    return m_zoomOnTouchDown;
}

void CCELabelButton::setPreferredSize(CCSize size)
{
    if(size.width == 0 && size.height == 0)
    {
        m_doesAdjustBackgroundImage = true;
    }
    else
    {
        m_doesAdjustBackgroundImage = false;
		std::map<int,CCELabelButtonSetting*>::const_iterator it;
		for(it = m_labelSettings.begin();it!=m_labelSettings.end();it++)
		{
			CCELabelButtonSetting* item = it->second;
			CCScale9Sprite* sprite = item->backgroupSprite;
			if(sprite!=NULL) {
				sprite->setPreferredSize(size);
			}
        }
    }

    m_preferredSize = size;
    updateControl();
}

CCSize CCELabelButton::getPreferredSize()
{
    return m_preferredSize;
}

void CCELabelButton::setAdjustBackgroundImage(bool adjustBackgroundImage)
{
    m_doesAdjustBackgroundImage=adjustBackgroundImage;
    updateControl();
}

bool CCELabelButton::doesAdjustBackgroundImage()
{
    return m_doesAdjustBackgroundImage;
}

CCPoint CCELabelButton::getLabelAnchorPoint()
{
    return this->m_labelAnchorPoint;
}

void CCELabelButton::setLabelAnchorPoint(CCPoint labelAnchorPoint)
{
    this->m_labelAnchorPoint = labelAnchorPoint;
    if (m_titleLabel != NULL)
    {
        this->m_titleLabel->setAnchorPoint(labelAnchorPoint);
    }
}

CCELabelButtonSetting* CCELabelButton::getSetting(int state,bool create)
{
	std::map<int,CCELabelButtonSetting*>::const_iterator it = m_labelSettings.find(state);
	if(it!=m_labelSettings.end()) {
		return it->second;
	}
	if(create) {
		CCELabelButtonSetting* r = new CCELabelButtonSetting();
		r->backgroupSprite = NULL;
		r->colorSet = false;
		r->titleColor = ccWHITE;
		m_labelSettings[state] = r;
		return r;
	}
	return NULL;
}

const char* CCELabelButton::getTitle(int state)
{
	CCELabelButtonSetting* set = getSetting(state,false);
    if (set!= NULL)
    {
		if(set->title.size()>0) {
			return set->title.c_str();
		}
		if(state!=CONTROL_STATE_NORMAL) {
			return getTitle(CONTROL_STATE_NORMAL);
		}
    }
    return "";
}

void CCELabelButton::setTitle(int state, const char* title)
{
	if(title==NULL) {
		CCELabelButtonSetting* set = getSetting(state, false);	
		set->title = "";
	} else {
		CCELabelButtonSetting* set = getSetting(state, true);
		set->title = title;
		// If the current state if equal to the given state we update the layout
		if (getState() == state)
		{
			updateControl();
		}
	}
}

const ccColor3B CCELabelButton::getTitleColor(int state)
{
    ccColor3B returnColor = ccWHITE;
	CCELabelButtonSetting* set = getSetting(state,false);
	if(set!=NULL && set->colorSet)
	{		
		return set->titleColor;
	}
	if(state!=CONTROL_STATE_NORMAL) {
		return getTitleColor(CONTROL_STATE_NORMAL);
    }
    return returnColor;
}

void CCELabelButton::setTitleColor(int state, ccColor3B color)
{
	CCELabelButtonSetting* set = getSetting(state, true);
	set->colorSet = true;
	set->titleColor = color;
     
    // If the current state if equal to the given state we update the layout
    if (getState() == state)
    {
		updateControl();
    }
}

CCScale9Sprite* CCELabelButton::getBackgroundSprite(int state)
{
	CCELabelButtonSetting* set = getSetting(state, false);
	if(set!=NULL && set->backgroupSprite) {
		return set->backgroupSprite;
    }
	if(state!=CONTROL_STATE_NORMAL) {
		return (CCScale9Sprite*) getBackgroundSprite(CONTROL_STATE_NORMAL);
	}
	return NULL;
}


void CCELabelButton::setBackgroundSprite(int state, CCScale9Sprite* sprite)
{
    CCSize oldPreferredSize = m_preferredSize;

	CCELabelButtonSetting* set = getSetting(state, true);
	CCScale9Sprite* previousBackgroundSprite = set->backgroupSprite;
    if (previousBackgroundSprite)
    {
        removeChild(previousBackgroundSprite, true);
		set->backgroupSprite = NULL;
    }

	set->backgroupSprite = sprite;

    sprite->setVisible(false);
    sprite->setAnchorPoint(ccp(0.5f, 0.5f));
    addChild(sprite,1);

    if (this->m_preferredSize.width != 0 || this->m_preferredSize.height != 0)
    {
        if (oldPreferredSize.equals(m_preferredSize))
        {
            // Force update of preferred size
            sprite->setPreferredSize(CCSizeMake(oldPreferredSize.width+1, oldPreferredSize.height+1));
        }
        
        sprite->setPreferredSize(this->m_preferredSize);
    }

    // If the current state if equal to the given state we update the layout
    if (getState() == state)
    {
        updateControl();
    }
}

void CCELabelButton::updateControl()
{
	if (getParent()==NULL) {
        return;
    }
    // Hide the background and the label
    if (m_titleLabel != NULL) {
        m_titleLabel->setVisible(false);
    }
    if (m_backgroundSprite) {
        m_backgroundSprite->setVisible(false);
    }
    // Update anchor of all labels
    this->setLabelAnchorPoint(this->m_labelAnchorPoint);
    
	int m_eState = getState();
	const char* title = getTitle(m_eState);
	ccColor3B color = getTitleColor(m_eState);

    CCLabelProtocol* label = dynamic_cast<CCLabelProtocol*>(m_titleLabel);
	if (label && title!=NULL)
    {
		label->setString(title);
    }

    CCRGBAProtocol* rgbaLabel = dynamic_cast<CCRGBAProtocol*>(m_titleLabel);
    if (rgbaLabel)
    {
        rgbaLabel->setColor(color);
    }
    if (m_titleLabel != NULL)
    {
        m_titleLabel->setPosition(ccp (getContentSize().width / 2, getContentSize().height / 2));
    }
    
    // Update the background sprite
	CC_SAFE_RELEASE_NULL(m_backgroundSprite);
	m_backgroundSprite = this->getBackgroundSprite(m_eState);
	CC_SAFE_RETAIN(m_backgroundSprite);
    if (m_backgroundSprite != NULL)
    {
        m_backgroundSprite->setPosition(ccp (getContentSize().width / 2, getContentSize().height / 2));
    }
   
    // Get the title label size
    CCSize titleLabelSize;
    if (m_titleLabel != NULL)
    {
        titleLabelSize = m_titleLabel->boundingBox().size;
    }
    
    // Adjust the background image if necessary
    if (m_doesAdjustBackgroundImage)
    {
        // Add the margins
        if (m_backgroundSprite != NULL)
        {
            m_backgroundSprite->setContentSize(CCSizeMake(titleLabelSize.width + m_marginH * 2, titleLabelSize.height + m_marginV * 2));
        }
    } 
    else
    {        
        //TODO: should this also have margins if one of the preferred sizes is relaxed?
        if (m_backgroundSprite != NULL)
        {
            CCSize preferredSize = m_backgroundSprite->getPreferredSize();
            if (preferredSize.width <= 0)
            {
                preferredSize.width = titleLabelSize.width;
            }
            if (preferredSize.height <= 0)
            {
                preferredSize.height = titleLabelSize.height;
            }

            m_backgroundSprite->setContentSize(preferredSize);
        }
    }
    
    // Set the content size
    CCRect rectTitle;
    if (m_titleLabel != NULL)
    {
        rectTitle = m_titleLabel->boundingBox();
    }
    CCRect rectBackground;
    if (m_backgroundSprite != NULL)
    {
        rectBackground = m_backgroundSprite->boundingBox();
    }

    CCRect maxRect = RectUtil::rectUnion(rectTitle, rectBackground);
    setContentSize(CCSizeMake(maxRect.size.width, maxRect.size.height));        
    
    if (m_titleLabel != NULL)
    {
        m_titleLabel->setPosition(ccp(getContentSize().width/2, getContentSize().height/2));
        // Make visible the background and the label
        m_titleLabel->setVisible(true);
    }
  
    if (m_backgroundSprite != NULL)
    {
        m_backgroundSprite->setPosition(ccp(getContentSize().width/2, getContentSize().height/2));
        m_backgroundSprite->setVisible(true);   
    }   
}

void CCELabelButton::setOpacity(GLubyte opacity)
{
	CCEControl::setOpacity(opacity);

	std::map<int,CCELabelButtonSetting*>::const_iterator it;
	for(it=m_labelSettings.begin();it!=m_labelSettings.end();it++) {
		CCELabelButtonSetting* item = it->second;
		CCScale9Sprite* sprite = item->backgroupSprite;
		if(sprite!=NULL) {
			sprite->setOpacity(opacity);
		}
    }
}

void CCELabelButton::onEnter()
{
	CCEControl::onEnter();
	updateControl();	
}

void CCELabelButton::cleanup()
{
	CCEControl::cleanup();	
	releaseSettings();	
	m_titleLabel = NULL;
    CC_SAFE_RELEASE_NULL(m_backgroundSprite);
}

void CCELabelButton::releaseSettings()
{
	std::map<int,CCELabelButtonSetting*>::const_iterator it;
	for(it=m_labelSettings.begin();it!=m_labelSettings.end();it++) {
		delete it->second;
    }
	m_labelSettings.clear();
}