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
#include "Zend/zend_exceptions.h"
#include "ext/spl/spl_exceptions.h"
#include "php_my_yii.h"
#include "yii.h"
#include "invalid_param_exception.h"

zend_class_entry *yii_base_invalid_param_exception_ce;

/** {{{ proto public static Yii_Base_InvalidCallException::getName()
*/
ZEND_METHOD(Yii_Base_InvalidParamException, getName)
{
	RETURN_STRING("Invalid Parameter", 1);
}
/*}}}*/

zend_function_entry yii_base_invalid_param_exception_methods[] = {
	ZEND_ME(Yii_Base_InvalidParamException, getName, NULL, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

EXT_STARTUP_FUNCTION(yii_base_invalid_param_exception)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "Yii\\Base\\InvalidParamException", yii_base_invalid_param_exception_methods);
	yii_base_invalid_param_exception_ce = zend_register_internal_class_ex(&ce, spl_ce_BadMethodCallException, NULL TSRMLS_CC);
	return SUCCESS;
}
