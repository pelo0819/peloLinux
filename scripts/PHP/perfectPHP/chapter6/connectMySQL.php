<?php
    $link = mysql_connect('192.168.3.50', 'pelo', '0819Tobita');
    if($link)
    {
        echo 'connect database successfully'."\n";
    }
    else
    {
        die('cannot connect database.'. mysql_error());
    }

    mysql_select_db('canvasdb');

    $user = 'neko';
    $pass = '0104kimura';
    $addr = 'eri@gamil.com';

    // 保存データを作成
    $sql = 'insert into userinfo values('.$user.','.$pass.','.$addr.')';

    // データを保存
    mysql_query($sql, $link);

    echo 'データを保存しました'."\n";
?>
