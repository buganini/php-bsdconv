<?php

$c=new Bsdconv('utf-8:utf-8');
$c->insert_phase('full',BSDCONV_INTER, 1);
echo $c."\n";
echo $c->conv('test')."\n";

var_dump(bsdconv_codec_check(BSDCONV_FROM,"_utf-8"));
var_dump(bsdconv_codec_check(BSDCONV_INTER,"_utf-8"));
print_r(bsdconv_codecs_list());
?>
