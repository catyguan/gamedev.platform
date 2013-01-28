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

class CCEControl : public CCEContainer
{
public:
    /** Tells whether the control is enabled. */
    virtual void setEnabled(bool v);
    virtual bool isEnabled();
	
    /** A Boolean value that determines the control selected state. */
    virtual void setSelected(bool v);
    virtual bool isSelected();
	
    /** A Boolean value that determines whether the control is highlighted. */
    virtual void setHighlighted(bool v);
    virtual bool isHighlighted();

	virtual int getState();
	
	virtual bool raiseEvent(const char* name, CCNodeEvent*);  
	virtual void updateControl();
	
protected:
    bool m_bEnabled;
    bool m_bSelected;
    bool m_bHighlighted;

public:
    CCEControl();
    virtual bool init(void);
    virtual ~CCEControl();

protected:
    static CCEControl* create();

};

#endif