CREATE TABLE `lua_account` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `account` varchar(128) NOT NULL,
  `passwd` varchar(128) NOT NULL,
  `status` int(10) unsigned NOT NULL,
  `create_time` datetime NOT NULL,
  `last_update_time` datetime NOT NULL,
  PRIMARY KEY (`id`),
  KEY `lua_account_i1` (`account`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;