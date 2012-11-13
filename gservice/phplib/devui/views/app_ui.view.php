<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" dir='ltr'>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	<title>APP运行界面</title>
	<script src="http://ajax.aspnetcdn.com/ajax/jQuery/jquery-1.8.2.min.js"></script>
	<script>
	var phTime = 0;
	var phSeq = 0;
	var pingTimer;
	var stopPing = true;
	function onPingClick() {		
		enablePing(stopPing);				
	}
	function enablePing(e) {
		var old = stopPing;
		if(e) {			
			stopPing = false;			
			$('#pingButton').val("stopPing");
			$('#pingMsg').html("");			
			if(old) {
				doping();
			}
		} else {
			stopPing = true;
			$('#pingButton').val("ping");
			$('#pingMsg').html("stop!!!");			
		}
	}
	function doping() {
		if(stopPing) {			
			return;
		}
		$('#pingMsg').html("pinging...");
		
		var pbutton = $('#pingButton')[0];		
		var request = $.ajax({
			url: "index.php",			
			data: {id : "<?php echo $id?>", m: "App", "do": "ping", t : phTime, s : phSeq},
			dataType: "json"
		});
		request.done(function(msg) {			
			setTimeout(doping, 1000);
			$('#pingMsg').html("");			
			showAppStatus(msg);
		});
		request.fail(function(jqXHR, textStatus) {
			enablePing(false);
			$('#pingMsg').html("ping failed: " + textStatus);
		});
	}
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
APP状态 <span id="appStatus"></span>
<script>
function popv(t, n) {
	var v = t[n];
	delete t[n];
	return v;
}
function showAppStatus(si) {
	var text = "";
	var err = popv(si, 'error');
	if(err) {
		text = "<font color=red>"+err+"</font> ";
	} else {
		var v = popv(si, 'memuse');
		if(v)text += "内存使用："+((v/1024).toFixed(3))+"K; ";
		
		var ph = popv(si, 'print');
		if(ph) {
			for(var i=ph.length-1;i>=0;i--) {
				var pitem = ph[i];
				phTime = pitem.t;
				phSeq = pitem.s;
				var pca = pitem.p;
				var str = "";
				for(var j=0;j<pca.length;j++) {
					if(str)str+="    ";
					str += pca[j];
				}				
				printConsole(str);		
			}
		}		
		
		var remain = ""
		for (property in si) {
			remain += property + '=' + si[property]+'; ';
		}
		if(remain!="") {
			text += remain;
		}
	}
	$('#appStatus').html(text);
}
</script>
	    <hr/>	    
<div>
	<div id="cmdConsole" style="float: left; height: 400px; width: 50%; background-color: #EEEEEE; overflow-y: scroll">
		<div style="display:none">-------</div>
	</div>		
	<div id="luaConsole" style=" float: right; height: 400px; width: 600px; background-color: #EEEEEE; overflow-y: scroll">
		<div style="display:none">-------</div>
	</div>
</div>

<script>
var cmdId = 0;
var maxCmd = 20;
var maxConsole = 200;
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
function printConsole(s) {
	var cc = $('#luaConsole')[0];
	while(cc.children.length>maxConsole) {
		cc.removeChild(cc.children[0]);
	}	
	$("<div>"+s+"</div>").insertAfter(cc.children[cc.children.length-1]);	
	scrollIt('#luaConsole',400);
}
</script>
<form onsubmit="return inputCommand(this);">
<input id="cmd" type="text"	value="" size="60"/>
<input id="m" type="hidden" value="App"/>
<input id="id" type="hidden" value="<?php echo $id?>"/>
<input type="submit" value="Send"/>
<input type="button" value="refresh" onclick="window.location.reload()"/>
<input type="button" value="ping" id="pingButton" onclick="onPingClick()"/>
<span id="pingMsg"></span>
</form>		

    </div>
</body>
</html>
