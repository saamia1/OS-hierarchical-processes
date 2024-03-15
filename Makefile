CC = gcc
TARGET1 = primes
TARGET2 = prime

OBJS1 = main.o worker.o delegator.o
OBJS2 = prime.o

# Build both executables
all: $(TARGET1) $(TARGET2)

$(TARGET1): $(OBJS1)
	$(CC) -o $(TARGET1) $(OBJS1)

$(TARGET2): $(OBJS2)
	$(CC) -o $(TARGET2) $(OBJS2)

# Object file rules
main3.o: main.c worker.h delegator.h 
	$(CC) -c main.c

worker.o: worker.c worker.h
	$(CC) -c worker.c

delegator.o: delegator.c delegator.h
	$(CC) -c delegator.c

prime.o: prime.c
	$(CC) -c prime.c

# Clean up
clean:
	rm -f $(OBJS1) $(OBJS2) $(TARGET1) $(TARGET2)
