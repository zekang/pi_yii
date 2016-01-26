<?php
namespace app\lib;
use Yii;
class User extends Yii\Base\Object
{
	public function __construct()
	{
		parent::__construct();
		echo "constuct:".__FILE__;
		echo "\n";
	}
}
