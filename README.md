[![Stories in Ready](https://badge.waffle.io/ZenProjects/ZBX-PHP.png?label=ready&title=Ready)](https://waffle.io/ZenProjects/ZBX-PHP)
# Zabbix PHP Loadable Module

This directory contains a [Zabbix Loadable module](https://www.zabbix.com/documentation/3.2/manual/config/items/loadablemodules), which extends functionality of Zabbix
Agent/Server/Proxy. 

# Prerequisite :

The **PHP Embed SAPI - libphp.so** :

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
- **zbx_php.php[phpscript.php, param1, param2, ...]** - execute phpscript with params

# configure module

The module as config file in the same place of the Zabbix Agentd/Server/proxy are, named **zbx_php.conf**.

for the moment containt only one parameter PHP_SCRIPT_PATH, that spcify where php script are searched to execute:

```
	PHP_SCRIPT_PATH=/usr/local/lib/zabbix/phpscripts
```


# how to test module

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

To execute a script (from PHP_SCRIPT_PATH directory) with the module:
```
# zabbix_get  -s 127.0.0.1 -k zbx_php.php[test.php,mon test a moi]
....
```

# How to code script

Generale examplle are :
```
<?php
   return $myitemvalue;
```

***$myitemvalue*** can be numeric (integer or float/double), string or boulean.

The module set the type returned correctly accordingly to the dectected type from php variable.

The module set **max_execution_time** to **Timeout** zabbix configuration setting.

the module set tree variable to the script:
- zabbix_timeout - setted to **Timeout** zabbix configuration parametter, by default to 3
- zabbix_key - normaly "php"
- zabbix_params - array starting with php and followed by argument sended to the module (that are in [...])

by default the php ini parametter are to :
- html_errors = 0
- register_argc_argv = 1
- implicit_flush = 1
- output_buffering = 0
- max_input_time = -1
