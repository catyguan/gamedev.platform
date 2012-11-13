<?php
/**
 *  @generated
 */
namespace ge\lua\service\thrift;
class TLuaAppHostManager_appAICall_args extends \TBase {
  static $_TSPEC;

  public $appId = null;
  public $name = null;
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
          'var' => 'name',
          'type' => \TType::STRING,
          ),
        3 => array(
          'var' => 'params',
          'type' => \TType::STRING,
          ),
        4 => array(
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
    return 'TLuaAppHostManager_appAICall_args';
  }

  public function read($input)
  {
    return $this->_read('TLuaAppHostManager_appAICall_args', self::$_TSPEC, $input);
  }
  public function write($output) {
    return $this->_write('TLuaAppHostManager_appAICall_args', self::$_TSPEC, $output);
  }
}


?>
