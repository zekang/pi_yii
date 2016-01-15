#include "php.h"
#include "Zend/zend_exceptions.h"
#include "base\exception.h"
#include "common.h"

char * yii_strtolower(char *str)
{
	char *p = str;
	if (str){
		while (*str){
			*str = tolower(*str);
			str++;
		}
	}
	return p;
}

zend_class_entry *yii_get_class_entry(char *class_name, int len TSRMLS_DC)
{
	zend_class_entry **pce;
	if (zend_lookup_class(class_name, len, &pce TSRMLS_CC) == SUCCESS){
		return *pce;
	}
	return NULL;
}

zend_bool yii_method_exists(zval *object, char *method_name, uint method_len TSRMLS_DC)
{
	zend_class_entry *ce=NULL;
	if (!object || method_len < 1){
		return FAILURE;
	}
	if (Z_TYPE_P(object) == IS_OBJECT){
		ce = Z_OBJCE_P(object);
	}
	else if (Z_TYPE_P(object) == IS_STRING){
		ce = yii_get_class_entry(Z_STRVAL_P(object), Z_STRLEN_P(object) TSRMLS_CC);
	}
	if (ce){
		if (zend_hash_exists(&(ce->function_table), method_name, method_len)){
			return SUCCESS;
		}
	}
	return FAILURE;
}

zend_bool yii_property_exists(zval *object, char *propery_name, uint propery_name_len TSRMLS_DC)
{
	zend_class_entry *ce = NULL;
	ulong h;
	zend_property_info *property_info;
	if (!object || propery_name_len < 1){
		return FAILURE;
	}
	if (Z_TYPE_P(object) == IS_OBJECT){
		ce = Z_OBJCE_P(object);
	}
	else if (Z_TYPE_P(object) == IS_STRING){
		ce = yii_get_class_entry(Z_STRVAL_P(object), Z_STRLEN_P(object) TSRMLS_CC);
	}
	if (ce){
		h = zend_get_hash_value(propery_name, propery_name_len);
		if (zend_hash_quick_find(&(ce->properties_info), propery_name, propery_name_len, h, (void **)&property_info) == SUCCESS){
			if (property_info->flags && ZEND_ACC_SHADOW){
				return SUCCESS;
			}
		}
	}
	return FAILURE;
}

void yii_throw_exception(long code, char *message TSRMLS_DC) 
{
	zend_class_entry *base_exception = yii_base_exception_ce;
	if ((code & YII_EXCEPTION_BASE) == YII_EXCEPTION_BASE
		&& yii_buildin_exceptions[YII_EXCEPTION_OFFSET(code)]) {
		base_exception = yii_buildin_exceptions[YII_EXCEPTION_OFFSET(code)];
	}
	zend_throw_exception(base_exception, message, code TSRMLS_CC);
}