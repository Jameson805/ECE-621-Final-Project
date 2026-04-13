CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

TARGET = sim

SRCS = main.c lattice.c noise.c syndrome.c decoder.c simulate.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)
