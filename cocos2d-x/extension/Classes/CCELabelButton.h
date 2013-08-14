#ifndef __CCE_LABEL_BUTTON_H__
#define __CCE_LABEL_BUTTON_H__

#include "CCEControl.h"
#include "CCScale9Sprite.h"

/* Define the button margin for Left/Right edge */
#define CCELabelButtonMarginLR 8 // px
/* Define the button margin for Top/Bottom edge */
#define CCELabelButtonMarginTB 2 // px

typedef struct {
	std::string title;
	bool colorSet;
	ccColor3B titleColor;
	CCScale9Sprite* backgroupSprite;
} CCELabelButtonSetting;

class CCELabelButton : public CCEControl
{        
public:
    CCELabelButton();
    virtual ~CCELabelButton();

    virtual void setHighlighted(bool enabled);
	
protected:
    /** Adjust the background image. YES by default. If the property is set to NO, the 
    background will use the prefered size of the background image. */
    bool doesAdjustBackgroundImage();
    void setAdjustBackgroundImage(bool adjustBackgroundImage);
    bool m_doesAdjustBackgroundImage;

    /** The prefered size of the button, if label is larger it will be expanded. */
    CC_PROPERTY(CCSize, m_preferredSize, PreferredSize);

    /** Adjust the button zooming on touchdown. Default value is YES. */
    CC_PROPERTY(bool, m_zoomOnTouchDown, ZoomOnTouchDown);

    CC_PROPERTY(CCPoint, m_labelAnchorPoint, LabelAnchorPoint);

    /* Override setter to affect a background sprite too */
    virtual void setOpacity(GLubyte var);
    
protected:
	std::map<int, CCELabelButtonSetting*> m_labelSettings;
	CCNode* m_titleLabel;
	CCScale9Sprite* m_backgroundSprite;
	
public:	
    /* Define the button margin for Top/Bottom edge */
    CC_SYNTHESIZE_READONLY(int, m_marginV, VerticalMargin);
    /* Define the button margin for Left/Right edge */
    CC_SYNTHESIZE_READONLY(int, m_marginH, HorizontalOrigin);
    //set the margins at once (so we only have to do one call of needsLayout)
    virtual void setMargins(int marginH, int marginV);
	
protected:
	CCELabelButtonSetting* getSetting(int state,bool create);	
	void releaseSettings();

public:
    virtual bool initWithLabelAndBackgroundSprite(CCNode* label, CCScale9Sprite* backgroundSprite);    
    static CCELabelButton* create(CCNode* label, CCScale9Sprite* backgroundSprite);
    
    virtual bool initWithTitleAndFontNameAndFontSize(std::string title, const char * fontName, float fontSize,CCScale9Sprite* backgroundSprite);
    static CCELabelButton* create(const char* title, const char * fontName, float fontSize,CCScale9Sprite* backgroundSprite);
    
    virtual const char* getTitle(int state);
	virtual void setTitle(int state, const char* title);
	
	virtual const ccColor3B getTitleColor(int state);    
	virtual void setTitleColor(int state, ccColor3B color);

    virtual CCScale9Sprite* getBackgroundSprite(int state);
    virtual void setBackgroundSprite(int state, CCScale9Sprite* sprite);	
	
    virtual void updateControl();
	virtual void cleanup();
	virtual void onEnter();
};

#endif