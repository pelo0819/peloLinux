<?php
    $link = mysql_connect('localhost', 'root', '0819Tobita');
    if($link)
    {
        echo 'connect database successfully'."\n";
    }
    else
    {
        die('cannot connect database.'. mysql_error());
    }
?>
