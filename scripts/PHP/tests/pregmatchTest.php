<?php

$routes = array
(
    'a' => 'neko',
    'b' => 'kuma',
    'c' => array('usage' => 'bad'),
);

// ターゲットにactが含まれていればマッチ
$pattern2 = '#^act#';

// ターゲットの前半が/account/で後半にaが含まれていなければマッチで、
// match[action]にマッチした要素を格納
$pattern3 = '#^/account/(?P<action>[^a]+)$#';

// /(スラッシュ)が含まれていなければマッチで、
// match[action]にマッチした要素を格納
$pattern5 = '#^(?P<action>[^/]+)$#';

$pattern6 = '#^/account/(?P<action>[^/])$#';

$pattern8 = '#(?P<action>[^/]+)#';


// ターゲットに/(スラッシュ)が含まれていなければマッチ
$pattern7 = '#^([^/]+)$#';


$val[] = 'actionavv';
$val[] = 'a';
$val[] = '//';
$val[] = '/account/sb_fffffb';

foreach($val as $i => $v)
{
    // echo 'v='.$v.' ';
    // preg_match() マッチしたら1を返す;
    $check = preg_match($pattern8, $v, $matches);
    // echo 'val['.$i.']='.$check."\n";
    // echo $matches['action']."\n";
    
    print_r($matches);
}


// $var = '(?P<action>[^/]+)';
// $path ='';
// if(preg_match('#^'.$var.'$#', $path, $matches))
// {
//     echo 'matches='.$matches."\n";
// }
// else
// {
//     echo 'do not match.'."\n";
// }

// foreach($routes as $key => $value)
// {
//     if(is_array($value))
//     {
//         foreach($value as $key_v => $value_v)
//         {
//             echo 'key='.$key.' key_v='.$key_v.' value_v='.$value_v."\n";
//         }
//     }
//     else
//     {
//         echo 'key='.$key.' value='.$value."\n";
//     }
// }
