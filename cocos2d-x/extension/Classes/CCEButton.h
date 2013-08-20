#ifndef __CCE_BUTTON_H__
#define __CCE_BUTTON_H__

#include "CCEControl.h"

typedef struct {	
	CCNode* node;
	CCNode* backgroup;
} CCEButtonSetting;

class CCEButton : public CCEControl
{        
public:
    CCEButton();
    virtual ~CCEButton();

    virtual void setHighlighted(bool enabled);
	
protected:
    /** Adjust the button zooming on touchdown. Default value is YES. */
    CC_PROPERTY(bool, m_zoomOnTouchDown, ZoomOnTouchDown);

protected:
	std::map<int, CCEButtonSetting*> m_settings;
	CCNode* m_node;
	CCNode* m_background;
	
public:
	
protected:
	CCEButtonSetting* getSetting(int state,bool create);	
	void releaseSettings();

public:
    virtual bool init();
	static CCEButton* create();
	
    virtual bool init(CCNode* node, CCNode* background);    
    static CCEButton* create(CCNode* node, CCNode* background);
    
    virtual CCNode* getNode(int state);
    virtual void setNode(int state, CCNode* node);
    virtual CCNode* getBackground(int state);
    virtual void setBackground(int state, CCNode* background);	

	virtual void setStateNode(int state, CCNode* node, CCNode* background){setNode(state,node);setBackground(state,background);};
	
    virtual void updateControl();
	virtual void cleanup();

	// cc_call
	CC_DECLARE_CALLS_BEGIN
	CC_DECLARE_CALL(setNode)
	CC_DECLARE_CALL(getNode)
	CC_DECLARE_CALL(setBackground)	
	CC_DECLARE_CALL(getBackground)
	CC_DECLARE_CALL(state)
	CC_DECLARE_CALLS_END
};

#endif