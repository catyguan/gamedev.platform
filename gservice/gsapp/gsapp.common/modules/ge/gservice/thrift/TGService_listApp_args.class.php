<?php
/**
 *  @generated
 */
namespace ge\gservice\thrift;
class TGService_listApp_args extends \TBase {
  static $_TSPEC;


  public function __construct() {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        );
    }
  }

  public function getName() {
    return 'TGService_listApp_args';
  }

  public function read($input)
  {
    return $this->_read('TGService_listApp_args', self::$_TSPEC, $input);
  }
  public function write($output) {
    return $this->_write('TGService_listApp_args', self::$_TSPEC, $output);
  }
}


?>
