<?php
    print('ペットは'.htmlspecialchars($_REQUEST['pet'].'です。', ENT_QUOTES));

    $zip = mb_convert_kana($_REQUEST['zip'], 'a', 'UTF-8');
    print($zip);

    if(preg_match("/\A\d{3}\-\d{4}\z/", $zip))
    {
        print('郵便番号：'.$zip);
    }
    else
    {
        print('形式が違う');
    }
?>