<?php
namespace controller;

class App {
	
    public function index($app) {
        $this->appUI($app);
    }

    public function appUI($app) {
		$aid = $app->req->request('aid');
		$id = $app->req->request('id');
	
        $vo = array();
		
		$vo['aid'] = $aid;
		$vo['id'] = $id;
		$v = $app->loadView('devui', 'app_ui');
        $v->show($vo);
    }
	
	public function restart($app) {
        $aid = $app->req->request('aid');
		$apo = \ge\lua\service\App::create($aid);
        $r = $apo->managerObject()->restartApp($apo->appId);
        $app->sendRedirect(APP_URL."index.php?m=App&aid=$aid");
    }
	
	public function appCall($app) {
        $aid = $app->req->request('aid');
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
			$r = \ge\lua\service\App::create($aid)->appCall($name, json_decode($params,true));
			$ret = $r ? json_encode($r) : "#void#";
			echo $ret;
        } catch(\Exception $e) {
			echo "<font color=red>".$e->getMessage()."</font>";
		}
    }
	
	public function ping($app) {
		$aid = $app->req->request('aid');
		$t = $app->req->request('t', 0);
		$seq = $app->req->request('s', 0);
		
		$apo = \ge\lua\service\App::create($aid);

		try {
			$r = $apo->appCall('devuiPing', array($t, $seq));
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