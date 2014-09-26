    <div class="node">
        <div class="content">
            <h2><span>Objednávky</span><br />Přidat novou</h2><span class="button"><a href="index.php?page=order">&laquo; Zpět na objednávky</a></span><form method="post" action="index.php?page=order_process">
                <fieldset>
                    <legend>Základní údaje</legend>
                    <div class="container">
                        <label class="text"><span>Stůl č.</span>
                            <select name="table">
                                <option value="0"></option>
                                <?php
                                $sql = mysql_query("SELECT * FROM `table`");
                                while ($row = mysql_fetch_array($sql)) {
                                    echo '<option value="' . $row['table_id'] . '">' . $row['table_id'] . '</option>';
                                }
                                ?>
                            </select>
                        <label class="text"><span>Rezervace</span>
                            <select name="reservation">
                                <option value="0"></option>
                                <?php
                                $sql = mysql_query("SELECT * FROM `reservation`");
                                while ($row = mysql_fetch_array($sql)) {
                                    echo '<option value="' . $row['reservation_id'] . '">' . $row['reservation_name'] . '</option>';
                                }
                                ?>
                            </select>
                    </div>
                 </fieldset>
                 <input type="submit" name="new_order" value="Uložiť"/>
                 </form>        </div>
    </div>