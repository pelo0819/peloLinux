<?php
    $link = true;
    // $link = mysql_connect('192.168.3.50', 'root', '0819Tobita');
    if($link)
    {
        echo 'connect database successfully';
    }
    else
    {
        die('cannot connect database.'. mysql_error());
    }
?>
