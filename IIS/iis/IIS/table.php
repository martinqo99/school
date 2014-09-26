    <div class="node">
        <div class="content">
            <h2><span>Stolování</span><br />Přehled</h2><span class="add"><a href="index.php?page=new_table">+ Přidat nový stůl</a></span><form action="index.php?page=table_process" method="post">
            <?php 
                $counter=0;
                $sql = mysql_query("SELECT * FROM `table`"); 
                if (mysql_num_rows($sql) == 0) {
                    echo "Žádná stůl";
                }
                else {
            ?>
            <table cellpadding="0" cellspacing="0">
                    <thead>
                            <tr><td>&nbsp;</td><td>Místnost</td><td>Popis</td><td>Počet míst</td><td>&nbsp;</td></tr>
                    </thead>
                    <tbody>
                        <?php
                        while ($row = mysql_fetch_array($sql)) {
                            $counter+=1;
                        echo "<tr><td><input type=\"checkbox\" value=" . $row['table_id'] ." name=\"del_table[]\" /></td>";
                        echo "<td>" . $row['table_room'] . "</td>";
                        echo "<td>" . $row['table_description'] . "</td>";
                        echo "<td>" . $row['table_seats'] . "</td>";
                        echo "<td><a href=\"index.php?page=update_table&product=" . $row['table_id'] . "\">Upravit</a></td>";
                        echo "</tr>";
                        }
                        ?>
                    </tbody>
                    <tfoot>
                        <tr>
                            <td colspan="5"><input type="submit" name="delete_table" value="Vymazat označené" /></td>
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
