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
#include "php_my_yii.h"
#include "yii.h"

zend_class_entry *yii_base_exception_ce;


/** {{{ zend_class_entry * yii_get_exception_base(int root TSRMLS_DC)
*/
zend_class_entry * yii_get_exception_base(int root TSRMLS_DC) {
#if can_handle_soft_dependency_on_SPL && defined(HAVE_SPL) && ((PHP_MAJOR_VERSION > 5) || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION >= 1))
	if (!root) {
		if (!spl_ce_RuntimeException) {
			zend_class_entry **pce;

			if (zend_hash_find(CG(class_table), "runtimeexception", sizeof("RuntimeException"), (void **)&pce) == SUCCESS) {
				spl_ce_RuntimeException = *pce;
				return *pce;
			}
		}
		else {
			return spl_ce_RuntimeException;
		}
	}
#endif

#if (PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 2)
	return zend_exception_get_default();
#else
	return zend_exception_get_default(TSRMLS_C);
#endif
}
/* }}} */

/** {{{ proto public static Yii_Base_Exception::getName()
*/
ZEND_METHOD(Yii_Base_Exception, getName)
{
	RETURN_STRING("Exception", 1);
}
/* }}} */

zend_function_entry yii_base_exception_methods[] = {
	ZEND_ME(Yii_Base_Exception, getName, NULL, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

EXT_STARTUP_FUNCTION(yii_base_exception)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "Yii\\Base\\Exception", yii_base_exception_methods);
	yii_base_exception_ce = zend_register_internal_class_ex(&ce, yii_get_exception_base(0 TSRMLS_CC), NULL TSRMLS_CC);
	return SUCCESS;
}
