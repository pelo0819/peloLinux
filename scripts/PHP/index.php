<?php
    $value = htmlspecialchars($_REQUEST['value'], ENT_QUOTES);
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>

</head>
<body>
    <dl>
        <dt>PHP index</dt>
        <dd><?php print('value='.$value);?></dd>
    </dl>
</body>
</html>