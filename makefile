.PTHONY:clean

BIN=fastupload upload

all:$(BIN)

VPATH= ./src

CC=gcc

LIBFLAG= -L /home/itheima/lib -lmylog -lfcgi
INCLFLAG= -I ./incl
CFLAGS= -Wall -g -c

fastupload:fastupload.o
	$(CC) $< $(LIBFLAG) -o $@

upload:upload_cgi.o
	$(CC) $< $(LIBFLAG) -o $@

%.o:%.c
	$(CC) $^ $(CFLAGS) $(INCLFLAG) -o $@
clean:
	rm *.o
	rm $(BIN)

