<?php
//sleep(20);
//error_reporting(0);
//$a = "1";
echo $a;
function handler()
{
	print_r(func_get_args());
}
set_exception_handler('handler');
try{
	throw new Exception("");
}catch(Exception $e){
}
function show()
{
	echo __FILE__;
	echo PHP_EOL;
}
show();
echo "END";
	echo PHP_EOL;
show();
