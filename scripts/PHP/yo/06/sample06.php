<?php
    $name = htmlspecialchars($_GET['my_name'], ENT_QUOTES);
    $sum = 2 + 8;
?>

<p>hello Mr.<?php print($name); ?> !!! </p>
<ul type="disc">
    <li><a href = "http://192.168.3.50/test04.html"><?php print($name); ?> look hp
    </a></li>
</ul>
<p>sum=<?php print($sum) ?></p>