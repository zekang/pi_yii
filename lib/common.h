#ifndef LIB_COMMON_H
#define LIB_COMMON_H
#include "php.h"
#include "Zend\zend_interfaces.h"

#define yii_call_method_with_0_params(obj, function_name,function_name_len,retval) \
	zend_call_method(obj, Z_OBJCE_PP(obj), NULL, function_name, function_name_len, retval, 0, NULL, NULL TSRMLS_CC)

#define yii_call_method_with_1_params(obj, function_name,function_name_len,retval,arg1) \
	zend_call_method(obj, Z_OBJCE_PP(obj), NULL, function_name, function_name_len, retval, 1, arg1, NULL TSRMLS_CC)

#define yii_call_method_with_2_params(obj, function_name,function_name_len,retval,arg1, arg2) \
	zend_call_method(obj, Z_OBJCE_PP(obj), NULL, function_name, function_name_len, retval, 2, arg1, arg2 TSRMLS_CC)

extern zend_class_entry *yii_buildin_exceptions[];
#define YII_MAX_BUILDIN_EXCEPTION			6
#define YII_EXCEPTION_MASK					127
#define YII_EXCEPTION_BASE					512
#define YII_EXCEPTION_OFFSET(x) (x & YII_EXCEPTION_MASK)

#define YII_BASE_EXCEPTION					512
#define YII_BASE_USER_EXCEPTION			    513
#define YII_BASE_INVALID_CALL_EXCEPTION		514
#define YII_BASE_INVALID_CONFIG_EXCEPTION   515
#define YII_BASE_UNKNOWN_METHOD_EXCEPTION   516
#define YII_BASE_UNKNOWN_PROPERTY_EXCEPTION 517



extern char *  yii_strtolower(char *str);
extern zend_bool yii_method_exists(zval *object, char *method_name, uint method_len TSRMLS_DC);
extern zend_bool yii_property_exists(zval *object, char *propery_name, uint propery_name_len TSRMLS_DC);
extern zend_class_entry *yii_get_class_entry(char *class_name, int len TSRMLS_DC);
extern zval * yii_request_query(uint type, char * name, uint len TSRMLS_DC);
extern void yii_throw_exception(long code TSRMLS_DC, const char *format, ...);
#endif