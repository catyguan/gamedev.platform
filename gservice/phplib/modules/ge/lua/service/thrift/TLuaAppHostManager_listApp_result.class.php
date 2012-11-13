<?php
/**
 *  @generated
 */
namespace ge\lua\service\thrift;
class TLuaAppHostManager_listApp_result extends \TBase {
  static $_TSPEC;

  public $success = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        0 => array(
          'var' => 'success',
          'type' => \TType::LST,
          'etype' => \TType::STRUCT,
          'elem' => array(
            'type' => \TType::STRUCT,
            'class' => '\ge\lua\service\thrift\TLuaAppInfo',
            ),
          ),
        );
    }
    if (is_array($vals)) {
      parent::__construct(self::$_TSPEC, $vals);
    }
  }

  public function getName() {
    return 'TLuaAppHostManager_listApp_result';
  }

  public function read($input)
  {
    return $this->_read('TLuaAppHostManager_listApp_result', self::$_TSPEC, $input);
  }
  public function write($output) {
    return $this->_write('TLuaAppHostManager_listApp_result', self::$_TSPEC, $output);
  }
}


?>
