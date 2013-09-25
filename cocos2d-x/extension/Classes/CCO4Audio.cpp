#include "CCO4Audio.h"

#include "SimpleAudioEngine.h"
#include "cocoa/CCValueSupport.h"

CC_BEGIN_CALLS(CCO4Audio, CCObject)
	CC_DEFINE_CALL(CCO4Audio, preloadMusic)
	CC_DEFINE_CALL(CCO4Audio, playMusic)
	CC_DEFINE_CALL(CCO4Audio, stopMusic)
	CC_DEFINE_CALL(CCO4Audio, pauseMusic)
	CC_DEFINE_CALL(CCO4Audio, resumeMusic)
	CC_DEFINE_CALL(CCO4Audio, rewindMusic)
	CC_DEFINE_CALL(CCO4Audio, willPlayMusic)
	CC_DEFINE_CALL(CCO4Audio, isMusicPlaying)
	CC_DEFINE_CALL(CCO4Audio, musicVolume)
	CC_DEFINE_CALL(CCO4Audio, preloadEffect)
	CC_DEFINE_CALL(CCO4Audio, unloadEffect)
	CC_DEFINE_CALL(CCO4Audio, playEffect)
	CC_DEFINE_CALL(CCO4Audio, stopEffect)
	CC_DEFINE_CALL(CCO4Audio, pauseEffect)
	CC_DEFINE_CALL(CCO4Audio, resumeEffect)
	CC_DEFINE_CALL(CCO4Audio, pauseAllEffects)	
	CC_DEFINE_CALL(CCO4Audio, resumeAllEffects)
	CC_DEFINE_CALL(CCO4Audio, stopAllEffects)
	CC_DEFINE_CALL(CCO4Audio, effectsVolume)	
CC_END_CALLS(CCO4Audio, CCObject)

using namespace CocosDenshion;

CCValue CCO4Audio::CALLNAME(preloadMusic)(CCValueArray& params)
{
	std::string p1 = ccvpString(params, 0);
	if(p1.size()>0) {
		SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic(p1.c_str());
	}
	return CCValue::nullValue();
}

CCValue CCO4Audio::CALLNAME(playMusic)(CCValueArray& params)
{
	std::string p1 = ccvpString(params, 0);
	bool p2 = ccvpBoolean(params, 1);	
	if(p1.size()>0) {
		SimpleAudioEngine::sharedEngine()->playBackgroundMusic(p1.c_str(), p2);
	}
	return CCValue::nullValue();
}

CCValue CCO4Audio::CALLNAME(stopMusic)(CCValueArray& params)
{
	bool p1 = ccvpBoolean(params, 0);
	SimpleAudioEngine::sharedEngine()->stopBackgroundMusic(p1);	
	return CCValue::nullValue();
}

CCValue CCO4Audio::CALLNAME(pauseMusic)(CCValueArray& params)
{
	SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
	return CCValue::nullValue();
}

CCValue CCO4Audio::CALLNAME(resumeMusic)(CCValueArray& params)
{
	SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
	return CCValue::nullValue();
}

CCValue CCO4Audio::CALLNAME(rewindMusic)(CCValueArray& params)
{
	SimpleAudioEngine::sharedEngine()->rewindBackgroundMusic();
	return CCValue::nullValue();
}

CCValue CCO4Audio::CALLNAME(willPlayMusic)(CCValueArray& params)
{
	bool r = SimpleAudioEngine::sharedEngine()->willPlayBackgroundMusic();
	return CCValue::booleanValue(r);
}

CCValue CCO4Audio::CALLNAME(isMusicPlaying)(CCValueArray& params)
{
	bool r = SimpleAudioEngine::sharedEngine()->isBackgroundMusicPlaying();
	return CCValue::booleanValue(r);
}

CCValue CCO4Audio::CALLNAME(musicVolume)(CCValueArray& params)
{
	if(params.size()>0) {
		float v = params[0].floatValue();
		SimpleAudioEngine::sharedEngine()->setBackgroundMusicVolume(v);
	}
	float v = SimpleAudioEngine::sharedEngine()->getBackgroundMusicVolume();
	return CCValue::numberValue(v);
}

CCValue CCO4Audio::CALLNAME(preloadEffect)(CCValueArray& params)
{
	std::string p1 = ccvpString(params, 0);
	if(p1.size()>0) {
		SimpleAudioEngine::sharedEngine()->preloadEffect(p1.c_str());
	}
	return CCValue::nullValue();
}

CCValue CCO4Audio::CALLNAME(unloadEffect)(CCValueArray& params)
{
	std::string p1 = ccvpString(params, 0);
	if(p1.size()>0) {
		SimpleAudioEngine::sharedEngine()->unloadEffect(p1.c_str());
	}
	return CCValue::nullValue();
}

CCValue CCO4Audio::CALLNAME(playEffect)(CCValueArray& params)
{
	std::string p1 = ccvpString(params, 0);
	bool p2 = ccvpBoolean(params, 1);	
	int r = -1;
	if(p1.size()>0) {
		r = SimpleAudioEngine::sharedEngine()->playEffect(p1.c_str(), p2);
	}
	return CCValue::intValue(r);
}

CCValue CCO4Audio::CALLNAME(stopEffect)(CCValueArray& params)
{
	int id = ccvpInt(params, 0);
	SimpleAudioEngine::sharedEngine()->stopEffect(id);
	return CCValue::nullValue();
}

CCValue CCO4Audio::CALLNAME(pauseEffect)(CCValueArray& params)
{
	int id = ccvpInt(params, 0);
	SimpleAudioEngine::sharedEngine()->pauseEffect(id);
	return CCValue::nullValue();
}

CCValue CCO4Audio::CALLNAME(resumeEffect)(CCValueArray& params)
{
	int id = ccvpInt(params, 0);
	SimpleAudioEngine::sharedEngine()->resumeEffect(id);
	return CCValue::nullValue();
}

CCValue CCO4Audio::CALLNAME(pauseAllEffects)(CCValueArray& params)
{
	SimpleAudioEngine::sharedEngine()->pauseAllEffects();
	return CCValue::nullValue();
}

CCValue CCO4Audio::CALLNAME(resumeAllEffects)(CCValueArray& params)
{
	SimpleAudioEngine::sharedEngine()->resumeAllEffects();
	return CCValue::nullValue();
}

CCValue CCO4Audio::CALLNAME(stopAllEffects)(CCValueArray& params)
{
	SimpleAudioEngine::sharedEngine()->stopAllEffects();
	return CCValue::nullValue();
}

CCValue CCO4Audio::CALLNAME(effectsVolume)(CCValueArray& params)
{
	if(params.size()>0) {
		float v = params[0].floatValue();
		SimpleAudioEngine::sharedEngine()->setEffectsVolume(v);
	}
	float v = SimpleAudioEngine::sharedEngine()->getEffectsVolume();
	return CCValue::numberValue(v);
}