<?php
$file = $_FILES['my_img'];
print('file name:'.$flie['name'].'<br />');
print('file type:'.$flie['type'].'<br />');
print('upload file:'.$flie['tmp_name'].'<br />');
print('error:'.$file['error'].'<br />');
print('size:'.$file['size'].'<br />');

$ext = substr($file['name'], -4);
if($ext == '.gif' || $ext == '.jpg' || $ext=='.png')
{
    $filePath = './user_img/'.$file['name'];
    move_uploaded_file($file['tmp_name'], $filePath);
    print('<img src="'.$filePath.'"/>');
}
else
{
    print('extension error');
}
?>
