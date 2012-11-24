<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" dir='ltr'>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	<title>访问设置</title>
</head>
<body>
	<div>
	    访问设置
	    <form action="?m=Home&do=listApp">
			NAME：<input type="text" name="name" value="test"/>
	        IP: <input type="text" name="ip" value=""/>
	        PORT：<input type="text" name="port" value="9098"/>
			MODULE：<input type="text" name="module" value="lua"/>
            <input type="hidden" name="m" value="Home"/>
            <input type="hidden" name="do" value="create"/>
            <input type="submit" value="访问"/>
	    </form>
		
		<hr/>
		快速访问:
		<a href="?m=Home&do=listApp&name=test&ip=127.0.0.1&port=9098&module=lua">本地-9098</a>
		
	    
    </div>
</body>
</html>
