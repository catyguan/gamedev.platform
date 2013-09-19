/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2012 James Chen
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "CCEditBox.h"
#include "CCEditBoxImpl.h"
#include "..\CCETouch.h"
#include "cocoa\CCValueSupport.h"

USING_NS_CC;

CCEditBox::CCEditBox(void)
: m_pEditBoxImpl(NULL)
, m_pDelegate(NULL)
, m_eEditBoxInputMode(kEditBoxInputModeSingleLine)
, m_eEditBoxInputFlag(kEditBoxInputFlagInitialCapsAllCharacters)
, m_eKeyboardReturnType(kKeyboardReturnTypeDefault)
, m_nFontSize(-1)
, m_nPlaceholderFontSize(-1)
, m_colText(ccWHITE)
, m_colPlaceHolder(ccGRAY)
, m_nMaxLength(0)
, m_fAdjustHeight(0.0f)
{
}

CCEditBox::~CCEditBox(void)
{
    CC_SAFE_DELETE(m_pEditBoxImpl);
}

bool CCEditBox::hasEventHandler(const char* name)
{
	bool r = CCEControl::hasEventHandler(name);
	if(strcmp(name, NODE_EVENT_TAP)==0)return true;
	return r;
}

bool CCEditBox::raiseEvent(const char* name, CCNodeEvent* e)
{	
	bool r = CCEControl::raiseEvent(name,e);
	if(strcmp(name,NODE_EVENT_TAP)==0) {
		touchDownAction(this, name, e);
	}
	return r;
}

void CCEditBox::touchDownAction(CCNode* node, const char* name, CCNodeEvent*)
{
    m_pEditBoxImpl->openKeyboard();
}

CCEditBox* CCEditBox::create(const CCSize& size)
{
    CCEditBox* pRet = new CCEditBox();
    
    if (pRet != NULL && pRet->initWithSize(size))
    {        
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    
    return pRet;
}

bool CCEditBox::initWithSize(const CCSize& size)
{
    if (CCEControl::init())
    {
        m_pEditBoxImpl = __createSystemEditBox(this);
        m_pEditBoxImpl->initWithSize(size);        
        
		this->setContentSize(size);
        this->setPosition(ccp(0, 0));
        
        return true;
    }
    return false;
}

void CCEditBox::setDelegate(CCEditBoxDelegate* pDelegate)
{
    m_pDelegate = pDelegate;
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->setDelegate(pDelegate);
    }
}

CCEditBoxDelegate* CCEditBox::getDelegate()
{
    return m_pDelegate;
}

void CCEditBox::setText(const char* pText)
{
    if (pText != NULL)
    {
        m_strText = pText;
        if (m_pEditBoxImpl != NULL)
        {
            m_pEditBoxImpl->setText(pText);
        }
    }
}

const char* CCEditBox::getText(void)
{
    if (m_pEditBoxImpl != NULL)
    {
		const char* pText = m_pEditBoxImpl->getText();
		if(pText != NULL)
			return pText;
    }
    
    return "";
}

void CCEditBox::setFont(const char* pFontName, int fontSize)
{
    m_strFontName = pFontName;
    m_nFontSize = fontSize;
    if (pFontName != NULL)
    {
        if (m_pEditBoxImpl != NULL)
        {
            m_pEditBoxImpl->setFont(pFontName, fontSize);
        }
    }
}

void CCEditBox::setFontName(const char* pFontName)
{
    m_strFontName = pFontName;
    if (m_pEditBoxImpl != NULL && m_nFontSize != -1)
    {
        m_pEditBoxImpl->setFont(pFontName, m_nFontSize);
    }
}

void CCEditBox::setFontSize(int fontSize)
{
    m_nFontSize = fontSize;
    if (m_pEditBoxImpl != NULL && m_strFontName.length() > 0)
    {
        m_pEditBoxImpl->setFont(m_strFontName.c_str(), m_nFontSize);
    }
}

void CCEditBox::setFontColor(const ccColor3B& color)
{
    m_colText = color;
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->setFontColor(color);
    }
}

void CCEditBox::setPlaceholderFont(const char* pFontName, int fontSize)
{
    m_strPlaceholderFontName = pFontName;
    m_nPlaceholderFontSize = fontSize;
    if (pFontName != NULL)
    {
        if (m_pEditBoxImpl != NULL)
        {
            m_pEditBoxImpl->setPlaceholderFont(pFontName, fontSize);
        }
    }
}

void CCEditBox::setPlaceholderFontName(const char* pFontName)
{
    m_strPlaceholderFontName = pFontName;
    if (m_pEditBoxImpl != NULL && m_nPlaceholderFontSize != -1)
    {
        m_pEditBoxImpl->setPlaceholderFont(pFontName, m_nFontSize);
    }
}

void CCEditBox::setPlaceholderFontSize(int fontSize)
{
    m_nPlaceholderFontSize = fontSize;
    if (m_pEditBoxImpl != NULL && m_strPlaceholderFontName.length() > 0)
    {
        m_pEditBoxImpl->setPlaceholderFont(m_strPlaceholderFontName.c_str(), m_nFontSize);
    }
}

void CCEditBox::setPlaceholderFontColor(const ccColor3B& color)
{
    m_colText = color;
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->setPlaceholderFontColor(color);
    }
}

void CCEditBox::setPlaceHolder(const char* pText)
{
    if (pText != NULL)
    {
        m_strPlaceHolder = pText;
        if (m_pEditBoxImpl != NULL)
        {
            m_pEditBoxImpl->setPlaceHolder(pText);
        }
    }
}

const char* CCEditBox::getPlaceHolder(void)
{
    return m_strPlaceHolder.c_str();
}

void CCEditBox::setInputMode(EditBoxInputMode inputMode)
{
    m_eEditBoxInputMode = inputMode;
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->setInputMode(inputMode);
    }
}

void CCEditBox::setMaxLength(int maxLength)
{
    m_nMaxLength = maxLength;
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->setMaxLength(maxLength);
    }
}


int CCEditBox::getMaxLength()
{
    return m_nMaxLength;
}

void CCEditBox::setInputFlag(EditBoxInputFlag inputFlag)
{
    m_eEditBoxInputFlag = inputFlag;
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->setInputFlag(inputFlag);
    }
}

void CCEditBox::setReturnType(KeyboardReturnType returnType)
{
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->setReturnType(returnType);
    }
}

/* override function */
void CCEditBox::setPosition(const CCPoint& pos)
{
    CCEControl::setPosition(pos);
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->setPosition(pos);
    }
}

void CCEditBox::setVisible(bool visible)
{
    CCEControl::setVisible(visible);
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->setVisible(visible);
    }
}

void CCEditBox::setContentSize(const CCSize& size)
{
    CCEControl::setContentSize(size);
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->setContentSize(size);
    }
}

void CCEditBox::setAnchorPoint(const CCPoint& anchorPoint)
{
    CCEControl::setAnchorPoint(anchorPoint);
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->setAnchorPoint(anchorPoint);
    }
}

void CCEditBox::visit(void)
{
    CCEControl::visit();
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->visit();
    }
}

void CCEditBox::onEnter(void)
{
    CCEControl::onEnter();
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->onEnter();
    }
}

void CCEditBox::onExit(void)
{
    CCEControl::onExit();
    if (m_pEditBoxImpl != NULL)
    {
        // remove system edit control
        m_pEditBoxImpl->closeKeyboard();
    }
}

static CCRect getRect(CCNode * pNode)
{
	CCSize contentSize = pNode->getContentSize();
	CCRect rect = CCRectMake(0, 0, contentSize.width, contentSize.height);
	return CCRectApplyAffineTransform(rect, pNode->nodeToWorldTransform());
}

void CCEditBox::keyboardWillShow(CCIMEKeyboardNotificationInfo& info)
{
    // CCLOG("CCEditBox::keyboardWillShow");
    CCRect rectTracked = getRect(this);
	// some adjustment for margin between the keyboard and the edit box.
	rectTracked.origin.y -= 4;

    // if the keyboard area doesn't intersect with the tracking node area, nothing needs to be done.
    if (!rectTracked.intersectsRect(info.end))
    {
        CCLOG("needn't to adjust view layout.");
        return;
    }
    
    // assume keyboard at the bottom of screen, calculate the vertical adjustment.
    m_fAdjustHeight = info.end.getMaxY() - rectTracked.getMinY();
    // CCLOG("CCEditBox:needAdjustVerticalPosition(%f)", m_fAdjustHeight);
    
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->doAnimationWhenKeyboardMove(info.duration, m_fAdjustHeight);
    }
}

void CCEditBox::keyboardDidShow(CCIMEKeyboardNotificationInfo& info)
{
	
}

void CCEditBox::keyboardWillHide(CCIMEKeyboardNotificationInfo& info)
{
    // CCLOG("CCEditBox::keyboardWillHide");
    if (m_pEditBoxImpl != NULL)
    {
        m_pEditBoxImpl->doAnimationWhenKeyboardMove(info.duration, -m_fAdjustHeight);
    }
}

void CCEditBox::keyboardDidHide(CCIMEKeyboardNotificationInfo& info)
{
	
}

// cc_call
CC_BEGIN_CALLS(CCEditBox, CCEControl)
	CC_DEFINE_CALL(CCEditBox, text)
	CC_DEFINE_CALL(CCEditBox, fontName)
	CC_DEFINE_CALL(CCEditBox, fontSize)
	CC_DEFINE_CALL(CCEditBox, fontColor)
	CC_DEFINE_CALL(CCEditBox, placeholderText)	
	CC_DEFINE_CALL(CCEditBox, placeholderFontName)
	CC_DEFINE_CALL(CCEditBox, placeholderFontSize)
	CC_DEFINE_CALL(CCEditBox, placeholderFontColor)
	CC_DEFINE_CALL(CCEditBox, maxLength)
	CC_DEFINE_CALL(CCEditBox, inputMode)
	CC_DEFINE_CALL(CCEditBox, inputFlag)
	CC_DEFINE_CALL(CCEditBox, returnType)
CC_END_CALLS(CCEditBox, CCEControl)

CCValue CCEditBox::CALLNAME(text)(CCValueArray& params) {
	if(params.size()>0) {
		setText(params[0].stringValue().c_str());
	}
	return CCValue::stringValue(getText());
}

CCValue CCEditBox::CALLNAME(fontName)(CCValueArray& params) {
	if(params.size()>0) {
		setFontName(params[0].stringValue().c_str());
	}
	return CCValue::nullValue();
}

CCValue CCEditBox::CALLNAME(fontSize)(CCValueArray& params) {
	if(params.size()>0) {
		int sz = params[0].intValue();
		setFontSize(sz);
	}
	return CCValue::nullValue();
}

CCValue CCEditBox::CALLNAME(fontColor)(CCValueArray& params) {
	if(params.size()>0) {
		ccColor3B color = CCValueUtil::color3b(params[0]);
		setFontColor(color);
	}
	return CCValue::nullValue();
}

CCValue CCEditBox::CALLNAME(placeholderText)(CCValueArray& params) {
	if(params.size()>0) {
		setPlaceHolder(params[0].stringValue().c_str());
	}
	return CCValue::stringValue(getPlaceHolder());
}

CCValue CCEditBox::CALLNAME(placeholderFontName)(CCValueArray& params) {
	if(params.size()>0) {
		setPlaceholderFontName(params[0].stringValue().c_str());
	}
	return CCValue::nullValue();
}

CCValue CCEditBox::CALLNAME(placeholderFontSize)(CCValueArray& params) {
	if(params.size()>0) {
		setPlaceholderFontSize(params[0].intValue());
	}
	return CCValue::nullValue();
}

CCValue CCEditBox::CALLNAME(placeholderFontColor)(CCValueArray& params) {
	if(params.size()>0) {
		ccColor3B color = CCValueUtil::color3b(params[0]);
		setPlaceholderFontColor(color);
	}
	return CCValue::nullValue();
}

CCValue CCEditBox::CALLNAME(maxLength)(CCValueArray& params) {
	if(params.size()>0) {
		setMaxLength(params[0].intValue());
	}
	return CCValue::intValue(getMaxLength());
}

CCValue CCEditBox::CALLNAME(inputMode)(CCValueArray& params) {
	if(params.size()>0) {
		setInputMode(EditBoxInputMode(params[0].intValue()));
	}
	return CCValue::nullValue();
}

CCValue CCEditBox::CALLNAME(inputFlag)(CCValueArray& params) {
	if(params.size()>0) {
		setInputFlag(EditBoxInputFlag(params[0].intValue()));
	}
	return CCValue::nullValue();
}

CCValue CCEditBox::CALLNAME(returnType)(CCValueArray& params) {
	if(params.size()>0) {
		setReturnType(KeyboardReturnType(params[0].intValue()));
	}
	return CCValue::nullValue();
}