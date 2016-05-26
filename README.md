# Zabbix PHP Loadable Module

This directory contains a [Zabbix Loadable module](https://www.zabbix.com/documentation/3.2/manual/config/items/loadablemodules), which extends functionality of Zabbix
Agent/Server/Proxy. 

# Prerequisite :

PHP libphp.so Embed SAPI:

compile php embed sapi (the important option are "--enable-embed"):
  ./configure --enable-embed --prefix=/path/to/php/install/dir \
  --with-snmp=shared --with-ldap=shared --enable-shared=yes  \
  --with-curl=shared  --with-mysqli=shared 

for example to have libphp5.so embeded library with snmp, ldap, curl and mysqli shared module.

# build

compile the zbx_php module with php (the importante option are "--with-php=..."):
  ./configure --prefix=/path/to/zabbix/install/dir \
	      --enable-server  \
	      --with-php=/path/to/php/install/dir
	      
Run 'make' to build it. It should produce zbx_php.so.

