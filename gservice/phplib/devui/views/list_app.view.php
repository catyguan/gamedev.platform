<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" dir='ltr'>
<head>
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	<title>APP列表</title>
</head>
<body>
	<div>
	    APP列表
	    <ul>
	        <?php foreach($results as $app) { ?>
	            <li><?php echo $app['id']." [".$app['type']."]"?> - <?php echo $app['createTime'] ?> -
	                <a href="?m=App&id=<?php echo $app['id']?>">运行</a> 
					<a href="?m=Home&do=close&id=<?php echo $app['id']?>&destroy=1" onclick="return confirm('确实删除？')">删除</a> 
	                <a href="?m=Home&do=close&id=<?php echo $app['id']?>" onclick="return confirm('确实关闭？')">关闭</a> 
	                <a href="?m=Home&do=restart&id=<?php echo $app['id']?>" onclick="return confirm('确实重启？')">重启</a></li>
	        <?php } ?>
	    </ul>	
	    <hr/>
	    <form>
	        ID: <input type="text" name="id" value=""/>
	        类型：<input type="text" name="appType" value=""/>
            <input type="hidden" name="m" value="Home"/>
            <input type="hidden" name="do" value="create"/>
            <input type="submit" value="创建APP"/>
	    </form>
		
		<hr/>
		快速功能:
		<a href="?m=Home&do=create&id=app1&appType=app1">app1-app1</a>
		<a href="?m=Home&do=create&id=app1&appType=app2">app1-app2</a>
		<a href="?m=Home&do=create&id=app1&appType=demo1">app1-demo1</a>
		
	    
    </div>
</body>
</html>
