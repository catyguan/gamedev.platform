<?php
namespace controller;

class Testcase {
	
    public function sessionAgent($app) {
        $vo = array();
        $v = $app->loadView('app1', 'testcase_sessionAgent');
        $v->show($vo);
    }
	
	public function proxy($app) {
		$SA = new \ge\lua\service\SessionAgent();
		$SA->appProxy($app, 15);
	}
    
}