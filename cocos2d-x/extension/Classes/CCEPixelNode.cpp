#include "CCEPixelNode.h"

/// CCEPixelNode
CCEPixelNode::CCEPixelNode()
: m_cOpacity(255)
, m_tColor( ccc3(0,0,0) )
, m_size(1)
, m_len(0)
, m_plen(0)
, m_pPixels(NULL)
{
    // default blend function
	m_tBlendFunc.src = CC_BLEND_SRC;
	m_tBlendFunc.dst = CC_BLEND_DST;
}
    
CCEPixelNode::~CCEPixelNode()
{	
	if(m_pPixels!=NULL) {
		delete[] m_pPixels;
	}
}

// Opacity and RGB color protocol
/// opacity getter
GLubyte CCEPixelNode::getOpacity()
{
	return m_cOpacity;
}
/// opacity setter
void CCEPixelNode::setOpacity(GLubyte var)
{
	m_cOpacity = var;
	updateColor();
}

/// color getter
ccColor3B CCEPixelNode::getColor()
{
	return m_tColor;
}

/// color setter
void CCEPixelNode::setColor(const ccColor3B& var)
{
	m_tColor = var;
	updateColor();
}

void CCEPixelNode::updateColor()
{
	for(int i=0; i < m_len; i++ )
	{
		if(m_pPixels[i].hide)continue;
		for(unsigned int j=0;j<4;j++) {
			if(m_pPixels[i].rgbColor) {
				m_pPixels[i].color[j].r = m_tColor.r;
				m_pPixels[i].color[j].g = m_tColor.g;
				m_pPixels[i].color[j].b = m_tColor.b;
			}
			m_pPixels[i].color[j].a = m_cOpacity;
		}
	}
}

/// blendFunc getter
ccBlendFunc CCEPixelNode::getBlendFunc()
{
	return m_tBlendFunc;
}
/// blendFunc setter
void CCEPixelNode::setBlendFunc(ccBlendFunc var)
{
	m_tBlendFunc = var;
}

CCEPixelNode * CCEPixelNode::create(int pixelSize)
{
	CCEPixelNode * node = new CCEPixelNode();
	if( node && node->init(pixelSize))
	{		
		node->autorelease();
		return node;
	}
	CC_SAFE_DELETE(node);
	return NULL;
}

bool CCEPixelNode::init(int pixelSize)
{	
	m_size = pixelSize;

	m_tBlendFunc.src = GL_SRC_ALPHA;
	m_tBlendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;

	setShaderProgram(CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionColor));

	return true;
}

void CCEPixelNode::alloc(int len)
{
	if(m_plen<len) {
		int newsize = max(m_plen+8,len);
		ccePixelInfo* tmp = new ccePixelInfo[newsize];
		if(m_pPixels!=NULL) {
			memcpy(tmp, m_pPixels, sizeof(ccePixelInfo)*m_len);
			delete[] m_pPixels;
		}
		m_pPixels = tmp;
		m_plen = newsize;
	}
}

void CCEPixelNode::addPixels(ccePixelDef* pdef,int len)
{
	alloc(m_len+len);
	for(int i=0;i<len;i++) {
		 pixel(m_pPixels+m_len, pdef+i); 
		 m_len++;
	}	
}

void CCEPixelNode::setPixels(ccePixelDef* pdef,int len)
{
	for(int j=0;j<len;j++) {
		ccePixelDef* def = pdef+j;
		for(int i=0;i<m_len;i++) {
			if(m_pPixels[i].type==def->type) {
				pixel(m_pPixels+i, def); 
			}
		}
	}
}

void CCEPixelNode::setPixelsColor(int type, ccColor3B color)
{
	for(int i=0; i < m_len; i++ )
	{
		if(m_pPixels[i].hide)continue;
		if(m_pPixels[i].type==type) {
			for(unsigned int j=0;j<4;j++) {
				m_pPixels[i].color[j].r = color.r / 255.0f;
				m_pPixels[i].color[j].g = color.g / 255.0f;
				m_pPixels[i].color[j].b = color.b / 255.0f;
			}
		}
	}
}

void CCEPixelNode::hidePixels(int type, bool hide)
{
	for(int i=0;i<m_len;i++) {
		if(m_pPixels[i].type==type) {
			m_pPixels[i].hide = hide;
		}
	}
}

void CCEPixelNode::removePixels(int type)
{
	for(int i=m_len-1;i>=0;i--) {
		if(m_pPixels[i].type==type) {
			if(i<m_len-1) {
				memcpy(m_pPixels+i, m_pPixels+i+1, sizeof(ccePixelInfo)*(m_len-i));
			}
			m_len--;
		}
	}	
}

void CCEPixelNode::clearPixels()
{
	m_len = 0;
}

void CCEPixelNode::pixel(ccePixelInfo* info, ccePixelDef* def)
{
	info->type = def->type;

	info->square[0].x = (GLfloat) def->x*m_size;
	info->square[0].y = (GLfloat) def->y*m_size;
	info->square[1].x = (GLfloat) (def->x+def->width)*m_size;
	info->square[1].y = info->square[0].y;
	info->square[2].x = info->square[0].x;
	info->square[2].y = (GLfloat) (def->y+def->height)*m_size;
	info->square[3].x = info->square[1].x;
	info->square[3].y = info->square[2].y;

	for(int i=0;i<4;i++) {
		if(def->rgbColor) {
			info->color[i].r = m_tColor.r / 255.0f;
			info->color[i].g = m_tColor.g / 255.0f;
			info->color[i].b = m_tColor.b / 255.0f;
		} else {
			info->color[i].r = def->r / 255.0f;
			info->color[i].g = def->g / 255.0f;
			info->color[i].b = def->b / 255.0f;
		}
		info->color[i].a = m_cOpacity / 255.0f;		
	}
	info->rgbColor = def->rgbColor;
	info->hide = def->hide;
}

void CCEPixelNode::draw()
{
	CC_NODE_DRAW_SETUP();

    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position | kCCVertexAttribFlag_Color );

    //
    // Attributes
    //
	for(int i=0;i<m_len;i++) {
		ccePixelInfo* p = m_pPixels+i;
		if(p->hide)continue;
		glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, p->square);
		glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_FLOAT, GL_FALSE, 0, p->color);		
		ccGLBlendFunc( m_tBlendFunc.src, m_tBlendFunc.dst );
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

    CC_INCREMENT_GL_DRAWS(1);	
}


int CCEPixelNode::count(ccePixelDef* pdef)
{
	for(int i=0;;i++) {
		if(pdef[i].type<0)return i;
	}
	return 0;
}