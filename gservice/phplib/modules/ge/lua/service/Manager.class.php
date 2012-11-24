<?php
namespace ge\lua\service;

/**
 * API服务对象
 * @author guanzhong
 */
class Manager extends \bma\thrift\ThriftObject {

	protected $logger;
	protected $client;
	
	public function __construct($cl) {
		$this->logger = \bma\log\AppLog::wrap(__CLASS__);		
		$this->client = $cl;
	}
	
	public static function create($accessId) {
		list($name, $host, $port, $module) = explode("-", $accessId);
		$cl = Service::getInstance()->getClient($name, $host, $port, $module);
		return new Manager($cl);
	}
	
	public function getClient() {	    
	    return $this->client;
	}
	
	public function listApp() {
        $cl = $this->getClient();
	    return \bma\thrift\Manager::objArr2Array($cl->listApp());
	}
	
	public function appObject($appId) {
		return new App($his->getClient(), $appId);
	}
	
	public function createApp($appId,$appType) {
	    $cl = $this->getClient();
	    $r = $cl->createApp($appId,$appType);
	    if($this->logger->isDebug()) {
            $dmsg = sprintf('createApp(%s,%s) => %s',$appId,$appType,$r);
			$this->logger->debug($dmsg);
        }
        return $r;
	}
	
	public function restartApp($appId) {
	    $cl = $this->getClient();
	    $r = $cl->restartApp($appId);
	    if($this->logger->isDebug()) {
            $dmsg = sprintf('restartApp(%s) => %s',$appId,$r);
			$this->logger->debug($dmsg);
        }
        return $r;
	}
	
	public function closeApp($appId, $destroy) {
        $cl = $this->getClient();
	    $r = $cl->closeApp($appId, $destroy);
	    if($this->logger->isDebug()) {
            $dmsg = sprintf('closeApp(%s, %s) => %s',$appId, $destroy, $r);
			$this->logger->debug($dmsg);
        }
        return $r;
	}
}