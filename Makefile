OPTIONS=-Wall

htmd: htmd.o markdown.o outstream.o misc.o
	gcc htmd.o markdown.o outstream.o misc.o -o htmd ${OPTIONS}

htmd.o: htmd.c
	gcc -c htmd.c ${OPTIONS}

markdown.o: markdown.c
	gcc -c markdown.c ${OPTIONS}

outstream.o: outstream.c
	gcc -c outstream.c ${OPTIONS}

misc.o: misc.c
	gcc -c misc.c ${OPTIONS}

clean:
	rm -f *.o htmd
