#!/usr/bin/env php
<?php
$f=fopen('php://stdin','r');
$c=new Bsdconv($argv[1]);
if(!$c){
	echo bsdconv_error()."\n";
	exit;
}
$c->insert_phase('normal_score', BSDCONV_INTER, 1);
$c->init();
while(!feof($f)){
	echo $c->conv_chunk(fread($f,1024));
}
echo $c->conv_chunk_last('');
$i=$c->info();
unset($c);
echo "\n\n=======Conversino Info=======\n";
print_r($i);
?>
