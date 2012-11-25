<?php
namespace ge\lua\service;

/**
 * API服务对象
 * @author guanzhong
 */
class App extends \bma\thrift\ThriftObject {

	protected $logger;
	protected $client;
	public $appId;
	
	public function __construct($cl, $appId) {
		$this->logger = \bma\log\AppLog::wrap(__CLASS__);		
		$this->client = $cl;
		$this->appId = $appId;
	}
	
	public static function create($accessId) {
		list($name, $host, $port, $module, $appId) = explode("-", $accessId, 5);
		$cl = Service::getInstance()->getClient($name, $host, $port, $module);
		return new App($cl, $appId);
	}
	
	public function managerObject() {
		return new Manager($this->client);
	}
	
	public function getClient() {	    
	    return $this->client;
	}
	
	public function appEval($content) {	    
	    $cl = $this->getClient();
	    $r = $cl->appEval($this->appId, $content);
	    if($this->logger->isDebug()) {
            $dmsg = sprintf('appEval(%s, %s)',$this->appId, $content);
			$this->logger->debug($dmsg);
        }
        return $r;
	}
	
	public function appCall($methodName, $params) {	    
	    $cl = $this->getClient();
	    $r = $cl->appCall($this->appId, $methodName, json_encode($params));		
	    if($this->logger->isDebug()) {
            $dmsg = sprintf('appCall(%s) => %s',$this->appId, $r->result);
			$this->logger->debug($dmsg);
        }
		if($r->result && $r->done) {
			$ret = json_decode($r->result, true);
			return $ret;
		}
        return null;
	}
	
	public function appCommand($sceneName, $commandName, $params, $timeoutInSeconds) {	    
	    $cl = $this->getClient();
		$callParams = array($sceneName, $commandName);
		if($params) {
			if(!is_array($params))throw new \Exception("params not a Array");
			$callParams = array_merge($callParams, $params);
		}
	    $r = $cl->appAICall($this->appId, "appCommand", json_encode($callParams), $timeoutInSeconds*1000);
	    if($this->logger->isDebug()) {
            $dmsg = sprintf('appCall(%s) => %s',$this->appId, $r->result);
			$this->logger->debug($dmsg);
        }
		if($r->result && $r->done) {
			$ret = json_decode($r->result, true);
			return $ret;
		}
        return null;
	}
}
