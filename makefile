CC = gcc
CFLAGS = -Wall -Wextra -pthread -g
LDFLAGS = -lm
OBJS = main.o 
HDRS = main.h
TARGET = cache_simulator

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

main.o: main.c $(HDRS)
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f $(OBJS) $(TARGET)
