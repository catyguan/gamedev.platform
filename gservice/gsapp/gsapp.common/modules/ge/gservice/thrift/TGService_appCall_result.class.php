<?php
/**
 *  @generated
 */
namespace ge\gservice\thrift;
class TGService_appCall_result extends \TBase {
  static $_TSPEC;

  public $success = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        0 => array(
          'var' => 'success',
          'type' => \TType::STRING,
          ),
        );
    }
    if (is_array($vals)) {
      parent::__construct(self::$_TSPEC, $vals);
    }
  }

  public function getName() {
    return 'TGService_appCall_result';
  }

  public function read($input)
  {
    return $this->_read('TGService_appCall_result', self::$_TSPEC, $input);
  }
  public function write($output) {
    return $this->_write('TGService_appCall_result', self::$_TSPEC, $output);
  }
}


?>
