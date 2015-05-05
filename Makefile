#
# Makfile
#

LIBS = -lmingw32 -lgdi32

all: compile link

compile: main.c
	gcc -o main.o -c main.c $(CFLAGS)
	gcc -o process.o -c process.c $(CFLAGS)
	gcc -o outsider.o -c outsider.c $(CFLAGS)
link: main.o
	gcc -o main.exe process.o main.o outsider.o $(LDFLAGS) $(LIBS)
