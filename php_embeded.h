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


#ifndef PHP_EMBEDED_H
#define PHP_EMBEDED_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "sapi/embed/php_embed.h"
#include "zend_interfaces.h"
#include "php_version.h"
#include "zend.h"
#include "zend_ini.h"
#include "zend_API.h"
#include "TSRM.h"

 int 		php_embed_rinit(TSRMLS_D);
void 		php_embed_excute(zval *exception TSRMLS_DC);
int 		php_embed_eval_string(char *str, zval *retval_ptr, char *string_name TSRMLS_DC);
void 		php_embed_mshutdown(TSRMLS_D);
void 		php_embed_rshutdown(TSRMLS_D);

BEGIN_EXTERN_C()
extern char HARDCODED_INI[];
extern const zend_function_entry additional_functions[];
END_EXTERN_C()

#endif

