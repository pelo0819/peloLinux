<select name="age" id="age">
<?php
    for($i = 10; $i < 70; $i++)
    {
        print('<option value="'.$i.'">'.$i.'歳</option>');
    }
?>
</select>

<select name="age" id="age">
<?php
    $i = 0;
    while($i <= 65)
    {
        print('<option value="'.$i.'">'.$i.'歳</option>');
        $i++;
    }
?>
</select>