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
#include "lib/profiler.h"
#include "base\exception.h"
#include "base\invalid_call_exception.h"
#include "base\invalid_param_exception.h"
#include "base\unknown_property_exception.h"
#include "base\unknown_method_exception.h"
#include "base\user_exception.h"
#include "base\invalid_config_exception.h"
#include "baseyii.h"
#include "yii.h"
#include "base/object.h"
#include "base/component.h"
#include "ext/standard/php_var.h"
#include "ext/standard/php_smart_str.h"




/* If you declare any globals in php_my_yii.h uncomment this:
*/
ZEND_DECLARE_MODULE_GLOBALS(my_yii)


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


/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
*/
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("my_yii.yii_path", "", PHP_INI_ALL, OnUpdateString, yii_path, zend_my_yii_globals, my_yii_globals)
	STD_PHP_INI_BOOLEAN("my_yii.track_error",0,PHP_INI_ALL,OnUpdateBool,track_error,zend_my_yii_globals,my_yii_globals)
PHP_INI_END()

/* }}} */

/* {{{ php_my_yii_init_globals
 */
/* Uncomment this function if you have INI entries
*/
static void php_my_yii_init_globals(zend_my_yii_globals *my_yii_globals)
{
	my_yii_globals->yii_path = NULL;
	my_yii_globals->track_error = 0;
}

/* }}} */

/** {{{ PHP_GINIT_FUNCTION
*/
PHP_GINIT_FUNCTION(my_yii)
{
	MY_YII_G(yii_path) = NULL;
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(my_yii)
{
	/* If you have INI entries, uncomment these lines 
	*/
	REGISTER_INI_ENTRIES();
	
	EXT_STARTUP(yii_baseyii);
	EXT_STARTUP(yii);
	EXT_STARTUP(yii_base_object);
	EXT_STARTUP(yii_base_exception);
	EXT_STARTUP(yii_base_invalid_call_exception);
	EXT_STARTUP(yii_base_invalid_param_exception);
	EXT_STARTUP(yii_base_unknown_propery_exception);
	EXT_STARTUP(yii_base_unknown_method_exception);
	EXT_STARTUP(yii_base_user_exception);
	EXT_STARTUP(yii_base_invalid_config_exception);
	EXT_STARTUP(yii_base_component);

	yii_buildin_exceptions[YII_EXCEPTION_OFFSET(YII_BASE_EXCEPTION)]					= yii_base_exception_ce;
	yii_buildin_exceptions[YII_EXCEPTION_OFFSET(YII_BASE_USER_EXCEPTION)]				= yii_base_user_exception_ce;
	yii_buildin_exceptions[YII_EXCEPTION_OFFSET(YII_BASE_INVALID_CALL_EXCEPTION)]		= yii_base_invalid_call_exception_ce;
	yii_buildin_exceptions[YII_EXCEPTION_OFFSET(YII_BASE_INVALID_CONFIG_EXCEPTION)]		= yii_base_invalid_config_exception_ce;
	yii_buildin_exceptions[YII_EXCEPTION_OFFSET(YII_BASE_UNKNOWN_METHOD_EXCEPTION)]		= yii_base_unknown_method_exception_ce;
	yii_buildin_exceptions[YII_EXCEPTION_OFFSET(YII_BASE_UNKNOWN_PROPERTY_EXCEPTION)]	= yii_base_unknown_propery_exception_ce;
	yii_buildin_exceptions[YII_EXCEPTION_OFFSET(YII_BASE_INVALID_PARAM_EXCEPTION)]		= yii_base_invalid_param_exception_ce;
	if (YII_G(track_error)){
	//	yii_init_error_hooks(TSRMLS_C);
	}
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(my_yii)
{
	/* uncomment this line if you have INI entries
	*/
	if (YII_G(track_error)){
	//	yii_recovery_error_hooks(TSRMLS_C);
	}
	UNREGISTER_INI_ENTRIES();
	
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(my_yii)
{
	YII_G(yii_path) = NULL;
//	yii_profile_init(TSRMLS_C);
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(my_yii)
{
	if (YII_G(yii_path)){
		efree(YII_G(yii_path));
	}
//	yii_profile_destory(TSRMLS_C);
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
	PHP_MODULE_GLOBALS(my_yii),
	PHP_GINIT(my_yii),
	NULL,
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_MY_YII
ZEND_GET_MODULE(my_yii)
#endif

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

static zval* parseResponse(php_stream *stream TSRMLS_DC)
{
	char buffer[1024] = { 0 };
	php_stream_gets(stream, buffer, 1024);
	zval *retval = NULL;
	MAKE_STD_ZVAL(retval);
	switch (buffer[0]){
	case '+':
		ZVAL_TRUE(retval);
		break;
	case '-':
		ZVAL_FALSE(retval);
		break;
	case ':':
		ZVAL_STRING(retval, buffer, 1);
		break;
	case '$':
		if (buffer[1] == '-1'){
			ZVAL_NULL(retval);
		}
		else{
			int length = atoi(buffer + 1) + 2;
			int readLen = 0;
			smart_str data = { 0 };
			do{
				buffer[0] = '\0';
				php_stream_gets(stream, buffer, 1024);
				readLen = strlen(buffer);
				smart_str_appendl(&data, buffer, readLen);
				length -= readLen;
			} while (length > 0);
			if (data.len > 0){
				smart_str_0(&data);
				ZVAL_STRINGL(retval, data.c, data.len-2, 0);
			}
		}
		break;
	case '*':
		array_init(retval);
		int lines = atoi(buffer + 1);
		for (int i = 0; i < lines; i++){
			add_next_index_zval(retval, parseResponse(stream TSRMLS_CC));
		}
		break;
	default:
		ZVAL_FALSE(retval);
		break;
	}
	return retval;
}
	
static void execute_command(php_stream *stream TSRMLS_DC, char *command, int command_len, ...)
{
	 int command_count = 0;
	 smart_str redis_command_head = { 0 };
	 smart_str redis_command_body = { 0 };
	 smart_str_appendc(&redis_command_body, '$');
	 smart_str_append_long(&redis_command_body, (long)command_len);
	 smart_str_appendc(&redis_command_body, '\r');
	 smart_str_appendc(&redis_command_body, '\n');
	 smart_str_appendl(&redis_command_body, command, command_len);
	 smart_str_appendc(&redis_command_body, '\r');
	 smart_str_appendc(&redis_command_body, '\n');
	 command_count++;
	 va_list args;
	 va_start(args, command_len);
	 char *p;
	 do{
		 p = va_arg(args, char *);
		 if (p == NULL){
			 break;
		 }
		 smart_str_appendc(&redis_command_body, '$');
		 smart_str_append_long(&redis_command_body, (long)strlen(p));
		 smart_str_appendc(&redis_command_body, '\r');
		 smart_str_appendc(&redis_command_body, '\n');
		 smart_str_appends(&redis_command_body, p);
		 smart_str_appendc(&redis_command_body, '\r');
		 smart_str_appendc(&redis_command_body, '\n');
		 command_count++;
	 } while (1);
	 va_end(args);
	 smart_str_0(&redis_command_body);
	 smart_str_appendc(&redis_command_head, '*');
	 smart_str_append_long(&redis_command_head, (long)command_count);
	 smart_str_appendc(&redis_command_head, '\r');
	 smart_str_appendc(&redis_command_head, '\n');
	 smart_str_0(&redis_command_head);

	 php_printf("%s%s", redis_command_head.c, redis_command_body.c);
	 php_stream_write(stream, redis_command_head.c, redis_command_head.len);
	 php_stream_write(stream, redis_command_body.c, redis_command_body.len);
	
	 zval *result = parseResponse(stream TSRMLS_CC);
	 php_var_dump(&result, 1 TSRMLS_CC);
	 zval_dtor(result);
	 efree(result);
	 smart_str_free(&redis_command_head);
	 smart_str_free(&redis_command_body);
}
ZEND_FUNCTION(yii_test)
{
	/*
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
	*/
	/*
	HashTable *auto_globals = CG(auto_globals);
	if (auto_globals){
		zend_hash_internal_pointer_reset(auto_globals);
		while (zend_hash_has_more_elements(auto_globals) == SUCCESS){
			zend_auto_global *tmp;
			if (zend_hash_get_current_data(auto_globals, &tmp) == SUCCESS){
				php_printf("%s\n", tmp->name);
			}
			zend_hash_move_forward(auto_globals);
		}
	}
	*/
	/*
	HashTable *call_symbol_table = NULL;
	if (EG(active_symbol_table)){
		call_symbol_table = EG(active_symbol_table);
	}
	ALLOC_HASHTABLE(EG(active_symbol_table));
	zend_hash_init(EG(active_symbol_table), 8, NULL, ZVAL_PTR_DTOR, 0);
	*/
	/*
	zval *val=NULL;
	yii_include("index.php", NULL TSRMLS_CC);
	if (val){
		php_var_dump(&val, 1 TSRMLS_CC);
	}
	*/
	/*
	if (call_symbol_table){
		zend_hash_destroy(EG(active_symbol_table));
		FREE_HASHTABLE(EG(active_symbol_table));
		EG(active_symbol_table) = call_symbol_table;
	}
	*/
	php_stream *stream = NULL;
	char *host = "127.0.0.1:6379";
	struct timeval tv = { 5, 0 };
	char *errorstr = NULL;
	int errono = 0;
	stream = php_stream_xport_create(
		host, 
		strlen(host),
		ENFORCE_SAFE_MODE|REPORT_ERRORS,
		STREAM_XPORT_CLIENT|STREAM_XPORT_CONNECT,
		0,
		&tv,
		NULL,
		&errorstr,
		&errono
		);
	if (stream == NULL){
		php_printf("connect redis error\n");
		return;
	}
	execute_command(stream TSRMLS_CC, "SELECT", 6, "1", NULL);
	//create_command(stream TSRMLS_CC,"SET", 3, "name", "zhangsan", NULL);
	execute_command(stream TSRMLS_CC, "GET", 3, "name", NULL);
	php_stream_free(stream, PHP_STREAM_FREE_CLOSE);
}