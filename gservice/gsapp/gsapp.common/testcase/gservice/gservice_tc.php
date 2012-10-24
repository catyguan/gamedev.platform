<?php
header("Content-Type:text/html; charset=utf-8");
require_once __DIR__.'/../bootstrap.php';

echo "<pre>";

\bma\log\AppLog::getInstance()->setWriter(array("\\bma\\log\\Log",'writeEcho'));

$m = \ge\gservice\Service::getInstance();
$cl  = $m->getClient();

$appId = "12345";
$appName = "test";

if(0) {
    echo "------ Thrift: getApp ------\n";
    $r = $cl->getApp($appId);
    var_dump($r);
}

if(0) {
    echo "------ Thrift: createGet ------\n";
    $r = $cl->createApp($appId,$appName);
    var_dump($r);
    
    $r = $cl->getApp($appId);
    var_dump($r);    
}

if(0) {
    echo "------ Thrift: appCall ------\n";
    $r = $cl->appCall($appId,"abc");
    var_dump($r);
}

if(1) {
    echo "------ Thrift: close ------\n";
    $r = $cl->closeApp($appId);
    var_dump($r);
}

if(0) {
    echo "------ GService: appCall ------\n";
    $r = $m->appCall($appId,"require","ge.gservice.GService",$appName);
    var_dump($r);
}

echo "</pre>";
echo "done";