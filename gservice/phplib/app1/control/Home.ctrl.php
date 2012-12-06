<?php
namespace controller;

class Home {
	
    public function index($app) {
        $vo = array();
        $v = $app->loadView('app1', 'main');
        $v->show($vo);
    }
	
	public function enter($app) {
		$SA = new \ge\lua\service\SessionAgent();
		$SA->init(true);
		if($SA->isNew()) {
			header("Location: ");
			return;
		}		
		echo "1";
	}
    
}