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

#ifndef __CCEDITTEXT_H__
#define __CCEDITTEXT_H__

#include "cocos2d.h"
#include "../CCEControl.h"

USING_NS_CC;


enum KeyboardReturnType {
    kKeyboardReturnTypeDefault = 0,
    kKeyboardReturnTypeDone,
    kKeyboardReturnTypeSend,
    kKeyboardReturnTypeSearch,
    kKeyboardReturnTypeGo
};


/**
 * \brief The EditBoxInputMode defines the type of text that the user is allowed
 * to enter.
 */
enum EditBoxInputMode
{
    /**
     * The user is allowed to enter any text, including line breaks.
     */
    kEditBoxInputModeAny = 0,
    
    /**
     * The user is allowed to enter an e-mail address.
     */
    kEditBoxInputModeEmailAddr,

    /**
     * The user is allowed to enter an integer value.
     */
    kEditBoxInputModeNumeric,

    /**
     * The user is allowed to enter a phone number.
     */
    kEditBoxInputModePhoneNumber,

    /**
     * The user is allowed to enter a URL.
     */
    kEditBoxInputModeUrl,

    /**
     * The user is allowed to enter a real number value.
     * This extends kEditBoxInputModeNumeric by allowing a decimal point.
     */
    kEditBoxInputModeDecimal,

    /**
     * The user is allowed to enter any text, except for line breaks.
     */
    kEditBoxInputModeSingleLine
};

/**
 * \brief The EditBoxInputFlag defines how the input text is displayed/formatted.
 */
enum EditBoxInputFlag
{
    /**
     * Indicates that the text entered is confidential data that should be
     * obscured whenever possible. This implies EDIT_BOX_INPUT_FLAG_SENSITIVE.
     */
    kEditBoxInputFlagPassword = 0,

    /**
     * Indicates that the text entered is sensitive data that the
     * implementation must never store into a dictionary or table for use
     * in predictive, auto-completing, or other accelerated input schemes.
     * A credit card number is an example of sensitive data.
     */
    kEditBoxInputFlagSensitive,

    /**
     * This flag is a hint to the implementation that during text editing,
     * the initial letter of each word should be capitalized.
     */
    kEditBoxInputFlagInitialCapsWord,

    /**
     * This flag is a hint to the implementation that during text editing,
     * the initial letter of each sentence should be capitalized.
     */
    kEditBoxInputFlagInitialCapsSentence,

    /**
     * Capitalize all characters automatically.
     */
    kEditBoxInputFlagInitialCapsAllCharacters

};

#define NODE_EVENT_EDITBOX_CHANGED	"editboxChanged"
#define NODE_EVENT_EDITBOX_ENDED	"editboxEnded"
#define NODE_EVENT_EDITBOX_RETURN	"editboxReturn"
#define NODE_EVENT_EDITBOX_BEGAN	"editboxBegan"

class CCEditBox;
class CCEditBoxImpl;


class CCEditBoxDelegate 
{
public:
    virtual ~CCEditBoxDelegate() {};
    
    /**
     * This method is called when an edit box gains focus after keyboard is shown.
     * @param editBox The edit box object that generated the event.
     */
    virtual void editBoxEditingDidBegin(CCEditBox* editBox) {};
    
    
    /**
     * This method is called when an edit box loses focus after keyboard is hidden.
     * @param editBox The edit box object that generated the event.
     */
    virtual void editBoxEditingDidEnd(CCEditBox* editBox) {};
    
    /**
     * This method is called when the edit box text was changed.
     * @param editBox The edit box object that generated the event.
     * @param text The new text.
     */
    virtual void editBoxTextChanged(CCEditBox* editBox, const std::string& text) {};
    
    /**
     * This method is called when the return button was pressed or the outside area of keyboard was touched.
     * @param editBox The edit box object that generated the event.
     */
    virtual void editBoxReturn(CCEditBox* editBox) = 0;
    
};

/**
 * \brief Class for edit box.
 *
 * You can use this widget to gather small amounts of text from the user.
 * 
 */
 
class CCEditBox
: public CCEControl
, public CCIMEDelegate
{
public:
    /**
     * Constructor.
     */
    CCEditBox(void);
    
    /**
     * Destructor.
     */
    virtual ~CCEditBox(void);

    /**
     * create a edit box with size.
     * @return An autorelease pointer of CCEditBox, you don't need to release it only if you retain it again.
     */
    static CCEditBox* create(const CCSize& size);
	bool initWithSize(const CCSize& size);
    
    /**
     * Gets/Sets the delegate for edit box.
     */
    void setDelegate(CCEditBoxDelegate* pDelegate);
    CCEditBoxDelegate* getDelegate();
    
    /**
     * Set the text entered in the edit box.
     * @param pText The given text.
     */
    void setText(const char* pText);
    
    /**
     * Get the text entered in the edit box.
     * @return The text entered in the edit box.
     */
    const char* getText(void);
	
	/**
	 * Set the font.
	 * @param pFontName The font name.
	 * @param fontSize The font size.
	 */
	void setFont(const char* pFontName, int fontSize);
    
	/**
	 * Set the font name.
	 * @param pFontName The font name.
	 */
	void setFontName(const char* pFontName);
    
    /**
	 * Set the font size.
	 * @param fontSize The font size.
	 */
	void setFontSize(int fontSize);
    
    /**
     * Set the font color of the widget's text.
     */
    void setFontColor(const ccColor3B& color);
    
	/**
	 * Set the placeholder's font.
	 * @param pFontName The font name.
	 * @param fontSize The font size.
	 */
	void setPlaceholderFont(const char* pFontName, int fontSize);
    
    /**
	 * Set the placeholder's font name.
	 * @param pFontName The font name.
	 */
	void setPlaceholderFontName(const char* pFontName);
    
    /**
	 * Set the placeholder's font size.
	 * @param fontSize The font size.
	 */
	void setPlaceholderFontSize(int fontSize);
    
    /**
     * Set the font color of the placeholder text when the edit box is empty.
     * Not supported on IOS.
     */
    void setPlaceholderFontColor(const ccColor3B& color);
    
    /**
     * Set a text in the edit box that acts as a placeholder when an
     * edit box is empty.
     * @param pText The given text.
     */
    void setPlaceHolder(const char* pText);
    
    /**
     * Get a text in the edit box that acts as a placeholder when an
     * edit box is empty.
     */
    const char* getPlaceHolder(void);
    
    /**
     * Set the input mode of the edit box.
     * @param inputMode One of the EditBoxInputMode constants.
     */
    void setInputMode(EditBoxInputMode inputMode);
    
    /**
     * Sets the maximum input length of the edit box.
     * Setting this value enables multiline input mode by default.
     * Available on Android, iOS and Windows Phone.
     *
     * @param maxLength The maximum length.
     */
    void setMaxLength(int maxLength);
    
    /**
     * Gets the maximum input length of the edit box.
     *
     * @return Maximum input length.
     */
    int getMaxLength();
    
    /**
     * Set the input flags that are to be applied to the edit box.
     * @param inputFlag One of the EditBoxInputFlag constants.
     */
    void setInputFlag(EditBoxInputFlag inputFlag);
    
    /**
     * Set the return type that are to be applied to the edit box.
     * @param returnType One of the CCKeyboardReturnType constants.
     */
    void setReturnType(KeyboardReturnType returnType);
    
    /* override functions */
    virtual void setPosition(const CCPoint& pos);
    virtual void setVisible(bool visible);
    virtual void setContentSize(const CCSize& size);
	virtual void setAnchorPoint(const CCPoint& anchorPoint);
    virtual void visit(void);
	virtual void onEnter(void);
    virtual void onExit(void);
    virtual void keyboardWillShow(CCIMEKeyboardNotificationInfo& info);
    virtual void keyboardDidShow(CCIMEKeyboardNotificationInfo& info);
    virtual void keyboardWillHide(CCIMEKeyboardNotificationInfo& info);
    virtual void keyboardDidHide(CCIMEKeyboardNotificationInfo& info);
    
    /* callback funtions */	
    void touchDownAction(CCNode* node, const char* name, CCNodeEvent*);

	virtual bool hasEventHandler(const char* name);
	virtual bool raiseEvent(const char* name, CCNodeEvent*);

	// cc_call
	CC_DECLARE_CALLS_BEGIN	
	CC_DECLARE_CALL(text)
	CC_DECLARE_CALL(fontName)
	CC_DECLARE_CALL(fontSize)
	CC_DECLARE_CALL(fontColor)
	CC_DECLARE_CALL(placeholderText)
	CC_DECLARE_CALL(placeholderFontName)
	CC_DECLARE_CALL(placeholderFontSize)
	CC_DECLARE_CALL(placeholderFontColor)
	CC_DECLARE_CALL(maxLength)
	CC_DECLARE_CALL(inputMode)
	CC_DECLARE_CALL(inputFlag)
	CC_DECLARE_CALL(returnType)
	CC_DECLARE_CALLS_END
	// end cc_call
    
protected:
    CCEditBoxImpl*      m_pEditBoxImpl;
    CCEditBoxDelegate*  m_pDelegate;
    
    EditBoxInputMode    m_eEditBoxInputMode;
    EditBoxInputFlag    m_eEditBoxInputFlag;
    KeyboardReturnType  m_eKeyboardReturnType;
    
    std::string m_strText;
    std::string m_strPlaceHolder;
    
    std::string m_strFontName;
    std::string m_strPlaceholderFontName;
    
    int m_nFontSize;
    int m_nPlaceholderFontSize;
    
    ccColor3B m_colText;
    ccColor3B m_colPlaceHolder;
    
    int   m_nMaxLength;
    float m_fAdjustHeight;
};

#endif /* __CCEDITTEXT_H__ */

