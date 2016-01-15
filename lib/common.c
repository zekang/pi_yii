#include "php.h"
#include "Zend/zend_exceptions.h"
#include "base\exception.h"
#include "common.h"

/** {{{ char * yii_strtolower(char *str)
*/
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
/**}}}*/


/** {{{ zend_class_entry *yii_get_class_entry(char *class_name, int len TSRMLS_DC)
*/
zend_class_entry *yii_get_class_entry(char *class_name, int len TSRMLS_DC)
{
	zend_class_entry **pce;
	if (zend_lookup_class(class_name, len, &pce TSRMLS_CC) == SUCCESS){
		return *pce;
	}
	return NULL;
}
/**}}}*/

/** {{{ zend_bool yii_method_exists(zval *object, char *method_name, uint method_len TSRMLS_DC)
*/
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
/*}}}*/

/** {{{ zend_bool yii_property_exists(zval *object, char *propery_name, uint propery_name_len TSRMLS_DC)
*/
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

/* }}} */

/** {{{ void  yii_throw_exception(long code, char *message TSRMLS_DC) 
*/
void yii_throw_exception(long code, char *message TSRMLS_DC) 
{
	zend_class_entry *base_exception = yii_base_exception_ce;
	if ((code & YII_EXCEPTION_BASE) == YII_EXCEPTION_BASE
		&& yii_buildin_exceptions[YII_EXCEPTION_OFFSET(code)]) {
		base_exception = yii_buildin_exceptions[YII_EXCEPTION_OFFSET(code)];
	}
	zend_throw_exception(base_exception, message, code TSRMLS_CC);
}
/* }}} */



/** {{{ zval * yii_request_query(uint type, char * name, uint len TSRMLS_DC)
*/
zval * yii_request_query(uint type, char * name, uint len TSRMLS_DC)
{
	zval 		**carrier = NULL, **ret;

#if (PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 4)
	zend_bool 	jit_initialization = (PG(auto_globals_jit) && !PG(register_globals) && !PG(register_long_arrays));
#else
	zend_bool 	jit_initialization = PG(auto_globals_jit);
#endif

	switch (type) {
	case TRACK_VARS_POST:
	case TRACK_VARS_GET:
	case TRACK_VARS_FILES:
	case TRACK_VARS_COOKIE:
		carrier = &PG(http_globals)[type];
		break;
	case TRACK_VARS_ENV:
		if (jit_initialization) {
			zend_is_auto_global(ZEND_STRL("_ENV") TSRMLS_CC);
		}
		carrier = &PG(http_globals)[type];
		break;
	case TRACK_VARS_SERVER:
		if (jit_initialization) {
			zend_is_auto_global(ZEND_STRL("_SERVER") TSRMLS_CC);
		}
		carrier = &PG(http_globals)[type];
		break;
	case TRACK_VARS_REQUEST:
		if (jit_initialization) {
			zend_is_auto_global(ZEND_STRL("_REQUEST") TSRMLS_CC);
		}
		(void)zend_hash_find(&EG(symbol_table), ZEND_STRS("_REQUEST"), (void **)&carrier);
		break;
	default:
		break;
	}

	if (!carrier || !(*carrier)) {
		zval *empty;
		MAKE_STD_ZVAL(empty);
		ZVAL_NULL(empty);
		return empty;
	}

	if (!len) {
		Z_ADDREF_P(*carrier);
		return *carrier;
	}

	if (zend_hash_find(Z_ARRVAL_PP(carrier), name, len + 1, (void **)&ret) == FAILURE) {
		zval *empty;
		MAKE_STD_ZVAL(empty);
		ZVAL_NULL(empty);
		return empty;
	}

	Z_ADDREF_P(*ret);
	return *ret;
}
/* }}} */