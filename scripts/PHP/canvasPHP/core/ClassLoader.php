<?php

/*
指定したディレクトリにあるクラスをロードするクラス
*/
class ClassLoader
{
    /* ディレクトリを格納する配列 */
    protected $dirs;

    public function register()
    {
        echo 'register()'."\n";
        spl_autoload_register(array($this, 'loadClass'));
    }

    public function registerDir($dir)
    {
        echo 'registerDir()'."\n";
        /* こういう配列の定義できるんだ */
        $this->dirs[] = $dir;
    }

    public function loadClass($class)
    {
        // echo 'directory count='.count($dirs)."\n";
        foreach($this->dirs as $dir)
        {
            $file = $dir . '/' . $class . '.php';
            if(is_readable($file))
            {
                echo 'load file='.$class.'.php'."\n";
                require $file;
                return;
            }
        }
    }

}