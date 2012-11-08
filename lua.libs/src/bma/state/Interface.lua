-- bma/state/Interface.lua

--[[
class bma.host.Service
{
	function loadObjectData(callback:function(err,data,version), appId:string, appType:string, objId:string, syn:boolean)

	function tryCall(opts, ...):callId:int or Boolean
	
	function canCall():Boolean
	
	functio discardCallResponse(callId):void
	
	function setTimer(fun:timerFunction, delayTime:int):timerId:int
		fun 定时调用的哦函数
		delayTime 延迟调用的时间，毫秒
		timerId 定时器的编号
		
	function removeTImer(timerId:int):void
}
]] 