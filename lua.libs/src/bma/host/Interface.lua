-- bma/host/Interface.lua

--[[
调用响应的回调函数
function responseFunction(error,...);
	err 是否错误，如果错误，后续第一个参数为错误消息，否则后续范围响应的结果
	
function successFunction(...);
function errorFunction(errorMessage);

function timerFunction();

opts : {
	'response' = 
	'success' = 
	'error' = 
	'timeout' = 
}

class bma.host.Service
{
	function call(opts, ...):callId:int

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