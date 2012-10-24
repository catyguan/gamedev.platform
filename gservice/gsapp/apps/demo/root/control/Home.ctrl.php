<?php

namespace controller;

class Home {

    public function index($app) {
        $vo = array();        
        $v = $app->loadView('demo', 'home_index');
        $v->show($vo);
        return;
    }    

}