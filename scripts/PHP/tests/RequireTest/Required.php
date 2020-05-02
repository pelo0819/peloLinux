<?php

class Required
{
    private $aa;

    function __construct()
    {
        $aa = 1;
        echo 'hello';
        echo '$aa='.$aa."\n";
    }
}