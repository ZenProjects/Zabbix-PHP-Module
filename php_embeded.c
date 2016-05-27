/*
* PHP Embed SAPI Helper
* Copyright (C) 2007-2016 Mathieu CARBONNEAUX
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
******************************************************************************
*
* This source simplify PHP Embed SAPI in reusable requester architecture.
* make possible to separate minit/mshutdown from rinit/rshudown 
* and to do not minit/mshudown at each php execution.
*
* generale php sapi module script execution sequence are:
*   - Initialize module (MINIT)
*   - Initialize Request (RINIT)
*   - Execute PHP script
*   - Shutdown Request (RSHUTDOWN)
*   - Shutdown Module (MSHUTDOWN)
*
* With PHP Embed SAPI they regroup module and request 
* phase and canot be donne separatly simply.
*
* Wtih this code you could do :
* - php_embed_minit
* - php_embed_rinit
*   and multiple ...
*   - php_embed_eval_string
*   - php_embed_eval_string
*   - php_embed_eval_string
*   and finaly ...
* - php_embed_rshutdown
* - php_embed_mshutdown
*
* or
* - php_embed_minit at programme start (or module load)
*   and multiple ...
*   - php_embed_rinit
*   - php_embed_execute or php_embed_eval_string
*   - php_embed_rshutdown
*
*   - php_embed_rinit
*   - php_embed_execute or php_embed_eval_string
*   - php_embed_rshutdown
*
*   - php_embed_rinit
*   - php_embed_execute or php_embed_eval_string
*   - php_embed_rshutdown
* - php_embed_mshutdown at end of the process (or module unload)
*
* PHP Internal documentation:
*  http://www.php.net/manual/en/internals2.php
*
* Extending and Embedding PHP:
*  by Sara Golemon
*  ISBN-10: 067232704X
*  https://www.amazon.fr/Extending-Embedding-PHP-Sara-Golemon/dp/067232704X
*
* Advanced PHP Programming :
*  by George Schlossnagle 
*  ISBN-10: 0672325616
*  https://www.amazon.fr/Advanced-PHP-Programming-George-Schlossnagle/dp/0672325616
*/

#include "php_embeded.h"

#if PHP_MAJOR_VERSION < 5 || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION < 5) 
    #error "Need PHP version >= 5.5 to compile this file"
#endif

/******************************************************************************
 *                                                                            *
 * Function: php_embed_eval_string                                            *
 *                                                                            *
 * Purpose: eval php script string transmit in str.                           *
 *                                                                            *
 ******************************************************************************/
int php_embed_eval_string(char *code, zval *retval_ptr, char *string_name TSRMLS_DC)
{
    int ret = 0;

    zend_try {
        ret = zend_eval_string(code, NULL, string_name TSRMLS_CC);
    } zend_catch {

    } zend_end_try();

    return ret == FAILURE;
}


int php_embed_execute(char *filename, char* key, char**params TSRMLS_DC)
{
  zend_file_handle zfd;

  zfd.type = ZEND_HANDLE_FILENAME;
  zfd.filename = filename;
  zfd.handle.fp = NULL;
  zfd.free_filename = 0;
  zfd.opened_path = NULL;
  zend_try {
           php_execute_script(&zfd TSRMLS_CC);
  } zend_end_try();
}

/***************************************************************************
 * php_embed_minit 
 *
 * PHP module initialisation phase
 *
 */
int php_embed_minit(const char* hardcoded_ini PTSRMLS_DC)
{
#ifdef ZTS
	void ***tsrm_ls = NULL;
#endif
	int ini_entries_len = 0;

#ifdef HAVE_SIGNAL_H
#if defined(SIGPIPE) && defined(SIG_IGN)
	signal(SIGPIPE, SIG_IGN); /* ignore SIGPIPE in standalone mode so
								 that sockets created via fsockopen()
								 don't kill PHP if the remote site
								 closes it.  in apache|apxs mode apache
								 does that for us!  thies@thieso.net
								 20000419 */
#endif
#endif

#ifdef ZTS
  tsrm_startup(1, 1, 0, NULL);
  tsrm_ls = ts_resource(0);
  *ptsrm_ls = tsrm_ls;
#endif

  if (hardcoded_ini!=NULL)
  {
    ini_entries_len = strlen(hardcoded_ini);
    php_embed_module.ini_entries = malloc(ini_entries_len+2);
    memcpy(php_embed_module.ini_entries, hardcoded_ini, ini_entries_len+1);
    php_embed_module.ini_entries[ini_entries_len+1] = 0;
  }

  sapi_startup(&php_embed_module);

#ifdef PHP_WIN32
  _fmode = _O_BINARY;			/*sets default for file streams to binary */
  setmode(_fileno(stdin), O_BINARY);		/* make the stdio mode be binary */
  setmode(_fileno(stdout), O_BINARY);		/* make the stdio mode be binary */
  setmode(_fileno(stderr), O_BINARY);		/* make the stdio mode be binary */
#endif

  if (hardcoded_ini!=NULL)
  {
    php_embed_module.ini_entries = malloc(sizeof(hardcoded_ini));
    memcpy(php_embed_module.ini_entries, hardcoded_ini, sizeof(hardcoded_ini));
  }

  php_embed_module.additional_functions = additional_functions;

  if (php_embed_module.startup(&php_embed_module)==FAILURE) {
	  return FAILURE;
  }
 
  /* Set some Embedded PHP defaults */
  SG(options) |= SAPI_OPTION_NO_CHDIR;
  return SUCCESS;
}

/***************************************************************************
 * php_embed_rinit 
 *
 * PHP request initialisation phase
 *
 */
int php_embed_rinit(TSRMLS_D)
{

  if (php_request_startup(TSRMLS_C)==FAILURE) {
	  php_module_shutdown(TSRMLS_C);
	  return FAILURE;
  }
  
  SG(headers_sent) = 1;
  SG(request_info).no_headers = 1;
  php_register_variable("PHP_SELF", "-", NULL TSRMLS_CC);

  return SUCCESS;
}

/***************************************************************************
 * php_embed_rshutdown 
 *
 * PHP request shutdown - "RSHUTDOWN" phase
 *
 */
void php_embed_rshutdown(TSRMLS_D)
{
	php_request_shutdown((void *) 0);
}

/***************************************************************************
 * php_embed_mshutdown 
 *
 * PHP module shutdown - "MSHUTDOWN" phase
 *
 */
void php_embed_mshutdown(TSRMLS_D)
{
	php_module_shutdown(TSRMLS_C);
	sapi_shutdown();
#ifdef ZTS
        tsrm_shutdown();
#endif
	if (php_embed_module.ini_entries) {
		free(php_embed_module.ini_entries);
		php_embed_module.ini_entries = NULL;
	}
}

