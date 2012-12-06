CREATE TABLE  `lua_stateful_object` (
  `app_id` varchar(32) NOT NULL,
  `app_type` varchar(32) NOT NULL,
  `object_id` varchar(128) NOT NULL,
  `data` longtext NOT NULL,
  `last_update_time` datetime NOT NULL,
  `create_time` datetime NOT NULL,
  `version` int(10) unsigned NOT NULL,
  PRIMARY KEY (`app_id`,`app_type`,`object_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;