<?php
/**
 *  @generated
 */
namespace ge\gservice\thrift;
class TGService_appCall_args extends \TBase {
  static $_TSPEC;

  public $id = null;
  public $cmd = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'id',
          'type' => \TType::STRING,
          ),
        2 => array(
          'var' => 'cmd',
          'type' => \TType::STRING,
          ),
        );
    }
    if (is_array($vals)) {
      parent::__construct(self::$_TSPEC, $vals);
    }
  }

  public function getName() {
    return 'TGService_appCall_args';
  }

  public function read($input)
  {
    return $this->_read('TGService_appCall_args', self::$_TSPEC, $input);
  }
  public function write($output) {
    return $this->_write('TGService_appCall_args', self::$_TSPEC, $output);
  }
}


?>
