<?php
/**
 *  应用引导程序
 *
 */
# 定义整个应用需要用的常量
define('APP_PATH',__DIR__.'/');						//应用根目录
define('BMA_WS_PATH','D:/workroom/php/workspace');
define('BMA_LIBS_PATH', BMA_WS_PATH.'/common/libs/');		    //bma_libs第三方代码库目录
define('BMA_MODULE_PATH', BMA_WS_PATH.'/common/modules/');	    //bma_module基础库目录
define('SITEONE_MODULE_PATH', BMA_WS_PATH.'/siteone/modules/');	//siteone_module功能库目录
define('GSAPP_MODULE_PATH', __DIR__.'/../modules/');	        //gsapp_module功能库目录

# 加载环境启动配置
require_once APP_PATH.'conf/env.conf.php';

# 指定 应用配置/模块配置 目录
define('APP_CONFIG_PATH', APP_PATH.'conf/');

# autoload
require_once BMA_MODULE_PATH.'bscore.php';
\bma\common\AutoLoader::install(
	// class mapper
	array(
	),
	// search path 
	array(
		BMA_MODULE_PATH,
		SITEONE_MODULE_PATH,
		GSAPP_MODULE_PATH,
		APP_PATH,
		BMA_LIBS_PATH,
	),	
	// search class file name
	array('%s.class.php','%s.php')
			
);