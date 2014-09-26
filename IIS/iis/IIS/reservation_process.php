<?php
    if (isset($_POST['new_reservation'])) {            
    if($_POST['name']!="") $name_error=0;
    else $name_error=1;

    if($_POST['seats']!=0) $seats_error=0;
    else $seats_error=1;

    if($_POST['date']!=0 ) $date_error=0;
    else $date_error=1;
    
    if($_POST['description']!="") $description_error=0;
    else $description_error=1;
  }

  if (isset($_POST['new_reservation']) and $name_error==0 and $seats_error==0 and $date_error==0 and $description_error==0) {        
    $sql = "INSERT INTO `reservation` (`reservation_id`, `reservation_user`, `reservation_name`, `reservation_persons`, `reservation_dateRes`, `reservation_dateCom`, `reservation_description`, `reservation_status`) 
    VALUES (NULL, " . $_GET['user'] . ", '" . $_POST['name'] . "', " . $_POST['seats'] . ", NOW(),'" . $_POST['date'] . "', '" . $_POST['description'] . "', 'Rezervováno')";
    mysql_query($sql);
    echo '<script type="text/javascript">window.location.href="index.php?page=reservation"</script>';
  }

    if (isset($_POST['update_reservation'])) {         
    if($_POST['name']!="") $name_error=0;
    else $name_error=1;
    
    if($_POST['seats']!=0 ) $seats_error=0;
    else $seats_error=1;
    
    if($_POST['date']!=0 ) $date_error=0;
    else $date_error=1;

    if($_POST['description']!="") $description_error=0;
    else $description_error=1;

    if($_POST['status']!="") $status_error=0;
    else $status_error=1;
  }

  if (isset($_POST['update_reservation']) and $name_error==0 and $seats_error==0 and $date_error==0 and $description_error==0 and $status_error==0) {      
    $sql = "UPDATE `reservation` SET reservation_user = " .$_GET['user']. ", reservation_name = '" .$_POST['name']. "', reservation_persons = " . $_POST['seats'] .", reservation_dateCom = " . $_POST['date'] . ", reservation_description = '" . $_POST['description'] ."', reservation_status = '" . $_POST['status'] ."' WHERE reservation_id = " . $_GET['product'] ."";
    mysql_query($sql);
    echo '<script type="text/javascript">window.location.href="index.php?page=reservation"</script>';
  }

  if (isset($_POST['delete_reservation'])) {            
    if($_POST['del_reservation']!=0 ) $reservation_error=0;
    else $reservation_error=1;
  }

  if (isset($_POST['delete_reservation']) and $reservation_error == 0) {     
    foreach ($_POST['del_reservation'] as $i) {
      $sql = "DELETE FROM `reservation` WHERE reservation_id = " . $i . "";
      mysql_query($sql);
    }
    echo '<script type="text/javascript">window.location.href="index.php?page=reservation"</script>';
  }

?>