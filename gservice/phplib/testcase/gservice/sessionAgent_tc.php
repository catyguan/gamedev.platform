<?php
namespace ge\lua\service;

header("Content-Type:text/html; charset=utf-8");
require_once __DIR__.'/../bootstrap.php';

echo "<pre>";

\bma\log\AppLog::getInstance()->setWriter(array("\\bma\\log\\Log",'writeEcho'));

if(0) {    
	echo "------ base ------\n";
	$SA = new SessionAgent();
	$SA->init();
	var_dump($SA->getSessionId());
	var_dump($SA->isNew());
}

if(1) {    
	echo "------ command ------\n";
	$SA = new SessionAgent();
	$SA->init();
	var_dump($SA->command("test", "hello", "say", array(2),5));
}

echo "</pre>";
echo "done";
