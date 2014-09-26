-- Autor: xlukac05@stud.fit.vutbr.cz, xloffa00@stud.fit.vutbr.cz
-- Doplnkovy skript pre projekt do predmetu IDS 2011/2012 na VUT FIT.
---------------------------------------------------------------------

-- Prava pre dalsiu osobu
--GRANT ALL ON Zamestnanec TO xlukac05;
--GRANT ALL ON Uctenka TO xlukac05;
--GRANT ALL ON Rezervacia TO xlukac05;
--GRANT ALL ON Rezervacia_Stol TO xlukac05;
--GRANT ALL ON Objednavka_Produkt TO xlukac05;
--GRANT ALL ON Objednavka TO xlukac05;
--GRANT ALL ON Stol TO xlukac05;
--GRANT ALL ON Produkt TO xlukac05;

-- Vytvor synonyma
CREATE OR REPLACE SYNONYM Zamestnanec FOR xloffa00.Zamestnanec;
CREATE OR REPLACE SYNONYM Uctenka FOR xloffa00.Uctenka;
CREATE OR REPLACE SYNONYM Rezervacia FOR xloffa00.Rezervacia;
CREATE OR REPLACE SYNONYM Rezervacia_Stol FOR xloffa00.Rezervacia_Stol;
CREATE OR REPLACE SYNONYM Objednavka_Produkt FOR xloffa00.Objednavka_Produkt;
CREATE OR REPLACE SYNONYM Objednavka FOR xloffa00.Objednavka;
CREATE OR REPLACE SYNONYM Stol FOR xloffa00.Stol;
CREATE OR REPLACE SYNONYM Produkt FOR xloffa00.Produkt;

-- Vytvorenie procedury pre vlozenie zamestnanca
CREATE OR REPLACE PROCEDURE vloz_zamestnanca (rc CHAR, me VARCHAR2, pr VARCHAR2, fu VARCHAR2, tc VARCHAR2) AS
BEGIN
   INSERT INTO Zamestnanec (rodne_cislo,meno,priezvysko,funkcia,tel_cislo) VALUES(rc,me,pr,fu,tc);
END vloz_zamestnanca;
/

-- Vyvorenie procedury pre zmazanie zamestnanca
CREATE OR REPLACE PROCEDURE zmaz_zamestnanec (id1 CHAR) AS
BEGIN
   DELETE FROM Zamestnanec WHERE Zamestnanec.rodne_cislo=id1;
END zmaz_zamestnanec;
/

-- Vytvorenie triggeru zmazanie zamestnanca
CREATE OR REPLACE TRIGGER zmaz_ucetzamestnanec
BEFORE DELETE
ON Zamestnanec FOR EACH ROW
BEGIN
DELETE FROM Uctenka
WHERE Uctenka.zamestnanec=:old.rodne_cislo;
DELETE FROM Objednavka
WHERE Objednavka.zamestnanec=:old.rodne_cislo;
DELETE FROM Rezervacia
WHERE Rezervacia.zamestnanec=:old.rodne_cislo;
END;
/

COMMIT;
-- Vytvor index vyhladavanie jedal pomocou LIKE
CREATE INDEX indmeno ON Produkt (nazov);
COMMIT;