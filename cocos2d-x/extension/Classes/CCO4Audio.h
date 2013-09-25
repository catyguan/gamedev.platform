#ifndef  __CCO_AUDIO_H__
#define  __CCO_AUDIO_H__

#include "cocos2d.h"

USING_NS_CC;

class CCO4Audio : public CCObject
{
public:
	// cc_call
	CC_DECLARE_CALLS_BEGIN
	CC_DECLARE_CALL(preloadMusic)
	CC_DECLARE_CALL(playMusic)
	CC_DECLARE_CALL(stopMusic)
	CC_DECLARE_CALL(pauseMusic)
	CC_DECLARE_CALL(resumeMusic)
	CC_DECLARE_CALL(rewindMusic)
	CC_DECLARE_CALL(willPlayMusic)
	CC_DECLARE_CALL(isMusicPlaying)
	CC_DECLARE_CALL(musicVolume)
	CC_DECLARE_CALL(preloadEffect)
	CC_DECLARE_CALL(unloadEffect)
	CC_DECLARE_CALL(playEffect)
	CC_DECLARE_CALL(stopEffect)
	CC_DECLARE_CALL(pauseEffect)
	CC_DECLARE_CALL(resumeEffect)
	CC_DECLARE_CALL(pauseAllEffects)	
	CC_DECLARE_CALL(resumeAllEffects)
	CC_DECLARE_CALL(stopAllEffects)
	CC_DECLARE_CALL(effectsVolume)	
	CC_DECLARE_CALLS_END
};

#endif