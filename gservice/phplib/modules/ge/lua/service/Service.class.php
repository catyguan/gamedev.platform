<?php
namespace ge\lua\service;

/**
 * API服务对象
 * @author guanzhong
 */
class Service extends \bma\thrift\ThriftObject {
    
	public static function getInstance() {
		if ( ! isset(self::$_instance)) {
			self::$_instance = new self();
		}
		return self::$_instance;
	}
	private static $_instance;
		
	protected $logger;
	
	public function __construct() {
		$this->logger = \bma\log\AppLog::wrap(__CLASS__);		
	}
	
	public function getClient() {
	    $name = 'luaAppHost';  
        $cm = \bma\thrift\Manager::getInstance();
	    return $cm->create($name,'\\ge\\lua\\service\\thrift\\TLuaAppHostManagerClient');
	}	
	
}