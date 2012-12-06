CREATE TABLE  `lua_auth_token` (
  `token` varchar(128) NOT NULL,
  `account_id` int(10) unsigned NOT NULL,
  `valid_time` datetime NOT NULL,
  `create_time` datetime NOT NULL,
  PRIMARY KEY (`token`),
  KEY `lua_auth_token_i1` (`account_id`),
  KEY `lua_auth_token_i2` (`valid_time`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;