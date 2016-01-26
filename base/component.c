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
#include "base/object.h"
#include "yii.h"
#include "base/component.h"

zend_class_entry *yii_base_component_ce;

ZEND_BEGIN_ARG_INFO_EX(arginfo_void, 0, 0, 0)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_yii_base_component_magic_get, 0, 0, 1)
ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yii_base_component_magic_set, 0, 0, 2)
ZEND_ARG_INFO(0, name)
ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yii_base_component_magic_isset, 0, 0, 1)
ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yii_base_component_magic_unset, 0, 0, 1)
ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yii_base_component_magic_call, 0, 0, 2)
ZEND_ARG_INFO(0, name)
ZEND_ARG_INFO(0, params)
ZEND_END_ARG_INFO()


ZEND_BEGIN_ARG_INFO_EX(arginfo_yii_base_component_has_property, 0, 0, 2)
ZEND_ARG_INFO(0, name)
ZEND_ARG_INFO(0, check_vars)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yii_base_component_can_set_property, 0, 0, 2)
ZEND_ARG_INFO(0, name)
ZEND_ARG_INFO(0, check_vars)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yii_base_component_can_get_property, 0, 0, 2)
ZEND_ARG_INFO(0, name)
ZEND_ARG_INFO(0, check_vars)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_yii_base_component_has_method, 0, 0, 1)
ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()



/** {{{ proto public Yii_Base_Component::__get($name)
*/
ZEND_METHOD(Yii_Base_Component, __get)
{
	char *name;
	int name_len;
	char method[MAX_PATH] = { 0 };
	int method_len;
	zval *ret = NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE){
		RETURN_FALSE;
	}
	method_len = php_sprintf(method, "get%s", name);
	yii_strtolower(method);
	if (yii_method_exists(getThis(), method, method_len + 1 TSRMLS_CC) == SUCCESS){
		yii_call_method_with_0_params(&(getThis()), method, method_len, &ret);
		RETURN_ZVAL(ret, 0, 0);
	}
	else{
		method[0] = 's';
		if (yii_method_exists(getThis(), method, method_len + 1 TSRMLS_CC) == SUCCESS){
			yii_throw_exception(YII_BASE_INVALID_CALL_EXCEPTION TSRMLS_CC, "Getting write-only property: %s::%s", EG(called_scope)->name, name);
		}
		else{
			yii_throw_exception(YII_BASE_UNKNOWN_PROPERTY_EXCEPTION TSRMLS_CC, "Getting unknown property: %s::%s", EG(called_scope)->name, name);
		}
	}
}
/** }}} */



/** {{{ proto public Yii_Base_Component::__set($name,$value)
*/
ZEND_METHOD(Yii_Base_Component, __set)
{
	char *name;
	int name_len;
	zval *value;
	char method[MAX_PATH] = { 0 };
	int method_len;
	zval *ret = NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &name_len, &value) == FAILURE){
		RETURN_FALSE;
	}
	method_len = php_sprintf(method, "set%s", name);
	yii_strtolower(method);
	if (yii_method_exists(getThis(), method, method_len + 1 TSRMLS_CC) == SUCCESS){
		yii_call_method_with_1_params(&(getThis()), method, method_len, &ret, value);
		RETURN_ZVAL(ret, 0, 0);
	}
	else{
		method[0] = 'g';
		if (yii_method_exists(getThis(), method, method_len + 1 TSRMLS_CC) == SUCCESS){
			yii_throw_exception(YII_BASE_INVALID_CALL_EXCEPTION TSRMLS_CC, "Setting read-only property: %s::%s", EG(called_scope)->name, name);
		}
		else{
			yii_throw_exception(YII_BASE_UNKNOWN_PROPERTY_EXCEPTION TSRMLS_CC, "Setting unknown property: %s::%s", EG(called_scope)->name, name);
		}
	}
}
/** }}} */

/** {{{ proto public Yii_Base_Component::__isset($name)
*/
ZEND_METHOD(Yii_Base_Component, __isset)
{
	char *name;
	int name_len;
	char method[MAX_PATH] = { 0 };
	int method_len;
	zval *ret = NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE){
		RETURN_FALSE;
	}
	method_len = php_sprintf(method, "get%s", name);
	yii_strtolower(method);
	if (yii_method_exists(getThis(), method, method_len + 1 TSRMLS_CC) == SUCCESS){
		yii_call_method_with_0_params(&(getThis()), method, method_len, &ret);
		if (ret && Z_TYPE_P(ret) != IS_NULL){
			RETVAL_TRUE;
		}
		else{
			RETVAL_FALSE;
		}
		if (ret){
			zval_dtor(ret);
		}
		return;
	}
	RETURN_FALSE;
}
/*** }}}*/



/** {{{ proto public Yii_Base_Component::__unset($name)
*/
ZEND_METHOD(Yii_Base_Component, __unset)
{
	char *name;
	int name_len;
	char method[MAX_PATH] = { 0 };
	int method_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE){
		RETURN_FALSE;
	}
	method_len = php_sprintf(method, "set%s", name);
	yii_strtolower(method);
	if (yii_method_exists(getThis(), method, method_len + 1 TSRMLS_CC) == SUCCESS){
		zval *value;
		ALLOC_INIT_ZVAL(value);
		yii_call_method_with_1_params(&(getThis()), method, method_len, NULL, value);
	}
	else{
		method[0] = 'g';
		if (yii_method_exists(getThis(), method, method_len + 1 TSRMLS_CC) == SUCCESS){
			yii_throw_exception(YII_BASE_INVALID_CALL_EXCEPTION TSRMLS_CC, "Unsetting read-only property: %s::%s", EG(called_scope)->name, name);
		}
	}
}
/** }}} */


/** {{{ proto public Yii_Base_Component::__call($name, $params)
*/
ZEND_METHOD(Yii_Base_Component, __call)
{
	char *name;
	int name_len;
	zval *params;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &name, &name_len, &params) == FAILURE){
		RETURN_FALSE;
	}

	yii_throw_exception(YII_BASE_INVALID_CALL_EXCEPTION TSRMLS_CC, "Calling unknown method: %s::%s()", EG(called_scope)->name, name);
}
/** }}} */

/*** {{{  proto public Yii_Base_Component::hasProperty($name, $checkVars = true)
*/
ZEND_METHOD(Yii_Base_Component, hasProperty)
{
	zend_bool bResult = 0;
	zend_bool checkVars = 1;
	zval *name, *ret, *pCheckVars;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|b", &name, &checkVars) == FAILURE){
		RETURN_FALSE;
	}
	MAKE_STD_ZVAL(pCheckVars);
	ZVAL_BOOL(pCheckVars, checkVars);
	yii_call_method_with_2_params(&(getThis()), "cangetproperty", sizeof("cangetproperty") - 1, &ret, name, pCheckVars);
	if (zend_is_true(ret)){
		bResult = 1;
	}
	else{
		zval_dtor(ret);
		ZVAL_FALSE(pCheckVars);
		yii_call_method_with_2_params(&(getThis()), "cansetproperty", sizeof("cansetproperty") - 1, &ret, name, pCheckVars);
		if (zend_is_true(ret)){
			bResult = 1;
		}
	}
	zval_dtor(ret);
	zval_dtor(pCheckVars);
	efree(pCheckVars);
	RETURN_BOOL(bResult);
}
/** }}} */

/*** {{{  proto public Yii_Base_Component::canGetProperty($name, $checkVars = true)
*/
ZEND_METHOD(Yii_Base_Component, canGetProperty)
{
	zend_bool checkVars = 1;
	char *name;
	int name_len;
	char method[MAX_PATH] = { 0 };
	int method_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b", &name, &name_len, &checkVars) == FAILURE){
		RETURN_FALSE;
	}
	method_len = php_sprintf(method, "get%s", name);
	yii_strtolower(method);
	if (yii_method_exists(getThis(), method, method_len + 1 TSRMLS_CC) == SUCCESS){
		RETURN_TRUE;
	}
	if (checkVars && yii_property_exists(getThis(), name, name_len + 1 TSRMLS_CC) == SUCCESS){
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/** }}} */

/*** {{{  proto public Yii_Base_Component::canSetProperty($name, $checkVars = true)
*/
ZEND_METHOD(Yii_Base_Component, canSetProperty)
{
	zend_bool checkVars = 1;
	char *name;
	int name_len;
	char method[MAX_PATH] = { 0 };
	int method_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b", &name, &name_len, &checkVars) == FAILURE){
		RETURN_FALSE;
	}
	method_len = php_sprintf(method, "set%s", name);
	yii_strtolower(method);
	if (yii_method_exists(getThis(), method, method_len + 1 TSRMLS_CC) == SUCCESS){
		RETURN_TRUE;
	}
	if (checkVars && yii_property_exists(getThis(), name, name_len + 1 TSRMLS_CC) == SUCCESS){
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/** }}} */


/*** {{{  proto public Yii_Base_Component::hasMethod($name)
*/
ZEND_METHOD(Yii_Base_Component, hasMethod)
{
	char *name;
	int name_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &name, &name_len) == FAILURE){
		RETURN_FALSE;
	}
	yii_strtolower(name);
	if (yii_method_exists(getThis(), name, name_len + 1 TSRMLS_CC) == SUCCESS){
		RETURN_TRUE;
	}
	RETURN_FALSE;
}
/** }}} */

zend_function_entry yii_base_component_methods[] = {
	ZEND_ME(Yii_Base_Component, __get, arginfo_yii_base_component_magic_get, ZEND_ACC_PUBLIC)
	ZEND_ME(Yii_Base_Component, __set, arginfo_yii_base_component_magic_set, ZEND_ACC_PUBLIC)
	ZEND_ME(Yii_Base_Component, __isset, arginfo_yii_base_component_magic_isset, ZEND_ACC_PUBLIC)
	ZEND_ME(Yii_Base_Component, __unset, arginfo_yii_base_component_magic_unset, ZEND_ACC_PUBLIC)
	ZEND_ME(Yii_Base_Component, __call, arginfo_yii_base_component_magic_call, ZEND_ACC_PUBLIC)
	ZEND_ME(Yii_Base_Component, hasProperty, arginfo_yii_base_component_has_property, ZEND_ACC_PUBLIC)
	ZEND_ME(Yii_Base_Component, canGetProperty, arginfo_yii_base_component_can_get_property, ZEND_ACC_PUBLIC)
	ZEND_ME(Yii_Base_Component, canSetProperty, arginfo_yii_base_component_can_set_property, ZEND_ACC_PUBLIC)
	ZEND_ME(Yii_Base_Component, hasMethod, arginfo_yii_base_component_has_method, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};


EXT_STARTUP_FUNCTION(yii_base_component)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "Yii\\Base\\Component", yii_base_component_methods);
	yii_base_component_ce = zend_register_internal_class_ex(&ce,yii_base_object_ce,"Yii\\Base\\Object" TSRMLS_CC);
	zend_declare_property_null(yii_base_component_ce, ZEND_STRL("_events"), ZEND_ACC_PRIVATE TSRMLS_CC);
	zend_declare_property_null(yii_base_component_ce, ZEND_STRL("_behaviors"), ZEND_ACC_PRIVATE TSRMLS_CC);
	return SUCCESS;
}