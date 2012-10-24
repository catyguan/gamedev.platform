<?php
namespace controller;

class Admin {
    
    public function __construct() {
		$this->m = new \ge\gservice\Manager();	
	}

    public function index($app) {
        $this->listApp($app);
    }
    
    public function listApp($app) {
        $vo = array();
        $lt = $this->m->listApp();
        $vlt = array();
        foreach($lt as $o) {
            array_push($vlt,array("id"=>$o->id,"appName"=>$o->appName));
        }
        $vo["applist"] = $vlt;        
        $v = $app->loadView('demo', 'admin_list');
        $v->show($vo);
        return;
    }    
    
    public function create($app) {
        $id = $app->req->request('id');
        $appName = $app->req->request('appName');
        
        $r = $this->m->createApp($id,$appName);
        
        $vo = array();
        $vo["message"] = $r?"创建成功":"创建失败";
        $vo["goto"] = "index.php?m=Admin&do=listApp";        
        $v = $app->loadView('demo', 'message');
        $v->show($vo);
        return;
    }    
    
    public function closeAll($app) {
        $lt = $this->m->listApp();
        $vlt = array();
        foreach($lt as $o) {
            $this->m->closeApp($o->id);
        }
        
        $vo = array();
        $vo["message"] = "关闭请求完成";
        $vo["goto"] = "index.php?m=Admin&do=listApp";        
        $v = $app->loadView('demo', 'message');
        $v->show($vo);
        return;
    }    

    public function close($app) {
        $id = $app->req->request('id');
        
        $r = $this->m->closeApp($id);
        
        $vo = array();
        $vo["message"] = $r?"关闭成功":"关闭失败";
        $vo["goto"] = "index.php?m=Admin&do=listApp";        
        $v = $app->loadView('demo', 'message');
        $v->show($vo);
        return;
    }
    
    public function recreate($app) {
        $id = $app->req->request('id');
        $appName = $app->req->request('appName');
        
        $this->m->closeApp($id);
        $r = $this->m->createApp($id,$appName);
        
        $vo = array();
        $vo["message"] = $r?"初始化成功":"初始化失败";
        $vo["goto"] = "index.php?m=Admin&do=listApp";        
        $v = $app->loadView('demo', 'message');
        $v->show($vo);
        return;
    }    
}