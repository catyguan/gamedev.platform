#ifndef __CCE_DIALOGUE_H__
#define __CCE_DIALOGUE_H__

#include "cocos2d.h"

USING_NS_CC;

#define NODE_EVENT_DIALOGUE_FINISH	"dialogueFinish"

class CCScale9Sprite;
class CCENarrate;
struct DialogueInfo;
class CCEDialogue : public CCLayer
{
public:
    CCEDialogue();
    virtual ~CCEDialogue();
	
public:
	virtual bool init();
	static CCEDialogue* create();

	CCNode* getFrame();
	void setFrame(CCNode* node);
	CCLabelTTF* getLabel();
	void setLabel(CCLabelTTF* label);
	CCNode* getPauseNode();
	void setPauseNode(CCNode* node);
	CCSize getMaginTopLeft();	
	void setMarginTopLeft(CCSize sz);
	CCSize getMarginBottomRight();
	void setMarginBottomRight(CCSize sz);
	
	virtual bool showDialogue(CCValue& properties, CCValue call);	

public:
	virtual void onEnter();
	virtual void cleanup();

	virtual void setContentSize(const CCSize& contentSize);
	
public:
	void onPauseNodeClick(CCNode* node, const char* name, CCNodeEvent*);
	void onNarratePause(CCNode* node, const char* name, CCNodeEvent*);
	void onNarrateEnd(CCNode* node, const char* name, CCNodeEvent*);

	// cc_call
	CC_DECLARE_CALLS_BEGIN	
	CC_DECLARE_CALL(label)
	CC_DECLARE_CALL(frame)
	CC_DECLARE_CALL(margin)
	CC_DECLARE_CALL(pauseNode)
	CC_DECLARE_CALL(show)
	CC_DECLARE_CALLS_END
	// end cc_call

protected:
	void clear();
	void build(DialogueInfo* info, DialogueInfo* old);	
	
	void nextPage();
	void pauseNarrate();
	void endNarrate();

	virtual void updatePauseNode();
	void layout();

protected:
	CCSize m_marginTopLeft, m_marginBottomRight;

	DialogueInfo* m_info;

	CCENarrate* m_narrate;
	CCLabelTTF* m_label;
	bool m_paused;

};

#endif