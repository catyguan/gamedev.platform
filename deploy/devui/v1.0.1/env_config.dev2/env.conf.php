<?php
/**
 *  Ӧ�ó��򻷾���������
 *
 */

/**************************** php���л������� ****************************/
date_default_timezone_set( 'Asia/Shanghai' );
# set 0 to close echo error
ini_set('display_errors', 1);
# error_reporting(E_ALL &~ E_NOTICE);

/**************************** Ӧ����־���� ****************************/
define('APP_LOG_PATH', '/home/www/dev_env/logs/devui/');
define('APP_LOG_LEVEL',5); // ALL = 5,	DEBUG = 4, INFO = 3, WARN = 2, ERROR = 1, NONE = 0

define('APP_URL' , 'http://172.19.16.195/dev/devui/webapp/');
