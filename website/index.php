<html>
<head><title>SuperCollider plugins</title>
<style type="text/css">
body { background-color: rgb(204, 204, 204); font-family: Arial,Helvetica,sans-serif; }
h1 {font-family: Arial,Helvetica,sans-serif; font-weight: normal;}

.readonlinelink {font-size: smaller; color: rgb(102,102,102); }

li              { list-style-type: none; clear: both;}
ul.grouplist    { width: 90%; clear: both; list-style-type: none; }
ul.grouplist li { width: 25%; float: left;  clear: none;}
</style>
</head>
<body>
<h1>UGen plugins for <a href="http://supercollider.sourceforge.net/">SuperCollider</a></h1>

<p>SuperCollider &quot;UGen&quot; plugins are extensions for the <a href="http://supercollider.sourceforge.net/">SuperCollider</a> audio synthesis server 
(see also <a href="http://quarks.sourceforge.net/">quarks</a> to extend the 
programming language). For community discussion and support see the <a href="http://supercollider.sourceforge.net/mod/resource/view.php?id=41">SuperCollider mailing lists</a>.</p>

<p style="padding: 10px; text-align: center; border: 1px solid blue; width: 50%; margin-left: 20%; "><a href="http://sourceforge.net/project/platformdownload.php?group_id=182627">Download the plugins</a></p>

<p>The following list of helpfiles gives you an idea of the plugins you'll get
when you download the plugin pack:</p>

<ul>
<?php

$list = file('filelist.txt');
foreach($list as $key =>$fpath){
	$group = array_shift(explode('/', $fpath));
	$filename = trim(basename($fpath));
	$ext = substr($filename, strrpos($filename, '.'));
	if($ext == '.html'){
		$grouped[$group][] = array('name'=>substr($filename, 0, -5), 'fpath'=>$fpath);
	}else{
		$grouped[$group][] = array('name'=>substr($filename, 0, strpos($filename, '.')));
	}
}

foreach($grouped as $group=>$grlist){
	?>
	<li><strong><?php echo $group ?></strong>:</li>
	<ul class="grouplist">
		<?php
		foreach($grlist as $item){
			?>
			<li><?php
				echo $item['name'];
				if(isset($item['fpath']))
					echo " <span class='readonlinelink'>[<a href='http://sc3-plugins.svn.sourceforge.net/viewvc/sc3-plugins/source/$item[fpath]'>read online</a>]</span>";
			?></li>
			<?php
		}
		?>
	</ul>
	<?php
}


?>
</ul>

<hr style="clear: both; margin: 10px; border-color: black;" />
<p style="text-align: right;"><a href="http://sourceforge.net/projects/sc3-plugins/">Sourceforge project page</a> <a href="http://sourceforge.net"><img src="http://sflogo.sourceforge.net/sflogo.php?group_id=182627&amp;type=4" width="125" height="37" border="0" alt="SourceForge.net Logo" /></a></p>
</body></html>
