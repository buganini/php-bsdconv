#!/usr/bin/env php
<?php
$s=file_get_contents('php://stdin');
$c=new Bsdconv($argv[1]);
if(!$c){
	echo bsdconv_error()."\n";
	exit;
}
echo $c->conv($s);
$i=$c->info();
unset($c);
echo "\n\n=======Conversino Info=======\n";
print_r($i);
?>
