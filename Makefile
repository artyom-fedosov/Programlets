CC = gcc
CFLAGS = -Wall
LDLIBS = -lcrypt

.PHONY: all clean

all: check_password intexec

check_password: check_password.o
	$(CC) check_password.o $(LDLIBS) -o check_password

check_password.o: check_password.c
	$(CC) $(CFLAGS) -c check_password.c -o check_password.o

intexec: command_interval_execution.o
	$(CC) command_interval_execution.o -o intexec

command_interval_execution.o: command_interval_execution.c
	$(CC) $(CFLAGS) -c command_interval_execution.c -o command_interval_execution.o

clean:
	rm -f check_password check_password.o intexec command_interval_execution.o
