<?php
session_start();
session_unset();
header('Location: sample18_input.html');
exit();
?>