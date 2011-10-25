<?php
$info=array();
function getlist($c){
	$pre='/usr/local/share/bsdconv/'.$c.'/';
	$s=glob($pre.'*');
	$r=array();
	foreach($s as $a){
		if(!preg_match('/\\.so$/',$a)){
			$r[]=str_replace($pre,'',$a);
		}
	}
	echo implode('<br />',$r);
}

if(isset($_POST['conversion'])){
	$p=bsdconv_create($_POST['conversion']);
	if($p!==false){
		$text=bsdconv($p, $_POST['text']);
		$info=bsdconv_info($p);
		bsdconv_destroy($p);
	}
}
?><html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8" />
<style type="text/css">
	span{
		float: left;
		margin-right: 1em;
		padding-left: 0.5em;
		border-left: solid 1px #000;
	}
</style>
</head>
<body style="background: #abf;">
<?php
$p=bsdconv_create("utf-8,ascii:ascii-html-info");
echo bsdconv($p, $text);
bsdconv_destroy($p);
?>
<form action="bsdconv.php" method="post">
<input style="width: 50%;" type="text" name="conversion" id="conversion" value="<?php
	echo htmlspecialchars($_POST['conversion']);
?>"/>
<select onChange="document.getElementById('conversion').value=this.value">
<option value="utf-8,ascii:zh_decomp:zh_comp:utf-8,ascii">Chinese Components Combination</option>
<option value="utf-8,ascii:chewing:utf-8,ascii">Chewing (Chinese Bopomofo)</option>
<option value="utf-8,ascii:zhtw:zhtw_words:utf-8,ascii">to Traditional Chinese</option>
<option value="utf-8,ascii:zhcn:utf-8,ascii">to Simplified Chinese</option>
<option value="utf-8,ascii:jp_pinyin:utf-8,ascii">Japanese to phonetics</option>
<option value="utf-8,ascii:ascii-hex-numeric-html-entity">Hex Numeric HTML Entity</option>
<option value="utf-8,ascii:full:utf-8,ascii">Full Width</option>
<option value="utf-8,ascii:half:utf-8,ascii">Half Width</option>
</select>
<input type="submit" /><br />
<textarea name="text" style="width: 80%; height: 60%;"><?php
echo htmlspecialchars($text);
?></textarea>
</form>
<h4>Info:</h4>
<?php
	foreach($info as $k=>$v){
		echo htmlspecialchars($k);
		echo ' => ';
		echo htmlspecialchars($v);
		echo '<br />';
	}
?>
<hr />
<div>
<span><h4>From:</h4><?php echo getlist('from');?></span>
<span><h4>Inter:</h4><?php echo getlist('inter');?></span>
<span><h4>To:</h4><?php echo getlist('to');?></span>
</div>
<script type="text/javascript">
	if(document.getElementById('conversion').value==''){
		document.getElementById('conversion').value='utf-8,ascii:zh_decomp:zh_comp:utf-8,ascii';
	}
</script>
</body>
</html>
