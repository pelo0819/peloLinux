<?php
session_start();
if(isset($_POST['my_id']))
{
    $_SESSION['my_id'] = $_POST['my_id'];
}
if(isset($_POST['password']))
{
    $_SESSION['password'] = $_POST['password'];
}
?>

<p>Hello<?php echo htmlspecialchars($_SESSION['my_id']);?></p>
<p>password is <?php print($_SESSION['password']);?></p>
<p><a href="sample18_second.php">next page</a></p>
