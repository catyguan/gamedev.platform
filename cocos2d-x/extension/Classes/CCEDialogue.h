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
	
	virtual bool showDialogue(CCValue& properties, CCValue call);	

public:
	virtual void onEnter();
	virtual void cleanup();
	
public:
	void onPauseNodeClick(CCNode* node, const char* name, CCNodeEvent*);
	void onNarratePause(CCNode* node, const char* name, CCNodeEvent*);
	void onNarrateEnd(CCNode* node, const char* name, CCNodeEvent*);

	// cc_call
	CC_DECLARE_CALLS_BEGIN	
	CC_DECLARE_CALL(show)
	CC_DECLARE_CALLS_END
	// end cc_call

protected:
	void clear();
	void build(DialogueInfo* info, DialogueInfo* old);	
	void nextPage();
	void endNarrate();

	virtual void updatePauseNode();

protected:
	CCScale9Sprite* m_frame;
	CCENarrate* m_narrate;
	CCNode* m_pauseNode;
	DialogueInfo* m_info;

};

#endif