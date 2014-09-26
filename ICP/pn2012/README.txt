Autori: Michal Lukáè xlukac05, Michal Muráò xmuran00

Popis:
Pokia¾ sa spúša program cez qt creator, spustite program v Release Mode a server pustite vdy s parametrom urèujúci èíslo portu.
Vıslednı program slúi na editáciu a simuláciu Petriho sietí. V programe sa nachádzajú dva módy Simulation and Editing mode.
V programe je moné meni farby, tvary a písmo prvkov v Petriho sieti.
Implicitne servej beí na porte 7123 a obsahuje vytvorenı úèet login=admin a password=admin. Ïalej sa na servery nachádzajú 2 ukákové siete v nieko¾kıch verziách.
Na lokálnom uloisku v prieèinku examples sa nachádza jedna ukáková sie v nieko¾kıch verziách.

Editácia:
V programe je moné vytvára a editova Petriho siete. Sie je moné si uloi lokálne alebo na server.
Pri kadom spustení programu je uivate¾ vyzvanı k pripojení sa k serveru.
Pri vytváraní hrany Arc medzi miestom Place a prechodom Transition treba tieto prvky oznaèi.
Editácia je moná iba v Edit mode, pomocou action Edit alebo dvojklikom na grafickı item(place,transition,arc).
U hrany Arc je moné editova jej názov a smer.
U miesta Place sa edituje názov a tokeny.
U prechdou Transition sa dá nastavi názov, podmienka a operácia.
Uivate¾ má monos si vytvori úèet na servery. 
Vïaka prístupu na server si môe zobrazi údaje o uloenıch sieach a zvoli si súbor, ktorı si stiahne a automaticky uloí lokálne a zobrazí v programe.

Simulácia:
Pre simuláciu si musí uivate¾ stiahnu súbor zo serveru. 
Sú dve monosti simulácie krokovanie a spustenie celej simulácie.
Pri krokovaní si uivate¾ musí zvoli prechod, ktorı chce vyhodnoti.
Pri krokovaní nie je moné na klientovi spúša ïalšie simulácie dokım dáná simulácia nie je ukonèená tlaèítkom pre koniec simulácie. Pre viac simulácií naráz treba spusti ïalšieho klienta.
Pri celkovej simulácií je nastavení poèet iterácií na 25.