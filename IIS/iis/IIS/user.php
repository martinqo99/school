    <div class="node">
        <div class="content">
            <h2><span>Zaměstnanci</span><br />Přehled</h2><span class="add"><a href="index.php?page=new_user">+ Přidat nového zaměstnance</a></span><form action="index.php?page=user_process" method="post">
            <?php 
                $counter=0;
                $sql = mysql_query("SELECT * FROM user"); 
                if (mysql_num_rows($sql) == 0) {
                    echo "Žádná rezervace";
                }
                else {
            ?>
            <table cellpadding="0" cellspacing="0">
                    <thead>
                            <tr><td>&nbsp;</td><td>Jméno</td><td>Příjmení</td><td>Login</td><td>Funkce</td><td>Telefonní číslo</td><td>&nbsp;</td></tr>
                    </thead>
                    <tbody>
                        <?php
                        while ($row = mysql_fetch_array($sql)) {
                            $counter+=1;
                        echo "<tr><td><input type=\"checkbox\" value=" . $row['user_id'] ." name=\"del_user[]\" /></td>";
                        echo "<td>" . $row['user_name'] . "</td>";
                        echo "<td>" . $row['user_surname'] . "</td>";
                        echo "<td>" . $row['user_login'] . "</td>";
                        echo "<td>" . $row['user_function'] . "</td>";
                        echo "<td>" . $row['user_phone'] . "</td>";
                        echo "<td><a href=\"index.php?page=update_user&product=" . $row['user_id'] . "\">Upravit</a></td>";
                        echo "</tr>";
                        }
                        ?>
                    </tbody>
                    <tfoot>
                        <tr>
                            <td colspan="5"><input type="submit" name="delete_user" value="Vymazat označené" /></td>
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
