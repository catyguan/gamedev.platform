<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" dir='ltr'>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	<title>系统繁忙</title>
</head>
<body id="error-page">
	<div align='center'>
    	<p>抱歉，系统繁忙，请您稍后再试！</p>
    	<p><?php echo $message?"错误原因：$message":'';?></p>
	</div>
	<?php if(DEV_MODE && isset($exception) && $exception){?>
	<pre>
	<?php echo ($exception);?>
	</pre>
	<?php }?>
</body>
</html>
