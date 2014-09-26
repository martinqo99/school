<?php
$show_form = 0;


if (checkLoggedin()) { //already logged
    echo '<script type="text/javascript">window.location.href="index.php?page=order"</script>';
}

if (isset($_POST['submit'])) {
    $login = htmlspecialchars(strtolower($_POST['name'])); //key sensitive????
    $pass = $_POST['password'];

    //$query="SELECT * FROM users WHERE user_login='" . $login . "' AND user_password='" . code_password($pass) . "' LIMIT 1";
    $query="SELECT * FROM user WHERE user_login='" . $login . "' AND user_password='" . $pass . "' LIMIT 1";
    $sql = mysql_query($query);
    if (mysql_num_rows($sql) == 0) {
        echo $query."<br />";
        echo '<strong>Špatné přihlašovací údaje!<br></strong>';
        $show_form = 1;
    }
    while ($row = mysql_fetch_array($sql)) {
        //mysql_query("UPDATE user SET user_date_last_login = NOW() WHERE user_id ='" . $row['user_id'] . "'");
        startSession($row['user_id'],$row['user_login'],$row['user_password'],$row['user_function'],True);
        echo '<script type="text/javascript">window.location.href="index.php?page=order"</script>';
    }
}
?>
</div>


