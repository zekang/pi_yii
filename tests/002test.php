<?php
//sleep(20);
ini_set('my_yii.yii_path',__DIR__);
Yii::init();
Yii::setAlias('@app/home/whq','1dd');
print_r(Yii::$aliases);
Yii::setAlias('@app/home','2dd');
print_r(Yii::$aliases);
Yii::setAlias('@app','3dd');
print_r(Yii::$aliases);
var_dump(Yii::getAlias('@app'));
Yii::setAlias('@app',null);
print_r(Yii::$aliases);
Yii::setAlias('@yii',null);
print_r(Yii::$aliases);
Yii::setAlias('@app',null);
print_r(Yii::$aliases);
