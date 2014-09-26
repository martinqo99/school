<?php
    if (isset($_POST['new_table'])) {
    if($_POST['room']!="") $room_error=0; 
    else $room_error=1;

    if($_POST['seats']!=0) $seats_error=0;
    else $seats_error=1;

    if($_POST['description']!="") $description_error=0;
    else $description_error=1;
  }
  
  if (isset($_POST['new_table']) and $room_error==0 and $seats_error==0 and $description_error==0) {       
    echo $_POST['room'];
    echo $_POST['seats'];
    echo $_POST['description'];
    $sql = "INSERT INTO `table` (`table_id`, `table_room`, `table_seats`, `table_description`) VALUES (NULL, '" . $_POST['room'] . "', " . $_POST['seats'] . ", '" . $_POST['description'] . "')";
    mysql_query($sql);
    echo '<script type="text/javascript">window.location.href="index.php?page=table"</script>';
  }

  if (isset($_POST['update_table'])) {
    if($_POST['room']!="") $room_error=0; 
    else $room_error=1;

    if($_POST['seats']!=0) $seats_error=0;
    else $seats_error=1;

    if($_POST['description']!="") $description_error=0;
    else $description_error=1;
  }

  if (isset($_POST['update_table']) and $room_error==0 and $seats_error==0 and $description_error==0) {                
    $sql = "UPDATE `table` SET table_room = '" .$_POST['room']. "', table_seats = " . $_POST['seats'] .", table_description = '" . $_POST['description'] . "' WHERE table_id = " . $_GET['product'] ."";
    mysql_query($sql);
    echo '<script type="text/javascript">window.location.href="index.php?page=table"</script>';
  }


  if (isset($_POST['delete_table'])) {            
    if($_POST['del_table']!=0 ) $table_error=0;
    else $table_error=1;
  }

  if (isset($_POST['delete_table']) and $table_error == 0) {     
    foreach ($_POST['del_table'] as $i) {
      $sql = "DELETE FROM `table` WHERE table_id = " . $i . "";
      mysql_query($sql);
    }
    echo '<script type="text/javascript">window.location.href="index.php?page=table"</script>';
  }
?>