<!DOCTYPE html>
<?php
	// Include common modules
	require_once('inc/menu.php');
	require_once('inc/plugins.php');
	require_once('inc/files.php');

	// Include configuration
	require_once('config/plugins.php');
	sort_plugins();
	
	require_once('config/menu.php');
	require_once('config/youtube.php');
	
	// Determine current page
	$PAGE       = isset($argv[1]) ? $argv[1] : 'index';
	$MENUITEM   = find_menu_item($PAGE);
	$DOCROOT    = (isset($MENUITEM)) ? ('../' . ((strlen($MENUITEM['root']) > 0) ? ($MENUITEM['root'] . '/') : '')) : '';
	$HEADER		= (isset($MENUITEM)) ? $MENUITEM['text'] : 'LSP Plugins Documentation';
	$FILENAME   = (isset($MENUITEM)) ? (isset($MENUITEM['file']) ? $MENUITEM['file'] : $PAGE) : 'index';
?>

<html>
	<head>
		<title>Linux Studio Plugins Project</title>

		<meta content="text/html; charset=utf-8" http-equiv="Content-Type">

		<link rel="stylesheet" href="<?= $DOCROOT ?>css/style.css" type="text/css">
	</head>
	<body>
		<div id="body">
			<!-- Header -->
			<div id="header">
				<img src="<?= $DOCROOT ?>img/header.png" alt="Linux Studio Plugins Project" >
			</div>
			
			<!-- Navigation top -->
			<div id="menu">
				<?php require("./manuals/menu.php"); ?>
			</div>
		
			<!-- Main content -->
			<div id="main">
				<h1><?php echo htmlspecialchars($HEADER); ?></h1>
				<?php require("./manuals/${MENUITEM['path']}/${FILENAME}.php"); ?>
			</div>
			
			<!-- Navigation bottom -->
			<div id="menu">
				<?php require("./manuals/menu.php"); ?>
			</div>
		
			<!-- Footer -->
			<div id="footer">
				<p>(C) Linux Studio Plugins, 2015-2016</p>
				<p>All rights reserved</p>
			</div>
		</div>
	</body>
</html>