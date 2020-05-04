<?php

class Session
{
    protected static $sessionStarted = false;
    protected static $sessionIdRegenerated = false;

    /*
    コンストラクタ
    セッションを開始する、セッションが重複しないようにスタックでフラグを管理
    */
    public function __construct()
    {
        if(!self::$sessionStarted)
        {
            session_start();
            self::$sessionStarted = true;
        }
    }

    /*
    セッションをセット
    */
    public function set($name, $value)
    {
        $_SESSION[$name] = $value;
    }

    /*
    セッションからパラメタを取得
    */
    public function get($name, $default = null)
    {
        if(isset($_SESSION[$name]))
        {
            return $_SESSION[$name];
        }
        return $default;
    }

    /*
    セッションを削除
    */
    public function remove($name)
    {
        unset($_SESSION[$name]);
    }

    public function clear()
    {
        $_SESSION = array();
    }

    /*
    セッションの再生成
    フラグ立てっぱなしだから、再度Idの割り当てできなくない？
    */
    public function regenerate($destroy = true)
    {
        if(!self::$sessionIdRegenerated)
        {
            session_regenerate_id($destroy);

            self::$sessionIdRegenerated = true;
        }
    }

    /*
    ログイン確認
    */
    public function setAuthenticated($bool)
    {
        $this->set('_authenticated', (bool)$bool);
    }

    /*
    ログインしているか確認
    */
    public function isAuthenticated()
    {
        return $this->get('_authenticated', false);
    }

    
}