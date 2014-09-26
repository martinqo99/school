QM=/usr/local/share/Qt-4.7.3/bin/qmake
all:
	$(QM) ./src/pn2012/pn2012.pro -o ./src/pn2012/Makefile
	make -C ./src/pn2012
	$(QM) ./src/server2012/server2012.pro -o ./src/server2012/Makefile
	make -C ./src/server2012
clean:
	make -C ./src/pn2012 clean
	make -C ./src/server2012 clean
	rm ./src/pn2012/Makefile
	rm ./src/server2012/Makefile
	rm ./src/server2012/server2012
	rm ./src/pn2012/pn2012
	rm *tar.gz
pack:
	tar -czf xmuran00.tar.gz ./

doxygen:
	doxygen src/Doxyfile

run:
	./src/pn2012/pn2012

runserver:
	./src/server2012/server2012 7123
