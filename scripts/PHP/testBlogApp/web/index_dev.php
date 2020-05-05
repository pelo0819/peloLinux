<?php

//require '../bootstrap.php';
require '/home/pelo/Documents/peloLinux/scripts/PHP/testBlogApp/bootstrap.php';
require '../MiniBlogApplication.php';

$app = new MiniBlogApplication(true);
$app->run();
