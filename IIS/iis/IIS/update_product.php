    <div class="node">
        <div class="content">
            <h2><span>Produkty</span><br />Upravit produkt</h2><span class="button"><a href="index.php?page=product">&laquo; Zpět na produkty</a></span><form method="post" action="index.php?page=product_process&product=<?php echo $_GET['product'];?>">
                <fieldset>
                    <legend>Základní údaje</legend>
                    <div class="container">
                        <label class="text"><span>Název</span><input type="text" value="" name="name" /></label>
                        <label class="text"><span>Cena</span><input type="text" value="" name="price" /></label>
                        <label class="text"><span>Popis</span><input type="text" value="" name="description" /></label>
                    </div>
                 </fieldset>
                 <input type="submit" name="update_product" value="Uložiť"/>
                 </form>        </div>
    </div>