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
#include "Zend\zend_interfaces.h"
#include "lib\common.h"
#include "php_my_yii.h"
#include "di\container.h"
#include "yii.h"


zend_class_entry *yii_di_container_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yii_di_container_get, 0, 0, 3)
ZEND_ARG_INFO(0, class)
ZEND_ARG_INFO(0, params)
ZEND_ARG_INFO(0, config)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yii_di_container_set, 0, 0, 3)
ZEND_ARG_INFO(0, class)
ZEND_ARG_INFO(0, definition)
ZEND_ARG_ARRAY_INFO(0, params, 0)
ZEND_END_ARG_INFO()

static zend_bool get(zval *obj, char *class, int class_length, zval *params, zval *config, zval *return_value TSRMLS_DC);
static zend_bool build(zval *obj, char *class, int class_length, zval *params, zval *config, zval *return_value TSRMLS_DC);

static zend_bool get(zval *obj, char *class, int class_length, zval *params, zval *config, zval *return_value TSRMLS_DC)
{
	zval *singletons, *definitions;
	zval **tmp,**definition;
	zend_fcall_info fci;
	zend_fcall_info_cache fcc;

	singletons = zend_read_property(yii_di_container_ce, obj, ZEND_STRL("_singletons"), 1 TSRMLS_CC);
	if (Z_TYPE_P(singletons) == IS_NULL){
		array_init(singletons);
	}
	else{
		if (zend_hash_find(Z_ARRVAL_P(singletons), class, class_length + 1, (void **)&tmp) == SUCCESS){
			RETVAL_ZVAL(*tmp, 1, 0);
			goto END;
		}
	}
	definitions = zend_read_property(yii_di_container_ce, obj, ZEND_STRL("_definitions"), 1 TSRMLS_CC);
	if (Z_TYPE_P(singletons) == IS_NULL){
		array_init(singletons);
	}
	if (zend_hash_find(Z_ARRVAL_P(definitions), class, class_length,(void **)&definition) == FAILURE){
		build(obj, class, class_length, params, config,return_value TSRMLS_CC);
		goto END;
	}

	if (zend_fcall_info_init(*definition, 0, &fci, &fcc, NULL, NULL) == SUCCESS){
		
		fci.param_count = 1;
		//$params = $this->resolveDependencies($this->mergeParams($class, $params));
		if (params){
			fci.param_count++;
		}
		if (config){
			fci.param_count++;
		}
		zval **fci_parmas[] = {&obj, &params ,&config};
		fci.params = fci_parmas;
		fci.retval_ptr_ptr = &tmp;
		if (zend_call_function(&fci, &fcc TSRMLS_CC) == SUCCESS && fci.retval_ptr_ptr && *fci.retval_ptr_ptr){
			RETVAL_ZVAL(*fci.retval_ptr_ptr, 1, 1);
		}
	}
	else if (Z_TYPE_PP(definition) == IS_ARRAY){

	}
	else if (Z_TYPE_PP(definition) == IS_OBJECT){

	}
	else{
		yii_throw_exception(YII_BASE_INVALID_CONFIG_EXCEPTION TSRMLS_CC, "Unexpected object definition type: %d",Z_TYPE_PP(definition));
		return FAILURE;
	}

END:
	return SUCCESS;
}

/** {{{ proto   public function get($class, $params = [], $config = [])
*/
ZEND_METHOD(Yii_Di_Container,get)
{
	zval *params = NULL, *config = NULL;
	char *class;
	int class_length;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|zz", &class, &class_length, &params, &config) == FAILURE){
		return;
	}
	get(this_ptr, class, class_length, params, config, return_value TSRMLS_CC);
}
/** }}} */

/** {{{ proto     public function set($class, $definition = [], array $params = [])
*/
ZEND_METHOD(Yii_Di_Container, set)
{
	RETURN_TRUE;
}
/** }}} */


zend_function_entry yii_di_container_methods[] = {
	ZEND_ME(Yii_Di_Container, get,		 arginfo_yii_di_container_get,ZEND_ACC_PUBLIC)
	ZEND_ME(Yii_Di_Container, set, arginfo_yii_di_container_set,  ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


EXT_STARTUP_FUNCTION(yii_di_container)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "Yii\\Di\\Container", yii_di_container_methods);
	yii_di_container_ce = zend_register_internal_class(&ce TSRMLS_CC);
	zend_declare_property_null(yii_di_container_ce, ZEND_STRL("_singletons"), ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_null(yii_di_container_ce, ZEND_STRL("_definitions"), ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_null(yii_di_container_ce, ZEND_STRL("_params"), ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_null(yii_di_container_ce, ZEND_STRL("_reflections"), ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_null(yii_di_container_ce, ZEND_STRL("_dependencies"), ZEND_ACC_PRIVATE TSRMLS_CC);
	return SUCCESS;
}
