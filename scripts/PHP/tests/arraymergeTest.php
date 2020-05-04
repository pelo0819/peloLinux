<?php

$array1 = array(
    'kuma' => 'bear',
    'neko' => 'cat',
    'usagi' => 'rabbir'
);

foreach($array1 as $key => $value)
{
    echo 'key='.$key.' value='.$value."\n";
}

echo "\n";

$array1 = array_merge(
    $array1,
    array(
    'tanuki' => 'raccoon',
    'usagi' => 'rabbit'
));

foreach($array1 as $key => $value)
{
    echo 'key='.$key.' value='.$value."\n";
}

 