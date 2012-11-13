<?php
/**
 *  @generated
 */
namespace ge\lua\service\thrift;
class TLuaAppHostManager_listApp_args extends \TBase {
  static $_TSPEC;


  public function __construct() {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        );
    }
  }

  public function getName() {
    return 'TLuaAppHostManager_listApp_args';
  }

  public function read($input)
  {
    return $this->_read('TLuaAppHostManager_listApp_args', self::$_TSPEC, $input);
  }
  public function write($output) {
    return $this->_write('TLuaAppHostManager_listApp_args', self::$_TSPEC, $output);
  }
}


?>
