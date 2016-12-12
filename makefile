.PTHONY:clean

all:upload

VPATH= ./src

CC=gcc

LIBFLAG= -L /home/itheima/lib -lmylog
INCLFLAG= -I ./incl
CFLAGS= -Wall -g -c

upload:upload.o
	$(CC) $< $(LIBFLAG) -o $@

%.o:%.c
	$(CC) $^ $(CFLAGS) $(INCLFLAG) -o $@
clean:
	rm *.o
	rm upload

