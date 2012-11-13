<?php
/**
 *  @generated
 */
namespace ge\lua\service\thrift;
class TLuaAppCallResult extends \TBase {
  static $_TSPEC;

  public $done = null;
  public $result = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'done',
          'type' => \TType::BOOL,
          ),
        2 => array(
          'var' => 'result',
          'type' => \TType::STRING,
          ),
        );
    }
    if (is_array($vals)) {
      parent::__construct(self::$_TSPEC, $vals);
    }
  }

  public function getName() {
    return 'TLuaAppCallResult';
  }

  public function read($input)
  {
    return $this->_read('TLuaAppCallResult', self::$_TSPEC, $input);
  }
  public function write($output) {
    return $this->_write('TLuaAppCallResult', self::$_TSPEC, $output);
  }
}


?>
