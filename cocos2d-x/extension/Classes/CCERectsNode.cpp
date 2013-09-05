#include "CCERectsNode.h"

/// CCEPixelNode
CCERectsNode::CCERectsNode()
: m_dataLen(0)
, m_size(0)
, m_pInfos(NULL)
{
    // default blend function
	m_tBlendFunc.src = CC_BLEND_SRC;
	m_tBlendFunc.dst = CC_BLEND_DST;
}
    
CCERectsNode::~CCERectsNode()
{	
	clear();	
}

/// blendFunc getter
ccBlendFunc CCERectsNode::getBlendFunc()
{
	return m_tBlendFunc;
}
/// blendFunc setter
void CCERectsNode::setBlendFunc(ccBlendFunc var)
{
	m_tBlendFunc = var;
}

CCERectsNode* CCERectsNode::create()
{
	CCERectsNode *node = new CCERectsNode();
	if( node && node->init())
	{		
		node->autorelease();
		CCSize sz = CCDirector::sharedDirector()->getWinSize();
		node->setContentSize(sz);
		return node;
	}
	CC_SAFE_DELETE(node);
	return NULL;
}

bool CCERectsNode::init()
{	
	m_tBlendFunc.src = GL_SRC_ALPHA;
	m_tBlendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;

	setShaderProgram(CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionColor));
	setAnchorPoint(CCPointZero);

	return true;
}

void CCERectsNode::add(CCRect rect, ccColor4B color)
{
	int idx = m_size;
	m_size++;
	if(m_size>m_dataLen) {		
		cceRectInfo* tmp = new cceRectInfo[m_dataLen+4];		
		if(m_pInfos!=NULL) {
			memcpy(tmp, m_pInfos, m_dataLen*sizeof(cceRectInfo));
			delete[] m_pInfos;
		}
		m_pInfos = tmp;
		m_dataLen += 4;
	}		
	m_pInfos[idx].square[0].x = (GLfloat) rect.getMinX();
	m_pInfos[idx].square[0].y = (GLfloat) rect.getMinY();
	m_pInfos[idx].square[1].x = (GLfloat) rect.getMaxX();
	m_pInfos[idx].square[1].y = m_pInfos[idx].square[0].y;
	m_pInfos[idx].square[2].x = m_pInfos[idx].square[0].x;
	m_pInfos[idx].square[2].y = (GLfloat) rect.getMaxY();
	m_pInfos[idx].square[3].x = m_pInfos[idx].square[1].x;
	m_pInfos[idx].square[3].y = m_pInfos[idx].square[2].y;
	for(unsigned int j=0;j<4;j++) {
		m_pInfos[idx].color[j].r = color.r / 255.0f;
		m_pInfos[idx].color[j].g = color.g / 255.0f;
		m_pInfos[idx].color[j].b = color.b / 255.0f;
		m_pInfos[idx].color[j].a = color.a / 255.0f;
	}
}

void CCERectsNode::clear()
{
	if(m_pInfos!=NULL) {
		delete[] m_pInfos;
	}
	m_size = 0;
	m_dataLen = 0;
}

void CCERectsNode::draw()
{
	CC_NODE_DRAW_SETUP();

    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position | kCCVertexAttribFlag_Color );

    //
    // Attributes
    //
	for(int i=0;i<m_size;i++) {
		cceRectInfo* p = m_pInfos+i;		
		glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, p->square);
		glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_FLOAT, GL_FALSE, 0, p->color);		
		ccGLBlendFunc( m_tBlendFunc.src, m_tBlendFunc.dst );
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

    CC_INCREMENT_GL_DRAWS(1);
}
