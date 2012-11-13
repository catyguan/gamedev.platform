<?php
namespace ge\lua\service;

/**
 * API服务对象
 * @author guanzhong
 */
class Manager extends \bma\thrift\ThriftObject {

	public static function getInstance() {
		if ( ! isset(self::$_instance)) {
			self::$_instance = new self();
		}
		return self::$_instance;
	}
	private static $_instance;
    
	protected $logger;
	protected $client;
	
	public function __construct() {
		$this->logger = \bma\log\AppLog::wrap(__CLASS__);		
	}
	
	public function getClient() {
	    if($this->client)return $this->client;
	    $this->client = Service::getInstance()->getClient();
	    return $this->client;
	}
	
	public function listApp() {
        $cl = $this->getClient();
	    return \bma\thrift\Manager::objArr2Array($cl->listApp());
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
	
	public function appEval($appId, $content) {	    
	    $cl = $this->getClient();
	    $r = $cl->appEval($appId, $content);
	    if($this->logger->isDebug()) {
            $dmsg = sprintf('appEval(%s) => %s',$appId, $content);
			$this->logger->debug($dmsg);
        }
        return $r;
	}
	
	public function appCall($appId, $methodName, $params) {	    
	    $cl = $this->getClient();
	    $r = $cl->appCall($appId, $methodName, json_encode($params));		
	    if($this->logger->isDebug()) {
            $dmsg = sprintf('appCall(%s) => %s',$appId, $r->result);
			$this->logger->debug($dmsg);
        }
		if($r->result && $r->done) {
			$ret = json_decode($r->result, true);
			return $ret;
		}
        return null;
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