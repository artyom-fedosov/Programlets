CC = gcc
CFLAGS = -Wall
LDLIBS = -lcrypt

.PHONY: all clean

all: check_password

check_password: check_password.o
	$(CC) check_password.o $(LDLIBS) -o check_password

check_password.o: check_password.c
	$(CC) $(CFLAGS) -c check_password.c -o check_password.o

clean:
	rm -f check_password check_password.o
