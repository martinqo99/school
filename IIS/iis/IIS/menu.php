<div class="page">
    <div class="navigation">
        <ul class="usermenu">
            <li class="signout"><a href="index.php?page=logout">Odhlásit</a></li>
        </ul>
        <ul>
            <li<?php if($page == "order" or $page == "new_order"  or $page == "update_order") echo " class=\"active\">"; else echo '>'; ?><a href="index.php?page=order">Objednávky</a></li>
            <li<?php if($page == "reservation" or $page == "new_reservation" or $page == "update_reservation") echo " class=\"active\">"; else echo '>'; ?><a href="index.php?page=reservation">Rezervace</a></li>
            <li<?php if($page == "product" or $page == "new_product" or $page == "update_product") echo " class=\"active\">"; else echo '>'; ?><a href="index.php?page=product">Jídelní lístek</a></li>
            <li<?php if($page == "table" or $page == "new_table" or $page == "update_table") echo " class=\"active\">"; else echo '>'; ?><a href="index.php?page=table">Stolování</a></li>
            <li<?php if($page == "user" or $page == "new_user" or $page == "update_user") echo " class=\"active\">"; else echo '>'; ?><a href="index.php?page=user">Zaměstnanci</a></li>
        </ul>
    </div>