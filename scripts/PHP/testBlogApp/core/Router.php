<?php

class Router
{
    protected $routes;
    // public $routes;

    public function __construct($definitions)
    {
        echo 'Router construct<br />'."\n";
        $this->routes = $this->compileRoutes($definitions);
    }



    /*
    ルーティング定義から
    */
    public function compileRoutes($definitions)
    {
        $routes = array();

        /*
        $definationsが
        array(
            'account/' => array('controller'=>'account', 'action'=>'index'), ... case1
            'account/:action' => array('controller'=>'account'), ... case2
        );
        だったら
        */
        foreach($definitions as $url => $params)
        {
            // ルーティング定義の先頭の/を削除し、/で分解する
            $tokens = explode('/', ltrim($url, '/'));
            
            // echo '$url='.$url."\n";
            // foreach($params as $key => $value)
            // {
            //     echo 'key='.$key.' value='.$value."\n";
            // }
            // echo "\n";

            foreach($tokens as $i => $token)
            {
                if(0 === strpos($token, ':'))
                {
                    $name = substr($token, 1);
                    $token = '(?P<' .$name.'>[^/]+)';
                    // echo 'token='.$token."\n";
                }
                $tokens[$i] = $token;
            }

            $pattern = '/' . implode('/', $tokens);
            $routes[$pattern] = $params;
            // echo '$pattern='.$pattern."\n";
            // foreach($params as $param)
            // {
            //     echo '$param='.$param."\n";
            // }
            // echo "\n";
        }

        foreach($routes as $key => $value)
        {
            echo 'pattern='.$key;
            if(is_array($value) === true)
            {
                foreach($value as $i => $v)
                {
                    echo ', param[' .$i. ']='.$v;
                }
                echo '<br />'."\n";
            }
            else
            {
                echo ', param='.$value.'<br />'."\n";
            }
        }
        return $routes;
    }

    public function resolve($path_info)
    {
        // 最初の一文字がスラッシュじゃなかったらスラッシュをつける
        if ('/' !== substr($path_info, 0, 1)) 
        {
            $path_info = '/' . $path_info;
        }

        foreach ($this->routes as $pattern => $params) 
        {
            if (preg_match('#^' . $pattern . '$#', $path_info, $matches)) 
            {
                $params = array_merge($params, $matches); 

                return $params;
            }
        }

        return false;
    }

    public function showRoutes()
    {
        foreach($this->$routes as $key => $value)
        {
            echo 'key='.$key.' value='.$value."\n";
        }
    }

}
