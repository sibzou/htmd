OPTIONS=-Wall

htmd: htmd.o markdown.o
	gcc htmd.o markdown.o -o htmd ${OPTIONS}

htmd.o: htmd.c htmd.h markdown.h
	gcc -c htmd.c ${OPTIONS}

markdown.o: markdown.c htmd.h markdown.h
	gcc -c markdown.c ${OPTIONS}
