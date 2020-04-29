
<?php
$tempfile = $_FILES['fname']['tmp_name'];
$filename = '/php/sample23/user_img/' . $_FILES['fname']['name'];
$error = $_FILES['fname']['error'];
$size = $_FILES['fname']['size'];

echo "tempfile=" . $tempfile . ",filename=" . $filename . "<br />";
echo " error=" . $error . ",size=" .$size."<br />";

if (is_uploaded_file($tempfile)) {
    if ( move_uploaded_file($tempfile , $filename )) {
	echo $filename . "をアップロードしました。<br />";
        print('<img src="'. $filename . '" />');
        echo '<img src="/php/sample23/user_img/kuma.png"/>';
    } else {
        echo "ファイルをアップロードできません。";
    }
} else {
    echo "ファイルが選択されていません。";
} 
?>
