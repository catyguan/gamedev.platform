<?php
namespace controller;

class Home {
	
    public function index($app) {
        $this->accessForm($app);
    }
	
	public function accessForm($app) {
		$vo = array();
        $v = $app->loadView('devui', 'access_form');
        $v->show($vo);
	}
	
    public function listApp($app) {
		$aid = $app->req->request('aid');
		if(!$aid) {
			$aaid = $app->req->request('aaid');
			if($aaid) {
				list($name,$ip,$port,$module) = explode("-",$aaid);
			} else {
				$name = $app->req->request('name');
				$ip = $app->req->request('ip');
				$port = $app->req->request('port');
				$module = $app->req->request('module');
			}			
			$aid = "$name-$ip-$port-$module";
		}
		
        $vo = array();

        $list = \ge\lua\service\Manager::create($aid)->listApp();
		$vo['aid'] = $aid;
        $vo['results'] = $list;
        
        $v = $app->loadView('devui', 'list_app');
        $v->show($vo);
    }
	
	public function create($app) {
		$aid = $app->req->request('aid');
        $id = $app->req->request('id');
        $appType = $app->req->request('appType');
        
        $r = \ge\lua\service\Manager::create($aid)->createApp($id,$appType);
        
        $vo = array();
        $vo["message"] = $r?"创建成功":"创建失败";
        $vo["goto"] = "index.php?m=Home&do=listApp&aid=".$aid;        
        $v = $app->loadView('devui', 'message');
        $v->show($vo);
        return;
    }    
	
	public function restart($app) {
		$aid = $app->req->request('aid');
        $id = $app->req->request('id');
        
        $r = \ge\lua\service\Manager::create($aid)->restartApp($id);
        
        $vo = array();
        $vo["message"] = $r?"重启成功":"重启失败";
        $vo["goto"] = "index.php?m=Home&do=listApp&aid=".$aid;
        $v = $app->loadView('devui', 'message');
        $v->show($vo);
    }
    
	public function close($app) {
		$aid = $app->req->request('aid');
        $id = $app->req->request('id');
		$destroy = $app->req->request('destroy', 0);
        
        $r = \ge\lua\service\Manager::create($aid)->closeApp($id, $destroy==1);
        
        $vo = array();
        $vo["message"] = $r?"关闭成功":"关闭失败";
        $vo["goto"] = "index.php?m=Home&do=listApp&aid=".$aid;        
        $v = $app->loadView('devui', 'message');
        $v->show($vo);
    }	
    
}