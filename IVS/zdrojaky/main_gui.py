#!/usr/bin/python

## \mainpage
# Dokumentace k tretimu projektu do predmetu IVS - Prakticke aspekty vyvoje software. Cilem tohoto projektu bylo vytvorit jednoduchou kalkulacku v libovolnem jazyce, nas program je implementovan v jazyce Python.
# @author Peter Michalik (vedouci tymu) @author Michal Lukac @author Milan Seitler @author Filip Holec

## \file main_gui.py
# \brief Graficke rozhrani programu
#
# Soubor main_gui.py obsahuje funkce pro inicializaci grafickeho prostredi. K jeho zobrazeni je vyuzita knihovna
# PyQt4.


from PyQt4 import QtCore, QtGui
import sys
import re
from mathlib import *

# Functions of calculator

## @fn clearLine()
# Vyprazdnuje pole pro zadavani vstupnich hodnot
# @return Funkce nema navratovou hodnotu

def clearLine():
  statusBar.showMessage("Zmazat")
  mainLineEdit.setText("0")

## @fn setNum(number)
# Prida do textu ve vstupnim poli kalkulacky jeden znak
# @param number vstupni znak
# @return Funkce nema navratovou hodnotu

def setNum(number):
  global ans, value, operator, value1, value2, prev
  if number.isdigit() and mainLineEdit.text() == "0":
    mainLineEdit.setText(number)
  elif number == "-" and mainLineEdit.text() == "0":
    mainLineEdit.setText(number+mainLineEdit.text())
  else:
    mainLineEdit.setText(mainLineEdit.text()+number)

## @fn equalClick()
# Spocita vyraz ve vstupnim poli kalkulacky
# @return Funkce nema navratovou hodnotu
def equalClick():
  global ans, value, operator, value1, value2, prev
  statusBar.clearMessage()
  text = str(mainLineEdit.text())
  text=re.sub(r' ', r"",text)
  text=re.sub(r'^([0-9]+)([\+\-*/])',r"\1.0\2",text)
  text=re.sub(r'([\+\-*/])([0-9]+)([\+\-*/])',r"\1\2.0\3",text)
  try:
    mainLineEdit.setText(str(eval(text)))
  except SyntaxError:
    statusBar.showMessage("Zly vyraz!")
  except ZeroDivisionError:
    statusBar.showMessage("Delenie nulou!")

## @fn xna2Click()
# Vola z matematicke knihovny mathlib.py funkci pro vypocet druhe mocniny
# @return Funkce nema navratovou hodnotu
def xna2Click():
  statusBar.clearMessage()
  x = float(mainLineEdit.text())
  mainLineEdit.setText(str(xna2(x)))

## @fn sinusClick()
# Vola z matematicke knihovny mathlib.py funkci pro vypocet sinus
# @return Funkce nema navratovou hodnotu
def sinusClick():
  statusBar.clearMessage()
  number = float(mainLineEdit.text())
  mainLineEdit.setText(str(sinus(number)))


## @fn factorialClick()
# Vola z matematicke knihovny mathlib.py funkci pro vypocet faktorialu
# @return Funkce nema navratovou hodnotu
def factorialClick():
  statusBar.clearMessage()
  if str(mainLineEdit.text()).isdigit():
    number = float(mainLineEdit.text())
    mainLineEdit.setText(str(fact(number)))
  else:
    statusBar.showMessage("Zly vyraz!")

## @var app
# inicializace prostredi Qt
## @var MainWindow
# vytvoreni okna aplikace

# Setting up params of gui
app = QtGui.QApplication(sys.argv)
MainWindow = QtGui.QMainWindow()
MainWindow.setWindowTitle("Kalkulacka")
MainWindow.setFixedSize(400,250)

## @var helpBox
# obsahuje okno napovedy pro program

# Create helpBox
helpBox = QtGui.QMessageBox()
helpBox = QtGui.QMessageBox()
helpBox.setWindowTitle("Help")
helpBox.setText("<b>Help</b>")
helpBox.setInformativeText("<b>1 Instalacia</b><br>\
Pri instalacii produktu sa predpoklada Linuxovy operacny system a pritomnost frameworku Qt (verzia 4).<br>\
<b>1.1 Instalacia frameworku Qt</b><br>\
V pripade absencie frameworku Qt sa da pripadne velmi jednoducho nainstalovat :<br>\
Do prikazoveho riadku napiste :<br>\
 su  <br>\
 apt-get install Qt (pripadne) yum install Qt <br>\
<b>1.2 Instalacia programu</b><br>\
Program sa instaluje velmi jednoducho pomocou prikazoveho riadku (nastavi sa mu priznak spustitelnosti).<br>\
Do prikazoveho riadku napiste :<br>\
  chmod a+x /cesta/k/suboru/main_gui.py (napriklad) chmod a+x ./main_gui.py<br>\
<b>1.3 Spustenie programu</b><br>\
Program sa spusta zadanim prikazu do prikazoveho riadku.<br>\
Spustenie z lubovolneho miesta :<br>\
  /cesta/k/suboru/main_gui.py<br>\
V danom adresari<br>\
  ./main_gui.py<br><br>\
<b>2 Uzivatelske rozhranie</b><br>\
<b>2.1 Funkcie kalkulacky</b><br>\
Nasledujuce funkcie su dostupne v kalkulacke :<br>\
<div>+ - * / sin x^2 x!</div><br>\
<b>2.1 Funkcie kalkulacky</b><br>\
Kalkulacka sa ovlada intuitivne, pomocou mysi alebo klavesnice kliknutim na displej.<br>\
Tlacidlo CLR sluzi na zmazanie displeja.")

## @var aboutBox
# obsahuje okno s informacemi o programu

# Create aboutBox
aboutBox = QtGui.QMessageBox()
aboutBox.setWindowTitle("About")
aboutBox.setText("<b>Kalkulacka v1.0</b>")
aboutBox.setInformativeText("Autori :<br>Peter Michalik, Michal Lukac, Milan Seitler, Filip Holec")

## @var MenuBar
# obsahuje horizontalni menu
## @var menuFile
# zalozka v menu - soubor
## @var menuHelp
# zalozka v menu - napoveda

# Create MenuBar
MenuBar = QtGui.QMenuBar(MainWindow)
menuFile = QtGui.QMenu("Subor", MenuBar)
menuHelp = QtGui.QMenu("Pomoc", MenuBar)

## @var actionExit
# akce po stisknuti tlacitka Exit
## @var actionHelp
# akce po stisknuti tlacitka Help
## @var actionAbout
# akce po stisknuti tlacitka About

# Create actions
actionExit = QtGui.QAction("Koniec",menuFile)
actionHelp = QtGui.QAction("Pomocnik",menuHelp)
actionAbout = QtGui.QAction("O programe",menuHelp)

# Include actions to menu
menuFile.addAction(actionExit)
menuHelp.addAction(actionHelp)
menuHelp.addAction(actionAbout)

# Add menu to menuBar
MenuBar.addMenu(menuFile)
MenuBar.addMenu(menuHelp)
MainWindow.setMenuBar(MenuBar)

## @var statusBar
# obsahuje ukazatel stavu

# Create statusBar
statusBar = QtGui.QStatusBar(MainWindow)
statusBar.showMessage("Pripraveny")
MainWindow.setStatusBar(statusBar)

## @var MainWidget
# obsahuje prazdny "kontejner"
## @var layout
# layout grafickeho rozhrani

# Creating empty widget and add to it a Gridlayout
MainWidget = QtGui.QWidget(MainWindow)
MainWindow.setCentralWidget(MainWidget)
layout = QtGui.QGridLayout(MainWidget)

## @var Button1
# stisk tlacitka 1
## @var Button2
# stisk tlacitka 2
## @var Button3
# stisk tlacitka 3
## @var Button4
# stisk tlacitka 4
## @var Button5
# stisk tlacitka 5
## @var Button6
# stisk tlacitka 6
## @var Button7
# stisk tlacitka 7
## @var Button8
# stisk tlacitka 8
## @var Button9
# stisk tlacitka 9
## @var Button0
# stisk tlacitka 0
## @var ButtonEq
# stisk tlacitka =
## @var ButtonDiv
# stisk tlacitka /
## @var ButtonCom
# stisk tlacitka .
## @var ButtonPlus
# stisk tlacitka +
## @var ButtonMult
# stisk tlacitka *
## @var ButtonXn
# stisk tlacitka x ^ 2
## @var ButtonMinus
# stisk tlacitka -
## @var ButtonFact
# stisk tlacitka x!
## @var ButtonSin
# stisk tlacitka sin
## @var ButtonClr
# stisk tlacitka CLR

# Create buttons
Button1 = QtGui.QPushButton("1", MainWidget)
Button2 = QtGui.QPushButton("2", MainWidget)
Button3 = QtGui.QPushButton("3", MainWidget)
Button4 = QtGui.QPushButton("4", MainWidget)
Button5 = QtGui.QPushButton("5", MainWidget)
Button6 = QtGui.QPushButton("6", MainWidget)
Button7 = QtGui.QPushButton("7", MainWidget)
Button8 = QtGui.QPushButton("8", MainWidget)
Button9 = QtGui.QPushButton("9", MainWidget)
Button0 = QtGui.QPushButton("0", MainWidget)
ButtonEq = QtGui.QPushButton("=", MainWidget)
ButtonDiv = QtGui.QPushButton("/",MainWidget)
ButtonCom = QtGui.QPushButton(".",MainWidget)
ButtonPlus = QtGui.QPushButton("+", MainWidget)
ButtonMult = QtGui.QPushButton("*", MainWidget)
ButtonXn = QtGui.QPushButton("x^2", MainWidget)
ButtonMinus = QtGui.QPushButton("-",MainWidget)
ButtonFact = QtGui.QPushButton("x!",MainWidget)
ButtonSin = QtGui.QPushButton("SIN",MainWidget)
ButtonClr = QtGui.QPushButton("CLR",MainWidget)

## @var mainLineEdit
# vstupni pole

# Create EditLine
mainLineEdit = QtGui.QLineEdit("0",MainWidget)

# Adding line to grid and set settings
layout.addWidget(mainLineEdit,0,0,1,5)
mainLineEdit.setAlignment(QtCore.Qt.AlignRight)

# Adding buttons to grid
layout.addWidget(Button1, 1, 0, 1, 1)
layout.addWidget(Button2, 1, 1, 1, 1)
layout.addWidget(Button3, 1, 2, 1, 1)
layout.addWidget(Button4, 2, 0, 1, 1)
layout.addWidget(Button5, 2, 1, 1, 1)
layout.addWidget(Button6, 2, 2, 1, 1)
layout.addWidget(Button7, 3, 0, 1, 1)
layout.addWidget(Button8, 3, 1, 1, 1)
layout.addWidget(Button9, 3, 2, 1, 1)
layout.addWidget(ButtonEq, 1, 3, 1, 1)
layout.addWidget(ButtonCom, 2, 3, 1, 1)
layout.addWidget(Button0, 3, 3, 1, 1)
layout.addWidget(ButtonDiv, 4, 2, 1, 1)
layout.addWidget(ButtonXn, 4, 4, 1, 1)
layout.addWidget(ButtonSin, 3, 4, 1, 1)
layout.addWidget(ButtonClr, 1, 4, 1, 1)
layout.addWidget(ButtonFact,2,4, 1, 1)
layout.addWidget(ButtonMult, 4, 3, 1, 1)
layout.addWidget(ButtonPlus, 4, 0, 1 ,1)
layout.addWidget(ButtonMinus, 4, 1, 1, 1)

# Connect signals/actions of buttons with functions
app.connect(Button0, QtCore.SIGNAL("clicked()"), lambda b="0": setNum(b))
app.connect(Button1, QtCore.SIGNAL("clicked()"), lambda b="1": setNum(b))
app.connect(Button2, QtCore.SIGNAL("clicked()"), lambda b="2": setNum(b))
app.connect(Button3, QtCore.SIGNAL("clicked()"), lambda b="3": setNum(b))
app.connect(Button4, QtCore.SIGNAL("clicked()"), lambda b="4": setNum(b))
app.connect(Button5, QtCore.SIGNAL("clicked()"), lambda b="5": setNum(b))
app.connect(Button6, QtCore.SIGNAL("clicked()"), lambda b="6": setNum(b))
app.connect(Button7, QtCore.SIGNAL("clicked()"), lambda b="7": setNum(b))
app.connect(Button8, QtCore.SIGNAL("clicked()"), lambda b="8": setNum(b))
app.connect(Button9, QtCore.SIGNAL("clicked()"), lambda b="9": setNum(b))
app.connect(ButtonCom, QtCore.SIGNAL("clicked()"),lambda b=".":setNum(b))
app.connect(ButtonMinus, QtCore.SIGNAL("clicked()"), lambda b="-":setNum(b))
app.connect(ButtonDiv, QtCore.SIGNAL("clicked()"), lambda b="/":setNum(b))
app.connect(ButtonPlus, QtCore.SIGNAL("clicked()"), lambda b="+":setNum(b))
app.connect(ButtonMult, QtCore.SIGNAL("clicked()"), lambda b="*":setNum(b))
app.connect(ButtonClr, QtCore.SIGNAL("clicked()"), clearLine)
app.connect(ButtonEq, QtCore.SIGNAL("clicked()"), equalClick)
app.connect(ButtonSin, QtCore.SIGNAL("clicked()"), sinusClick)
app.connect(ButtonFact, QtCore.SIGNAL("clicked()"), factorialClick)
app.connect(ButtonXn, QtCore.SIGNAL("clicked()"), xna2Click)

# Connect signals from menu
app.connect(actionExit, QtCore.SIGNAL("triggered(bool)"), MainWindow.close)
app.connect(actionHelp, QtCore.SIGNAL("triggered(bool)"), helpBox.show)
app.connect(actionAbout, QtCore.SIGNAL("triggered(bool)"), aboutBox.show)

# Show main window
MainWindow.show()
sys.exit(app.exec_())
