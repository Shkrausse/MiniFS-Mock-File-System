CC = gcc
CFLAGS = -Wall -Wextra -g
LIB_OBJS = minifs.0

# Build both executables
all: minifs_shell minifs_driver

# The Interactive Shell
minifs_shell: main.o $(LIB_OBJS)
	$(CC) $(CFLAGS) -o minifs_shell main.o $(LIB_OBJS)

# The Automatic Demo
minifs_driver: driver.o $(LIB_OBJS)
	$(CC) $(CFLAGS) -o minifs_driver driver.o $(LIB_OBJS)

# Library logic
minifs.o: minifs.c minifs.h
	$(CC) $(CFLAGS) -c minifs.c

# Object files for the specific programs
main.o: main.c minifs.h
	$(CC) $(CFLAGS) -c main.c

driver.o: driver.c minifs.h
	$(CC) $(CFLAGS) -c driver.c

clean:
	rm -f minifs_shell minifs_driver *.o disk.img
