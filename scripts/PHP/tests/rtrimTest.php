<?php

$dir = 'abckjadfja';
echo '$dir ='.$dir."\n";

$targets = 'abc';

$rtrim = rtrim($dir, $targets);
echo '$rtrim='.$rtrim."\n";

$ltrim = ltrim($dir, $targets);
echo '$ltrim='.$ltrim."\n";

$trim = trim($dir, $targets);
echo '$trim='.$trim."\n";
 