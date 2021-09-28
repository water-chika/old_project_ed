DESTDIR=
PREFIX=$$HOME/.local/bin
ed : ed.c
	gcc -g ed.c -o ed

install: ed
	install ed ${DESTDIR}${PREFIX}

