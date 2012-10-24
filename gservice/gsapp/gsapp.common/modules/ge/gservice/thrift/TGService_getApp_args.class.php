<?php
/**
 *  @generated
 */
namespace ge\gservice\thrift;
class TGService_getApp_args extends \TBase {
  static $_TSPEC;

  public $id = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'id',
          'type' => \TType::STRING,
          ),
        );
    }
    if (is_array($vals)) {
      parent::__construct(self::$_TSPEC, $vals);
    }
  }

  public function getName() {
    return 'TGService_getApp_args';
  }

  public function read($input)
  {
    return $this->_read('TGService_getApp_args', self::$_TSPEC, $input);
  }
  public function write($output) {
    return $this->_write('TGService_getApp_args', self::$_TSPEC, $output);
  }
}


?>
