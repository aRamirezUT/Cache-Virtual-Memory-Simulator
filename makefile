CC = gcc
CFLAGS = -Wall -Wextra -pthread -g
OBJS = main.o 
# OBJS = file_read.o cpu_scheduler.o io_system.o main.o
HDRS = main.h
TARGET = prog

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

main.o: main.c $(HDRS)
	$(CC) $(CFLAGS) -c main.c

# queue.o: queue.c $(HDRS)
# 	$(CC) $(CFLAGS) -c queue.c

# cpu_scheduler.o: cpu_scheduler.c $(HDRS)
# 	$(CC) $(CFLAGS) -c cpu_scheduler.c

# io_system.o: io_system.c $(HDRS)
# 	$(CC) $(CFLAGS) -c io_system.c

clean:
	rm -f $(OBJS) $(TARGET)
