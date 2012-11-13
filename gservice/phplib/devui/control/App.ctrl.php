<?php
namespace controller;

class App {
	
    public function index($app) {
        $this->appUI($app);
    }

    public function appUI($app) {
		$id = $app->req->request('id');
	
        $vo = array();
		
		$vo['id'] = $id;
		$v = $app->loadView('devui', 'app_ui');
        $v->show($vo);
    }
	
	public function restart($app) {
        $id = $app->req->request('id');        
        $r = \ge\lua\service\Manager::getInstance()->restartApp($id);
        $app->sendRedirect(APP_URL."index.php?m=App&id=$id");
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
	
	public function ping($app) {
		$id = $app->req->request('id');
		$t = $app->req->request('t', 0);
		$seq = $app->req->request('s', 0);
		
		$m = \ge\lua\service\Manager::getInstance();

		try {
			$r = $m->appCall($id, 'devuiPing', array($t, $seq));
			if($r) {
				$info = $r[0];
			} else {
				$info = array("response", json_encode($r));
			}
		} catch(\Exception $e) {
			$info = array(
				"error"=>$e->getMessage()
			);
		}
		
		echo json_encode($info);	
	}
    
}