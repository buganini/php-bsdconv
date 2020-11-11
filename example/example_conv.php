#!/usr/bin/env php
<?php
$s=file_get_contents('php://stdin');
try {
	$c=new Bsdconv($argv[1]);
}
catch(Exception $e){
	echo bsdconv_error()."\n";
	exit;
}
echo $c->conv($s);
$i=$c->counter();
echo "\n\n=======Conversino Info=======\n";
print_r($i);
$c->counter_reset("HALF");
print_r($c->counter());
unset($c);
?>
