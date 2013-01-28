#include "CCEDialog.h"
#include "base_nodes/CCNode_Events.h"

CCEDialog::CCEDialog()
: m_result(0)
{
	
}

CCEDialog::~CCEDialog()
{
    
}

bool CCEDialog::init()
{
	if(!CCELayerTouch::init()) {
		return false;
	}
	setVisible(false);
	setTouchPriority(kCCEDialogHandlerPriority);
	return true;
}

CCEDialog* CCEDialog::create()
{    
    CCEDialog *pRet = new CCEDialog();
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

bool CCEDialog::showDialog()
{
	setVisible(true);
	setZOrder(KCCEDialogZOrder);
	return true;
}

void CCEDialog::closeDialog(int resultType)
{
	m_result = resultType;
	setVisible(false);

	CCNodeEvent e;
	raiseEvent(NODE_EVENT_DIALOG_CLOSE, &e);
}
	
int CCEDialog::getResult()
{
	return m_result;
}
	
void CCEDialog::bindOKClick(CCNode* node)
{
	createTouch(node);	
	node->onEvent(NODE_EVENT_CLICK, this, nodeevent_selector(CCEDialog::onOKClick));
}

void CCEDialog::bindCancelClick(CCNode* node)
{
	createTouch(node);
	node->onEvent(NODE_EVENT_CLICK, this, nodeevent_selector(CCEDialog::onCancelClick));
}
	
void CCEDialog::onOKClick(CCNode* node, const char* name, CCNodeEvent*)
{
	doneDialog();
}

void CCEDialog::onCancelClick(CCNode* node, const char* name, CCNodeEvent*)
{
	cancelDialog();
}

void CCEDialog::createBarrier(ccColor4B color)
{
	CCLayerColor* l = CCLayerColor::create(color);
	addChild(l,getZOrder());
}

void CCEDialog::createBarrier()
{
	createBarrier(ccc4(255,255,255,128));
}