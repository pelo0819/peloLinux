<?php
    if(isset($_COOKIE['my_id']))
    {
        $myid = $_COOKIE['my_id'];
    }
    else
    {
        $myid ='';
    }
?>


<form action="sample17.php" method="post">
<dl>
<dt>ID</dt>
<dd>
    <input type="text" name="my_id" id="my_id" value="<?php echo $myid; ?>" />
</dd>
<dt>pass</dt>
<dd>
    <input type="password" name="password" id="password"/>
</dd>
<dt>IDの保存</dt>
</dl>
<p><input type ="checkbox" name="save" id="save" value="on"/>
<label for="save">IDを保存する</label></p>
<input type="submit" value="send" />
</form>
