<?php
  if (isset($_POST['new_product'])) {            
    if($_POST['name']!="") $name_error=0;
    else $name_error=1;

    if($_POST['price']!=0) $price_error=0;
    else $price_error=1;

    if($_POST['description']!="") $description_error=0;
    else $description_error=1;
  }

  if (isset($_POST['new_product']) and $name_error==0 and $price_error==0 and $description_error==0) {        
    $sql = "INSERT INTO `product` (`product_id`, `product_name`, `product_price`, `product_description`) VALUES (NULL, '" . $_POST['name'] . "', " . $_POST['price'] . ", '" . $_POST['description'] . "')";
    mysql_query($sql);
    echo '<script type="text/javascript">window.location.href="index.php?page=product"</script>';
  }

  if (isset($_POST['update_product'])) {            
    if($_POST['name']!="") $name_error=0;
    else $name_error=1;

    if($_POST['price']!=0) $price_error=0;
    else $price_error=1;

    if($_POST['description']!="") $description_error=0;
      else $description_error=1;
  }

  if (isset($_POST['update_product']) and $name_error==0 and $price_error==0 and $description_error==0) {        
    $sql = "UPDATE `product` SET product_name = '" .$_POST['name']. "', product_price = " . $_POST['price'] .", product_description = '" . $_POST['description'] . "' WHERE product_id = " . $_GET['product'] ."";
    mysql_query($sql);
    echo '<script type="text/javascript">window.location.href="index.php?page=product"</script>';
  }

  if (isset($_POST['delete_product'])) {            
    if($_POST['del_product']!=0 ) $product_error=0;
    else $product_error=1;
  }

  if (isset($_POST['delete_product']) and $product_error == 0) {     
    foreach ($_POST['del_product'] as $i) {
      $sql = "DELETE FROM `product` WHERE product_id = " . $i . "";
      mysql_query($sql);
    }
    echo '<script type="text/javascript">window.location.href="index.php?page=product"</script>';
  }
?>