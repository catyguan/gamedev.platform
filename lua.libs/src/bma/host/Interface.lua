-- bma/host/Interface.lua

--[[
AInvoke的回调函数
function responseFunction(errorMessage,...);	
	
成功的回调函数
function successFunction(...);

失败的回调函数
function errorFunction(errorMessage);

复合AInvoke的回调数据
opts : {
	'response' = 
	'success' = 
	'error' = 
	'timeout' = 
};

定时器的回调函数
function timerFunction();

传递给host的闭包函数
function closureFunction(errorMessage,...);

class bma.host.Service
{
	function invoke(...): VAR
	
	function ainvoke(opts, ...):int[callId]

	function discardAInvokeResponse(callId:int):void
		callId 等待响应的调用编号
	
	function wrapClosure(f:function):int[callId]
		f 包装的函数
		callId 包装后的调用编号
		
	function removeClosure(callId:int):void
		callId 包装后的调用编号
	
	function setTimer(fun:timerFunction, delayTime:int): int[timerId]
		fun 定时调用的函数
		delayTime 延迟调用的时间，毫秒
		timerId 定时任务的编号
		
	function setFixTimer(fun:timerFunction, delayTime:int, fixTime:int): int[timerId]
		fun 定时调用的哦函数
		delayTime 延迟调用的时间，毫秒
		fixTime 固定调用的间隔时间，毫秒
		timerId 定时任务的编号
		
	function removeTimer(timerId:int):void
		timerId 定时任务编号
}
]] 