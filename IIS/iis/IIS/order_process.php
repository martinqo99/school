<?php
  if (isset($_POST['new_order'])) {            
    if($_POST['table']!=0) $order_error=0;
    else $order_error=1;

    if($_POST['reservation']!=0) $reservation_error=0;
    else $reservation_error=1;
  }

  if (isset($_POST['new_order']) and $order_error==0 and $reservation_error==0) {
    $sql = "INSERT INTO `order` (`order_id`, `order_user`, `order_table`, `order_reservation`, `order_date`) VALUES (NULL, " . $_SESSION['id'] . ", " . $_POST['table'] . ", " . $_POST['reservation'] . ", NOW())";
    mysql_query($sql);
    echo '<script type="text/javascript">window.location.href="index.php?page=order"</script>';
  }

  if (isset($_POST['update_order'])) {      
    $order_id = $_GET['product'];      
    if($_POST['table']!=0) $order_error=0;
    else $order_error=1;

    if($_POST['reservation']!=0) $reservation_error=0;
    else $reservation_error=1;
  }

  if (isset($_POST['update_order']) and $order_error==0 and $reservation_error==0) {            
    $sql = "UPDATE `order` SET order_user = " .$_SESSION['id']. ", order_table = " . $_POST['table'] .", order_reservation = " . $_POST['reservation'] . ", order_date = NOW() WHERE order_id = " . $order_id ."";
    mysql_query($sql);
    echo '<script type="text/javascript">window.location.href="index.php?page=order"</script>';
  }

  if (isset($_POST['delete_order'])) {            
    if($_POST['del_order']!=0 ) $order_error=0;
    else $order_error=1;
  }

  if (isset($_POST['delete_order']) and $order_error == 0) {     
    foreach ($_POST['del_order'] as $i) {
      $sql = "DELETE FROM `order` WHERE order_id = " . $i . "";
      mysql_query($sql);
    }
    echo '<script type="text/javascript">window.location.href="index.php?page=order"</script>';
  }
?>