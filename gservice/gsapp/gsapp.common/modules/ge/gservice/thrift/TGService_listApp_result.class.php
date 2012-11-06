<?php
/**
 *  @generated
 */
namespace ge\gservice\thrift;
class TGService_listApp_result extends \TBase {
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
            'class' => '\ge\gservice\thrift\TGAppInfo',
            ),
          ),
        );
    }
    if (is_array($vals)) {
      parent::__construct(self::$_TSPEC, $vals);
    }
  }

  public function getName() {
    return 'TGService_listApp_result';
  }

  public function read($input)
  {
    return $this->_read('TGService_listApp_result', self::$_TSPEC, $input);
  }
  public function write($output) {
    return $this->_write('TGService_listApp_result', self::$_TSPEC, $output);
  }
}


?>