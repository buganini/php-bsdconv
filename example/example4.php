<?php

$sin="utf-8:utf-8,ascii";
$sout=bsdconv_insert_phase($sin, "upper", BSDCONV_INTER, 1);
echo $sout."\n";

$sin=$sout;
$sout=bsdconv_replace_phase($sin, "full", BSDCONV_INTER, 1);
echo $sout."\n";

$sin=$sout;
$sout=bsdconv_replace_codec($sin, "big5", 2, 1);
echo $sout."\n";

$sin=$sout;
$sout=bsdconv_insert_codec($sin, "ascii", 0, 1);
echo $sout."\n";
?>
