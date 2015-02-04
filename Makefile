build:
	gcc -std=c11 -D_BSD_SOURCE -o csv2fixedwidth ./main.c
debug:
	gcc -std=c11 -g -D_BSD_SOURCE -o csv2fixedwidth-dbg ./main.c
