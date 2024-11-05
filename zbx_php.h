/*
** ZABBIX PHP Lodable Module
** Copyright (C) 2007-2016 Mathieu CARBONNEAUX
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
*/


#ifndef ZABBIX_CHECKS_PHP_H
#define ZABBIX_CHECKS_PHP_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>

// this macro are defined in php and in zabbix
// to avoid warning in redefiniting it they are undef her befor 
// zabbix header inclusion
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#undef PACKAGE_NAME

// autoconf include
#include "zbx_php_config.h"

// zabbix include
#ifndef HAVE_SYSINC_H
#include "common/zbxsysinc.h"
#include "common/zbxtypes.h"
#include "zbxcommon.h"
#include "zbxcfg.h"
#include "zbxlog.h"
#else 
#include "sysinc.h"
#include "common.h"
#include "cfg.h"
#include "log.h"
#endif

#include "module.h"
#include "version.h"
//#include "db.h"
//#include "sysinfo.h"

#include "GetPathDir.h"

extern char 	*CONFIG_LOAD_MODULE_PATH;

#endif

