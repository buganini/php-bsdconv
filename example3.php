<?php
list($score,$score_path)=bsdconv_mktemp("score.XXXXXX");
$list=bsdconv_fopen("characters_list.txt","w+");
unlink($score_path);

$c=new Bsdconv("utf-8:score_train:null");

$c->ctl(BSDCONV_CTL_ATTACH_SCORE, $score, 0);
$c->ctl(BSDCONV_CTL_ATTACH_OUTPUT_FILE, $list, 0);
$c->conv("忠孝復興忠孝敦化");
bsdconv_fclose($score);
bsdconv_fclose($list);
?>
