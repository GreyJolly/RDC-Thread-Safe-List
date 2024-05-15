CC = gcc
EXEC_FILE = test
CFLAGS = -g
OBJECTS = test-suite.o thread-safe-list.o thread_pool.o

all: $(EXEC_FILE)

$(EXEC_FILE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $@ 

test-suite.o: test-suite.c thread-safe-list.h
	$(CC) $(CFLAGS) -c -o $@ test-suite.c
	
thread-safe-list.o: thread-safe-list.c thread-safe-list.h
	$(CC) $(CFLAGS) -c -o $@ thread-safe-list.c

thread_pool.o: thread_pool.c thread_pool.h
	$(CC) $(CFLAGS) -c -o $@ thread_pool.c
	
clean:
	rm -f $(EXEC_FILE) $(OBJECTS)