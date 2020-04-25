<?php
mb_language("Japanese");
mb_internal_encoding("UTF-8");

if(!empty($_POST['email']))
{
    $to = $_POST['email'];
    $subject = $_POST['subject'];
    $body = $_POST['message'];
    $from = mb_encode_mimeheader(mb_convert_encoding("PELO"), "JIS", "UTF-8", "<canvas.pelo@gmail.com>");
    $success = mb_send_mail($to, $subject, $body, "From:".$from);
}
?>

<?php
if($success)
{
    print('送信しました');
}
else
{
    print('送信失敗');
}
?>
