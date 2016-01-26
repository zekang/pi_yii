#include "php.h"
#include "profiler.h"
#include "Zend/zend_exceptions.h"
#include "ext/standard/php_var.h"
zval *yii_profile_stack_before = NULL;
zval *yii_profile_stack_after  = NULL;
zval *yii_profile_trace = NULL;

/**{{{ ulong yii_get_millisecond()
*/
ulong yii_get_millisecond()
{
	ulong msec;
#ifdef	WINDOWS
	msec = GetTickCount();
#else
	struct timeval tval;
	gettimeofday(&tval, NULL);
	msec = 1000000 * tval.tv_sec + tval.tv_usec;
#endif
	return msec;
}
/* }}} */
void yii_profile_init(TSRMLS_D)
{
	if (yii_profile_stack_before == NULL){
		MAKE_STD_ZVAL(yii_profile_stack_before);
		array_init(yii_profile_stack_before);
	}
	if (yii_profile_stack_after == NULL){
		MAKE_STD_ZVAL(yii_profile_stack_after);
		array_init(yii_profile_stack_after);
	}
	if (yii_profile_trace == NULL){
		MAKE_STD_ZVAL(yii_profile_trace);
		array_init(yii_profile_trace);
	}
}

void yii_profile_destory(TSRMLS_D)
{
	if (yii_profile_stack_before != NULL){
		php_var_dump(&yii_profile_stack_before, 1 TSRMLS_CC);
		zval_dtor(yii_profile_stack_before);
	}
	if (yii_profile_stack_after != NULL){
		php_var_dump(&yii_profile_stack_after, 1 TSRMLS_CC);
		zval_dtor(yii_profile_stack_after);
	}
	if (yii_profile_trace != NULL){
		php_var_dump(&yii_profile_trace, 1 TSRMLS_CC);
		zval_dtor(yii_profile_trace);
	}
}


/* {{{ static zval * yii_php_pop(zval *stack, zval *val TSRMLS_DC)
*/
static zval * yii_php_pop(zval *stack, zval *val TSRMLS_DC)
{
	zval **tmp;		/* Value to be popped */
	char *key = NULL;
	uint key_len = 0;
	ulong index;
	if (zend_hash_num_elements(Z_ARRVAL_P(stack)) == 0) {
		return val;
	}
	zend_hash_internal_pointer_end(Z_ARRVAL_P(stack));
	zend_hash_get_current_data(Z_ARRVAL_P(stack), (void **)&tmp);
	MAKE_STD_ZVAL(val);
	*val = **tmp;
	zval_copy_ctor(val);
	Z_SET_REFCOUNT_P(val, 1);
	/* Delete last value */
	zend_hash_get_current_key_ex(Z_ARRVAL_P(stack), &key, &key_len, &index, 0, NULL);
	zend_hash_del_key_or_index(Z_ARRVAL_P(stack), key, key_len, index, (key) ? HASH_DEL_KEY : HASH_DEL_INDEX);
	if (!key_len && Z_ARRVAL_P(stack)->nNextFreeElement > 0 && index >= Z_ARRVAL_P(stack)->nNextFreeElement - 1) {
		Z_ARRVAL_P(stack)->nNextFreeElement = Z_ARRVAL_P(stack)->nNextFreeElement - 1;
	}
	zend_hash_internal_pointer_reset(Z_ARRVAL_P(stack));
	return val;
}
/* }}} */

void check_stack()
{

}
void yii_profile_stack_before_push(char *func)
{
	zval **tmp;
	if (zend_hash_find(Z_ARRVAL_P(yii_profile_stack_before), func, strlen(func) + 1, (void **)&tmp) == FAILURE){
		add_assoc_long(yii_profile_stack_before, func, yii_get_millisecond());
	}
}
void yii_profile_stack_after_push(char *func)
{
	zval **tmp;
	if (zend_hash_find(Z_ARRVAL_P(yii_profile_stack_after), func, strlen(func) + 1, (void **)&tmp) == FAILURE){
		add_assoc_long(yii_profile_stack_after, func, yii_get_millisecond());
	}
}



zend_bool yii_profile_stack_pop(char *func)
{
	zval **tmp;
	if (zend_hash_find(Z_ARRVAL_P(yii_profile_stack_before), func, strlen(func) + 1, (void **)&tmp) == SUCCESS){
		if (Z_LVAL_PP(tmp) == 1){
			zend_hash_del(Z_ARRVAL_P(yii_profile_stack_before), func, strlen(func) + 1);
			return SUCCESS;
		}
		ZVAL_LONG(*tmp, Z_LVAL_PP(tmp) - 1);
	}
	return FAILURE;
}



/**
* Takes an input of the form /a/b/c/d/foo.php and returns
* a pointer to one-level directory and basefile name
* (d/foo.php) in the same string.
*/
static const char *yii_get_base_filename(const char *filename) {
	const char *ptr;
	int   found = 0;

	if (!filename)
		return "";

	/* reverse search for "/" and return a ptr to the next char */
	for (ptr = filename + strlen(filename) - 1; ptr >= filename; ptr--) {
		if (*ptr == '/') {
			found++;
		}
		if (found == 2) {
			return ptr + 1;
		}
	}

	/* no "/" char found, so return the whole string */
	return filename;
}
char *yii_get_function_name(TSRMLS_D) {
	zend_execute_data *data;
	const char        *func = NULL;
	const char        *clazz = NULL;
	char              *ret = NULL;
	zend_function      *curr_func;
	data = EG(current_execute_data);
	if (data) {
		curr_func = data->function_state.function;
		func = curr_func->common.function_name;
		if (func) {
			if (curr_func->common.scope) {
				clazz = curr_func->common.scope->name;
			}
			else if (data->object) {
				clazz = Z_OBJCE(*data->object)->name;
			}
			if (clazz) {
				spprintf(&ret, 0, "%s::%s", clazz, func);
			}
			else {
				ret = estrdup(func);
			}
		}
		else {
			long     curr_op;
			int      add_filename = 0;
			curr_op = data->opline->extended_value;
			switch (curr_op) {
			case ZEND_EVAL:
				func = "eval";
				break;
			case ZEND_INCLUDE:
				func = "include";
				add_filename = 1;
				break;
			case ZEND_REQUIRE:
				func = "require";
				add_filename = 1;
				break;
			case ZEND_INCLUDE_ONCE:
				func = "include_once";
				add_filename = 1;
				break;
			case ZEND_REQUIRE_ONCE:
				func = "require_once";
				add_filename = 1;
				break;
			default:
				func = "unknown_op";
				break;
			}

			/* For some operations, we'll add the filename as part of the function
			* name to make the reports more useful. So rather than just "include"
			* you'll see something like "run_init::foo.php" in your reports.
			*/
			if (add_filename){
				spprintf(&ret, 0, "%s::%s", func, yii_get_base_filename((curr_func->op_array).filename));
			}
			else {
				ret = estrdup(func);
			}
		}
	}
	return ret;
}

void(*old_execute)(zend_op_array *op_array TSRMLS_DC);

void yii_execute(zend_op_array *op_array TSRMLS_DC)
{
	char *fun_name;
	ulong start, end;
	fun_name = yii_get_function_name(TSRMLS_C);
	if (fun_name){
		start = yii_get_millisecond();
		yii_profile_stack_before_push(fun_name);
	}
	old_execute(op_array TSRMLS_CC);
	end = yii_get_millisecond();
	if (fun_name){
		end = yii_get_millisecond();
		php_printf("function %s elapse %u millisecond\n", fun_name, end - start);
		efree(fun_name);
	}
}


void(*old_error_cb)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);

/**{{{ yii_error_cb(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args)
*/
void yii_error_cb(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args)
{
	TSRMLS_FETCH();
	char *msg;
	va_list ap;
	va_copy(ap, args);
	vspprintf(&msg, 0, format, ap);
	va_end(ap);
	php_printf("error found:%s\n", msg);
	efree(msg);
	old_error_cb(type, error_filename, error_lineno, format, args);
}
/* }}} */

void(*old_throw_exception_hook)(zval *exception TSRMLS_DC);


/**{{{ void yii_throw_exception_hook(zval *exception TSRMLS_DC)
*/
void yii_throw_exception_hook(zval *exception TSRMLS_DC)
{
	if (!exception){
		return;
	}
	php_printf("exception occur\n");
	if (old_throw_exception_hook){
		php_printf("next \n");
		old_throw_exception_hook(exception TSRMLS_CC);
	}
}
/* }}} */


/**{{{ void yii_init_error_hooks(TSRMLS_D)
*/
void yii_init_error_hooks(TSRMLS_D)
{
	old_execute = zend_execute;
	zend_execute = yii_execute;
	old_error_cb = zend_error_cb;
	zend_error_cb = yii_error_cb;
	if (zend_throw_exception_hook) {
		old_throw_exception_hook = zend_throw_exception_hook;
	}
	zend_throw_exception_hook = yii_throw_exception_hook;
}
/* }}} */

/**{{{ void yii_recovery_error_hooks(TSRMLS_D)
*/
void yii_recovery_error_hooks(TSRMLS_D)
{
	if (old_execute){
		zend_execute = old_execute;
	}
	if (old_error_cb) {
		zend_error_cb = old_error_cb;
	}
	if (old_throw_exception_hook) {
		zend_throw_exception_hook = old_throw_exception_hook;
	}

}
/* }}} */
