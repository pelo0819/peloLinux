<!DOCTYPE html>
<html>
<head>
<meta charset='utf-8'>
<title>HTML内でのPHP処理</title>
</head>
<body>
<h1><?php echo 'タイトルです'; ?></h1>
<?php
$text = "HTML内でのPHPスクリプト実行処理";
echo "<p> $text </p>";
?>
</body>
</html>