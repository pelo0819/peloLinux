<?php

/*
データベースとのやり取りをするクラス
・留意点
　外部ホストから本フレームワークを用いたWebアプリケーションを利用した場合でも
　データベースへのアクセスは、外部ホストではなく、Webサーバーが行っている(ログ確認)
*/
class DbManager
{
    // データベースとのコネクション PDOの配列
    protected $connections = array();

    // レポジトリがコネクションを紐づける配列
    // あるレポジトリがどのコネクションを使用しているか
    // レポジトリコネクションマップと命名
    protected $repository_connection_map = array();

    // レポジトリの配列 DbRepositoryクラスの継承クラスの配列
    protected $repositories = array();

    /*
    $paramsで指定した情報を基にデータベースに接続する
    接続に成功したら$connectionsに追加
    $connection_key : 作成するコネクションのキー
    $params         : コネクションするのに必要なパラメータ
    */
    public function connect($connection_key, $params)
    {
        $params = array_merge(array(
            'dsn'      => null,
            'user'     => '',
            'password' => '',
            'options'  => array(),
        ), $params);
      
        try
        {
            $con = new PDO(
                $params['dsn'],
                $params['user'],
                $params['password'],
                $params['options']
            );
            $con->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
            $this->connection[$connection_key] = $con;
            echo 'connetct Database successfully. <br />'."\n";
        }
        catch(PDOException $e)
        {
            echo 'failed connetct Database.<br />'."\n";
        }
        
    }

    /*
    コネクションを取得する
    $$connection_key : 取得したコネクションのキー
    */
    public function getConnection($connection_key = null)
    {
        if(is_null($connection_key))
        {
            return current($this->connections);
        }
        return $this->connections[$connection_key];
    }

    /*
    レポジトリコネクションマップをセットする
    $repository_name : レポジトリの名前
    $connection_key  : コネクションのキー
    */
    public function setRepositoryConnectionMap($repository_name, $connection_key)
    {
        $this->repository_connection_map[$repository_name] = $connection_key;
    }

    /*
    指定したレポジトリが使用しているコネクションを取得
    $repository_name : レポジトリの名前
    */
    public function getConnectionForRepository($repository_name)
    {
        if(isset($this->repository_connection_map[$repository_name]))
        {
            $key = $this->repository_connection_map[$repository_name];
            $con = $this->getConnection($key);
        }
        else
        {
            $con = $this->getConnection();
        }
        return $con;
    }

    /*
    指定した名前のレポジトリを取得
    存在しないなら新規レポジトリを生成
    */
    public function get($repository_name)
    {
        // レポジトリが生成されていないなら生成する
        if(!isset($this->repositories[$repository_name]))
        {
            $repository_class = $repository_name . 'Repository';
            $con = $this->getConnectionForRepository($repository_name);

            $repository = new $repository_class($con);
            $this->repositories[$repository_name] = $repository;
        }

        return $this->repositories[$repository_name];
    }

    /*
    デストラクタ
    コネクション、レポジトリを全削除
    */
    public function __destruct()
    {
        foreach($this->repositories as $rep)
        {
            unset($rep);
        }

        foreach($this->connections as $con)
        {
            unset($con);
        }
    }

}
