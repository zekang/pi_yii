<?php
//sleep(20);
Yii::init();
Yii::setAlias('@app',__DIR__);
Yii::$classMap['app\lib\User'] = '@app/lib/user.php';
print_r(yii::$aliases);
echo app\lib\User::className();
