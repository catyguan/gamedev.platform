<?php
namespace ge\lua\service;

/**
 * SessionAgent对象，实现HTMLApp和LuaApp的会话和功能代理
 *
 * @author guanzhong
 */
class SessionAgent {

	protected $logger;
	
	protected $cookieKey;
	protected $cookieTimeout; // seconds
	protected $cookiePath;	
	protected $cookieDomain;	
	
	protected $sessionPrefix;
	protected $sessionSignKey;
	
	protected $sessionId;
	protected $newSession;	
	protected $appCommand;
	
	public function __construct() {
		$this->logger = \bma\log\AppLog::wrap(__CLASS__);
		
		$this->cookieKey = "_LASESSIONID";		
		$this->cookieTimeout = 0;
		$this->cookiePath = "";
		$this->cookieDomain = "";
		$this->sessionPrefix = "";
		$this->sessionSignKey = "SessionAgent";
		$this->sessionId = "";
		
		$conf = \bma\base\Config::getModuleConf('luaservice','session');		
		if(isset($conf['cookieKey'])) {
			$this->cookieKey = $conf['cookieKey'];
		}
		if(isset($conf['cookieTimeout'])) {
			$this->cookieTimeout = $conf['cookieTimeout'];
		}
		if(isset($conf['cookiePath'])) {
			$this->cookiePath = $conf['cookiePath'];
		}
		if(isset($conf['sessionPrefix'])) {
			$this->sessionPrefix = $conf['sessionPrefix'];
		}
		if(isset($conf['cookieDomain'])) {
			$this->cookieDomain = $conf['cookieDomain'];
		}
		if(isset($conf['sessionSignKey'])) {
			$this->sessionSignKey = $conf['sessionSignKey'];
		}
	}
	
	public function init($createIfNew = false) {
		$this->sessionId = isset($_COOKIE[$this->cookieKey])?$_COOKIE[$this->cookieKey]:"";
		if($this->sessionId) {
			@list($v, $code) = explode('-', $this->sessionId);
			$mycode = substr(md5($this->sessionSignKey.$v),0,16);
			if(!$code || $code != $mycode) {
				if($this->logger->isWarn()) {
					$dmsg = sprintf('invalid session value(%s) => %s',$this->sessionId,$mycode);
					$this->logger->warn($dmsg);
				}
			}
		}
		
		if($this->sessionId) {
			$this->newSession = false;
		} else {
			$this->newSession = true;
			if($createIfNew) {
				$this->create();
			}
		}
	}
	
	public function create() {
		$v = uniqid($this->sessionPrefix, true);
		$this->newSession = true;
		$this->sessionId = $v.'-'.substr(md5($this->sessionSignKey.$v),0,16);
		setcookie($this->cookieKey, $this->sessionId, 
			$this->cookieTimeout?(time()+$this->cookieTimeout):0, $this->cookiePath, $this->cookieDomain);
	}
	
	public function isValid() {
		return $this->sessionId != "";
	}
	
	public function isNew() {
		return $this->newSession;
	}
	
	public function getSessionId() {
		return $this->sessionId;
	}
	public function setSessionId($v) {
		$this->sessionId = $v;
	}
	
	public function command($name, $sceneName, $commandName, $params, $timeoutInSeconds) {	 
		if(!$this->isValid()) {
			return array(array("mistake"=>true, "code"=>"session.invalid"));
		}
		if($this->isNew()) {
			return array(array("delay"=>true, "time"=>0));
		}
		if(!$this->appCommand) {
			$this->appCommand = new AppCommand();
		}
		if($params) {
			$cparams = array_merge(array($this->sessionId), $params);
		} else {
			$cparams = array($this->sessionId);
		}
		return $this->appCommand->command($name, $sceneName, $commandName, $cparams, $timeoutInSeconds);
	}
	
	public function proxy($name, $seceneName, $commandName, $params, $timeoutInSeconds) {
		$result = $this->command($name, $sceneName, $commandName, $cparams, $timeoutInSeconds);
		if($result && is_array($result) && isset($result[0])) {
			return response($result[0]);
		}
		return response($result);
	}
	
	public function appProxy($app, $defaultTM = 15) {
		$name = $app->req->request('name');
        $scene = $app->req->request('scene');
        $command = $app->req->request('command');
		$params = $app->req->request('params');
		$tm = $app->req->request('tm', $defaultTM);
		
		$this->init();
		if($params) {
			$cparams = json_decode($params,true);
		} else {
			$cparams = array();
		}
		$this->proxy($name,$scene,$command,$cparams, $tm);
	}
	
	public function response($data) {
		header('Content-type: application/json');
		echo json_encode($data);
	}
	
	public function refresh() {
		echo "<script>window.location.reload()</script>";
	}
}
