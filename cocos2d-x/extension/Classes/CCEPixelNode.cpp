#include "CCEPixelNode.h"

/// CCEPixelNode
CCEPixelNode::CCEPixelNode()
: m_cOpacity(255)
, m_tColor( ccc3(0,0,0) )
, m_size(1)
, m_pPixels(NULL)
, m_flipX(false)
, m_flipY(false)
{
    // default blend function
	m_tBlendFunc.src = CC_BLEND_SRC;
	m_tBlendFunc.dst = CC_BLEND_DST;
}
    
CCEPixelNode::~CCEPixelNode()
{	
	clearPixels();	
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
	ccePixelPart* p = m_pPixels;
	while(p!=NULL) {		
		for(int i=0; i < p->len; i++ )
		{
			for(unsigned int j=0;j<4;j++) {
				if(p->pixels[i].rgbColor) {
					p->pixels[i].color[j].r = m_tColor.r;
					p->pixels[i].color[j].g = m_tColor.g;
					p->pixels[i].color[j].b = m_tColor.b;
				}
				p->pixels[i].color[j].a = m_cOpacity;
			}
		}
		p = p->next;
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

CCEPixelNode * CCEPixelNode::create(int pixelSize, CCSize& pixelContentSize)
{
	CCEPixelNode * node = new CCEPixelNode();
	if( node && node->init(pixelSize, pixelContentSize))
	{		
		node->autorelease();
		return node;
	}
	CC_SAFE_DELETE(node);
	return NULL;
}

bool CCEPixelNode::init(int pixelSize, CCSize& pixelContentSize)
{	
	m_size = pixelSize;

	m_tBlendFunc.src = GL_SRC_ALPHA;
	m_tBlendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;

	setShaderProgram(CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionColor));

	setPixelContentSize(pixelContentSize);
	setAnchorPoint(ccp(0.5,0.5));

	return true;
}

ccePixelPart* CCEPixelNode::part(int partId)
{
	ccePixelPart* p = m_pPixels;
	while(p!=NULL) {
		if(p->id==partId)return p;
		p = p->next;
	}
	return NULL;
}

ccePixelPart* CCEPixelNode::alloc(int partId, int len)
{
	removePart(partId);

	ccePixelPart* r = new ccePixelPart();
	r->hide = true;
	r->id = partId;
	r->len = len;
	r->next = NULL;
	r->pixels = new ccePixelInfo[len];

	ccePixelPart* p = m_pPixels;
	while(p!=NULL) {
		if(p->next==NULL) {
			p->next = r;
			return r;
		}
	}
	m_pPixels = r;
	return r;
}

void CCEPixelNode::loadPart(int partId, ccePixelDef* def, int len)
{
	ccePixelPart* p = alloc(partId, len);
	for(int i=0;i<len;i++) {
		pixel(p->pixels+i, def+i);		 
	}	
}

void CCEPixelNode::setPixelsColor(int type, ccColor3B color)
{
	ccePixelPart* p = m_pPixels;
	while(p!=NULL) {
		for(int i=0; i < p->len; i++ )
		{
			if(p->pixels[i].type==type) {
				for(unsigned int j=0;j<4;j++) {
					p->pixels[i].color[j].r = color.r / 255.0f;
					p->pixels[i].color[j].g = color.g / 255.0f;
					p->pixels[i].color[j].b = color.b / 255.0f;
				}
			}
		}
		p = p->next;
	}
}

void CCEPixelNode::showPart(int partId, bool show)
{
	ccePixelPart* p = part(partId);
	if(p!=NULL)p->hide = !show;
}

void CCEPixelNode::showPixels(int type, bool show)
{
	ccePixelPart* p = m_pPixels;
	while(p!=NULL) {
		for(int i=0;i<p->len;i++) {
			if(p->pixels[i].type==type) {
				p->pixels[i].hide = !show;
			}
		}
		p = p->next;
	}
}

void CCEPixelNode::removePart(int partId)
{
	ccePixelPart* pp = NULL;
	ccePixelPart* p = m_pPixels;
	while(p!=NULL) {
		if(p->id==partId) {
			if(pp==NULL) {
				m_pPixels = NULL;
			} else {
				pp->next = p->next;
			}
			delete[] p->pixels;			
			delete p;
			return;
		}
		pp = p;
		p = p->next;
	}	
}

void CCEPixelNode::clearPixels()
{
	ccePixelPart* p = m_pPixels;
	m_pPixels = NULL;
	while(p!=NULL) {
		ccePixelPart* next = p->next;
		delete[] p->pixels;
		delete p;
		p = next;		
	}
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

	if(m_flipX || m_flipY) {
		CCPoint cp = getAnchorPointInPoints();
		for(int j=0;j<4;j++) {
			if(m_flipX) {
				info->square[j].x = cp.x + cp.x - info->square[j].x;
			}
			if(m_flipY) {
				info->square[j].y = cp.y + cp.y - info->square[j].y;
			}
		}		
	}

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
	ccePixelPart* pt = m_pPixels;
	while(pt!=NULL) {
		if(!pt->hide) {
			for(int i=0;i<pt->len;i++) {
				ccePixelInfo* p = pt->pixels+i;
				if(p->hide)continue;
				glVertexAttribPointer(kCCVertexAttrib_Position, 2, GL_FLOAT, GL_FALSE, 0, p->square);
				glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_FLOAT, GL_FALSE, 0, p->color);		
				ccGLBlendFunc( m_tBlendFunc.src, m_tBlendFunc.dst );
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}
		}
		pt = pt->next;
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

void CCEPixelNode::setPixelContentSize(CCSize& pixelContentSize)
{
	m_pixelContentSize = pixelContentSize;
	setContentSize(CCSize(m_pixelContentSize.width*m_size, m_pixelContentSize.height*m_size));
}

void CCEPixelNode::setFlipX(bool v)
{
	bool old = m_flipX?1:0;
	m_flipX = v;
	if(old==(m_flipX?1:0))return;

	// flip
	float cx = getAnchorPointInPoints().x;
	ccePixelPart* pt = m_pPixels;
	while(pt!=NULL) {
		for(int i=0;i<pt->len;i++) {
			ccePixelInfo* p = pt->pixels+i;
			for(int j=0;j<4;j++) {
				p->square[j].x = cx + cx- p->square[j].x;			
			}
		}
		pt = pt->next;
	}
}

void CCEPixelNode::setFlipY(bool v)
{
	bool old = m_flipY?1:0;
	m_flipY = v;
	if(old==(m_flipY?1:0))return;

	// flip
	float cy = getAnchorPointInPoints().y;
	ccePixelPart* pt = m_pPixels;
	while(pt!=NULL) {
		for(int i=0;i<pt->len;i++) {
			ccePixelInfo* p = pt->pixels+i;
			for(int j=0;j<4;j++) {
				p->square[j].y = cy + cy - p->square[j].y;
			}
		}
		pt = pt->next;
	}
}