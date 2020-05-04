OBJS = main.o commandParser.o tupleList.o serverUtils.o
SOURCE = main.c commandParser.c tupleList.c serverUtils.c
HEADER = main.h commandParser.h tupleList.h serverUtils.h
OUT = dropbox_server
CC = gcc
FLAGS = -g3 -c -Wall

all: $(OBJS)
	$(CC) -g3 $(OBJS) -o $(OUT) -lm -lpthread 

main.o: main.c 
	$(CC) $(FLAGS) main.c 

commandParser.o:commandParser.c
	$(CC) $(FLAGS) commandParser.c

tupleList.o:tupleList.c
	$(CC) $(FLAGS) tupleList.c

serverUtils.o:serverUtils.c
	$(CC) $(FLAGS) serverUtils.c

clean:
	rm -rf *.o *.gch $(OBJS)

valgrind:
	valgrind --leak-check=full --trace-children=yes --track-origins=yes --show-leak-kinds=all ./$(OUT) -p serverPort
