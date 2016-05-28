<?php
if (count($zabbix_params)!=1)
 sleep(10);
else 
 sleep($zabbix_params[1]);
return "ok";
