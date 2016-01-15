/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2014 The PHP Group                                 |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_my_yii.h"
#include "lib/common.h"
#include "base\exception.h"
#include "base\invalid_call_exception.h"
#include "base\unknown_property_exception.h"
#include "base\unknown_method_exception.h"
#include "base\user_exception.h"
#include "base\invalid_config_exception.h"
#include "baseyii.h"
#include "yii.h"
#include "base/object.h"




/* If you declare any globals in php_my_yii.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(my_yii)
*/

/* True global resources - no need for thread safety here */
static int le_my_yii;
zend_class_entry *yii_buildin_exceptions[YII_MAX_BUILDIN_EXCEPTION];

/* {{{ my_yii_functions[]
 *
 * Every user visible function must have an entry in my_yii_functions[].
 */
const zend_function_entry my_yii_functions[] = {
	ZEND_FE(yii_test,NULL)
	PHP_FE_END	/* Must be the last line in my_yii_functions[] */
};
/* }}} */

/* {{{ my_yii_module_entry
 */
zend_module_entry my_yii_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"my_yii",
	my_yii_functions,
	PHP_MINIT(my_yii),
	PHP_MSHUTDOWN(my_yii),
	PHP_RINIT(my_yii),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(my_yii),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(my_yii),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_MY_YII_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_MY_YII
ZEND_GET_MODULE(my_yii)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("my_yii.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_my_yii_globals, my_yii_globals)
    STD_PHP_INI_ENTRY("my_yii.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_my_yii_globals, my_yii_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_my_yii_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_my_yii_init_globals(zend_my_yii_globals *my_yii_globals)
{
	my_yii_globals->global_value = 0;
	my_yii_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(my_yii)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	EXT_STARTUP(yii_baseyii);
	EXT_STARTUP(yii);
	EXT_STARTUP(yii_base_object);
	EXT_STARTUP(yii_base_exception);
	EXT_STARTUP(yii_base_invalid_call_exception);
	EXT_STARTUP(yii_base_unknown_propery_exception);
	EXT_STARTUP(yii_base_unknown_method_exception);
	EXT_STARTUP(yii_base_user_exception);
	EXT_STARTUP(yii_base_invalid_config_exception);

	yii_buildin_exceptions[YII_EXCEPTION_OFFSET(YII_BASE_EXCEPTION)]					= yii_base_exception_ce;
	yii_buildin_exceptions[YII_EXCEPTION_OFFSET(YII_BASE_USER_EXCEPTION)]				= yii_base_user_exception_ce;
	yii_buildin_exceptions[YII_EXCEPTION_OFFSET(YII_BASE_INVALID_CALL_EXCEPTION)]		= yii_base_invalid_call_exception_ce;
	yii_buildin_exceptions[YII_EXCEPTION_OFFSET(YII_BASE_INVALID_CONFIG_EXCEPTION)]		= yii_base_invalid_config_exception_ce;
	yii_buildin_exceptions[YII_EXCEPTION_OFFSET(YII_BASE_UNKNOWN_METHOD_EXCEPTION)]		= yii_base_unknown_method_exception_ce;
	yii_buildin_exceptions[YII_EXCEPTION_OFFSET(YII_BASE_UNKNOWN_PROPERTY_EXCEPTION)]	= yii_base_unknown_propery_exception_ce;

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(my_yii)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(my_yii)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(my_yii)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(my_yii)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "my_yii support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

zend_op *get_opcode(zend_op_array *op_array, uint offset)
{
	zend_op *op = op_array->opcodes;
	zend_op *end = op + op_array->last;
	++offset;
	while (op < end){
		if ((op->opcode == ZEND_RECV || op->opcode == ZEND_RECV_INIT) && op->op1.num == (long) offset){
			return op;
		}
		op++;
	}
	return NULL;
}

ZEND_FUNCTION(yii_test)
{
	char *name;
	uint name_len;
	zend_function  *zf;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,"s",&name,&name_len) == FAILURE){
		RETURN_FALSE;
	}
	yii_strtolower(name);
	zend_op *op;
	if (zend_hash_find(EG(function_table), name, name_len + 1, (void**)&zf) == SUCCESS){
		for (int i = 0, len = zf->common.num_args; i < len; i++){
			op = get_opcode(&(zf->op_array), i);
			if (op && op->opcode == ZEND_RECV_INIT && op->op2_type != IS_UNUSED){
				if (Z_TYPE_P(op->op2.zv) == IS_STRING){
					printf("param %d : %s = %s\n", i + 1, zf->common.arg_info[i].name,Z_STRVAL_P(op->op2.zv));
				}
				else{
					zval *tmp;
					MAKE_STD_ZVAL(tmp);
					tmp = op->op2.zv;
					zval_copy_ctor(tmp);
					convert_to_string(tmp);
					printf("param %d : %s = %s\n", i + 1, zf->common.arg_info[i].name, Z_STRVAL_P(tmp));
					zval_dtor(tmp);
					efree(tmp);

				}
			}
			else{
				printf("param %d : %s = %s\n", i + 1, zf->common.arg_info[i].name, "NULL");
			}
		}
		
	}

	
}