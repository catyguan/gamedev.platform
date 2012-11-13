<?php
header("Content-Type:text/html; charset=utf-8");
require_once __DIR__.'/../bootstrap.php';

echo "<pre>";

\bma\log\AppLog::getInstance()->setWriter(array("\\bma\\log\\Log",'writeEcho'));
$testNodeId = 'node1';
// $testNodeId = 'admin-test';

if(0) {    
    echo "------ base ------\n";
    $cl  = \bma\cloud\Service::getInstance()->getClient("cloud");
    $r = $cl->getCloudNodeDesc("node1");
    var_dump($r);
}

if(0) {    
    echo "------ CloudNode ------\n";
    $c  = new \bma\cloud\Cloud();
    $c->open('cloud');
    $obj = $c->getNode($testNodeId);
	$r = $obj->getDesc();
	var_dump($r);
	
	$r = $obj->listAppDesc();
	var_dump($r);
	
	// $r = $obj->listApp();
	// var_dump($r);
}

if(0) {
    echo "------ createApp ------\n";
    $c  = new \bma\cloud\Cloud();
    $c->open('cloud');
    $obj = $c->getNode($testNodeId);
    
	$r = $obj->createApp("app1","appName");
	var_dump($r);
}

if(0) {
    echo "------ closeApp ------\n";
    $c  = new \bma\cloud\Cloud();
    $c->open('cloud');
    $obj = $c->getNode($testNodeId);
    
	$r = $obj->closeApp("app1");
	var_dump($r);
}

if(0) {    
    echo "------ CloudApp ------\n";
    $c  = new \bma\cloud\Cloud();
    $c->open('cloud');
    $obj = $c->getApp($testNodeId,'app1');
	$r = $obj->getDesc();
	var_dump($r);
	
	$r = $obj->listServiceDesc();
	var_dump($r);
	
	// $r = $obj->listService();
	// var_dump($r);	
}

if(0) {    
    echo "------ CloudService ------\n";
    $c  = new \bma\cloud\Cloud();
    $c->open('cloud');
    $obj = $c->getService($testNodeId,'app1','s1');
	$r = $obj->getDesc();
	var_dump($r);
	
	$r = $obj->listApiDesc();
	var_dump($r);
	
	$r = $obj->getApiDesc('api1');
	var_dump($r);
	
	$req = \bma\cloud\Cloud::textReq("hello world",null,true);
	$r = $obj->cloudCall('api1',$req);
	var_dump($r);
}

if(1) {    
    echo "------ redirect ------\n";
    $c  = new \bma\cloud\Cloud();
    $c->open('cloud');
    $obj = $c->getService($testNodeId,'app1','s1');
	
	$req = \bma\cloud\Cloud::textReq("hello world",null,true);
	$r = $obj->cloudCall('api3',$req);
	var_dump($r);
}

if(0) {    
    echo "------ shutdown API ------\n";
    $c  = new \bma\cloud\Cloud();
    $c->open('cloud');
    $obj = $c->getService($testNodeId,'app1','manageService');
	
	$r = $obj->cloudCall('shutdown',"",null,true);
	var_dump($r);
}

echo "</pre>";
echo "done";