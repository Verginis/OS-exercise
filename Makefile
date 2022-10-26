OBJS =askisi1.o
SOURCE = askisi1.c
HEADER = askisi1.h 
OUT = ask1
CC = gcc
FLAGS = -g -c 


all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) -lpthread

ask1.o: askisi1.c
	$(CC) $(FLAGS) askisi1.c


clean:
	rm -f $(OBJS) $(OUT)
