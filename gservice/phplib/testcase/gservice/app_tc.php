<?php
namespace ge\lua\service;

header("Content-Type:text/html; charset=utf-8");
require_once __DIR__.'/../bootstrap.php';

echo "<pre>";

\bma\log\AppLog::getInstance()->setWriter(array("\\bma\\log\\Log",'writeEcho'));

if(0) {    
    echo "------ create client ------\n";
	$S = Service::getInstance();
	$cl = $S->getClient("test", "127.0.0.1", 9098, "lua");    
}

if(1) {    
    echo "------ manager:listApp ------\n";
	$m = Manager::create("test-127.0.0.1-9098-lua");
	var_dump($m->listApp());
}

echo "</pre>";
echo "done";