CC = gcc
CFLAGS = -Wall -Wextra -pthread -g
LDFLAGS = -lm
OBJS = main.o data.o queue.o simulation.o cache.o
HDRS = main.h
TARGET = sim

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
