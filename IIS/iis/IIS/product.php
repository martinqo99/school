    <div class="node">
        <div class="content">
            <h2><span>Produkty</span><br />Přehled</h2><span class="add"><a href="index.php?page=new_product">+ Přidat nový produkt</a></span><form action="index.php?page=product_process" method="post">
            <?php 
                $counter=0;
                $sql = mysql_query("SELECT * FROM product"); 
                if (mysql_num_rows($sql) == 0) {
                    echo "Žádný produkt";
                }
                else {
            ?>
            <table cellpadding="0" cellspacing="0">
                    <thead>
                            <tr><td>&nbsp;</td><td>Název</td><td>Cena</td><td>Popis</td><td>&nbsp;</td></tr>
                    </thead>
                    <tbody>
                        <?php
                        while ($row = mysql_fetch_array($sql)) {
                            $counter+=1;
                        echo "<tr><td><input type=\"checkbox\" value=" . $row['product_id'] ." name=\"del_product[]\" /></td>";
                        echo "<td>" . $row['product_name'] . "</td>";
                        echo "<td>" . $row['product_price'] . "</td>";
                        echo "<td>" . $row['product_description'] . "</td>";
                        echo "<td><a href=\"index.php?page=update_product&product=" . $row['product_id'] . "\">Upravit</a></td>";
                        echo "</tr>";
                        }
                        ?>
                    </tbody>
                    <tfoot>
                        <tr>
                            <td colspan="5"><input type="submit" name="delete_product" value="Vymazat označené" /></td>
                        </tr>
                    </tfoot>
                </table>
                </form>
                <ul class="pages">
                    <li class="previous">&nbsp;</li>
                    <li class="next">&nbsp;</li>
                </ul> 
                <?php } ?>       
            </div>
    </div>
