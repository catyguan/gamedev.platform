<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" dir='ltr'>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	<title>APP运行界面</title>
	<script src="http://ajax.aspnetcdn.com/ajax/jQuery/jquery-1.8.2.min.js"></script>
	<script>
	$(document).ready(function() {
		$("#cmd").focus();
	});
	</script>
</head>
<body>
	<div>
		功能:
		<a href="?m=App&do=restart&id=<?php echo $id?>">重启</a>
		<a href="?m=Home&do=listApp">返回</a>
		<hr/>		
APP状态
<?php
function popv(&$t, $n) {
	if(isset($t[$n])) {
		$r = $t[$n];
		unset($t[$n]);
		return $r;
	}
	return null;
}

$si = $statusInfo;
$error = popv($si, 'error');
if($error) {
	echo "<font color=red>".$error."</font>";
} else {
	if(1) {
		$v = popv($si, 'memuse');
		if($v)echo "内存使用：".sprintf("%.2f",$v/1024)."K; ";
	}
}
?>
<pre><?php if(count($si))var_dump($si); ?></pre>
	    <hr/>	    
<div style=" float: left; height: 100%; width: 600px;">
<div id="cmdConsole" style="height: 400px; width: 100%; background-color: #EEEEEE; overflow-y: scroll">
	<div style="display:none">-------</div>
</div>		
<script>
var cmdId = 0;
var maxCmd = 20;
function scrollIt(n,h) {
	var cc = $(n)[0];
	var t = cc.scrollHeight - h;
	cc.scrollTop = t>0? t: 0;	
}
function inputCommand(f) {
	cmdId++;
	var cc = $('#cmdConsole')[0];
	while(cc.children.length>maxCmd) {
		cc.removeChild(cc.children[0]);
	}
	var cmd = $('#cmd').val();
	$('<div>command:'+cmd+' => <span id="cmdrep'+cmdId+'"></span></div>').insertAfter(cc.children[cc.children.length-1]);
	$('#cmdrep'+cmdId).load('index.php',{"m":"App","do":"appCall","cmd":cmd,"id":"<?php echo $id?>"}, function(){scrollIt('#cmdConsole',400);});	
	scrollIt('#cmdConsole',400);
	$("#cmd").select();
	return false;
}
</script>
<form onsubmit="return inputCommand(this);">
<input id="cmd" type="text"	value="" size="60"/>
<input id="m" type="hidden" value="App"/>
<input id="id" type="hidden" value="<?php echo $id?>"/>
<input type="submit" value="Send"/>
<input type="button" value="refresh" onclick="window.location.reload()"/>
</form>		
</div>
<div id="luaConsole" style=" float: right; height: 400px; width: 600px; background-color: #EEEEEE; overflow-y: scroll">
</div>
    </div>
</body>
</html>
