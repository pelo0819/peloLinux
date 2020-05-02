<?php

$values;
$values[] = 1;
$values[] = 2;
$values[] = 3;

echo 'values size='.count($values)."\n";
foreach($values as $value)
{
    echo 'value='.$value."\n";
}
