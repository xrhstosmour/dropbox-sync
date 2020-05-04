OBJS = main.o commandParser.o clientUtils.o tupleList.o
SOURCE = main.c commandParser.c clientUtils.c tupleList.c
HEADER = main.h commandParser.h clientUtils.h tupleList.h
OUT = dropbox_client
CC = gcc
FLAGS = -g3 -c -Wall

all: $(OBJS)
	$(CC) -g3 $(OBJS) -o $(OUT) -lm -lpthread 

main.o: main.c 
	$(CC) $(FLAGS) main.c 

commandParser.o:commandParser.c
	$(CC) $(FLAGS) commandParser.c

clientUtils.o:clientUtils.c
	$(CC) $(FLAGS) clientUtils.c

tupleList.o:tupleList.c
	$(CC) $(FLAGS) tupleList.c

clean:
	rm -rf *.o *.gch $(OBJS)

valgrind:
	valgrind --leak-check=full --trace-children=yes --track-origins=yes --show-leak-kinds=all ./$(OUT) -d syncFolder -p clientPort -w workersNumber -b bufferSize -sp serverPort -sip serverIP

