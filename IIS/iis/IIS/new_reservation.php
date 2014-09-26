    <div class="node">
        <div class="content">
            <h2><span>Rezervace</span><br />Přidat novou</h2><span class="button"><a href="index.php?page=reservation">&laquo; Zpět na rezervace</a></span><form method="post" action="index.php?page=reservation_process&user=<?php echo $_SESSION['id'];?>">
                <fieldset>
                    <legend>Základní údaje</legend>
                    <div class="container">
                        <label class="text"><span>Jméno</span><input type="text" value="" name="name" /></label>
                        <label class="text"><span>Počet osob</span><input type="text" value="" name="seats" /></label>
                        <label class="text"><span>Rezervováno na</span><input type="text" value="" name="date" /></label>
                        <label class="text"><span>Popis</span><input type="text" value="" name="description" /></label>
                    </div>
                 </fieldset>
                 <input type="submit" name="new_reservation" value="Uložiť"/>
                 </form>        </div>
    </div>