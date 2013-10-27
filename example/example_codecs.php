<?php

$c=new Bsdconv('utf-8:utf-8');
echo $c->conv('test')."\n";

var_dump(bsdconv_codec_check(BSDCONV_FROM,"_utf-8"));
var_dump(bsdconv_codec_check(BSDCONV_INTER,"_utf-8"));
echo "From:\n";
print_r(bsdconv_codecs_list(BSDCONV_FROM));
echo "Inter:\n";
print_r(bsdconv_codecs_list(BSDCONV_INTER));
echo "To:\n";
print_r(bsdconv_codecs_list(BSDCONV_TO));
?>
