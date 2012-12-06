<?php
namespace ge\lua\service;

header("Content-Type:text/html; charset=utf-8");
require_once __DIR__.'/../bootstrap.php';

echo "<pre>";

\bma\log\AppLog::getInstance()->setWriter(array("\\bma\\log\\Log",'writeEcho'));

if(1) {    
	echo "------ base ------\n";
	$SA = new SessionAgent();
	$SA->init(true);
	
	// $r = $SA->command("test", "session", "setValue", array("key1", 1234), 5);
	// var_dump($r);
	
	// $r = $SA->command("test", "session", "getValue", array("key1"), 5);
	// var_dump($r);
	
	$r = $SA->command("test", "session", "removeValue", array("key1"), 5);
	var_dump($r);
	
	$r = $SA->command("test", "session", "removeSession", array(), 5);
	var_dump($r);
	
}

echo "</pre>";
echo "done";
