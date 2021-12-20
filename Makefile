OPTIONS=-Wall

htmd: htmd.o markdown.o outstream.o misc.o
	gcc htmd.o markdown.o outstream.o misc.o -o htmd ${OPTIONS}

htmd.o: htmd.c htmd.h markdown.h misc.h
	gcc -c htmd.c ${OPTIONS}

markdown.o: markdown.c htmd.h markdown.h
	gcc -c markdown.c ${OPTIONS}

outstream.o: outstream.c outstream.h
	gcc -c outstream.c ${OPTIONS}

misc.o:
	gcc -c misc.c ${OPTIONS}

clean:
	rm -f *.o htmd
