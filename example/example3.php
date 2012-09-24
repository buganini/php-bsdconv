<?php
list($score,$score_path)=bsdconv_mktemp("score.XXXXXX");
$list=bsdconv_fopen("characters_list.txt","w+");
unlink($score_path);

$c=new Bsdconv("utf-8:score_train:null");

$c->ctl(BSDCONV_CTL_ATTACH_SCORE, $score, 0);
$c->ctl(BSDCONV_CTL_ATTACH_OUTPUT_FILE, $list, 0);

$fp=fopen($argv[1],"r");
$c->init();
while(!feof($fp)){
	$c->conv_chunk(fread($fp, 1024));
}
$c->conv_chunk_last("");
fclose($fp);

bsdconv_fclose($score);
bsdconv_fclose($list);

echo "bsdconv utf-32be:utf-8 characters_list.txt\n";
?>
