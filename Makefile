#CC = arm-xilinx-linux-gnueabi-gcc
#CC = gcc
CC = aarch64-linux-gnu-gcc
#CFLAGS = -I./include -g
CFLAGS = -I./include -I./ -g
TARGET = recordCpuMem.bin
SOURCES = $(wildcard *.c)
OBJS = $(patsubst %.c,%.o,$(SOURCES))
all:$(OBJS)
	$(CC) $(OBJS) -o $(TARGET)
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@	 

.PHONY: clean

clean:
	rm -rf *.o $(TARGET)
