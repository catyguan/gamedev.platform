<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" dir='ltr'>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	<title>APP列表</title>
</head>
<body id="error-page">
	<div>
	    APP列表
	    <ul>
	        <?php foreach($applist as $app) { ?>
	            <li><?php echo $app['id']."(".$app['appName'].")"?> - 
	                <a href="?m=<?php echo $app['appName'] ?>&id=<?php echo $app['id']?>" target="_blank">运行</a> 
	                <a href="?m=Admin&do=close&id=<?php echo $app['id']?>" onclick="return confirm('确实关闭？')">关闭</a> 
	                <a href="?m=Admin&do=recreate&id=<?php echo $app['id']?>&appName=<?php echo $app['appName']?>" onclick="return confirm('确实初始化？')">初始</a></li>
	        <?php } ?>
	    </ul>	
	    <hr/>
	    <form>
	        ID: <input type="text" name="id" value="12345"/>
	        类型：
	        <select name="appName">
	            <option value="Demo1">Demo1</option>
	            <option value="Demo2">Demo2</option>
	        </select>
            <input type="hidden" name="m" value="Admin"/>
            <input type="hidden" name="do" value="create"/>
            <input type="submit" value="创建APP"/>
	    </form>
	    <form onsubmit="return confirm('确实关闭？')">
	        <input type="hidden" name="m" value="Admin"/>
            <input type="hidden" name="do" value="closeAll"/>
            <input type="submit" value="关闭全部"/>
	    </form>
    </div>
</body>
</html>
