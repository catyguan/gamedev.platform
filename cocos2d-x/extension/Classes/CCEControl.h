#ifndef __CCE_CONTROL_H__
#define __CCE_CONTROL_H__

#include "CCEContainer.h"

USING_NS_CC;

#define NODE_PROPERTY_ENABLE		"enable"
#define NODE_PROPERTY_SELECT		"select"
#define NODE_PROPERTY_HIGHLIGHT		"highlight"

#define CONTROL_STATE_NORMAL		(1 << 0)
#define CONTROL_STATE_HIGHLIGHTED	(1 << 1)
#define CONTROL_STATE_DISABLED		(1 << 2)
#define CONTROL_STATE_SELECTED		(1 << 3)

#define CONTROL_STATE_NORMAL_TEXT		"normal"
#define CONTROL_STATE_HIGHLIGHTED_TEXT	"highlighted"
#define CONTROL_STATE_DISABLED_TEXT		"disabled"
#define CONTROL_STATE_SELECTED_TEXT		"selected"

class CCEControl : public CCEContainer
{
public:
	virtual void onEnter();

    /** Tells whether the control is enabled. */
    virtual void setEnabled(bool v);
    virtual bool isEnabled();
	
    /** A Boolean value that determines the control selected state. */
    virtual void setSelected(bool v);
    virtual bool isSelected();
	
    /** A Boolean value that determines whether the control is highlighted. */
    virtual void setHighlighted(bool v);
    virtual bool isHighlighted();

	void setTouchType(const char* v);
	const char* getTouchType();

	virtual int getState();
	static int toState(const char* v);
	static const char* toState(int v);
	
	virtual bool raiseEvent(const char* name, CCNodeEvent*);  
	virtual void updateControl();
	
protected:
    bool m_bEnabled;
    bool m_bSelected;
    bool m_bHighlighted;
	std::string m_touchType;

public:
    CCEControl();
	virtual ~CCEControl();

    virtual bool init(void);
	static CCEControl* create();

protected:
   
	// cc_call
	CC_DECLARE_CALLS_BEGIN
	CC_DECLARE_CALL(enabled)
	CC_DECLARE_CALL(selected)
	CC_DECLARE_CALL(highlighted)
	CC_DECLARE_CALL(touch)
	CC_DECLARE_CALLS_END	
};

#endif