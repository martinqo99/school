## \file mathlib.py
# \brief Matematicka knihovna
#
# Soubor mathlib.py je vlastni matematicka knihovna obsahujici funkce pro vypocet slozitejsich matematickych operaci.

## @fn xna2(x)
# Vypocita druhou mocninu cisla
# @param x vstupni cislo
# @return Funkce vraci druhou mocninu zadaneho cisla
def xna2(x):
  return x*x


## @fn fact(x)
# Vypocita faktorial cisla
# @param x vstupni cislo
# @return Funkce vraci vypocitany faktorial zadaneho cisla
def fact(x):
  if x > 1:
    x = x * fact(x-1)
  return x

## @fn sinus(x)
# Vypocita sinus cisla (uhlu)
# @param x vstupni cislo (uhel)
# @return Funkce vraci vypocitany sinus zadaneho cisla
def sinus(x):
  pi = 3.141592653589
  while x >= 2*pi:
    x = x-(2*pi)
  item = x
  suma = x
  k = 1
  while abs(item) >= 0.00001:
    k += 2
    item = -1*(item*x*x)/(k*(k-1))
    suma += item
  return suma