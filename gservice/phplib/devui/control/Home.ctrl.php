<?php
namespace controller;

class Home {
	
    public function index($app) {
        $this->listApp($app);
    }

    public function listApp($app) {
        $vo = array();

        $list = \ge\lua\service\Manager::getInstance()->listApp();
        $vo['results'] = $list;
        
        $v = $app->loadView('devui', 'list_app');
        $v->show($vo);
    }
	
	public function create($app) {
        $id = $app->req->request('id');
        $appType = $app->req->request('appType');
        
        $r = \ge\lua\service\Manager::getInstance()->createApp($id,$appType);
        
        $vo = array();
        $vo["message"] = $r?"创建成功":"创建失败";
        $vo["goto"] = "index.php?m=Home&do=listApp";        
        $v = $app->loadView('devui', 'message');
        $v->show($vo);
        return;
    }    
	
	public function restart($app) {
        $id = $app->req->request('id');
        
        $r = \ge\lua\service\Manager::getInstance()->restartApp($id);
        
        $vo = array();
        $vo["message"] = $r?"重启成功":"重启失败";
        $vo["goto"] = "index.php?m=Home&do=listApp";        
        $v = $app->loadView('devui', 'message');
        $v->show($vo);
    }
    
	public function close($app) {
        $id = $app->req->request('id');
		$destroy = $app->req->request('destroy', 0);
        
        $r = \ge\lua\service\Manager::getInstance()->closeApp($id, $destroy==1);
        
        $vo = array();
        $vo["message"] = $r?"关闭成功":"关闭失败";
        $vo["goto"] = "index.php?m=Home&do=listApp";        
        $v = $app->loadView('devui', 'message');
        $v->show($vo);
    }	
    
}