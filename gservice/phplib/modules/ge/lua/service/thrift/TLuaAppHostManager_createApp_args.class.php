<?php
/**
 *  @generated
 */
namespace ge\lua\service\thrift;
class TLuaAppHostManager_createApp_args extends \TBase {
  static $_TSPEC;

  public $appId = null;
  public $appType = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'appId',
          'type' => \TType::STRING,
          ),
        2 => array(
          'var' => 'appType',
          'type' => \TType::STRING,
          ),
        );
    }
    if (is_array($vals)) {
      parent::__construct(self::$_TSPEC, $vals);
    }
  }

  public function getName() {
    return 'TLuaAppHostManager_createApp_args';
  }

  public function read($input)
  {
    return $this->_read('TLuaAppHostManager_createApp_args', self::$_TSPEC, $input);
  }
  public function write($output) {
    return $this->_write('TLuaAppHostManager_createApp_args', self::$_TSPEC, $output);
  }
}


?>
