<?php
header("Content-Type:text/html; charset=utf-8");
require_once __DIR__.'/../bootstrap.php';

echo "<pre>";

\bma\log\AppLog::getInstance()->setWriter(array("\\bma\\log\\Log",'writeEcho'));

$m  = \bma\cloud\CloudNodeManager::getInstance()->getInstance();

if(1) {    
    echo "------ addNode ------\n";    
    $datas = array(
        "node_id" => "node1",
        "host" => "localhost",
        "cloud_port" => "9098",
        "service_port" => "9097",
        "description" => "dev node",
    );
    $r = $m->addNode($datas);
    var_dump($r);
}

if(0) {    
    echo "------ updateNode ------\n";    
    $datas = array(        
        "host" => "localhost",
        "cloud_port" => "9098",
        "service_port" => "9097",
        "description" => "dev node 2",
    );
    $r = $m->updateNode('node1', $datas);
    var_dump($r);
}

if(0) {    
    echo "------ deleteNode ------\n";    
    $r = $m->deleteNode('node1');
    var_dump($r);
}

if(0) {    
    echo "------ getNode ------\n";    
    $r = $m->getNode("node1");
    var_dump($r);
}

if(1) {    
    echo "------ listNode ------\n";    
    $r = $m->listNode();
    var_dump($r);
}

echo "</pre>";
echo "done";