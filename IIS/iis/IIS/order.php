    <div class="node">
        <div class="content">
            <h2><span>Objednávky</span><br />Přehled</h2><span class="add"><a href="index.php?page=new_order">+ Přidat novou objednávku</a></span><form action="index.php?page=order_process" method="post">
            <?php 
                $counter=0;
                $sql = mysql_query("SELECT * FROM `order` o, `user` u, `table` t, `reservation` r WHERE o.order_user = u.user_id AND o.order_table = t.table_id AND o.order_reservation = r.reservation_id"); 
                if (mysql_num_rows($sql) == 0) {
                    echo "Žádná objednávka";
                }
                else {
            ?>
            <table cellpadding="0" cellspacing="0">
                    <thead>
                            <tr><td>&nbsp;</td><td>Přijal</td><td>Rezervace</td><td>Stůl</td><td>Datum</td><td>Počet osob</td><td>&nbsp;</td></tr>
                    </thead>
                    <tbody>
                        <?php
                        while ($row = mysql_fetch_array($sql)) {
                            $counter+=1;
                        echo "<tr><td><input type=\"checkbox\" value=" . $row['order_id'] ." name=\"del_order[]\" /></td>";
                        echo "<td>" . $row['user_name'] . " " .$row['user_surname'] . "</td>";
                        echo "<td>" . $row['reservation_name'] . "</td>";
                        echo "<td>" . $row['table_description'] . "</td>";
                        echo "<td>" . $row['reservation_dateCom'] . "</td>";
                        echo "<td>" . $row['reservation_persons'] . "</td>";
                        echo "<td><a href=\"index.php?page=update_order&product=" . $row['order_id'] . "\">Upravit</a></td>";
                        echo "</tr>";
                        }
                        ?>
                    </tbody>
                    <tfoot>
                        <tr>
                            <td colspan="5"><input type="submit" name="delete_order" value="Vymazat označené" /></td>
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
