CC = gcc
CFLAGS = -O3
SRCS = host.c
OBJS = $(SRCS:.c=.o)
LIBS = -lusb
MAIN = main

all: $(MAIN)
	@echo main has been compiled
$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJS) -o $(MAIN)
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -rf *.o $(MAIN)
