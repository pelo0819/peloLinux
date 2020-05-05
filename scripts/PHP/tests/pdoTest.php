<?php

$dsn = 'mysql: dbname=mini_blog; host=192.168.3.50';
$user = 'pelo';
$password = '0819Tobita';

try
{
    $con = new PDO($dsn, $user, $password);
    echo 'アクセス成功';
}
catch(PDOException $e)
{
    echo 'アクセス失敗 ' .$e->getMessage();
    exit;
}
