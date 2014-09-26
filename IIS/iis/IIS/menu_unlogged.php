<div class="page">
    <div class="navigation">
        <ul class="usermenu">
            <li class="signout"><a href="index.php?page=logout">Odhlásit</a></li>
        </ul>
        <ul>
            <li></li>
            <li<?php if($page == "login_window") echo " class=\"active\">"; else echo '>'; ?><a href="index.php?page=login_window">Přihlášení</a></li>
            <li<?php if($page == "about_window") echo " class=\"active\">"; else echo '>'; ?><a href="index.php?page=about_window">O projektu</a></li>
        </ul>
    </div>