<?php
/**
 *  @generated
 */
namespace ge\lua\service\thrift;
class TLuaAppInfo extends \TBase {
  static $_TSPEC;

  public $id = null;
  public $type = null;
  public $createTime = null;

  public function __construct($vals=null) {
    if (!isset(self::$_TSPEC)) {
      self::$_TSPEC = array(
        1 => array(
          'var' => 'id',
          'type' => \TType::STRING,
          ),
        2 => array(
          'var' => 'type',
          'type' => \TType::STRING,
          ),
        3 => array(
          'var' => 'createTime',
          'type' => \TType::STRING,
          ),
        );
    }
    if (is_array($vals)) {
      parent::__construct(self::$_TSPEC, $vals);
    }
  }

  public function getName() {
    return 'TLuaAppInfo';
  }

  public function read($input)
  {
    return $this->_read('TLuaAppInfo', self::$_TSPEC, $input);
  }
  public function write($output) {
    return $this->_write('TLuaAppInfo', self::$_TSPEC, $output);
  }
}


?>
