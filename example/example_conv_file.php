#!/usr/bin/env php
<?php
$c=new Bsdconv($argv[1]);
if(!$c){
	echo bsdconv_error()."\n";
	exit;
}
$c->conv_file($argv[2], $argv[3]);
echo $c."\n";
print_r($c->counter());
unset($c);
?>
