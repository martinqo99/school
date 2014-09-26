<?php
function code_password($text) {
    return md5("@!_?" . $text . "karel=42?");
}

function startSession($id, $username, $password, $type, $cookie) {
    //Add additional member to Session array as per requirement 
    //session_register();

    $_SESSION["id"] = $id;
    $_SESSION["username"] = $username;
    $_SESSION["password"] = $password;
    $_SESSION["type"] = $type;

    $cities[] = "Vše";
    $_SESSION["filter_loc"] = $cities;


    if ($cookie) {
        //Add additional member to cookie array as per requirement 
        setcookie("id", $_SESSION['id'], time() + 60 * 60 * 24 * 100, "/");
        setcookie("username", $_SESSION['username'], time() + 60 * 60 * 24 * 100, "/");
        setcookie("password", $_SESSION['password'], time() + 60 * 60 * 24 * 100, "/");
        setcookie("type", $_SESSION['type'], time() + 60 * 60 * 24 * 100, "/");
        return;
    }
}

function endSession() {
    unset($_SESSION['id']);
    unset($_SESSION['username']);
    unset($_SESSION['password']);
    unset($_SESSION['type']);

    session_unset();
    session_destroy();

    setcookie("id", "", time() - 60 * 60 * 24 * 100, "/");
    setcookie("username", "", time() - 60 * 60 * 24 * 100, "/");
    setcookie("password", "", time() - 60 * 60 * 24 * 100, "/");
    setcookie("type", "", time() - 60 * 60 * 24 * 100, "/");
}

function confirmUser($username, $password) {
    $query = "SELECT * FROM users WHERE user_deleted!=1 AND (user_email='" . $username . "' OR user_login='" . $username . "') AND user_password='" . $password . "' LIMIT 1";
    $sql = mysql_query($query);
    if (mysql_num_rows($sql) == 0)
        return False;
    else
        return True;
}

function checkLoggedin() {
    if (isset($_SESSION['username']) AND isset($_SESSION['password']))
        return true;
    elseif (isset($_COOKIE['username']) && isset($_COOKIE['password'])) {
        if (confirmUser($_COOKIE['username'], $_COOKIE['password'])) {
            createsessions($_COOKIE['username'], $_COOKIE['password']);
            return true;
        } else {
            clearsessionscookies();
            return false;
        }
    }
    else
        return false;
}
?>
