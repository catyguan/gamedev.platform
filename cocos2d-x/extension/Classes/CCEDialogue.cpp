#include "CCEDialogue.h"
#include "CCELayerTouch.h"
#include "CCScale9Sprite.h"
#include "CCENarrate.h"
#include "cocoa\CCValueSupport.h"
#include "base_nodes/CCNode_Events.h"

struct DialogueInfo {
	// this
	CCSize size;
	// frame
	std::string frameImage;
	// narrate
	CCSize narrateSize;
	float speed;
	float pauseTime;
	// label
	int shadow;
	int stroke;
	ccColor3B fontColor;
	std::string text;
	int horizontalAlignment;
	int verticalAlignment;
	int fontSize;
	std::string fontName;
	// pauseNode
	CCSize pauseNodeSize;
	CCPoint pauseNodePoint;
};

CCEDialogue::CCEDialogue()
{
	m_frame = NULL;
	m_narrate = NULL;
	m_pauseNode = NULL;
	m_info = NULL;
}

CCEDialogue::~CCEDialogue()
{
    clear();
}

void CCEDialogue::cleanup()
{
	CCLayer::cleanup();
	clear();
}

void CCEDialogue::clear()
{
	if(m_info!=NULL) {
		delete m_info;
		m_info = NULL;
	}
}

bool CCEDialogue::init()
{
	if(!CCLayer::init()) {
		return false;
	}
	setVisible(false);
	return true;
}

CCEDialogue* CCEDialogue::create()
{    
    CCEDialogue *pRet = new CCEDialogue();
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

void CCEDialogue::build(DialogueInfo* info, DialogueInfo* old)
{
	setVisible(false);
	removeAllChildren();	
	CCSize size = info->size;
	CCRect rect = CCRectMake(0,0,size.width,size.height);
	setContentSize(size);	
		
	if(info->frameImage.size()>0) {		
		if(old==NULL || old->frameImage.compare(info->frameImage)!=0) {
			CC_SAFE_RELEASE_NULL(m_frame);
			m_frame = CCScale9Sprite::create(info->frameImage.c_str());
			m_frame->setAnchorPoint(CCPointZero);
		}
		m_frame->setContentSize(size);
		addChild(m_frame);

		rect = m_frame->getClientRect(0);
	}
	CCSize dm(0,0);
	CCLabelTTF* label = CCLabelTTF::create(
		info->text.c_str(), 
		info->fontName.c_str(), 
		(float) info->fontSize,
		dm, CCTextAlignment(info->horizontalAlignment), CCVerticalTextAlignment(info->verticalAlignment));	
	label->setColor(info->fontColor);

	if(m_narrate!=NULL) {
		m_narrate->removeEventHandler(NULL,	this);
	}

	m_narrate = CCENarrate::create(label, (int) rect.size.width, (int) rect.size.height,0,info->speed);
	m_narrate->setPagePauseTime(info->pauseTime);
	m_narrate->setPosition(CCPointMake(rect.origin.x, rect.origin.y));
	addChild(m_narrate,1);
	m_narrate->onEvent(NODE_EVENT_NARRATE_PAUSE, this, nodeevent_selector(CCEDialogue::onNarratePause));

	setVisible(true);
	m_narrate->process(CCValue::nullValue());
}

bool CCEDialogue::showDialogue(CCValue& properties, CCValue call)
{
	CCValueReader r(&properties);
	DialogueInfo* info = new DialogueInfo();
	// init
	info->fontColor = m_info!=NULL?m_info->fontColor:ccc3(255,255,255);	
	info->fontName = m_info!=NULL?m_info->fontName:"Arial";
	info->fontSize = m_info!=NULL?m_info->fontSize:32;
	info->frameImage = m_info!=NULL?m_info->frameImage:"";	
	info->horizontalAlignment = m_info!=NULL?m_info->horizontalAlignment:0;	
	info->narrateSize = m_info!=NULL?m_info->narrateSize:CCSizeMake(-1,-1);
	info->pauseNodePoint = m_info!=NULL?m_info->pauseNodePoint:CCPointMake(-1,-1);
	info->pauseNodeSize = m_info!=NULL?m_info->pauseNodeSize:CCSizeMake(-1,-1);
	info->pauseTime = m_info!=NULL?m_info->pauseTime:-1;
	info->shadow = m_info!=NULL?m_info->shadow:-1;
	info->size = m_info!=NULL?m_info->size:CCSizeMake(-1,-1);
	info->speed = m_info!=NULL?m_info->speed:0.75f;
	info->stroke = m_info!=NULL?m_info->stroke:-1;
	info->text = m_info!=NULL?m_info->text:"";
	info->verticalAlignment = m_info!=NULL?m_info->verticalAlignment:0;

	// read
	CCValue* v;
	if(r.isMap()) {		
		v = r.getNull("fontColor");
		if(v!=NULL)info->fontColor = CCValueUtil::color3b(*v);

		v = r.getNull("fontName");
		if(v!=NULL)info->fontName = v->stringValue();

		v = r.getNull("fontSize");
		if(v!=NULL)info->fontSize = v->intValue();

		v = r.getNull("frameImage");
		if(v!=NULL)info->frameImage = v->stringValue();

		v = r.getNull("horizontalAlignment");
		if(v!=NULL)info->horizontalAlignment = v->intValue();

		v = r.getNull("narrateSize");
		if(v!=NULL)info->narrateSize = CCValueUtil::size(*v);

		v = r.getNull("pauseNodePoint");
		if(v!=NULL)info->pauseNodePoint = CCValueUtil::point(*v);

		v = r.getNull("pauseNodeSize");
		if(v!=NULL)info->pauseNodeSize = CCValueUtil::size(*v);

		v = r.getNull("pauseTime");
		if(v!=NULL)info->pauseTime = v->floatValue();

		v = r.getNull("shadow");
		if(v!=NULL)info->shadow = v->intValue();
		
		v = r.getNull("size");
		if(v!=NULL)info->size = CCValueUtil::size(*v);

		v = r.getNull("speed");
		if(v!=NULL)info->speed = v->floatValue();

		v = r.getNull("stroke");
		if(v!=NULL)info->stroke = v->intValue();

		v = r.getNull("text");
		if(v!=NULL)info->text = v->stringValue();

		v = r.getNull("verticalAlignment");
		if(v!=NULL)info->verticalAlignment = v->intValue();
	}

	// buildup	
	build(info,m_info);
	CC_SAFE_DELETE(m_info);
	m_info = info;

	return true;
}

void CCEDialogue::onPauseNodeClick(CCNode* node, const char* name, CCNodeEvent*)
{

}

void CCEDialogue::onNarratePause(CCNode* node, const char* name, CCNodeEvent*)
{
	m_narrate->process(CCValue::nullValue());
}

void CCEDialogue::onNarrateEnd(CCNode* node, const char* name, CCNodeEvent*)
{
	
}

void CCEDialogue::updatePauseNode()
{
	
}

// cc_call
CC_BEGIN_CALLS(CCEDialogue, CCLayer)
	CC_DEFINE_CALL(CCEDialogue, show)	
CC_END_CALLS(CCEDialogue, CCLayer)

CCValue nullValue;
CCValue CCEDialogue::CALLNAME(show)(CCValueArray& params) {	
	CCValue& v = params.size()>0?params.at(0):nullValue;
	showDialogue(v, ccvp(params, 1));
	return CCValue::nullValue();
}
// end cc_call