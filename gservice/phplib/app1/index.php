<?php
header("Content-Type:text/html; charset=utf-8");
require_once('bootstrap.php');

class TheApplication extends \bma\webapp\Application
{
}

$app = new TheApplication();
$app->simpleInit(
		array(
            __DIR__.'/control/',
		),
		array(		    
			__DIR__.'/views/',
		)
);

try {
	$app->run();
} catch(\Exception $e) {
	$vo['message'] = isset($e->title) ? $e->title : '';
    $vo['exception'] = $e;	
	$v = $app->loadView('base','50x');
	$v->show($vo);
	exit();
}