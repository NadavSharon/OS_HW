CC = gcc
CFLAGS = -Wall -g

#Create the static library only
all: libscheduler.a

#Create the static library from object files
libscheduler.a: scheduler.o
	ar rcs libscheduler.a scheduler.o

#Compile source code into object file
scheduler.o: scheduler.c scheduler.h process.h
	$(CC) $(CFLAGS) -c scheduler.c -o scheduler.o

#clean up temporary files
clean:
	rm -f *.o *.a