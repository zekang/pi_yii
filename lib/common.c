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


/** {{{ void yii_throw_exception(long code TSRMLS_DC, char *format,...) 
*/
void yii_throw_exception(long code TSRMLS_DC, const char *format, ...)
{
	char *message = NULL;
	va_list ap;
	zend_class_entry *base_exception = yii_base_exception_ce;
	if ((code & YII_EXCEPTION_BASE) == YII_EXCEPTION_BASE
		&& yii_buildin_exceptions[YII_EXCEPTION_OFFSET(code)]) {
		base_exception = yii_buildin_exceptions[YII_EXCEPTION_OFFSET(code)];
	}
	va_start(ap, format);
	vspprintf(&message, 0, format, ap);
	va_end(ap);
	zend_throw_exception(base_exception, message, code TSRMLS_CC);
	efree(message);
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

/** {{{ zend_bool yii_php_valid_var_name(char *var_name, int var_name_len) 
*/
zend_bool yii_php_valid_var_name(char *var_name, int var_name_len) 
{
	int i, ch;

	if (!var_name || !var_name_len) {
		return FAILURE;
	}

	/* These are allowed as first char: [a-zA-Z_\x7f-\xff] */
	ch = (int)((unsigned char *)var_name)[0];
	if (var_name[0] != '_' &&
		(ch < 65  /* A    */ || /* Z    */ ch > 90) &&
		(ch < 97  /* a    */ || /* z    */ ch > 122) &&
		(ch < 127 /* 0x7f */ || /* 0xff */ ch > 255)
		) {
		return FAILURE;
	}

	/* And these as the rest: [a-zA-Z0-9_\x7f-\xff] */
	if (var_name_len > 1) {
		for (i = 1; i < var_name_len; i++) {
			ch = (int)((unsigned char *)var_name)[i];
			if (var_name[i] != '_' &&
				(ch < 48  /* 0    */ || /* 9    */ ch > 57) &&
				(ch < 65  /* A    */ || /* Z    */ ch > 90) &&
				(ch < 97  /* a    */ || /* z    */ ch > 122) &&
				(ch < 127 /* 0x7f */ || /* 0xff */ ch > 255)
				) {
				return FAILURE;
			}
		}
	}
	return SUCCESS;
}
/* }}} */

/**{{{zend_bool yii_extract(zval *params TSRMLS_DC)
*/
zend_bool yii_extract(zval *params TSRMLS_DC)
{
	zval **tmp;
	char *var_name;
	int var_name_len;

	if (!params || (Z_TYPE_P(params) != IS_ARRAY)){
		return FAILURE;
	}
	if (!EG(active_symbol_table)){
		return FAILURE;
	}
	for (
		zend_hash_internal_pointer_reset(Z_ARRVAL_P(params));
		zend_hash_has_more_elements(Z_ARRVAL_P(params)) == SUCCESS;
	zend_hash_move_forward(Z_ARRVAL_P(params))
		){
		if (zend_hash_get_current_key_ex(Z_ARRVAL_P(params), &var_name, &var_name_len, NULL, 0, NULL) != HASH_KEY_IS_STRING){
			continue;
		}
		if (yii_php_valid_var_name(var_name, var_name_len - 1) == FAILURE){
			continue;
		}
		/* GLOBALS protection */
		if (var_name_len == sizeof("GLOBALS") && !strcmp(var_name, "GLOBALS")) {
			continue;
		}
		if (var_name_len == sizeof("this") && !strcmp(var_name, "this") && EG(scope) && EG(scope)->name_length != 0) {
			continue;
		}
		if (zend_hash_get_current_data(Z_ARRVAL_P(params), (void **)&tmp) == FAILURE){
			continue;
		}

		ZEND_SET_SYMBOL_WITH_LENGTH(
			EG(active_symbol_table), 
			var_name, 
			var_name_len, 
			*tmp, 
			Z_REFCOUNT_P(*tmp) + 1, 
			PZVAL_IS_REF(*tmp)
			);
	}
	return SUCCESS;
}
/** }}} */


/**{{{ zend_bool yii_include(char *path,zval **return_value TSRMLS_DC) 
*/
zend_bool yii_include(char *path, zval **return_value TSRMLS_DC)
{
	zend_op_array *op_array;
	zend_file_handle file_handle;
	if (zend_stream_open(path, &file_handle TSRMLS_CC) == FAILURE){
		return FAILURE;
	}
	op_array = zend_compile_file(&file_handle, ZEND_INCLUDE TSRMLS_CC);
	if (op_array && file_handle.handle.stream.handle){
		int dummy = 1;
		zend_hash_add(&EG(included_files), file_handle.opened_path, strlen(file_handle.opened_path) + 1, (void *)&dummy, sizeof(int), NULL);
	}
	zend_destroy_file_handle(&file_handle TSRMLS_CC);
	if (op_array){
		zval *result = NULL;
		YII_STORE_EG_ENVIRON();
		EG(return_value_ptr_ptr) = &result;
		EG(active_op_array) = op_array;
		if (!EG(active_symbol_table)) {
			zend_rebuild_symbol_table(TSRMLS_C);
		}
		zend_execute(op_array TSRMLS_CC);
		destroy_op_array(op_array TSRMLS_CC);
		efree(op_array);
		if (!EG(exception)) {
			if (EG(return_value_ptr_ptr)) {
				if (return_value){
					*return_value = *EG(return_value_ptr_ptr);
				}else{
					zval_ptr_dtor(EG(return_value_ptr_ptr));
				}
			}
		}
		YII_RESTORE_EG_ENVIRON();
	}
	return SUCCESS;
}
/* }}} */

