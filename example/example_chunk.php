#!/usr/bin/env php
<?php
$f=fopen('php://stdin','r');
try {
	$c=new Bsdconv($argv[1]);
}
catch(Exception $e){
	echo bsdconv_error()."\n";
	exit;
}
$c->init();
while(!feof($f)){
	echo $c->conv_chunk(fread($f,1024));
}
echo $c->conv_chunk_last('');
$i=$c->counter();
unset($c);
echo "\n\n=======Conversino Info=======\n";
print_r($i);
?>
