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
	zval *aliases = zend_read_static_property(yii_baseyii_ce, ZEND_STRL("aliases"), 1 TSRMLS_CC);
	if (Z_TYPE_P(aliases) != IS_ARRAY){
		yii_throw_exception(YII_EXCEPTION_BASE, "Yii\\BaseYii::$aliases should be Array" TSRMLS_CC);
		RETURN_FALSE;
	}
	zend_bool find_alias = 0;
	char *root = estrndup(Z_STRVAL_P(alias), Z_STRLEN_P(alias));
	char *pos = strchr(root, '/');
	if (pos){
		*pos = '\0';
	}
	zval **alias_values;
	if (zend_hash_find(Z_ARRVAL_P(aliases), root, strlen(root) + 1, (void **)&alias_values) == SUCCESS){
		php_var_dump(alias_values, 1 TSRMLS_CC);
		find_alias = 1;
	}
	efree(root);
	if (find_alias){
		RETURN_TRUE;
	}
	if (throwException){
		yii_throw_exception(YII_EXCEPTION_BASE, "Invalid path alias" TSRMLS_CC);
	}
	RETURN_FALSE;
}
/*}}}*/

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




zend_function_entry yii_baseyii_methods[] = {
	ZEND_ME(Yii_BaseYii, configure,arginfo_configure,ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(Yii_BaseYii, init, arginfo_void, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	ZEND_ME(Yii_BaseYii, getVersion, arginfo_void, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	ZEND_ME(Yii_BaseYii, getAlias, arginfo_getalias, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
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
	return SUCCESS;
}
