[![Build Status](https://travis-ci.org/ZenProjects/Zabbix-PHP-Module.svg?branch=master)](https://travis-ci.org/ZenProjects/Zabbix-PHP-Module)
[![Stories in Ready](https://badge.waffle.io/ZenProjects/ZBX-PHP.png?label=ready&title=Ready)](https://waffle.io/ZenProjects/ZBX-PHP)
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](http://www.gnu.org/licenses/gpl-3.0)
[![](https://piwik.ch2o.info/piwik.php?idsite=4)](https://piwik.ch2o.info)

# Zabbix PHP Loadable Module

This directory contains a [Zabbix Loadable module](https://www.zabbix.com/documentation/3.2/manual/config/items/loadablemodules), which extends functionality of Zabbix Agent/Server/Proxy. 

This module add the ability to load PHP interpreter inside Zabbix Server/Proxy/Agent address space.

Is based on my precedente work https://www.zabbix.com/forum/showthread.php?t=8305 to add the possibility to call script inside the zabbix engine. At this time I talked with Alexei Vladishev to add the ability to load module ... Now it's done! 

With that module you can extend functionality of the Zabbix with PHP module at the infinite.

# Prerequisite :

The **PHP Embed SAPI - libphp5.so** :

Install it on ubuntu trusty :
```
# apt-get install libphp5-embed php5-dev
```

Install it on centos 7 :
```
# yum install php-embedded php-cli php-devel 
```

Or compile it (the important option are "--enable-embed") from [php 5.6 source](http://php.net/downloads.php#v5.6.26) :
```
# wget http://download.php.net/to/php/embed/php-5.6.x.tar.gz
# tar xzvf php-5.6.x.tar.gz
# cd php-5.6.x
# ./configure --enable-embed --prefix=/path/to/php/install/dir \
		      --with-snmp=shared --with-ldap=shared --enable-shared=yes  \
		      --with-curl=shared  --with-mysqli=shared 
# make
# make install
```
For example to have **libphp5.so** embeded library with snmp, ldap, curl and mysqli shared module.

# How to Build the module

Compile the **zbx_php** module with php :

For have get zabbix include necessary to build the module you must download [zabbix source](http://www.zabbix.com/download.php) :
```
# wget http://download.zabbix.com/path/tozabbix/zabbix.tar.gz
# tar xzvf zabbix.tar.gz
# cd zabbix
# ./configure
# cd -
```

Them compile the module.
```
# ./bootstrap.sh
# ./configure --with-php=/path/to/php/install/dir 
              --with-zabbix-include=./zabbix/include 
              --prefix=/path/to/zabbix/install/modules/dir
# make
# make install
```	      
It should produce **zbx_php.so** in **/path/to/zabbix/install/modules/dir**.

# Configure zbx_php with zabbix

Zabbix agent, server and proxy support two parameters to deal with modules:

- **LoadModulePath** – full path to the location of loadable modules, where to copy **zbx_php.so**
- **LoadModule** – module(s) to load at startup. The modules must be located in a directory specified by **LoadModulePath**. It is allowed to include multiple **LoadModule** parameters.

For example, to extend Zabbix agent we could add the following parameters:

```
	LoadModulePath=/path/to/zabbix/install/modules/dir
	LoadModule=zbx_php.so
```

Upon agent startup it will load the **zbx_php.so** modules from the **/path/to/zabbix/install/modules/dir** directory. It will fail if a module is missing, in case of bad permissions or if a shared library is not a Zabbix module.

# Zabbix Frontend configuration

Loadable modules are supported by Zabbix **agent**, **server** and **proxy**. Therefore, item type in Zabbix frontend depends on where the module is loaded. If the module is loaded into the **agent**, then the item type should be **“Zabbix agent”** or **“Zabbix agent (active)”**. If the module is loaded into **server** or **proxy**, then the item type should be **“Simple check”**.

- **zbx_php.ping** - always returns '1'
- **zbx_php.version** - returns the php version
- **php[phpscript.php, param1, param2, ...]** - execute phpscript with params

# Configure module

The module as config file in the same place of the Zabbix Agentd/Server/proxy are, named **zbx_php.conf**.

for the moment containt only one parameter **PHP_SCRIPT_PATH**, that specify where php script are searched to execute:

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

The module set **"max_execution_time"** according to **"Timeout"** zabbix configuration setting, after this time the php interpreter interupt the script and zabbix receve **"ZBX_NOT_SUPPORTED"** on the metric item.

> **Warning note : http://php.net/manual/en/function.sleep.php#33732**
>
> The **set_time_limit()** function and the configuration directive **max_execution_time** only affect the execution time of the script itself. Any time spent on activity that happens outside the execution of the script such as system calls using system(), the sleep() function, database queries, etc. is not included when determining the maximum time that the script has been running.

You must pay attention on this when you code your scripts because Zabbix not abort your script on timeout and they can block zabbix unlimitedly if you call PHP extension that block...

The module set tree variable to the script:
- **zabbix_timeout** - setted to **"Timeout"** zabbix configuration parametter, by default to 3 in zabbix configuration.
- **zabbix_key** - while be **"php[param1,param2,...]"**
- **zabbix_params** - array starting with php at indice 0 and followed by argument sended to the module (that are in [...])

By **default** the php ini parametter are to :
- **html_errors** = 0
- **register_argc_argv** = 1
- **implicit_flush** = 1
- **output_buffering** = 0
- **max_input_time** = -1

To execute the script **"test.php"** in **"PHP_SCRIPT_PATH"** directory with arguments "mon test a moi" by the module:
```
# zabbix_get  -s 127.0.0.1 -k php[test.php,mon test a moi]
....
```
# Samples

With this items `php[snmpget.php,<hostname>,<community>]` they do snmp get of `IF-MIB::ifInOctets.1` on `<hostname>`, with `<community>`.
```php
<?php
        $zabbix_hostname=$zabbix_params[1];
        $zabbix_community=$zabbix_params[2];
	snmp_set_quick_print(1);
	$snmp_retval = snmpget($zabbix_hostname, $zabbix_community, "IF-MIB::ifInOctets.1");
	return $snmp_retval;
```

You can find sample script in **"scripts_examples"** folder of the project.

# Some idea that can do with this module

- Monitoring ldap directory in ldap:

https://docs.oracle.com/cd/E19476-01/821-0506/monitoring-using-cn-monitor.html

http://www.openldap.org/devel/admin/monitoringslapd.html

http://php.net/manual/en/book.ldap.php

- Monitoring sql database with sql requesting:

https://www.vividcortex.com/blog/2014/11/03/mysql-query-performance-statistics-in-the-performance-schema/

http://php.net/manual/en/ref.pdo-mysql.php

https://dev.mysql.com/doc/refman/5.6/en/performance-schema.html

https://blog.serverdensity.com/how-to-monitor-mysql/ 

- SNMP equipement that need data manipulation before sending result to zabbix

http://php.net/manual/en/book.snmp.php

- With ssh2 you can call script on other machine in php

http://php.net/manual/en/book.ssh2.php

- With rpc protocol (soap/rest/xml-rpc/trait...) you can get remote information 

http://php.net/manual/en/book.soap.php

http://php.net/manual/en/book.xmlrpc.php

http://thrift.apache.org/ 

https://thrift.apache.org/tutorial/php

- With sockets you can call many network service to retreive information

http://php.net/manual/en/book.sockets.php

- You can check dns

http://php.net/manual/en/function.dns-get-record.php

https://netdns2.com/documentation/examples/

- With curl you can use many protocol to retrieve information

http://php.net/manual/en/book.curl.php

- Many nosql have there statistics acessible with her protocol like memcache

http://php.net/manual/en/memcached.getstats.php

- Linux kernel monitoring in parsing /proc

https://www.kernel.org/doc/Documentation/filesystems/proc.txt

http://blog.tsunanet.net/2011/03/out-of-socket-memory.html

http://kaivanov.blogspot.fr/2013/01/troubleshooting-out-of-socket-memory.html

- Disk drive space

http://php.net/manual/en/function.disk-total-space.php

http://php.net/manual/en/function.disk-free-space.php

- nginx with http get or curl module

http://nginx.org/en/docs/http/ngx_http_stub_status_module.html?utm_campaign=monitor-nginx-2014-aug&utm_medium=post&utm_source=blog

- apache with mod_status ou mod_status_text

https://httpd.apache.org/docs/current/mod/mod_status.html

https://github.com/ZenProjects/Apache-Status-Text-Module

- docker with docker api... using "/containers/(id or name)/stats" api...

https://docs.docker.com/engine/reference/api/docker_remote_api/

https://github.com/docker-php/docker-php

- many kernel statistics of /proc like /proc/diskstats or /proc/net/tcp or /proc/net/sockstat

http://www.onlamp.com/pub/a/linux/2000/11/16/LinuxAdmin.html

http://linux.die.net/man/5/proc
