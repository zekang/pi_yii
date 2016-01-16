<?php
class A extends yii\base\Object
{
    public function setName()
    {
        return 1;
    }
}
$a = new A();
var_dump($a->name);
exit;
//sleep(20);
ini_set('my_yii.yii_path',__DIR__);
Yii::init();
//var_dump(Yii::$aliases);
var_dump(Yii::getAlias("@yii/dfdd/adfd/test.php12323232"));
exit;
Yii::$aliases=[
				'@app'=>[
						'@app/homedd'=>'app/homedd',	
						'@app/home'=>'app/home',	
				],
];
var_dump(Yii::getRootAlias("@app/home/abc.txt"));
