<?php

$c=new Bsdconv('utf-8:utf-8');
$c->insert_phase('full',BSDCONV_INTER, 1);
echo $c."\n";
echo $c->conv('test')."\n";

?>
