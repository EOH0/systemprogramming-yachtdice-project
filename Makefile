# Makefile for YachtDice

CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lncurses

TARGET = game
SRCS = game.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)

readme:
	@cat README.md

deps:
	sudo apt-get update

.PHONY: all clean readme deps