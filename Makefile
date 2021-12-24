OPTIONS=-Wall

htmd: htmd.o markdown.o outstream.o paragraph.o
	gcc htmd.o markdown.o outstream.o paragraph.o -o htmd ${OPTIONS}

htmd.o: htmd.c
	gcc -c htmd.c ${OPTIONS}

markdown.o: markdown.c
	gcc -c markdown.c ${OPTIONS}

outstream.o: outstream.c
	gcc -c outstream.c ${OPTIONS}

paragraph.o: paragraph.c
	gcc -c paragraph.c ${OPTIONS}

clean:
	rm -f *.o htmd
