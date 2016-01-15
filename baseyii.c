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

zend_class_entry *yii_baseyii_ce;
ZEND_BEGIN_ARG_INFO_EX(arginfo_configure, 0, 1, 2)
ZEND_ARG_INFO(1,object)
ZEND_ARG_ARRAY_INFO(0,properties,0)
ZEND_END_ARG_INFO()

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
	ZEND_FE_END
};

EXT_STARTUP_FUNCTION(yii_baseyii)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "Yii\\BaseYii", yii_baseyii_methods);
	yii_baseyii_ce = zend_register_internal_class(&ce TSRMLS_CC);
	return SUCCESS;
}
