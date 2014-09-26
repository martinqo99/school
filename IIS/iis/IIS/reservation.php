    <div class="node">
        <div class="content">
            <h2><span>Rezervace</span><br />Přehled</h2><span class="add"><a href="index.php?page=new_reservation">+ Přidat novou rezervaci</a></span><form action="index.php?page=reservation_process" method="post">
            <?php 
                $counter=0;
                $sql = mysql_query("SELECT * FROM reservation"); 
                if (mysql_num_rows($sql) == 0) {
                    echo "Žádná rezervace";
                }
                else {
            ?>
            <table cellpadding="0" cellspacing="0">
                    <thead>
                            <tr><td>&nbsp;</td><td>Název</td><td>Počet osob</td><td>Datum rezervace</td><td>Rezervováno na</td><td>Popis</td><td>Stav</td><td>&nbsp;</td></tr>
                    </thead>
                    <tbody>
                        <?php
                        while ($row = mysql_fetch_array($sql)) {
                            $counter+=1;
                        echo "<tr><td><input type=\"checkbox\" value=" . $row['reservation_id'] ." name=\"del_reservation[]\" /></td>";
                        echo "<td>" . $row['reservation_name'] . "</td>";
                        echo "<td>" . $row['reservation_persons'] . "</td>";
                        echo "<td>" . $row['reservation_dateRes'] . "</td>";
                        echo "<td>" . $row['reservation_dateCom'] . "</td>";
                        echo "<td>" . $row['reservation_description'] . "</td>";
                        echo "<td>" . $row['reservation_status'] . "</td>";
                        echo "<td><a href=\"index.php?page=update_reservation&product=" . $row['reservation_id'] . "\">Upravit</a></td>";
                        echo "</tr>";
                        }
                        ?>
                    </tbody>
                    <tfoot>
                        <tr>
                            <td colspan="5"><input type="submit" name="delete_reservation" value="Vymazat označené" /></td>
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
