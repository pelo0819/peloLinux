<?php

require '../bootstrap.php';

$request = new Request();

$host = $request->getHost();
$myaddr = $request->getMyAddr();
$userAddr = $request->getUserAddr();

echo 'host='.$host.' myAddr='.$myaddr.' userAddr='.$userAddr."\n";