###############################################
#
#        Makefile pro dokumentaci IFJ
#
###############################################



FILE=slidy

$(FILE): $(FILE).tex
	vlna $(FILE).tex
	latex $(FILE).tex
	latex $(FILE).tex
	dvips -t a4 $(FILE).dvi
	ps2pdf $(FILE).ps
clean:
	rm -f *.dvi *.ps *.aux *.out *.log *.bbl *.blg *.te~

all: $(FILE) clean