<?php

class DbManager
{
    protected $connections = array();

    public function connect($name, $param)
    {
        $params = array_merge(array(
            'dsn'      => null,
            'user'     => '',
            'password' => '',
            'options'  => '',
        ), $params);

        $con = new PDO(

        );
        
    }


    public function getConnection($name = null)
    {

    }

}