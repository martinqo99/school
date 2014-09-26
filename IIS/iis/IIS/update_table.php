    <div class="node">
        <div class="content">
            <h2><span>Stolování</span><br />Upravit stůl</h2><span class="button"><a href="index.php?page=table">&laquo; Zpět na stolování</a></span><form method="post" action="index.php?page=table_process&product=<?php echo $_GET['product'];?>">
                <fieldset>
                    <legend>Základní údaje</legend>
                    <div class="container">
                        <label class="text"><span>Místnost</span><input type="text" value="" name="room" /></label>
                        <label class="text"><span>Popis</span><input type="text" value="" name="description" /></label>
                        <label class="text"><span>Počet míst k sezení</span><input type="text" value="" name="seats" /></label>
                    </div>
                 </fieldset>
                 <input type="submit" name="update_table" value="Uložiť"/>
                 </form>        </div>
    </div>