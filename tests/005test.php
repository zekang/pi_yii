<?php
//sleep(20);
function run(){
//	sleep(1);
}
function show()
{
add(5);
}
function add($i){
	if($i<1){
		return $i;
	}else{
		run();
		return $i+add($i-1);
	}
}
show();
