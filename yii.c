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
#include "ext\standard\php_var.h"
#include "php_my_yii.h"
#include "baseyii.h"
#include "yii.h"


zend_class_entry *yii_ce;

zend_function_entry yii_methods[] = {
	ZEND_FE_END
};

EXT_STARTUP_FUNCTION(yii)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "Yii", yii_methods);
	yii_ce = zend_register_internal_class_ex(&ce, yii_baseyii_ce, "Yii\\BaseYii" TSRMLS_CC);
	return SUCCESS;
}
