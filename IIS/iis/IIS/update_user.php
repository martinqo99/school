    <div class="node">
        <div class="content">
            <h2><span>Zaměstnanci</span><br />Upravit zaměstnace</h2><span class="button"><a href="index.php?page=user">&laquo; Zpět na zaměstnance</a></span><form method="post" action="index.php?page=user_process&product=<?php echo $_GET['product'];?>">
                <fieldset>
                    <legend>Základní údaje</legend>
                    <div class="container">
                        <label class="text"><span>Jméno</span><input type="text" value="" name="name" /></label>
                        <label class="text"><span>Příjmení</span><input type="text" value="" name="surname" /></label>
                        <label class="text"><span>Login</span><input type="text" value="" name="login" /></label>
                        <label class="text"><span>Heslo</span><input type="text" value="" name="password" /></label>
                        <label class="text"><span>Funkce</span><input type="text" value="" name="function" /></label>
                        <label class="text"><span>Telefon</span><input type="text" value="" name="phone" /></label>
                    </div>
                 </fieldset>
                 <input type="submit" name="update_user" value="Uložiť"/>
                 </form>        </div>
    </div>