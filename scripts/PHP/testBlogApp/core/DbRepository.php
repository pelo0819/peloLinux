<?php

abstract class DbRepository
{
    // データベースとのコネクション PDO
    protected $con;

    /*
    コンストラクタ コネクションを設定
    */
    public function __contruct($con)
    {
        $this->setConnection($con);
    }

    /*
    コネクションを設定する
    */
    public function setConnection($con)
    {
        $this->con = $con;
    }

    /*
    データベースへの処理を行う
    $sql    : SQL文
    $params : 動的パラメタ
    */
    public function execute($sql, $params = array())
    {
        $stmt = $this->con->prepare($sql);
        $stmt->execute($params);

        return $stmt;
    }

    /*
    データベースから情報を取得する(1文ずつ) 
    $sql    : SQL文
    $params : 動的パラメタ
    */
    public function fetch($sql, $params = array())
    {
        return $this->execute($sql, $params)->fetch(PDO::FETCH_ASSOC);
    }

    /*
    データベースから情報を取得する(全部) 
    $sql    : SQL文
    $params : 動的パラメタ
    */
    public function fetchAll($sql, $params = array())
    {
        return $this->execute($sql, $params)->fetchAll(PDO::FETCH_ASSOC);
    }    
    
}