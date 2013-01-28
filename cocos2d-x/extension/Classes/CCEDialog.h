#ifndef __CCE_DIALOG_H__
#define __CCE_DIALOG_H__

#include "CCELayerTouch.h"

USING_NS_CC;

#define DIALOG_RESULT_CANCEL	0
#define DIALOG_RESULT_OK		1

#define kCCEDialogHandlerPriority -64
#define KCCEDialogZOrder		64

#define NODE_EVENT_DIALOG_CLOSE	"dialogClose"

class CCEDialog : public CCELayerTouch
{
protected:
	int m_result;
	
public:
    CCEDialog();
    virtual ~CCEDialog();
	
public:
	virtual bool init();
	static CCEDialog* create();
	
	virtual bool showDialog();
	virtual void closeDialog(int resultType);
	void cancelDialog(){closeDialog(DIALOG_RESULT_CANCEL);};
	void doneDialog(){closeDialog(DIALOG_RESULT_OK);};
	
	int getResult();
	bool isDone(){return getResult()==DIALOG_RESULT_OK;};
	bool isCancel(){return getResult()==DIALOG_RESULT_CANCEL;};
	
	void bindOKClick(CCNode* node);
	void bindCancelClick(CCNode* node);

	void createBarrier(ccColor4B color);
	void createBarrier();
	
public:
	void onOKClick(CCNode* node, const char* name, CCNodeEvent*);
	void onCancelClick(CCNode* node, const char* name, CCNodeEvent*);
};

#endif