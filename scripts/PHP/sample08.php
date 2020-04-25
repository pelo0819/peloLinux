<form action="sample08_2.php" method="get">
<dl>
<dt>動物</dt>
<dd>
<?php
    $pets = array('ねこ🐱','くま🐻','たぬきち');
    array_push($pets, 'うさぎ🐰');
    $items = array('a'=>'morning', 'b'=>'evening', 'c'=>'night');
?>
<select name="pet" id="pet">
<?php
    foreach($pets as $pet)
    {
        print('<option value="'.$pet.'">'.$pet.'</option>');
    }

    foreach($items as $key => $value)
    {
        print('<input type="checkbox" id="'.$key.'" value ="'.$key.'"/><label for="'.$key.'">'.$value.'</label>');
    }
?>
</select>
</dd>
<dt>郵便番号</dt>
<dd>
    <input type="text"  name="zip" size="10" maxlength="255" id="zip"/>
</dd>
</dl>
<input type="submit" value="send" />
</form>
