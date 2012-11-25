<?php
namespace ge\lua\service;

header("Content-Type:text/html; charset=utf-8");
require_once __DIR__.'/../bootstrap.php';

echo "<pre>";

\bma\log\AppLog::getInstance()->setWriter(array("\\bma\\log\\Log",'writeEcho'));

if(0) {    
	echo "------ base ------\n";
	$AC = new AppCommand();
	var_dump($AC->command("test", "hello", "say", array("World FromPHP"),5));    
}

if(1) {    
	echo "------ forward ------\n";
	$AC = new AppCommand();
	var_dump($AC->command("test", "hello", "forward", array("World FromPHP"),5)); }

echo "</pre>";
echo "done";
