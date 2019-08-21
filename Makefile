

all:
	gcc main.c -L/usr/X11R6/lib -lX11 -lm -o mandel
