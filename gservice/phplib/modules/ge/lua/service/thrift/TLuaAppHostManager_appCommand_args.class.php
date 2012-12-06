<?php
/**
 *  @generated
 */
namespace ge\lua\service\thrift;
class TLuaAppHostManager_appCommand_args extends \TBase {
  static $_TSPEC;

  public $appId = null;
  public $caseName = null;
  public $methodName = null;
  public $session = null;
  public $params = null;
  public $timeout = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'appId',
          'type' => \TType::STRING,
          ),
        2 => array(
          'var' => 'caseName',
          'type' => \TType::STRING,
          ),
        3 => array(
          'var' => 'methodName',
          'type' => \TType::STRING,
          ),
        4 => array(
          'var' => 'session',
          'type' => \TType::STRING,
          ),
        5 => array(
          'var' => 'params',
          'type' => \TType::STRING,
          ),
        6 => array(
          'var' => 'timeout',
          'type' => \TType::I32,
          ),
        );
    }
    if (is_array($vals)) {
      parent::__construct(self::$_TSPEC, $vals);
    }
  }

  public function getName() {
    return 'TLuaAppHostManager_appCommand_args';
  }

  public function read($input)
  {
    return $this->_read('TLuaAppHostManager_appCommand_args', self::$_TSPEC, $input);
  }
  public function write($output) {
    return $this->_write('TLuaAppHostManager_appCommand_args', self::$_TSPEC, $output);
  }
}


?>
