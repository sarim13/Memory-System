CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGET = memorysystem
SRCS = memorysystem.c
OBJS = $(SRCS:.c=.o)

.PHONY: all compile build clean

all: build

compile: $(OBJS)

build: compile
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
