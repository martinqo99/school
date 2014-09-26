<?php
    if (isset($_POST['new_user'])) {            
    if($_POST['name']!="") $name_error=0;
    else $name_error=1;
    
    if($_POST['surname']!="") $surname_error=0;
    else $surname_error=1;
    
    if($_POST['login']!="") $login_error=0;
    else $login_error=1;
    
    if($_POST['password']!="") $password_error=0;
    else $password_error=1;
    
    if($_POST['function']!="") $function_error=0;
    else $function_error=1;
    
    if($_POST['phone']!="") $phone_error=0;
    else $phone_error=1;
  }

  if (isset($_POST['new_user']) and $name_error==0 and $surname_error==0 and $login_error==0 and $password_error==0  and $function_error==0 and $phone_error==0) {        
    $sql = "INSERT INTO `user` (`user_id`, `user_name`, `user_surname`, `user_login`, `user_password`, `user_function`, `user_phone`) 
    VALUES (NULL, '" . $_POST['name'] . "', '" . $_POST['surname'] . "', '" . $_POST['login'] . "', '" . $_POST['password'] . "', '" . $_POST['function'] . "', '" . $_POST['phone'] . "')";
    mysql_query($sql);
    echo '<script type="text/javascript">window.location.href="index.php?page=user"</script>';
  }

  if (isset($_POST['update_user'])) {            
    if($_POST['name']!="") $name_error=0;
    else $name_error=1;
    
    if($_POST['surname']!="") $surname_error=0;
    else $surname_error=1;
    
    if($_POST['login']!="") $login_error=0;
    else $login_error=1;
    
    if($_POST['password']!="") $password_error=0;
    else $password_error=1;
    
    if($_POST['function']!="") $function_error=0;
    else $function_error=1;
    
    if($_POST['phone']!="") $phone_error=0;
    else $phone_error=1;
  }

  if (isset($_POST['update_user']) and $name_error==0 and $surname_error==0 and $login_error==0 and $password_error==0  and $function_error==0 and $phone_error==0) { 
    $sql = "UPDATE `user` SET user_name = '" .$_POST['name']. "', user_surname = '" . $_POST['surname'] ."', user_login = '" . $_POST['login'] . "', user_password = '" . $_POST['password'] ."', user_function = '" . $_POST['function'] ."', user_phone = '" . $_POST['phone'] ."' WHERE user_id = " . $_GET['product'] ."";
    mysql_query($sql);
    echo '<script type="text/javascript">window.location.href="index.php?page=user"</script>';
  }


  if (isset($_POST['delete_user'])) {            
    if($_POST['del_user']!=0 ) $user_error=0;
    else $user_error=1;
  }

  if (isset($_POST['delete_user']) and $user_error == 0) {     
    foreach ($_POST['del_user'] as $i) {
      $sql = "DELETE FROM `user` WHERE user_id = " . $i . "";
      mysql_query($sql);
    }
    echo '<script type="text/javascript">window.location.href="index.php?page=user"</script>';
  }
?>