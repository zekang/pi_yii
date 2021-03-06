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
#include "php_my_yii.h"
#include "yii.h"
#include "lib/common.h"
#include "ext/standard/php_var.h"
#include "ext/standard/php_string.h"
#include "ext/standard/php_smart_str.h"
#include "ext/standard/php_filestat.h"

zend_class_entry *yii_baseyii_ce;
ZEND_BEGIN_ARG_INFO_EX(arginfo_configure, 0, 1, 2)
ZEND_ARG_INFO(1,object)
ZEND_ARG_ARRAY_INFO(0,properties,0)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_void,0,0,0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_getalias,0,0,2)
ZEND_ARG_INFO(0,alias)
ZEND_ARG_INFO(0,throwException)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_getrootalias,0,0,1)
ZEND_ARG_INFO(0,alias)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_setalias,0,0,2)
ZEND_ARG_INFO(0,alias)
ZEND_ARG_INFO(0,path)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_autoload,0,0,1)
ZEND_ARG_INFO(0,className)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_createObject,0,0,2)
ZEND_ARG_INFO(0,type)
ZEND_ARG_ARRAY_INFO(0,params,1)
ZEND_END_ARG_INFO()



/** {{{ static char * yii_get_alias(const char *alias,int alias_len,int mode TSRMLS_DC)
    mode = 0 alias
	mode = 1 rootalis
*/
static char * yii_get_alias(const char *alias, int alias_len, int mode TSRMLS_DC)
{
	zval *aliases = zend_read_static_property(yii_baseyii_ce, ZEND_STRL("aliases"), 1 TSRMLS_CC);
	if (Z_TYPE_P(aliases) != IS_ARRAY){
		yii_throw_exception(YII_BASE_INVALID_PARAM_EXCEPTION TSRMLS_CC,"Yii\\BaseYii::$aliases should be Array");
		return NULL;
	}
	zend_bool find_alias = 0;
	smart_str result = { 0 };
	char *root = estrndup(alias, alias_len);
	char *pos = strchr(root, '/');
	if (pos){
		*pos = '\0';
	}
	zval **alias_values;
	if (zend_hash_find(Z_ARRVAL_P(aliases), root, strlen(root) + 1, (void **)&alias_values) == SUCCESS){
		if (Z_TYPE_PP(alias_values) == IS_STRING){
			if (mode == 0){
				smart_str_appendl(&result, Z_STRVAL_PP(alias_values), Z_STRLEN_PP(alias_values));
				if (pos){
					smart_str_appendc(&result, '/');
					smart_str_appends(&result, pos + 1);
				}
			}
			else{
				smart_str_appends(&result, root);
			}
			smart_str_0(&result);
			find_alias = 1;
		}
		else if (Z_TYPE_PP(alias_values) == IS_ARRAY){
			zval **tmp;
			char *key;
			int key_len;
			ulong idx;
			if (pos){
				*pos = '/';
			}
			for (zend_hash_internal_pointer_reset(Z_ARRVAL_PP(alias_values));
				!find_alias && zend_hash_has_more_elements(Z_ARRVAL_PP(alias_values)) == SUCCESS;
				zend_hash_move_forward(Z_ARRVAL_PP(alias_values))
				){
				if (zend_hash_get_current_key_ex(Z_ARRVAL_PP(alias_values), &key, &key_len, &idx, 0, NULL) != HASH_KEY_IS_STRING){
					continue;
				}
				if (key_len - 1 > alias_len){
					continue;
				}
				if (!strncmp(root, key, key_len - 1)){
					if (*(root + key_len - 1) == '\0' || *(root + key_len - 1) == '/'){
						if (zend_hash_get_current_data(Z_ARRVAL_PP(alias_values), (void **)&tmp) == SUCCESS){
							if (Z_TYPE_PP(tmp) == IS_STRING){
								if (mode == 0){
									smart_str_appendl(&result, Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp));
									smart_str_appends(&result, root + key_len - 1);
									smart_str_0(&result);
								}
								else{
									smart_str_appendl(&result, key, key_len - 1);
									smart_str_0(&result);
								}
								find_alias = 1;
							}
						}
					}
				}
			}
		}
	}
	efree(root);
	return find_alias ? result.c : NULL;
}
/** }}} */


/** {{{ static zend_bool yii_set_alias(char *alias,int alias_len,char *path,int path_len TSRMLS_DC)
*/
static zend_bool yii_set_alias(const char *alias, int alias_len, char *path, int path_len TSRMLS_DC)
{
	zval *aliases = zend_read_static_property(yii_baseyii_ce, ZEND_STRL("aliases"), 1 TSRMLS_CC);
	if (Z_TYPE_P(aliases) != IS_ARRAY){
		yii_throw_exception(YII_BASE_INVALID_PARAM_EXCEPTION TSRMLS_CC, "Yii\\BaseYii::$aliases should be Array");
		return FAILURE;
	}
	char *root = NULL,*pos = NULL,*alias_value = NULL;
	if (*alias == '@'){
		root = estrndup(alias, alias_len);
	}
	else{
		root = emalloc(alias_len + 1);
		*root = '@';
		memcpy(root + 1, alias, alias_len);
	}
	pos = strchr(root, '/');
	if (pos){
		*pos = '\0';
	}
	alias_value = *path == '@' ?
		yii_get_alias(path, path_len, 0 TSRMLS_CC) :
		php_trim(path, path_len, ZEND_STRL("\\/"), NULL, 2 TSRMLS_CC);
	if (!alias_value){
		alias_value = estrdup("");
	}

	zval **alias_values;
	if (zend_hash_find(Z_ARRVAL_P(aliases), root, strlen(root) + 1, (void **)&alias_values) == SUCCESS){
		if (Z_TYPE_PP(alias_values) == IS_STRING) {
			if (pos){
				zval *array;
				MAKE_STD_ZVAL(array);
				array_init(array);
				*pos = '/';
				add_assoc_string(array, root, alias_value, 1);
				*pos = '\0';
				char *root_value = yii_get_alias(root, strlen(root), 0 TSRMLS_CC);
				if (!root_value){
					root_value = estrdup("");
				}
				add_assoc_string(array, root, root_value, 0);
				add_assoc_zval(aliases, root, array);
			}
			else{
				add_assoc_string(aliases, root, alias_value, 0);
			}
		}
		else if (Z_TYPE_PP(alias_values) == IS_ARRAY){
			if (pos){
				*pos = '/';
			}
			add_assoc_string(*alias_values, root, alias_value, 0);
		}
	}
	else{
		if (pos){
			zval *array;
			MAKE_STD_ZVAL(array);
			array_init(array);
			*pos = '/';
			add_assoc_string(array, root, alias_value, 0);
			*pos = '\0';
			add_assoc_zval(aliases, root, array);
		}
		else{
			add_assoc_string(aliases, root, alias_value, 0);
		}
	}
	efree(root);
	zend_update_static_property(yii_baseyii_ce, ZEND_STRL("aliases"), aliases TSRMLS_CC);
	return SUCCESS;
}
/** }}} */


/** {{{ static zend_bool yii_delete_alias(const char *alias,int alias_len TSRMLS_DC)
*/
static zend_bool yii_delete_alias(const char *alias, int alias_len TSRMLS_DC)
{
	zval *aliases = zend_read_static_property(yii_baseyii_ce, ZEND_STRL("aliases"), 1 TSRMLS_CC);
	if (Z_TYPE_P(aliases) != IS_ARRAY){
		yii_throw_exception(YII_BASE_INVALID_PARAM_EXCEPTION TSRMLS_CC, "Yii\\BaseYii::$aliases should be Array");
		return FAILURE;
	}
	char *root = NULL, *pos = NULL;
	if (*alias == '@'){
		root = estrndup(alias, alias_len);
	}
	else{
		root = emalloc(alias_len + 1);
		*root = '@';
		memcpy(root + 1, alias, alias_len);
	}
	pos = strchr(root, '/');
	if (pos){
		*pos = '\0';
	}
	zval **alias_values;
	zend_bool change = 0;
	if (zend_hash_find(Z_ARRVAL_P(aliases), root, strlen(root) + 1, (void **)&alias_values) == SUCCESS){
		if (Z_TYPE_PP(alias_values) == IS_ARRAY){
			if (pos){
				*pos = '/';
			}
			zend_hash_del(Z_ARRVAL_PP(alias_values), root, strlen(root) + 1);
			change = 1;
		}
		else if (Z_TYPE_PP(alias_values) == IS_STRING){
			zend_hash_del(Z_ARRVAL_P(aliases), root, strlen(root) + 1);
			change = 1;
		}
	}
	efree(root);
	if (change){
		zend_update_static_property(yii_baseyii_ce, ZEND_STRL("aliases"), aliases TSRMLS_CC);
	}
	return SUCCESS;
}
/** }}} */

/** {{{ zend_bool yii_register_autoload(TSRMLS_D) */
zend_bool yii_register_autoload(TSRMLS_D)
{
	zend_fcall_info fci;
	zval function;
	zval *autoload,*ret;
	zval **params[1] = { &autoload };
	INIT_PZVAL(&function);
	ZVAL_STRING(&function, "spl_autoload_register", 0);
	zend_bool result;
	MAKE_STD_ZVAL(autoload);
	array_init(autoload);
	add_next_index_string(autoload, "Yii\\BaseYii", 1);
	add_next_index_string(autoload, "autoload", 1);
	fci.function_name  = &function;
	fci.function_table = EG(function_table);
	fci.no_separation  = 1;
	fci.size = sizeof(fci);
	fci.object_ptr = NULL;
	fci.param_count = 1;
	fci.params = (zval ***)&params;
	fci.symbol_table = NULL;
	fci.retval_ptr_ptr = &ret;
	result =  zend_call_function(&fci, NULL TSRMLS_CC);
	if (ret){
		zval_ptr_dtor(&ret);
	}
	zval_ptr_dtor(&autoload);
	return result;
}
/**/

/** {{{ proto public static Yii_BaseYii::init()
*/
ZEND_METHOD(Yii_BaseYii, init)
{
	zval *classMap, *aliases;
	if (YII_G(yii_path) == NULL){
		zval *script_filename = yii_request_query(TRACK_VARS_SERVER, ZEND_STRL("SCRIPT_FILENAME") TSRMLS_CC);
		char filename[MAX_PATH] = { 0 };
		memcpy(filename, Z_STRVAL_P(script_filename), Z_STRLEN_P(script_filename));
		int dir_len = php_dirname(filename, Z_STRLEN_P(script_filename));
		YII_G(yii_path) = estrndup(filename, dir_len);
	}
	classMap = zend_read_static_property(yii_baseyii_ce, ZEND_STRL("classMap"), 1 TSRMLS_CC);
	if (Z_TYPE_P(classMap) == IS_NULL){
		array_init(classMap);
		zend_update_static_property(yii_baseyii_ce, ZEND_STRL("classMap"), classMap TSRMLS_CC);
	}
	aliases = zend_read_static_property(yii_baseyii_ce, ZEND_STRL("aliases"), 1 TSRMLS_CC);
	if (Z_TYPE_P(aliases) == IS_NULL){
		array_init(aliases);
		add_assoc_string(aliases, "@yii", YII_G(yii_path), 1);
		zend_update_static_property(yii_baseyii_ce, ZEND_STRL("aliases"), aliases TSRMLS_CC);
	}
	yii_register_autoload(TSRMLS_C);
}
/*}}}*/

/** {{{ proto public static Yii_BaseYii::getVersion()
*/
ZEND_METHOD(Yii_BaseYii, getVersion)
{
	RETURN_STRINGL("2.0.6", 3, 1);
}
/*}}}*/

/** {{{ proto public static Yii_BaseYii::getAlias($alias, $throwException = true)
*/
ZEND_METHOD(Yii_BaseYii, getAlias)
{
	zend_bool throwException = 1;
	zval *alias;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|b", &alias, &throwException) == FAILURE){
		RETURN_FALSE;
	}
	if (Z_TYPE_P(alias) != IS_STRING || strncmp(Z_STRVAL_P(alias), "@", 1)){
		RETURN_ZVAL(alias, 1, 0);
	}

	char *alias_value = yii_get_alias(Z_STRVAL_P(alias), Z_STRLEN_P(alias), 0 TSRMLS_CC);
	if (alias_value){
		RETURN_STRING(alias_value, 0);
	}
	if (throwException){
		yii_throw_exception(YII_BASE_INVALID_PARAM_EXCEPTION TSRMLS_CC, "Invalid path alias:%s", Z_STRVAL_P(alias));
	}
	RETURN_FALSE;
}
/*}}}*/

/** {{{ proto public static Yii_BaseYii::getRootAlias($alias)
*/
ZEND_METHOD(Yii_BaseYii, getRootAlias)
{
	zval *alias;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &alias) == FAILURE){
		RETURN_FALSE;
	}
	if (Z_TYPE_P(alias) != IS_STRING){
		RETURN_FALSE;
	}
	char *alias_value = yii_get_alias(Z_STRVAL_P(alias), Z_STRLEN_P(alias), 1 TSRMLS_CC);
	if (alias_value){
		RETURN_STRING(alias_value, 0);
	}
	RETURN_FALSE;
}
/*}}}*/

/** {{{ proto public static function setAlias($alias, $path)
*/
ZEND_METHOD(Yii_BaseYii, setAlias)
{
	zval *alias, *path;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &alias, &path) == FAILURE){
		RETURN_FALSE;
	}
	if (Z_TYPE_P(alias) != IS_STRING){
		yii_throw_exception(YII_BASE_INVALID_PARAM_EXCEPTION TSRMLS_CC, "BaseYii::setAlias alias parameter should be a string");
		RETURN_FALSE;
	}
	if (Z_TYPE_P(path) == IS_STRING ){
		yii_set_alias(Z_STRVAL_P(alias), Z_STRLEN_P(alias), Z_STRVAL_P(path), Z_STRLEN_P(path) TSRMLS_CC);
		RETURN_TRUE;
	}
	if (Z_TYPE_P(path) == IS_NULL){
		yii_delete_alias(Z_STRVAL_P(alias), Z_STRLEN_P(alias) TSRMLS_CC);
		RETURN_TRUE;
	}
	yii_throw_exception(YII_BASE_INVALID_PARAM_EXCEPTION TSRMLS_CC, "BaseYii::setAlias path parameter should be a string or null");
	RETURN_FALSE;
}
/** }}} */



/** {{{ proto public static Yii_BaseYii::configure($object, $properties)
*/
ZEND_METHOD(Yii_BaseYii, configure)
{
	zval *object, *properties;
	zend_class_entry *ce = NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "oa", &object, &properties) == FAILURE){
		RETURN_FALSE;
	}
	ce = zend_get_class_entry(object TSRMLS_CC);
	for (
		zend_hash_internal_pointer_reset(Z_ARRVAL_P(properties));
		zend_hash_has_more_elements(Z_ARRVAL_P(properties))==SUCCESS;
		zend_hash_move_forward(Z_ARRVAL_P(properties))
		){
		char *key;
		uint keylen;
		zval **zval;
		if (zend_hash_get_current_key_ex(Z_ARRVAL_P(properties), &key, &keylen, NULL, 0,NULL) != HASH_KEY_IS_STRING){
			continue;
		}
		if (zend_hash_get_current_data(Z_ARRVAL_P(properties), (void **)&zval) == SUCCESS){
			zend_update_property(ce, object, key, keylen-1, *zval TSRMLS_CC);
		}
	}
	RETURN_ZVAL(object, 1, 0);
}
/*}}}*/


/** {{{ proto public static function autoload($className)
*/
ZEND_METHOD(Yii_BaseYii, autoload)
{
	char *classname;
	int classname_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &classname, &classname_len) == FAILURE){
		RETURN_FALSE;
	}
	zval *classmap = zend_read_static_property(yii_baseyii_ce, ZEND_STRL("classMap"), 1 TSRMLS_CC);
	char *classpath = NULL;
	zend_bool usealias = 0;
	do{
		if (Z_TYPE_P(classmap) == IS_ARRAY){
			zval **tmp;
			if (zend_hash_find(Z_ARRVAL_P(classmap), classname, classname_len + 1, (void **)&tmp) == SUCCESS){
				classpath = Z_STRVAL_PP(tmp);
				if (*classpath == '@'){
					classpath = yii_get_alias(Z_STRVAL_PP(tmp), Z_STRLEN_PP(tmp), 0 TSRMLS_CC);
					usealias = 1;
				}
				break;
			}
		}
		if (strchr(classname, '\\')){
			char *tmp_classpath;
			int tmp_classpath_len = spprintf(&tmp_classpath, 0, "@%s.php", classname);
			while (*tmp_classpath != '\0'){
				if (*tmp_classpath == '\\'){
					*tmp_classpath = '/';
				}
				tmp_classpath++;
			}
			tmp_classpath -= tmp_classpath_len;//reset pointer 

			classpath = yii_get_alias(tmp_classpath, tmp_classpath_len, 0 TSRMLS_CC);
			efree(tmp_classpath);
			usealias = 1;
		}
	} while (0);

	RETVAL_FALSE;
	if (classpath){
		zval is_file;
		php_stat(classpath, strlen(classpath), FS_IS_FILE, &is_file TSRMLS_CC);
		if (zend_is_true(&is_file)){
			yii_include(classpath, NULL TSRMLS_CC);
			RETVAL_TRUE;
		}
		if (usealias){
			efree(classpath);
		}
	}
	
}

/*}}}*/


/** public static function createObject($type, array $params = [])
*/
ZEND_METHOD(Yii_BaseYii, createObject)
{
	zval *container, *type, *params =NULL,*retval_ptr = NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|a", &type, &params) == FAILURE){
		RETURN_FALSE;
	}
	int flag = 0;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;
	zval **class = NULL;

	if (Z_TYPE_P(type) == IS_STRING){
		flag = 1;
	}
	else if (Z_TYPE_P(type) == IS_ARRAY && zend_hash_find(Z_ARRVAL_P(type), ZEND_STRS("class"),(void **)&class)==SUCCESS){
		flag = 2;
	}
	else if (zend_fcall_info_init(type,0,&fci,&fcc,NULL,NULL TSRMLS_CC)==SUCCESS){
		flag = 3;
	}
	else if (Z_TYPE_P(type) == IS_ARRAY){
		yii_throw_exception(YII_BASE_INVALID_CONFIG_EXCEPTION TSRMLS_CC, "Object configuration must be an array containing a 'class' element.'");
	}
	else {
		yii_throw_exception(YII_BASE_INVALID_CONFIG_EXCEPTION TSRMLS_CC, "Unsupported configuration type:");
	}
	if (!flag){
		RETURN_FALSE;
	}
	if (flag == 1 || flag == 2){
		container = zend_read_static_property(yii_baseyii_ce, ZEND_STRL("container"), 0 TSRMLS_CC);
		if (Z_TYPE_P(container) != IS_OBJECT){
			yii_throw_exception(YII_BASE_INVALID_CONFIG_EXCEPTION TSRMLS_CC, "Yii::$container should be A Object");
			RETURN_FALSE;
		}
		if (ZEND_NUM_ARGS() == 1){
			MAKE_STD_ZVAL(params);
			array_init(params);
		}
		if (flag == 1){
			yii_call_method_with_2_params(&container,"get",strlen("get"), &retval_ptr, type, params);
		}
		else if (flag == 2){
			zval *clazz,*method, *methodName;
			zval **fci_parmas[3];
			zend_fcall_info fci;

			MAKE_STD_ZVAL(clazz);
			MAKE_STD_ZVAL(method);
			MAKE_STD_ZVAL(methodName);
			
			*clazz = **class;
			zval_copy_ctor(clazz);
			zend_hash_del(Z_ARRVAL_P(type), "class", sizeof("class"));
			
			ZVAL_STRINGL(methodName, "get", strlen("get"), 1);

			array_init(method);
			zval_addref_p(container);
			add_next_index_zval(method, container);
			add_next_index_zval(method, methodName);

			fci_parmas[0] = &clazz;
			fci_parmas[1] = &type;
			fci_parmas[2] = &params;
			fci.size = sizeof(fci);
			fci.object_ptr =NULL;
			fci.function_name = method;
			fci.retval_ptr_ptr = &retval_ptr;
			fci.param_count = 3;
			fci.params = fci_parmas;
			fci.no_separation = 1;
			fci.symbol_table = NULL;
			if (zend_call_function(&fci, NULL TSRMLS_CC) == SUCCESS && fci.retval_ptr_ptr && *fci.retval_ptr_ptr){
			}
			zval_dtor(method);
			zval_dtor(clazz);
			efree(method);
			efree(methodName);
			efree(clazz);
		}
		
		if (retval_ptr != NULL){
			COPY_PZVAL_TO_ZVAL(*return_value, retval_ptr);
		}
		if (ZEND_NUM_ARGS() == 1){
			zval_dtor(params);
			efree(params);
		}
	}
	else{
		if (ZEND_NUM_ARGS() == 2){
			fci.param_count = 1;
			zval **fci_parmas[] = {&params};
			fci.params = fci_parmas;
		}
		else{
			fci.param_count = 0;
			fci.params = NULL;
		}
		fci.retval_ptr_ptr = &retval_ptr;
		if (zend_call_function(&fci, &fcc TSRMLS_CC) == SUCCESS && fci.retval_ptr_ptr && *fci.retval_ptr_ptr){
			COPY_PZVAL_TO_ZVAL(*return_value, *fci.retval_ptr_ptr);
		}
	}

	

}
/**}}}*/


zend_function_entry yii_baseyii_methods[] = {
	ZEND_ME(Yii_BaseYii, configure,arginfo_configure,ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Yii_BaseYii, init, arginfo_void, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	ZEND_ME(Yii_BaseYii, getVersion, arginfo_void, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	ZEND_ME(Yii_BaseYii, getAlias, arginfo_getalias, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	ZEND_ME(Yii_BaseYii, getRootAlias, arginfo_getrootalias, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	ZEND_ME(Yii_BaseYii, setAlias, arginfo_setalias, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	ZEND_ME(Yii_BaseYii, autoload, arginfo_autoload,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	ZEND_ME(Yii_BaseYii,createObject,arginfo_createObject,ZEND_ACC_STATIC | ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

EXT_STARTUP_FUNCTION(yii_baseyii)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "Yii\\BaseYii", yii_baseyii_methods);
	yii_baseyii_ce = zend_register_internal_class(&ce TSRMLS_CC);
	zend_declare_property_null(yii_baseyii_ce, ZEND_STRL("classMap"), ZEND_ACC_PUBLIC | ZEND_ACC_STATIC TSRMLS_CC);
	zend_declare_property_null(yii_baseyii_ce, ZEND_STRL("aliases"), ZEND_ACC_PUBLIC | ZEND_ACC_STATIC TSRMLS_CC);
	zend_declare_property_null(yii_baseyii_ce, ZEND_STRL("app"), ZEND_ACC_PUBLIC | ZEND_ACC_STATIC TSRMLS_CC);
	zend_declare_property_null(yii_baseyii_ce, ZEND_STRL("container"), ZEND_ACC_PUBLIC | ZEND_ACC_STATIC TSRMLS_CC);
	zend_declare_property_null(yii_baseyii_ce, ZEND_STRL("_logger"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC TSRMLS_CC);
	return SUCCESS;
}
