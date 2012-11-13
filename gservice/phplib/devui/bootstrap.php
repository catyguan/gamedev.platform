<?php
/**
 *  应用引导程序
 *
 */
# 定义整个应用需要用的常量
define('APP_PATH',__DIR__.'/');						//应用根目录
require_once APP_PATH.'conf/bootdir.conf.php';

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
		GSERVICE_MODULE_PATH,
		APP_PATH,
		BMA_LIBS_PATH,
	),	
	// search class file name
	array('%s.class.php','%s.php')
			
);