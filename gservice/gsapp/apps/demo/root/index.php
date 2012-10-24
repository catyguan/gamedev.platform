<?php
session_start();
header("Content-Type:text/html; charset=utf-8");
require_once('bootstrap.php');
$app = new \bma\webapp\Application();
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
	if(DEV_MODE){
		$vo['exception'] = $e;
	}
	$v = $app->loadView('live','50x');
	$v->show($vo);
	exit();
}