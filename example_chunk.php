#!/usr/bin/env php
<?php
$f=fopen('php://stdin','r');
$cd=bsdconv_create($argv[1]);
if($cd===false){
	echo bsdconv_error()."\n";
	exit;
}
bsdconv_insert_phase($cd, 'normal_score', BSDCONV_INTER, 1);
bsdconv_init($cd);
while(!feof($f)){
	echo bsdconv_chunk($cd,fread($f,1024));
}
echo bsdconv_chunk_last($cd,'');
$i=bsdconv_info($cd);
bsdconv_destroy($cd);
echo "\n\n=======Conversino Info=======\n";
print_r($i);
?>
