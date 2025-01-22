TARGET = server

$(TARGET) : obj/main.o obj/file.o obj/parse.o
	gcc obj/main.o obj/file.o obj/parse.o -o server

obj/main.o : src/main.c
	gcc -c src/main.c -o obj/main.o

obj/file.o : src/file.c
	gcc -c src/file.c -o obj/file.o

obj/parse.o : src/parse.c
	gcc -c src/parse.c -o obj/parse.o

clean :
	rm server obj/*.o
