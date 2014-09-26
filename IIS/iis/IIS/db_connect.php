<?php
$dbc = @mysql_connect($db_server, $db_login, $db_password) OR die('Cannot connect to MySQL: ' . mysql_error());
mysql_select_db($db_database) OR die('Cannot choose database : ' . mysql_error());
mysql_query('SET NAMES utf-8');
mysql_query("SET CHARACTER SET utf8");
?>
