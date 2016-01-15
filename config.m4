dnl $Id$
dnl config.m4 for extension my_yii

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(my_yii, for my_yii support,
dnl Make sure that the comment is aligned:
dnl [  --with-my_yii             Include my_yii support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(my_yii, whether to enable my_yii support,
dnl Make sure that the comment is aligned:
dnl [  --enable-my_yii           Enable my_yii support])

if test "$PHP_MY_YII" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-my_yii -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/my_yii.h"  # you most likely want to change this
  dnl if test -r $PHP_MY_YII/$SEARCH_FOR; then # path given as parameter
  dnl   MY_YII_DIR=$PHP_MY_YII
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for my_yii files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       MY_YII_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$MY_YII_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the my_yii distribution])
  dnl fi

  dnl # --with-my_yii -> add include path
  dnl PHP_ADD_INCLUDE($MY_YII_DIR/include)

  dnl # --with-my_yii -> check for lib and symbol presence
  dnl LIBNAME=my_yii # you may want to change this
  dnl LIBSYMBOL=my_yii # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $MY_YII_DIR/lib, MY_YII_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_MY_YIILIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong my_yii lib version or lib not found])
  dnl ],[
  dnl   -L$MY_YII_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(MY_YII_SHARED_LIBADD)

  PHP_NEW_EXTENSION(my_yii, my_yii.c, $ext_shared)
fi
