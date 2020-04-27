<?php

$file = $_FILES['my_img'];

print('file name:' . $flie['name']);
print('file type:'. $flie['type']);
print('upload file:'. $flie['tmp_name']);
print('error:'. $file['error']);
print('size:'. $file['size']);

$ext = substr($file['name'], -4);
if($ext == '.gif' || $ext == '.jpg' || $ext=='.png')
{
    $filePath = './user_img/'.$file['name'];
    print('filePath='.$filePath.'');
    move_uploaded_file($file['tmp_name'], $filePath);
    print('<img src="'.$filePath.'"/>');
}
else
{
    print('extension error');
}
?>
