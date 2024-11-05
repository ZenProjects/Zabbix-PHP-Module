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

// PHP Embed Helper
#include "php_embeded.h"


/* the variable keeps timeout setting for item processing */
int		zbx_php_timeout = 0;
char        	*php_path = NULL;

#define ZBX_MODULE "ZBX_MODULE "

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
	zbx_php_timeout = timeout;
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
 * Purpose: load ZBX-PHP module configuration file from zbx_php.cfg in        *
 * zabbix CONFIG_FILE directory.                                              *
 *                                                                            *
 ******************************************************************************/
int load_php_env_config(void)  {
    char conf_file[MAX_STRING_LEN];
    char base_path[MAX_STRING_LEN];
    int ret=0;
	#if ZABBIX_VERSION_MAJOR <7
    static struct cfg_line cfg[] = {
	    { "PHP_SCRIPT_PATH", &php_path, TYPE_STRING, PARM_MAND, 0, 0 },
	    { NULL },
    };
	#else
    static struct cfg_line cfg[] = {
	    { "PHP_SCRIPT_PATH", &php_path, ZBX_CFG_TYPE_STRING , ZBX_CONF_PARM_MAND, 0, 0 },
	    { NULL },
    };
	#endif
    // CONFIG_FILE are populated a execution time with the default compiled path 
    // (DEFAULT_CONFIG_FILE) or path set in zabbix commande line (with -c or --config) 
    // then get basepath and add zbx_php.cfg.
    if ((ret=get_base_path_from_pathname(CONFIG_FILE,strlen(CONFIG_FILE),base_path,MAX_STRING_LEN))!=MCA_SUCCESS)
    {
      zabbix_log( LOG_LEVEL_ERR, ZBX_MODULE "load_php_env_config get base path error : %d!!!!", ret);
      return ZBX_MODULE_FAIL;
    }

    zbx_snprintf(conf_file, MAX_STRING_LEN, "%szbx_php.conf", base_path);
    zabbix_log( LOG_LEVEL_INFORMATION, ZBX_MODULE "Module Config lodaded from %s", conf_file);
    
    // use zabbix config parser
	#if ZABBIX_VERSION_MAJOR >= 7
    if (zbx_parse_cfg_file(conf_file, cfg, ZBX_CFG_FILE_OPTIONAL, ZBX_CFG_STRICT,ZBX_CFG_EXIT_FAILURE)!=SUCCESS)
	#elif ZABBIX_VERSION_MAJOR >= 6
    if (parse_cfg_file(conf_file, cfg, ZBX_CFG_FILE_OPTIONAL, ZBX_CFG_STRICT,ZBX_CFG_EXIT_FAILURE)!=SUCCESS)
	#else
	if (parse_cfg_file(conf_file, cfg, ZBX_CFG_FILE_OPTIONAL, ZBX_CFG_STRICT)!=SUCCESS)
	#endif
    {
       zabbix_log( LOG_LEVEL_ERR, ZBX_MODULE "load_php_env_config parse file error!!!!");
       return ZBX_MODULE_FAIL;
    }

    zabbix_log( LOG_LEVEL_INFORMATION, ZBX_MODULE "Module PHP_SCRIPT_PATH set to %s", php_path);
    return ZBX_MODULE_OK;
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
	TSRMLS_FETCH();

        void *handle;	
	char szPhpLib[256];
	snprintf(szPhpLib, sizeof szPhpLib, "%s%d%s", "libphp", PHP_MAJOR_VERSION, ".so");
        handle = dlopen (szPhpLib, RTLD_LAZY | RTLD_GLOBAL);
        if (!handle) {
	    zabbix_log( LOG_LEVEL_ERR, ZBX_MODULE "load %s error!!!!\n%s",szPhpLib,dlerror());
            exit(1);
        }

	// load zbx_php.cfg config file
	if (load_php_env_config()!=ZBX_MODULE_OK) 
	{
	 zabbix_log( LOG_LEVEL_ERR, ZBX_MODULE "load_php_env_config error!!!!");
	 return ZBX_MODULE_FAIL;
	}

	////////////////////////////////////////
	// php module init - "MINIT" phase
	// initialization for zbx_php_call 
	if (php_embed_minit(MY_HARDCODED_INI TSRMLS_CC)!=SUCCESS) 
	{
	  zabbix_log( LOG_LEVEL_ERR, ZBX_MODULE "php_embed_minit error!!!!");
	  return ZBX_MODULE_FAIL;
	}

	zabbix_log( LOG_LEVEL_WARNING, "PHP support:           YES");
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
    if (Z_TYPE_P(ret) == IS_LONG) {
        SET_UI64_RESULT(result, Z_LVAL_P(ret));
#if PHP_VERSION_ID < 70000
    } else if (Z_TYPE_P(ret) == IS_BOOL) {
        SET_UI64_RESULT(result, Z_BVAL_P(ret));
#else
    } else if (Z_TYPE_P(ret) == IS_TRUE) {
        SET_UI64_RESULT(result, 0);
    } else if (Z_TYPE_P(ret) == IS_FALSE) {
        SET_UI64_RESULT(result, 1);
#endif
    } else if (Z_TYPE_P(ret) == IS_DOUBLE) {
        SET_DBL_RESULT(result, Z_DVAL_P(ret));
    } else if (Z_TYPE_P(ret) == IS_STRING) {
        SET_STR_RESULT(result, strdup(Z_STRVAL_P(ret)));
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
	char php_script_filename[MAX_STRING_LEN];
	char error[MAX_STRING_LEN];
	char cmd[MAX_STRING_LEN];
	long new_timeout;
	char *new_timeout_str;
	int new_timeout_strlen;
	int     i;
#if PHP_VERSION_ID >= 70000
	zval php_key; // string sended to the php script, containing request->key 
	zval php_params; // array sended to the php script, containing request->params
	zval php_timeout; // long sended to the php script, containing request->timeout
	zval retval;
#else
	zval *php_key; // string sended to the php script, containing request->key 
	zval *php_params; // array sended to the php script, containing request->params
	zval *php_timeout; // long sended to the php script, containing request->timeout
	zval *retval;
#endif
	int     ret = SYSINFO_RET_OK;

	TSRMLS_FETCH();

	// zabbix init result
	//init_result(result);

	// check the number of argument
	if ((request->nparam < 1) || (strlen(request->params[0]) == 0))   {
	    SET_MSG_RESULT(result, strdup("Invalid number of parameters."));
	    return SYSINFO_RET_FAIL;
	}

	//////////////////////////////////////////////////
	//// check php file script existance in php_path
        zbx_snprintf(php_script_filename, MAX_STRING_LEN, "%s/%s", php_path,get_rparam(request, 0));
	if( access(php_script_filename, F_OK ) == -1 ) {
	   zbx_snprintf(cmd, MAX_STRING_LEN-1, "PHP Script file '%s' not found in script folder '%s'.", request->key, php_path);
	   SET_MSG_RESULT(result, strdup(cmd));
	   return SYSINFO_RET_FAIL;
	}


	////////////////////////////////////////
	//// php request init - "RINIT" phase
	////////////////////////////////////////
	if (php_embed_rinit(TSRMLS_C)!=SUCCESS) 
	{
	  zabbix_log( LOG_LEVEL_ERR, ZBX_MODULE "php_embed_minit error!!!!");
	  return SYSINFO_RET_OK;
	}

	// tray/catch php to execute the request
	zend_first_try 
	{

	  //////////////////////////////////////////////////////
	  // set php execution timeout with request->timeout
	  new_timeout_strlen = spprintf(&new_timeout_str, 0, "%d", zbx_php_timeout);

#if PHP_VERSION_ID >= 70000
	  zend_string *zszIniEntry = NULL;
	  zszIniEntry = zend_string_init("max_execution_time",(long unsigned int)sizeof("max_execution_time"),0);
	  if (zend_alter_ini_entry_chars_ex(zszIniEntry, (const char *)(new_timeout_str), new_timeout_strlen, ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME, 0 TSRMLS_CC) != SUCCESS) 
#else
	  if (zend_alter_ini_entry_ex("max_execution_time", sizeof("max_execution_time"), new_timeout_str, new_timeout_strlen, ZEND_INI_USER, ZEND_INI_STAGE_RUNTIME, 0 TSRMLS_CC) != SUCCESS) 
#endif
	  {
	        efree(new_timeout_str);
		zbx_snprintf(error,MAX_STRING_LEN-1,"setting execution timeout to [%d] not succed", zbx_php_timeout);
		zabbix_log( LOG_LEVEL_ERR, ZBX_MODULE "%s", error);
		SET_STR_RESULT(result, strdup(error));
	        ret=SYSINFO_RET_FAIL;
		zend_bailout();
	  }
	  efree(new_timeout_str);

	  //////////////////////////////////////////////////////////////////
	  // prepare argument information to send to the script to execute
	  //////////////////////////////////////////////////////////////////

#if PHP_VERSION_ID < 70000
	  MAKE_STD_ZVAL(php_params);
	  MAKE_STD_ZVAL(php_key); // initialize an php variable to contains the request->key string
	  MAKE_STD_ZVAL(php_timeout); // initialize an php variable to contains the request->timeout value
          array_init(php_params); // initialize an php array to contains request->parms array
	  ZVAL_LONG(php_timeout, zbx_php_timeout);
	  ZEND_SET_SYMBOL(&EG(symbol_table), "zabbix_timeout", php_timeout);
#else
          array_init(&php_params); // initialize an php array to contains request->parms array
	  ZVAL_LONG(&php_timeout, zbx_php_timeout);
	  zend_string *zszVariableName = NULL;
	  zszVariableName = zend_string_init("zabbix_timeout",(long unsigned int)sizeof("zabbix_timeout"),0);
	  zend_hash_update(&EG(symbol_table), zszVariableName, &php_timeout);
#endif

	  zbx_strlcpy(cmd,request->key,MAX_STRING_LEN);
	  zbx_strlcat(cmd,"[",MAX_STRING_LEN);

#if PHP_VERSION_ID < 70000
	  // set zabbix_params as php variable
	  for (i=0;i<request->nparam;i++) {
	    add_next_index_string(php_params, get_rparam(request, i),1);
	    zbx_strlcat(cmd,get_rparam(request, i),MAX_STRING_LEN);
	    if (i!=request->nparam-1) zbx_strlcat(cmd,",",MAX_STRING_LEN);
	    else zbx_strlcat(cmd,"]",MAX_STRING_LEN);
	  }
	  ZEND_SET_SYMBOL(&EG(symbol_table), "zabbix_params", php_params);

	  // set zabbix_key as php variable
	  ZVAL_STRING(php_key, cmd, 1);
	  ZEND_SET_SYMBOL(&EG(symbol_table), "zabbix_key", php_key);
#else
	  // set zabbix_params as php variable
	  for (i=0;i<request->nparam;i++) {
	    add_next_index_string(&php_params, get_rparam(request, i));
	    zbx_strlcat(cmd,get_rparam(request, i),MAX_STRING_LEN);
	    if (i!=request->nparam-1) zbx_strlcat(cmd,",",MAX_STRING_LEN);
	    else zbx_strlcat(cmd,"]",MAX_STRING_LEN);
	  }
	  zend_string *zszZabbixParams = NULL;
	  zszZabbixParams = zend_string_init("zabbix_params",(long unsigned int)sizeof("zabbix_params"),0);
	  zend_hash_update(&EG(symbol_table), zszZabbixParams, &php_params);
	  
	  // set zabbix_key as php variable
	  ZVAL_STRING(&php_key, cmd);
	  zend_string *zszZabbixKey = NULL;
	  zszZabbixKey = zend_string_init("zabbix_key",(long unsigned int)sizeof("zabbix_key"),0);
	  zend_hash_update(&EG(symbol_table), zszZabbixKey, &php_key);
#endif


	  zabbix_log( LOG_LEVEL_DEBUG, ZBX_MODULE "Get item : \"%s\" with script \"%s\"", cmd , php_script_filename );

#if PHP_VERSION_ID < 70400
	  // re-initiliaze zval
	  //memset(&retval,0,sizeof(retval));
	  ALLOC_INIT_ZVAL(retval);
#endif

	  //////////////////////////////
	  // execute php script
	  //////////////////////////////
#if PHP_VERSION_ID < 70000
	  if (php_embed_execute(php_script_filename,retval TSRMLS_CC) != SUCCESS) 
#else
	  if (php_embed_execute(php_script_filename,&retval TSRMLS_CC) != SUCCESS) 
#endif
	  {
	      zbx_snprintf(error,MAX_STRING_LEN-1,"External check [%s] is not supported, failed execution", request->key);
	      zabbix_log( LOG_LEVEL_ERR, ZBX_MODULE "%s", error);
	      SET_STR_RESULT(result, strdup(error));
	      ret=SYSINFO_RET_FAIL;
	      zend_bailout();
	  }
	  else
	  {
#if PHP_VERSION_ID < 70000
	    // set result in function of the type of zval retval
	    ret=zbx_set_return_value(result,retval);

	    // display the ret val value in string
	    convert_to_string(retval);
	    zabbix_log(LOG_LEVEL_DEBUG, ZBX_MODULE "PHP script code returned: \"%s\"", Z_STRVAL_P(retval));
	    
	    // free php ressource
	    zval_dtor(retval);
	    zval_dtor(php_key);
	    zval_dtor(php_params);
	    zval_dtor(php_timeout);
#else
	    // set result in function of the type of zval retval
	    ret=zbx_set_return_value(result,&retval);

	    // display the ret val value in string
	    convert_to_string(&retval);
	    zabbix_log(LOG_LEVEL_DEBUG, ZBX_MODULE "PHP script code returned: \"%s\"", Z_STRVAL(retval));
#endif

	  }
	} zend_catch {
	   ret=SYSINFO_RET_FAIL;
	} zend_end_try(); 

	///////////////////////////////////////////////
	// php request shutdown - "RSHUTDOWN" phase
	///////////////////////////////////////////////
	php_embed_rshutdown(TSRMLS_C);

	return ret;
}

