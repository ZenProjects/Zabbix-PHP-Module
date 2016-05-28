<?php
    
if (count($zabbix_params)!=3)
  return "ko";
else
{
  ini_set("max_execution_time",$zabbix_params[2]);
  for($i=0;$i <= $zabbix_params[1];$i++) { $rand = rand (); }
}

return $zabbix_params[1];
