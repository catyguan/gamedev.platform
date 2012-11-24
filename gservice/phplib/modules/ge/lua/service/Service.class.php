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
	
	public function getClient($name, $host, $port, $module) {		
		$conf = array(
			'type' => 'socket',
			'host' => $host,
			'port' => $port,
			'module' => $module,
			'persist' => true,
			'frame'	=>	true,
		);
		return $this->createClient($name, $conf);
	}	
	
	public function createClient($name, $conf) {	    
	    $client = \bma\objpool\Manager::getObject('thrift',$name);
		if(!$client) {		
		    $cm = \bma\thrift\Manager::getInstance();
		    $client = $cm->open($name,$conf);
		}
		return $client->create("\\ge\\lua\\service\\thrift\\TLuaAppHostManagerClient");
	}
	
}