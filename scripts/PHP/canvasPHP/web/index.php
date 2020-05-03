<?php

require '../bootstrap.php';

$request = new Request();

$host = $request->getHost();
$myaddr = $request->getMyAddr();
$userAddr = $request->getUserAddr();
$baseUrl = $request->getBaseUrl();
$requestUrl = $request->getRequestUri();
$scriptName = $request->getScriptName();
$path_info = $request->getPathInfo();

echo 'host='.$host.' myAddr='.$myaddr.' userAddr='.$userAddr."\n";
echo 'baseUrl='.$baseUrl.' requestUrl='.$requestUrl.' scriptName=',$scriptName."\n";