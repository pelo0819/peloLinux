<?php

require '../canvasPHP/core/Router.php';

$def = array(
    '/account'=> array('controller' => 'account', 'action' => 'index'),
    '/account/:action' => array('controller' => 'account'),
);
$router = new Router($def);
$router->showRoutes();
$path_info='list';
$params = $router->resolve($path_info);
$controller = $params['controller'];
$action = $params['action'];
echo 'controller='.$controller.' action='.$action."\n";

echo "\n";

$name_str = 'tobita,kimura,daijiro,eri,neko,kuma';
$names = explode(",", $name_str);
foreach($names as $i => $name)
{
    echo '$i='.$i.' $name='.$name."\n";
    if($i % 2 == 0)
    {
        $name = 'usagi';
    }
    $names[$i] = $name;
}

$name2_str = implode('??', $names);
echo '$name2_str='.$name2_str."\n";

$str = 'abcdefg';
echo 'substr='. substr($str, 1) . "\n";



 