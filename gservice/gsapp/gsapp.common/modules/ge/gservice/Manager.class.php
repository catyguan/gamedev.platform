<?php
namespace ge\gservice;

/**
 * API服务对象
 * @author guanzhong
 */
class Manager extends \bma\thrift\ThriftObject {
    
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
	
	public function createApp($appId,$appName) {
	    $cl = $this->getClient();
	    $r = $cl->createApp($appId,$appName);
	    if($this->logger->isDebug()) {
            $dmsg = sprintf('createApp(%s,%s) => %s',$appId,$appName,$r);
			$this->logger->debug($dmsg);
        }
        return $r;
	}
	
	public function appCall($appId,$cmd,$params,$appName4autoCreate) {
	    $req = array(
	        "command" => $cmd,
	        "params" => $params,
	    );
	    $cl = $this->getClient();
	    if($appName4autoCreate) {	        
            $app = $cl->getApp($appId);
            if(!$app->id) {
                if($this->logger->isDebug()) {
                    $dmsg = sprintf('auto create app(%s,%s)',$appId,$appName4autoCreate);
			        $this->logger->debug($dmsg);
                }
                if(!$cl->createApp($appId,$appName4autoCreate)) {
                    throw new \Exception("create app fail");
                }
            }
	    }
	    
	    $reqstr = json_encode($req);
	    $repstr = $cl->appCall($appId, $reqstr);
	    if($this->logger->isDebug()) {
            $dmsg = sprintf('appCall(%s,%s) => ',$appId,$reqstr,$repstr);
			$this->logger->debug($dmsg);
        }
        $rep = json_decode($repstr,true);
        if(isset($rep["err"]) && $rep["err"]) {
            throw new \Exception($rep["message"]);
        }
        return $rep;
	}
	
	public function listApp() {
        $cl = $this->getClient();
	    return $cl->listApp();
	}
	
	public function closeApp($appId) {
        $cl = $this->getClient();
	    $r = $cl->closeApp($appId);
	    if($this->logger->isDebug()) {
            $dmsg = sprintf('closeApp(%s) => %s',$appId,$r);
			$this->logger->debug($dmsg);
        }
        return $r;
	}
}