OBJS	= main.o field.o interface.o
SOURCE	= main.c field.c interface.c
HEADER	=
OUT	= mineswp
CC	= gcc
FLAGS	= -g -std=c99 -c -Wall -D_XOPEN_SOURCE=500
LFLAGS	=

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

%.o: %.c
	$(CC) $(FLAGS) $^

clean:
	rm -f $(OBJS) $(OUT)

