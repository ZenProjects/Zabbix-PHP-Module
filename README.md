[![Stories in Ready](https://badge.waffle.io/ZenProjects/ZBX-PHP.png?label=ready&title=Ready)](https://waffle.io/ZenProjects/ZBX-PHP)
# Zabbix PHP Loadable Module

This directory contains a [Zabbix Loadable module](https://www.zabbix.com/documentation/3.2/manual/config/items/loadablemodules), which extends functionality of Zabbix Agent/Server/Proxy. 

Like [zlm-cython](https://github.com/vulogov/zlm-cython) this module is loading PHP interpreter inside Zabbix Server/Proxy/Agent address space.

Is based on my precedente work https://www.zabbix.com/forum/showthread.php?t=8305 to add the possibility to call script inside the zabbix engine.

it's at this epoc that i'have exchanged with Alexei Vladishev to add the possibility of loadable module...

Now is done ! thank Alexei !

With that module you can extend functionality of the Zabbix with PHP module at the infinite.

# Prerequisite :

The **PHP Embed SAPI - libphp5.so** :

How to compile it (the important option are "--enable-embed") :
```
# ./configure --enable-embed --prefix=/path/to/php/install/dir \
		      --with-snmp=shared --with-ldap=shared --enable-shared=yes  \
		      --with-curl=shared  --with-mysqli=shared 
# make
# make install
```
For example to have libphp5.so embeded library with snmp, ldap, curl and mysqli shared module.

# How to Build the module

Compile the zbx_php module with php :

```
# ./bootstrap.sh
# ./configure --with-php=/path/to/php/install/dir 
              --with-zabbix-include=/path/to/zabbix/include/dir 
              --prefix=/path/to/zabbix/install/modules/dir
# make
# make install
```	      
It should produce zbx_php.so in /path/to/zabbix/install/modules/dir.

# Configure zbx_php with zabbix

Zabbix agent, server and proxy support two parameters to deal with modules:

- **LoadModulePath** – full path to the location of loadable modules, where to copy zbx_php.so
- **LoadModule** – module(s) to load at startup. The modules must be located in a directory specified by LoadModulePath. It is allowed to include multiple LoadModule parameters.

For example, to extend Zabbix agent we could add the following parameters:

```
	LoadModulePath=/usr/local/lib/zabbix/agent/
	LoadModule=zbx_php.so
```

Upon agent startup it will load the zbx_php.so modules from the /usr/local/lib/zabbix/agent directory. It will fail if a module is missing, in case of bad permissions or if a shared library is not a Zabbix module.

# Zabbix Frontend configuration

Loadable modules are supported by Zabbix agent, server and proxy. Therefore, item type in Zabbix frontend depends on where the module is loaded. If the module is loaded into the agent, then the item type should be “Zabbix agent” or “Zabbix agent (active)”. If the module is loaded into server or proxy, then the item type should be “Simple check”.

- **zbx_php.ping** - always returns '1'
- **zbx_php.version** - returns the php version
- **php[phpscript.php, param1, param2, ...]** - execute phpscript with params

# Configure module

The module as config file in the same place of the Zabbix Agentd/Server/proxy are, named **zbx_php.conf**.

for the moment containt only one parameter PHP_SCRIPT_PATH, that specify where php script are searched to execute:

```
	PHP_SCRIPT_PATH=/usr/local/lib/zabbix/phpscripts
```

# How to test module

To get php version with what the module are compiled :
```
# zabbix_get  -s 127.0.0.1 -k zbx_php.version
5.6.23
```

To ping the module:
```
# zabbix_get  -s 127.0.0.1 -k zbx_php.ping
1
```

# How to code script

Generale example are :
```
<?php
   return $myitemvalue;
```

***$myitemvalue*** can be numeric (integer or float/double), string or boulean.

The module set the type returned correctly accordingly to the dectected type from php variable.

The module set **max_execution_time** according to **Timeout** zabbix configuration setting, after this time the php interpreter interupt the script and zabbix receve ZBX_NOT_SUPPORTED on the metric item.

warning : http://php.net/manual/en/function.sleep.php#33732

	Note: The **set_time_limit()** function and the configuration directive **max_execution_time** only affect the execution time of the script itself. Any time spent on activity that happens outside the execution of the script such as system calls using system(), the sleep() function, database queries, etc. is not included when determining the maximum time that the script has been running.

The module set tree variable to the script:
- **zabbix_timeout** - setted to **Timeout** zabbix configuration parametter, by default to 3 in zabbix configuration.
- **zabbix_key** - normaly "php"
- **zabbix_params** - array starting with php and followed by argument sended to the module (that are in [...])

By **default** the php ini parametter are to :
- **html_errors** = 0
- **register_argc_argv** = 1
- **implicit_flush** = 1
- **output_buffering** = 0
- **max_input_time** = -1

To execute the script **"test.php"** in **PHP_SCRIPT_PATH** directory with arguments "mon test a moi" by the module:
```
# zabbix_get  -s 127.0.0.1 -k php[test.php,mon test a moi]
....
```

# Some idea that can do with this module

- Monitoring ldap directory in ldap:

https://docs.oracle.com/cd/E19476-01/821-0506/monitoring-using-cn-monitor.html
http://php.net/manual/fr/book.ldap.php
- Monitoring mysql with sql requesting:

https://www.vividcortex.com/blog/2014/11/03/mysql-query-performance-statistics-in-the-performance-schema/
http://php.net/manual/fr/ref.pdo-mysql.php
https://dev.mysql.com/doc/refman/5.6/en/performance-schema.html
https://blog.serverdensity.com/how-to-monitor-mysql/ 
- SNMP equipement that need data manipulation before sending result to zabbix

http://php.net/manual/fr/book.snmp.php
- With ssh2 you can call script on other machine in php

http://php.net/manual/fr/book.ssh2.php
- With rpc protocol (soap/rest/xml-rpc/trait...) you can get remote information 

http://php.net/manual/fr/book.soap.php
http://php.net/manual/fr/book.xmlrpc.php
- With sockets you can call many network service to retreive information

http://php.net/manual/fr/book.sockets.php
- You can check dns

http://php.net/manual/fr/function.dns-get-record.php
https://netdns2.com/documentation/examples/
- With curl you can use many protocol to retrieve information

http://php.net/manual/fr/book.curl.php
- Many nosql have there statistics acessible with her protocol like memcache

http://php.net/manual/fr/memcached.getstats.php
- Linux kernel monitoring in parsing /proc

https://www.kernel.org/doc/Documentation/filesystems/proc.txt
http://blog.tsunanet.net/2011/03/out-of-socket-memory.html
http://kaivanov.blogspot.fr/2013/01/troubleshooting-out-of-socket-memory.html
- Monitorer les disques

http://php.net/manual/fr/function.disk-total-space.php
http://php.net/manual/fr/function.disk-free-space.php


