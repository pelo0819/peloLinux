<?php
    require_once 'MDB2.php';

    $con = & MDB2::connect('mysql://root:0819Tobita@localhost/sample01_db');

    if(PEAR::isError($con))
    {
        die($con->getMessage());
    }

    $con->setFetchMode(MDB2_FETCHMODE_ASSOC);
    $sql_res = $con->queryAll('select * from testdb01');

    foreach($sql_res as $line)
    {
        print $line['pid'].'   ';
        print $line['uid'].'   ';
        print "\n";
    }

    if(PEAR::isError($sql_res))
    {
        die($sql_res->getMessage());
    }

    $con->disconnect();

?>