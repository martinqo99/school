Autori: Michal Luk�� xlukac05, Michal Mur�� xmuran00

Popis:
Pokia� sa sp���a program cez qt creator, spustite program v Release Mode a server pustite v�dy s parametrom ur�uj�ci ��slo portu.
V�sledn� program sl��i na edit�ciu a simul�ciu Petriho siet�. V programe sa nach�dzaj� dva m�dy Simulation and Editing mode.
V programe je mo�n� meni� farby, tvary a p�smo prvkov v Petriho sieti.
Implicitne servej be�� na porte 7123 a obsahuje vytvoren� ��et login=admin a password=admin. �alej sa na servery nach�dzaj� 2 uk�kov� siete v nieko�k�ch verzi�ch.
Na lok�lnom ulo�isku v prie�inku examples sa nach�dza jedna uk�kov� sie� v nieko�k�ch verzi�ch.

Edit�cia:
V programe je mo�n� vytv�ra� a editova� Petriho siete. Sie� je mo�n� si ulo�i� lok�lne alebo na server.
Pri ka�dom spusten� programu je u�ivate� vyzvan� k pripojen� sa k serveru.
Pri vytv�ran� hrany Arc medzi miestom Place a prechodom Transition treba tieto prvky ozna�i�.
Edit�cia je mo�n� iba v Edit mode, pomocou action Edit alebo dvojklikom na grafick� item(place,transition,arc).
U hrany Arc je mo�n� editova� jej n�zov a smer.
U miesta Place sa edituje n�zov a tokeny.
U prechdou Transition sa d� nastavi� n�zov, podmienka a oper�cia.
U�ivate� m� mo�nos� si vytvori� ��et na servery. 
V�aka pr�stupu na server si m��e zobrazi� �daje o ulo�en�ch sie�ach a zvoli� si s�bor, ktor� si stiahne a automaticky ulo�� lok�lne a zobraz� v programe.

Simul�cia:
Pre simul�ciu si mus� u�ivate� stiahnu� s�bor zo serveru. 
S� dve mo�nosti simul�cie krokovanie a spustenie celej simul�cie.
Pri krokovan� si u�ivate� mus� zvoli� prechod, ktor� chce vyhodnoti�.
Pri krokovan� nie je mo�n� na klientovi sp���a� �al�ie simul�cie dok�m d�n� simul�cia nie je ukon�en� tla��tkom pre koniec simul�cie. Pre viac simul�ci� nar�z treba spusti� �al�ieho klienta.
Pri celkovej simul�ci� je nastaven� po�et iter�ci� na 25.