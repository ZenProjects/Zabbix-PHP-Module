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

#include "zbx_php.h"

/* the variable keeps timeout setting for item processing */
static int	item_timeout = 0;
char        *php_path = NULL;
extern char *CONFIG_LOAD_MODULE_PATH;

int	zbx_module_zbx_php_ping(AGENT_REQUEST *request, AGENT_RESULT *result);
int	zbx_module_zbx_php_version(AGENT_REQUEST *request, AGENT_RESULT *result);
int	zbx_module_zbx_php_call(AGENT_REQUEST *request, AGENT_RESULT *result);

static ZBX_METRIC keys[] =
/*      KEY                     	FLAG		FUNCTION        		TEST PARAMETERS */
{
	{"zbx_php.ping",		0,		zbx_module_zbx_php_ping,	NULL},
	{"zbx_php.version",		0,		zbx_module_zbx_php_version, 	NULL},
	{"php",				CF_HAVEPARAMS,	zbx_module_zbx_php_call,	""},
	{NULL}
};

#define BUFSIZE 4096

/******************************************************************************
 *                                                                            *
 * Function: zbx_module_api_version                                           *
 *                                                                            *
 * Purpose: returns version number of the module interface                    *
 *                                                                            *
 * Return value: ZBX_MODULE_API_VERSION_ONE - the only version supported by   *
 *               Zabbix currently                                             *
 *                                                                            *
 ******************************************************************************/
int	zbx_module_api_version()
{
	return ZBX_MODULE_API_VERSION_ONE;
}

/******************************************************************************
 *                                                                            *
 * Function: zbx_module_item_timeout                                          *
 *                                                                            *
 * Purpose: set timeout value for processing of items                         *
 *                                                                            *
 * Parameters: timeout - timeout in seconds, 0 - no timeout set               *
 *                                                                            *
 ******************************************************************************/
void	zbx_module_item_timeout(int timeout)
{
	item_timeout = timeout;
}

/******************************************************************************
 *                                                                            *
 * Function: zbx_module_item_list                                             *
 *                                                                            *
 * Purpose: returns list of item keys supported by the module                 *
 *                                                                            *
 * Return value: list of item keys                                            *
 *                                                                            *
 ******************************************************************************/
ZBX_METRIC	*zbx_module_item_list()
{
	return keys;
}

/******************************************************************************
 *                                                                            *
 * Function: zbx_module_zbx_php_ping                                          *
 *                                                                            *
 * Purpose: module health check                                               *
 *                                                                            *
 ******************************************************************************/
int	zbx_module_zbx_php_ping(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	SET_UI64_RESULT(result, 1);

	return SYSINFO_RET_OK;
}

/******************************************************************************
 *                                                                            *
 * Function: load_php_env_config                                              *
 *                                                                            *
 * Purpose: load php configuration file                                       *
 *                                                                            *
 ******************************************************************************/
void load_php_env_config(void)  {
    char conf_file[BUFSIZE];
    static struct cfg_line cfg[] = {
	    { "PHPPATH", &php_path, TYPE_STRING, PARM_MAND, 0, 0 },
	    { NULL },
    };
    zbx_snprintf(conf_file, BUFSIZE, "%s/php.cfg", CONFIG_LOAD_MODULE_PATH);
    parse_cfg_file(conf_file, cfg, ZBX_CFG_FILE_OPTIONAL, ZBX_CFG_STRICT); // use zabbix config parser
}

/******************************************************************************
 *                                                                            *
 * Function: zbx_module_init                                                  *
 *                                                                            *
 * Purpose: the function is called on agent startup                           *
 *          It should be used to call any initialization routines             *
 *                                                                            *
 * Return value: ZBX_MODULE_OK - success                                      *
 *               ZBX_MODULE_FAIL - module initialization failed               *
 *                                                                            *
 * Comment: the module won't be loaded in case of ZBX_MODULE_FAIL             *
 *                                                                            *
 ******************************************************************************/
int	zbx_module_init()
{
	////////////////////////////////////////
	// php module init - "MINIT" phase
	// initialization for zbx_php_call 
	if (php_embed_minit(HARDCODED_INI)!=SUCCESS) 
	{
	  zabbix_log( LOG_LEVEL_ERR, "php_embed_minit error!!!!");
	  return NOTSUPPORTED;
	}

	return ZBX_MODULE_OK;
}

/******************************************************************************
 *                                                                            *
 * Function: zbx_module_uninit                                                *
 *                                                                            *
 * Purpose: the function is called on agent shutdown                          *
 *          It should be used to cleanup used resources if there are any      *
 *                                                                            *
 * Return value: ZBX_MODULE_OK - success                                      *
 *               ZBX_MODULE_FAIL - function failed                            *
 *                                                                            *
 ******************************************************************************/
int	zbx_module_uninit()
{
	return ZBX_MODULE_OK;
}

/******************************************************************************
 *                                                                            *
 * Function: zbx_module_zbx_php_version                                       *
 *                                                                            *
 * Purpose: return php version                                                *
 *                                                                            *
 * Return value: SYSINFO_RET_OK - success                                     *
 *                                                                            *
 ******************************************************************************/
int	zbx_module_zbx_php_version(AGENT_REQUEST *request, AGENT_RESULT *result)
{
	SET_STR_RESULT(result, strdup(PHP_VERSION));
	return SYSINFO_RET_OK;
}


int zbx_set_return_value(AGENT_RESULT *result, zval *ret) 
{
    if (IS_LONG(ret)) {
        SET_UI64_RESULT(result, Z_LVAL(ret));
    } else if (IS_BOOL(ret)) {
        SET_UI64_RESULT(result, Z_BVAL(ret));
    } else if (IS_DOUBLE(ret)) {
        SET_DBL_RESULT(result, Z_DVAL(ret));
    } else if (IS_STRING(ret)) {
        SET_STR_RESULT(result, strdup(Z_STRVAL(ret)));
    } else {
        SET_MSG_RESULT(result, strdup("PHP returned unsupported value"));
        return SYSINFO_RET_FAIL;
    }
    return SYSINFO_RET_OK;
}
/******************************************************************************
 *                                                                            *
 * Function: zbx_module_zbx_php_call                                          *
 *                                                                            *
 * Purpose: retrieve data from PHP script executed                            *
 *          on ZABBIX server/proxy/agent.                                     *
 *                                                                            *
 * Parameters: request - structure that contains item key and parameters      *
 *              request->key - item key without parameters                    *
 *              request->nparam - number of parameters                        *
 *              request->timeout - processing should not take longer than     *
 *                                 this number of seconds                     *
 *              request->params[N-1] - pointers to item key parameters        *
 *                                                                            *
 *             result - structure that will contain result                    *
 *                                                                            *
 * Return value: SYSINFO_RET_FAIL - function failed, item will be marked      *
 *                                 as not supported by zabbix                 *
 *               SYSINFO_RET_OK - success                                     *
 *                                                                            *
 * Comment: get_rparam(request, N-1) can be used to get a pointer to the Nth  *
 *          parameter starting from 0 (first parameter). Make sure it exists  *
 *          by checking value of request->nparam.                             *
 *                                                                            *
 ******************************************************************************/
int	zbx_module_zbx_php_call(AGENT_REQUEST *request, AGENT_RESULT *result)
{
#ifdef ZTS
	void ***tsrm_ls;
#endif
	char    scriptname[MAX_STRING_LEN];
	char    key[MAX_STRING_LEN];
	char    params[MAX_STRING_LEN];
	char    error[MAX_STRING_LEN];
	char    cmd[MAX_STRING_LEN];
	long new_timeout;
	char *new_timeout_str;
	int new_timeout_strlen;
	char    *p,*p2;
	int     i;
	zval *php_key; // string sended to the php script, containing request->key 
	zval *php_params; // array sended to the php script, containing request->params
	zval *php_timeout; // long sended to the php script, containing request->timeout
	zval retval;

	int     ret = SYSINFO_RET_OK;

	zabbix_log( LOG_LEVEL_DEBUG, "In get_value_php([%s])",request->key);

	//init_result(result);

	if ((request->nparam < 1) || (strlen(get_rparam(request, 0)) == 0))   {
	    SET_MSG_RESULT(result, strdup("Invalid number of parameters."));
	    return SYSINFO_RET_FAIL;
	}

        if (false)
	{
		zbx_snprintf(error,MAX_STRING_LEN-1,"this key for PHP check [%s] is not supported", item->key);
		zabbix_log( LOG_LEVEL_ERR, "%s", error);
		SET_STR_RESULT(result, strdup(error));
		//return NOTSUPPORTED;
	        return SYSINFO_RET_FAIL;
	}

	zbx_snprintf(cmd, MAX_STRING_LEN-1, "Executing PHP script -%s- on -%s- with args:-%s-", request->key, item->host_name, params);
	zabbix_log( LOG_LEVEL_DEBUG, "%s", cmd );

	////////////////////////////////////////
	//// php request init - "RINIT" phase
	if (php_embed_rinit(0, NULL PTSRMLS_CC)!=SUCCESS) 
	{
	  zabbix_log( LOG_LEVEL_ERR, "php_embed_minit error!!!!");
	  return SYSINFO_RET_OK;
	}

	// tray/catch php to execute the request
	zend_first_try 
	{

	  // set php execution timeout with request->timeout
	  new_timeout_strlen = zend_spprintf(&new_timeout_str, 0, "%ld", request->timeout);

	  if (zend_alter_ini_entry_ex("max_execution_time", sizeof("max_execution_time"), new_timeout_str, new_timeout_strlen, PHP_INI_USER, PHP_INI_STAGE_RUNTIME, 0 TSRMLS_CC) != SUCCESS) 
	  {
	        efree(new_timeout_str);
		zbx_snprintf(error,MAX_STRING_LEN-1,"setting execution timeout to [%d] not succed", request->timeout);
		zabbix_log( LOG_LEVEL_ERR, "%s", error);
		SET_STR_RESULT(result, strdup(error));
	        ret=SYSINFO_RET_FAIL;
		zend_bailout();
	  }
	  efree(new_timeout_str);

	  // 
	  // prepare argument information to send to the script to execute
	  //

	  MAKE_STD_ZVAL(php_params);
          init_array(php_params); // initialize an php array to contains request->parms array

	  MAKE_STD_ZVAL(php_key); // initialize an php variable to contains the request->key string
	  MAKE_STD_ZVAL(php_timeout); // initialize an php variable to contains the request->timeout value

	  ZVAL_LONG(php_key, request->timeout, 1);
	  ZEND_SET_SYMBOL(&EG(symbol_table), "zabbix_timeout", php_timeout);

	  ZVAL_STRING(php_key, request->key, 1);
	  ZEND_SET_SYMBOL(&EG(symbol_table), "zabbix_key", php_key);

	  for (i=0;i<request->nparam;i++) {
	    add_next_index_string(return_value, get_rparam(request, i),1);
	  }
	  ZEND_SET_SYMBOL(&EG(symbol_table), "zabbix_args", php_args);

	  // re-initiliaze zval
	  memset(&retval,0,sizeof(retval));

	  // execute php script
	  zabbix_log(LOG_LEVEL_DEBUG, "PHP script: \nSCRIPT BEGIN\n%s\nSCRIPT END",request->key);
	  if (php_embed_eval_string(item->params, &retval, item->key TSRMLS_CC) == FAILURE) 
	  {
	      zbx_snprintf(error,MAX_STRING_LEN-1,"External check [%s] is not supported, failed execution", item->key);
	      zabbix_log( LOG_LEVEL_ERR, "%s", error);
	      SET_STR_RESULT(result, strdup(error));
	      zabbix_php_exception(EG(exception) TSRMLS_CC);
	      ret=NOTSUPPORTED;
	      zend_bailout();
	  }
	  else
	  {
	    // set result in function of the type of zval retval
	    ret=zbx_set_return_value(result,retval);

	    // display the ret val value in string
	    convert_to_string(&retval);
	    zabbix_log(LOG_LEVEL_DEBUG, "PHP script code returned: -%s-", Z_STRVAL(retval));

	    // free php ressource
	    zval_dtor(&retval);
	    zval_dtor(php_key);
	    zval_dtor(php_args);
	    zval_dtor(php_timeout);
	  }
	} zend_catch {
	   zabbix_php_exception(EG(exception) TSRMLS_CC);
	   ret=NOTSUPPORTED;
	} zend_end_try(); 

	// php request shutdown - "RSHUTDOWN" phase
	php_embed_rshutdown(TSRMLS_C);

	return ret;
}

