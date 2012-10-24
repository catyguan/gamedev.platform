<?php
/**
 *  应用程序环境启动配置
 *
 */

/**************************** php运行环境配置 ****************************/
date_default_timezone_set( 'Asia/Shanghai' );

define('DEV_MODE', true);

if(DEV_MODE) {
	define('DEBUG_UDB_USER',4);
}

# set 0 to close echo error
ini_set('display_errors', DEV_MODE?1:0);
# error_reporting(E_ALL &~ E_NOTICE);

define('APP_LOG_PATH', APP_PATH.'../logs/');
define('APP_LOG_LEVEL',DEV_MODE?5:3); // ALL = 5,DEBUG = 4, INFO = 3, WARN = 2, ERROR = 1, NONE = 0

# 应用参数
# 应用的名称
define('APP_NAME', 'gsapp.demo');
# 应用的名称
define('APP_TITLE', 'GSApp范例');

# 应用的URL
define('APP_URL','http://localhost:8080/gsapp/apps/demo/root/');