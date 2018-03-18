WSUCrypt: thread.o block.o key.o
	gcc -lpthread -o WSUCrypt ./src/main.c ./src/thread.o ./src/block.o ./src/key.o

thread.o: ./src/thread.c
	gcc -g -c ./src/thread.c -o ./src/thread.o

block.o: ./src/block.c
	gcc -g -c ./src/block.c -o ./src/block.o

key.o: ./src/key.c
	gcc -g -c ./src/key.c -o ./src/key.o

clean:
	rm -rf ./src/*.o WSUCrypt