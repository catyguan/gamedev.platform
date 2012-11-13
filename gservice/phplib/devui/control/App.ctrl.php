<?php
namespace controller;

class App {
	
    public function index($app) {
        $this->appUI($app);
    }

    public function appUI($app) {
		$id = $app->req->request('id');
	
        $vo = array();
		
		$m = \ge\lua\service\Manager::getInstance();

		try {
			$r = $m->appCall($id, 'appStatus', array(), 5000);
			if($r)$statusInfo = $r[0];
		} catch(\Exception $e) {
			$statusInfo = array(
				"error"=>$e->getMessage()
			);
		}
		
		$vo['id'] = $id;
		$vo['statusInfo'] = $statusInfo;
		$v = $app->loadView('devui', 'app_ui');
        $v->show($vo);
    }
	
	public function appCall($app) {
        $id = $app->req->request('id');
		$cmd = $app->req->request('cmd');
		$clist = explode(" ", $cmd,2 );
		$name = $clist[0];		
		if(count($clist)>1) {
			$p = $clist[1];
		} else {
			$p = "";
		}
		$params = '['.$p.']';
        
		try {
			$r = \ge\lua\service\Manager::getInstance()->appCall($id, $name, json_decode($params,true));
			$ret = $r ? json_encode($r) : "#void#";
			echo $ret;
        } catch(\Exception $e) {
			echo "<font color=red>".$e->getMessage()."</font>";
		}
    }
	
	public function restart($app) {
        $id = $app->req->request('id');        
        $r = \ge\lua\service\Manager::getInstance()->restartApp($id);
        $app->sendRedirect(APP_URL."index.php?m=App&id=$id");
    }
    
}