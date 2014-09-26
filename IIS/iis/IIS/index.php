<?php
session_start();
require("config.php");
require("db_connect.php");
require("functions.php");

require ("header.php");

if (isset($_GET['page'])) {
   $page = $_GET['page'];
   switch($page) {
      case 'order':
         require ("menu.php");
         require($page.".php");
         break;
      case 'reservation':
         require ("menu.php");
         require($page.".php");
         break;
      case 'product':
         require ("menu.php");
         require($page.".php");
         break;
      case 'table':
         require ("menu.php");
         require($page.".php");
         break;
      case 'user':
         require ("menu.php");
         require($page.".php");
         break;
      case 'new_order':
         require ("menu.php");
         require($page.".php");
         break;
      case 'update_order':
         require ("menu.php");
         require($page.".php");
         break;
      case 'order_process':
         require($page.".php");
         break;
      case 'new_reservation':
         require ("menu.php");
         require($page.".php");
         break;
      case 'update_reservation':
         require ("menu.php");
         require($page.".php");
         break;
      case 'reservation_process':
         require($page.".php");
         break;
      case 'new_product':
         require ("menu.php");
         require($page.".php");
         break;
      case 'update_product':
         require ("menu.php");
         require($page.".php");
         break;
      case 'product_process':
         require($page.".php");
         break;
      case 'new_table':
         require ("menu.php");
         require($page.".php");
         break;
      case 'update_table':
         require ("menu.php");
         require($page.".php");
         break;
      case 'table_process':
         require($page.".php");
         break;
      case 'new_user':
         require ("menu.php");
         require($page.".php");
         break;
      case 'update_user':
         require ("menu.php");
         require($page.".php");
         break;
      case 'user_process':
         require($page.".php");
         break;
      case 'login':
         require($page.".php");
         break;
      case 'logout':
         require($page.".php");
         break;
      case 'login_window':
         require ("menu_unlogged.php");
         require($page.".php");
         break;
      case 'about_window':
         require ("menu_unlogged.php");
         require($page.".php");
         break;
      case 'index':
         $page = "login_window";
         require ("menu_unlogged.php");
         require($page.".php");
      default:
         $page = "404";
         require($page.".php");
         break;
   }
} else {
    if (!isset($_SESSION["id"])){
        $page = "login_window";
        require ("menu_unlogged.php");
        require($page.".php");
        
    }
    else {
        $page = "order";
        require ("menu.php");
        require($page.".php");
    }
}

require ("footer.php");

?>