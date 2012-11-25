<?php
namespace ge\lua\service;

/**
 * AppCommand对象
 * @author guanzhong
 */
class AppCommand {

	protected $conf;
	
	public function __construct() {
		$this->conf = \bma\base\Config::getModuleConf('luaservice','appcommand');
	}
	
	public function command($name, $sceneName, $commandName, $params, $timeoutInSeconds) {	   
	    	if(!isset($this->conf['gname']))throw new \Exception('config miss "gname"');
		$accessId = isset($this->conf['gname'][$name])?$this->conf['gname'][$name]:'';
		if(!$accessId)throw new \Exception('invalid gname "'.$name.'"');
		$app = App::create($accessId);
		$result = $app->appCommand($sceneName, $commandName, $params, $timeoutInSeconds);
		if($result && isset($result[0])) {
			$r = $result[0];
			// var_dump($r);
			if(isset($r['forward']) && $r['forward']) {
				return $this->command($r['name'],$r['scene'],$r['command'],isset($r['params'])?$r['params']:null,$timeoutInSeconds);
			}
		}
		return $result;
	}
}
