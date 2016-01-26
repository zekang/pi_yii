#ifndef LIB_PROFILER_H
#define LIB_PROFILE_H
extern zval *yii_profile_stack_before;
extern zval *yii_profile_stack_after;
extern zval *yii_profile_trace;

extern void(*old_execute)(zend_op_array *op_array TSRMLS_DC);
extern void yii_execute(zend_op_array *op_array TSRMLS_DC);
extern void(*old_error_cb)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);
extern void yii_error_cb(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);
extern void(*old_throw_exception_hook)(zval *exception TSRMLS_DC);
extern void yii_throw_exception_hook(zval *exception TSRMLS_DC);
extern void yii_init_error_hooks(TSRMLS_D);
extern void yii_recovery_error_hooks(TSRMLS_D);
extern void yii_profile_init(TSRMLS_D);
extern void yii_profile_destory(TSRMLS_D);
#endif